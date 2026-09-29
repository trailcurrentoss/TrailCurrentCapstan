#!/bin/bash
# Build Capstan firmware for every supported panel.
#
# One source tree, three boards. Each board gets its own sdkconfig and its own
# build directory (see docs/build.md, "Building several boards"), so the
# builds never share objects and tools/flash.sh can flash any of them over USB
# afterwards without a rebuild.
#
# Produces two binaries per board, collected in release/:
#   capstan_{board}.bin         — app-only (for OTA via Headwaters)
#   capstan_{board}_merged.bin  — merged  (for web flasher, full flash at 0x0)
#
# The app-only binary contains just the application image. Headwaters OTA
# writes it to a single app partition via esp_ota_write, which validates
# the image as an app. A merged binary would fail that validation because
# it starts with the bootloader, not an app header.
#
# The merged binary combines bootloader + partition table + OTA data + app
# into one file flashable at offset 0x0. The web flasher requires this
# because it writes the entire flash from a single binary.
#
# The board is baked into every image. A 480x480 build on a 240x240 panel
# does not error -- it boots to a blank screen -- so the board name is in
# every filename and there is deliberately no board-less capstan.bin.
#
# Usage:
#   ./build-all.sh                  build all three boards
#   ./build-all.sh matouch21 ...    build only the named boards
#   ./build-all.sh --clean [...]    regenerate each board's sdkconfig from the
#                                   tracked defaults first (use for releases)
#
# Without --clean an existing sdkconfig.{board} is reused as-is, and ESP-IDF
# does not re-apply sdkconfig.defaults or boards/*.defaults to it. A release
# built that way can silently carry a stale or locally-tweaked config, so
# always pass --clean for anything that goes on a GitHub release.
set -e

cd "$(dirname "$0")"

ALL_BOARDS=(matouch21 crowpanel146 crowpanel128)
CHIP="esp32s3"
APP_BIN="trailcurrent_capstan.bin"
RELEASE_DIR="release"

# An IDF_TARGET left over in the shell for another chip makes idf.py refuse
# to build against an esp32s3 sdkconfig.
export IDF_TARGET="$CHIP"

CLEAN=0
BOARDS=()
for arg in "$@"; do
    case "$arg" in
        --clean) CLEAN=1 ;;
        -h|--help) sed -n '2,35p' "$0"; exit 0 ;;
        matouch21|crowpanel146|crowpanel128) BOARDS+=("$arg") ;;
        *) echo "unknown argument: $arg (boards: ${ALL_BOARDS[*]})" >&2; exit 2 ;;
    esac
done
[ ${#BOARDS[@]} -eq 0 ] && BOARDS=("${ALL_BOARDS[@]}")

if ! command -v idf.py >/dev/null 2>&1; then
    echo "idf.py not found -- source ESP-IDF first: . \$IDF_PATH/export.sh" >&2
    exit 1
fi

mkdir -p "$RELEASE_DIR"

for board in "${BOARDS[@]}"; do
    b="build.$board"
    cfg="sdkconfig.$board"

    echo "========================================"
    echo "Building Capstan: $board ..."
    echo "========================================"

    if [ "$CLEAN" -eq 1 ]; then
        rm -f "$cfg"
        rm -rf "$b"
    fi

    idf.py -B "$b" \
           -D SDKCONFIG="$cfg" \
           -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;boards/$board.defaults" \
           build

    # Confirm the image is for the board we asked for. A reused sdkconfig
    # that was switched in menuconfig would otherwise ship under the wrong
    # name -- exactly the wrong-panel flash this naming exists to prevent.
    if ! grep -q "^CONFIG_CAPSTAN_BOARD_NAME=\"$board\"$" "$cfg"; then
        echo "$cfg does not select board '$board':" >&2
        grep "^CONFIG_CAPSTAN_BOARD_NAME=" "$cfg" >&2 || true
        echo "Re-run with --clean to regenerate it from boards/$board.defaults." >&2
        exit 1
    fi

    # App-only binary with board-specific name (for OTA)
    cp "$b/$APP_BIN" "$RELEASE_DIR/capstan_${board}.bin"

    # Merged binary (for web flasher -- flashable at 0x0). Offsets and flash
    # settings come from the flash_args IDF generated for this build rather
    # than being restated here: Capstan's partition table puts otadata at
    # 0xf000 and the app at 0x20000, not the 0xe000/0x10000 other modules use.
    (cd "$b" && esptool.py --chip "$CHIP" merge_bin \
        -o "../$RELEASE_DIR/capstan_${board}_merged.bin" \
        @flash_args)
    echo ""
done

echo "========================================"
echo "Build complete"
echo "========================================"
echo ""
echo "App-only binaries (for OTA):"
for board in "${BOARDS[@]}"; do ls -lh "$RELEASE_DIR/capstan_${board}.bin"; done
echo ""
echo "Merged binaries (for web flasher):"
for board in "${BOARDS[@]}"; do ls -lh "$RELEASE_DIR/capstan_${board}_merged.bin"; done
echo ""
echo "Local USB flashing uses the build directories directly:"
for board in "${BOARDS[@]}"; do echo "  tools/flash.sh $board"; done
echo ""
if [ "$CLEAN" -eq 0 ]; then
    echo "NOTE: built without --clean. Rebuild with --clean before attaching to a release."
else
    echo "Attach ALL of the release/ binaries to the GitHub release."
fi
