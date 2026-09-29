#!/usr/bin/env bash
#
# Stage the firmware's UI code into an EEZ Studio export, so the full
# (Docker) simulator can build it with realistic rig data.
#
# WHY THIS EXISTS
#
# EEZ Studio's simulator copies ONLY the export folder (main/ui/<res>/) into
# its container and compiles every .c under it. Everything that puts a value
# on a screen -- main/ui_*.c, the data model, the settings -- lives outside
# that folder, so without this the simulator can only draw empty screens.
#
# This copies those sources, plus the simulator's stand-ins for ESP-IDF and
# the board (sim/), into main/ui/<res>/sim/. That folder is gitignored and
# rebuilt from scratch on every run, so main/ui stays disposable: delete it,
# re-export, re-stage.
#
# Every staged .c is wrapped in #ifdef EEZ_LVGL_SIMULATOR, and the firmware
# never compiles the folder anyway -- main/CMakeLists.txt globs only the
# generated file names at the top of main/ui/<res>/.
#
# Run it after an EEZ Studio export (Ctrl+B) and after editing any staged
# source, then press Build in the simulator. See docs/simulator.md.
#
# Usage:  tools/sim_stage.sh [480|360|240 ...]    (default: every export)
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

# The firmware's own UI layer, the model and settings it reads, and the
# simulator's replacements for everything below them.
SOURCES=(
    main/ui_*.c
    main/actions.c
    main/vars.c
    components/capstan_model/src/capstan_model.c
    components/capstan_config/src/capstan_config.c
    sim/*.c
)
HEADERS=(
    main/ui_*.h
    components/capstan_board/include/capstan_board.h
    components/capstan_config/include/capstan_config.h
    components/capstan_model/include/capstan_model.h
    components/capstan_mqtt/include/capstan_mqtt.h
    components/capstan_portal/include/capstan_portal.h
    components/capstan_wifi/include/capstan_wifi.h
    sim/*.h
)

if [ "$#" -gt 0 ]; then
    RESOLUTIONS=("$@")
else
    RESOLUTIONS=()
    for d in main/ui/*/; do
        [ -f "$d/ui.h" ] && RESOLUTIONS+=("$(basename "$d")")
    done
fi

if [ "${#RESOLUTIONS[@]}" -eq 0 ]; then
    echo "No EEZ Studio export found under main/ui/. Open GUI/Capstan<res>.eez-project" >&2
    echo "in EEZ Studio and press Ctrl+B first." >&2
    exit 1
fi

for res in "${RESOLUTIONS[@]}"; do
    export_dir="main/ui/$res"
    if [ ! -f "$export_dir/ui.h" ]; then
        echo "$export_dir has no export -- open GUI/Capstan$res.eez-project in" >&2
        echo "EEZ Studio and press Ctrl+B, then run this again." >&2
        exit 1
    fi

    dest="$export_dir/sim"
    rm -rf "$dest"
    mkdir -p "$dest"

    cp -r sim/shim/. "$dest/"
    cp "${HEADERS[@]}" "$dest/"

    for src in "${SOURCES[@]}"; do
        {
            printf '/* STAGED by tools/sim_stage.sh from %s -- edit that file, not this copy. */\n' "$src"
            printf '#ifdef EEZ_LVGL_SIMULATOR\n'
            printf '#include "sim_prefix.h"\n'
            printf '#line 1 "%s"\n' "$src"
            cat "$src"
            printf '\n#endif /* EEZ_LVGL_SIMULATOR */\n'
        } > "$dest/$(basename "$src")"
    done

    # Read-only, so an edit made here by mistake fails at save time instead
    # of vanishing on the next run.
    find "$dest" -type f -exec chmod a-w {} +
    echo "staged $(ls "$dest" | wc -l) files into $dest"
done

echo
echo "Now press Build in EEZ Studio's simulator. If a project was already"
echo "open when you exported, it picks the new files up on the next build."
