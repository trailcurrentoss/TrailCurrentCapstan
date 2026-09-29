# Generated UI — do not edit, do not hand-write anything here

Every file under `main/ui/<res>/` is output from **EEZ Studio → Build
(Ctrl+B)**. The whole tree is disposable: delete `main/ui` entirely and
re-export from EEZ Studio to recreate it.

| Directory | Source project | Board |
|---|---|---|
| `main/ui/480/` | `GUI/Capstan480.eez-project` | MaTouch 2.1" (480×480) |
| `main/ui/360/` | `GUI/Capstan360.eez-project` | CrowPanel 1.46" (360×360) |
| `main/ui/240/` | `GUI/Capstan240.eez-project` | CrowPanel 1.28" (240×240) |

`main/CMakeLists.txt` compiles only the directory matching the board selected
in menuconfig.

## Nothing hand-written goes in here

`actions.c` and `vars.c` live in `main/`, not in these folders, precisely so
this tree stays disposable. If hand-written code lived here, deleting the
folder would delete real source.

**The one exception, and it is not hand-written:** `sim/` inside each
variant is staged by `tools/sim_stage.sh` for EEZ Studio's Docker full
simulator, which compiles only this folder. It is gitignored, rebuilt from
`sim/` and `main/` on every run, and never compiled into firmware
(`main/CMakeLists.txt` globs only the generated file names). Deleting it is
always safe. See [docs/simulator.md](../../docs/simulator.md).

## Workflow

1. Edit `GUI/Capstan<res>.eez-project` — via EEZ Studio, or a patch script in
   `GUI/tmp/`.
2. **Close and reopen the project in EEZ Studio** if it was edited on disk.
   EEZ Studio holds an in-memory copy and will otherwise export the old one —
   producing output that is genuinely newer than the file you edited, which
   defeats the obvious timestamp check.
3. Ctrl+B to export.
4. `idf.py build`.

Running `idf.py build` before the export just produces
`undefined reference to objects` errors that have nothing to do with your C.
