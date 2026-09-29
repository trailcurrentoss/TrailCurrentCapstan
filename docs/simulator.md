# The simulator — real UI code, simulated rig

EEZ Studio's Docker *full simulator* compiles the project to WebAssembly and
runs it in a window. For Capstan it runs **the firmware's own UI code**, the
`main/ui_*.c` files, `actions.c`, `vars.c`, `capstan_model.c` and
`capstan_config.c`, against a simulated trailer. What you see is what the
panel would show, down to the formatting, stale-value handling, LED ring
colour and alarm evaluation.

## Running it

1. Export the project in EEZ Studio (Ctrl+B).
2. `tools/sim_stage.sh` stages every exported resolution. Pass `480`, `360`
   or `240` to stage just one.
3. Press **Build** in EEZ Studio's simulator.

Re-run step 2 after every export and after editing any staged source. It
takes under a second.

## Controls

| Input | Acts as |
|---|---|
| Mouse wheel | Turning the ring, one detent per notch |
| Middle click, Enter or Space | Pressing the ring |
| Holding the press for 700 ms | Long press (back) |
| Esc or Backspace | Back |
| Arrow keys, `+`, `-` | One detent |
| Left click | Touch, on screens whose policy enables it. On other screens it counts as the press, like pushing the glass on a CrowPanel. |

LED ring colours have no pixels in the simulator, so they are printed to the
console panel as `LED ring #rrggbb`.

## How it fits together

```
sim/                 tracked -- the simulator's own code
  sim_boot.c         app_main()'s UI start-up, run after the simulator's ui_init()
  sim_feed.c         the rig: every reading, at each module's real cadence
  sim_board.c        capstan_board.h: mouse/wheel/keys -> ring callbacks
  sim_net.c          Wi-Fi/MQTT/portal: always connected; light commands echoed
  sim_platform.c     esp_timer, esp_restart (page reload), in-memory NVS
  shim/              header stand-ins: esp_log.h, FreeRTOS, sdkconfig.h, ...
tools/sim_stage.sh   copies sim/ + the firmware's UI sources into main/ui/<res>/sim/
```

The simulator compiles only the export folder, which is why anything has to be
copied there. The staged `main/ui/<res>/sim/` is gitignored, read-only, and
rebuilt from scratch on every run, so `main/ui` stays disposable. Every staged
`.c` is wrapped in `#ifdef EEZ_LVGL_SIMULATOR`, and the firmware never compiles
the folder anyway: `main/CMakeLists.txt` globs only the generated file names.

**Edit the originals, never the staged copies.** A `#line` directive in each
copy points compiler errors back at the original path.

Readings go in through the same `capstan_model_set_*()` calls that
`capstan_mqtt.c` makes for a broker message, and the rig's configuration
(device controls, alarms, Wi-Fi and broker) goes in through the
`capstan_config_set_*()` calls the retained-config handlers make. Nothing
downstream of those two boundaries knows it is simulated.

## The scenario

A travel trailer parked at a campsite on solar, running on the **local wall
clock**, so the dial reads the way the rig would at the current hour.

| Domain | Behaviour |
|---|---|
| Energy | 200 Ah LiFePO4 (2560 Wh). Solar follows the sun, with sunrise ~06:30, sunset ~19:30, ~340 W at a clear noon, and drifting cloud shade. Loads are ~9 W always-on, a fridge compressor (7 min on, 11 off), the water pump while water runs, the furnace fan, and every light that is switched on. SOC integrates the net power. Voltage follows the LiFePO4 curve plus IR drop. Charge state goes off, bulk, absorption, then float, and the MPPT backs off when the battery is full. Time-to-go is sent only while discharging, as the SmartShunt does. |
| Air | Cabin runs ~61 °F before dawn to ~73 °F mid-afternoon, with a first-order lag, and the furnace adds heat. Humidity runs opposite to temperature, and cooking and the tap add some. CO₂ builds overnight and in the evening, and rises while cooking. Verdicts use Borealis's own thresholds (CO₂ warn 1500 / alarm 2500 ppm), so dinner with everyone inside turns the air ring **Moderate** for a few minutes. |
| Water | A tap runs 25 s every 9 min, but only if the Water Pump is switched on. Fresh drains into grey, and a flush every 40 min moves water into black. |
| Level | Parked slightly nose-down and to the right, with footstep wobble that settles in a couple of seconds. Height differences come from a 5200 mm wheelbase and a 2350 mm track. |
| Lights | Seven controls (four PDM, three relays). Evening lights are on after sunset, the pump is on, and the furnace runs in the small hours. A press publishes the real command, and the status comes back 150 ms later, as on the rig. |
| Clock | GNSS time once a second from the browser's clock. The timezone is the browser's own IANA zone, so a zone missing from `capstan_model.c`'s table is logged exactly as it would be on the device. |
| Doors | The entry door opens for 20 s every 11 min. Mode is Camping, so the configured door alarms are disarmed and no alert fires. |

Nothing raises the alert overlay by default. To see it, edit the scenario, for
example by setting the mode to `CAPSTAN_MODE_DRIVING` in
`sim_feed_seed_config()`, then re-stage and rebuild. There is no runtime
control panel. EEZ Studio's simulator does not offer one.

## What it does not tell you

It says nothing about the board: no touch controller, display driver, redraw
timing, PSRAM, or the real LED ring. It also cannot catch a missing `ui_tick()`
timer (see the note in `main.c`), because the simulator's own loop calls
`ui_tick()` every frame.

## When something is missing

- **Link error `undefined symbol: capstan_...`**: a UI file started calling
  a component function the stand-ins in `sim/` do not provide. Add it to the
  matching `sim_*.c`.
- **A new `CONFIG_*` in UI code**: add it to `sim/shim/sdkconfig.h`, which
  mirrors `main/Kconfig.projbuild` by hand.
- **A new `main/ui_*.c`** is picked up automatically. Any other new source the
  UI depends on goes in `SOURCES` in `tools/sim_stage.sh`.
- **The simulator shows old behaviour**: re-run `tools/sim_stage.sh`, then
  read the Build Logs panel. A failed build leaves the previous simulator on
  screen.
