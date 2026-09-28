#!/usr/bin/env bash
#
# Flash one board, identified by what it SAYS it is.
#
# WHY THIS EXISTS
#
# The three panels enumerate in whatever order they were plugged in, and
# that order changes. Flashing by remembered port number will eventually
# put 360x360 firmware on a 240x240 panel, which does not error -- it
# boots to a blank screen and looks like a hardware fault. That happened.
#
# It happened through a specific hole: a port lookup returned an empty
# string, and `idf.py -p ""` does not fail, it silently auto-detects and
# flashes the first board it finds. So this script refuses to run
# without a confirmed port rather than letting the tool guess.
#
# Usage:  tools/flash.sh matouch21|crowpanel146|crowpanel128
set -euo pipefail

BOARD="${1:-}"
case "$BOARD" in
    matouch21|crowpanel146|crowpanel128) ;;
    *) echo "usage: $0 matouch21|crowpanel146|crowpanel128" >&2; exit 2 ;;
esac

[ -d "build.$BOARD" ] || { echo "build.$BOARD does not exist -- build first" >&2; exit 1; }

echo "Looking for a board that reports itself as '$BOARD'..."
PORT="$(python3 - "$BOARD" <<'PY'
import glob, sys, time
try:
    import serial
except ImportError:
    sys.exit(0)
want = sys.argv[1]
for p in sorted(glob.glob('/dev/ttyACM*')):
    try:
        s = serial.Serial(p, 115200, timeout=1)
    except Exception:
        continue
    # Pulse RTS to reset, then read the banner it prints on boot.
    #
    # Every step here can throw. These boards use the ESP32-S3's native
    # USB-Serial/JTAG, so a reset re-enumerates the device and the port
    # vanishes mid-read. An exception must skip the port, never abort
    # the search -- a crash here leaves the caller with no port, and an
    # empty port is exactly what caused the wrong board to be flashed.
    buf = b''
    try:
        s.setDTR(False); s.setRTS(True); time.sleep(0.1)
        s.setRTS(False); time.sleep(0.1)
        end = time.time() + 8
        while time.time() < end:
            buf += s.read(4096)
            if b"Capstan board '" in buf:
                break
    except Exception:
        pass
    finally:
        try:
            s.close()
        except Exception:
            pass
    txt = buf.decode('utf-8', 'replace')
    if "Capstan board '%s'" % want in txt:
        print(p)
        break
PY
)"

# The whole point. An empty port must stop us, because idf.py would
# happily auto-detect and flash the wrong panel.
if [ -z "$PORT" ]; then
    echo "REFUSING TO FLASH: no board identified itself as '$BOARD'." >&2
    echo "Boards present:" >&2
    ls /dev/ttyACM* 2>/dev/null >&2 || echo "  (none)" >&2
    echo "A board already in a bootloader or mid-reset will not answer;" >&2
    echo "unplug/replug it and try again." >&2
    exit 1
fi

echo "Found '$BOARD' on $PORT -- flashing."
idf.py -B "build.$BOARD" -D SDKCONFIG="sdkconfig.$BOARD" -p "$PORT" flash
