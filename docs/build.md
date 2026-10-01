# Building, flashing and bring-up

## Toolchain

ESP-IDF **v5.3 or newer** (developed against v5.5.2). Target `esp32s3` for
all three boards.

```sh
. ~/esp/<version>/esp-idf/export.sh
```

Dependencies are fetched by the component manager on first configure — LVGL
9.2.2, `esp_lvgl_port`, `esp_lcd_st7701`, `esp_lcd_gc9a01`, `esp_lcd_touch`,
`esp_lcd_touch_cst816s`, `mdns`. Nothing to install by hand.

## First build

```sh
idf.py set-target esp32s3
idf.py menuconfig          # Capstan -> Rotary display board
idf.py build
```

`set-target` must come first. Running `menuconfig` before it configures for
the wrong chip.

## Choosing a board

Everything board-specific lives behind one menuconfig choice:

```
Capstan  --->
    Rotary display board (Makerfabs MaTouch ESP32-S3 Rotary 2.1in)  --->
        (X) Makerfabs MaTouch ESP32-S3 Rotary 2.1in (480x480, ST7701S RGB)
        ( ) Elecrow CrowPanel 1.46in Rotary (360x360, JD9855)
        ( ) Elecrow CrowPanel 1.28in Rotary (240x240, GC9A01 SPI)
```

Switching boards changes the panel driver, the pin map, and which generated
UI variant is compiled (`main/ui/480`, `360` or `240`).

### Building from an IDE's build button

An IDE extension's build and flash buttons run a plain `idf.py build`
against the single `sdkconfig` in the project root. That build never reads
`boards/*.defaults`, and it reuses whatever `sdkconfig` already holds.

Settings that have a menuconfig prompt are **sticky**: once written to
`sdkconfig` they keep their value when the board choice changes, even though
their default is different for the new board. Two of them decide whether a
board works at all:

| Setting | MaTouch 2.1" | CrowPanel 1.46" | CrowPanel 1.28" |
|---------|--------------|-----------------|-----------------|
| `CONFIG_CAPSTAN_WIFI_TX_POWER_QDBM` | 44 | 44 | 80 |
| `CONFIG_CAPSTAN_LCD_SWAP_RB` | y | — | — |

A MaTouch built with transmit power left at 80 boots normally, reports its
setup access point as up, and cannot be seen by any phone or joined to any
network. With `SWAP_RB` unset its reds and blues are transposed.

So after switching boards, or after pulling a change to the Kconfig
defaults, regenerate the config instead of trusting the existing one:

```sh
rm sdkconfig
idf.py menuconfig          # pick the board, save
```

A regenerated config starts as the MaTouch, so its transmit power of 44
carries over to whichever board is picked next. That is right for the
CrowPanel 1.46"; on the 1.28" it works at reduced range until
`Wi-Fi maximum transmit power` is set back to 80 in the same menuconfig
session.

Then build from the IDE as usual, and confirm on the MaTouch's boot log:

```
board.disp: ST7701S 480x480 RGB up, pclk 12 MHz, R/B swapped
wifi: tx power limited to 11.0 dBm (asked 11.0)
```

If either line is missing or says otherwise, the `sdkconfig` is stale. The
per-board recipe below avoids the problem entirely, because each board
keeps its own config file.

### Building several boards without re-running menuconfig

`menuconfig` is fine when you work on one board at a time, but it writes the
single tracked-adjacent `sdkconfig`, so going back and forth between boards
means reconfiguring on every switch — and `sdkconfig.defaults` will not help,
because ESP-IDF only applies it when *creating* a new `sdkconfig`.

`boards/` holds a one-line defaults file per board. Give each board its own
config file and its own build directory and they stop colliding:

```sh
BOARD=crowpanel128          # or crowpanel146, matouch21

idf.py -B build.$BOARD \
       -D SDKCONFIG=sdkconfig.$BOARD \
       -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;boards/$BOARD.defaults" \
       build

idf.py -B build.$BOARD -D SDKCONFIG=sdkconfig.$BOARD -p <port> flash monitor
```

The per-board `sdkconfig.*` and `build.*/` are gitignored. This also sidesteps
the stale-object problem below, since each board keeps a separate build tree.

To pick up a changed `boards/*.defaults` or `sdkconfig.defaults`, delete that
board's config file first (`rm sdkconfig.$BOARD`) — same seeding rule as the
main `sdkconfig`.

`boards/*.defaults` only selects the board. Every verified per-board value —
colour element order, mirroring, encoder counts — lives in the Kconfig
defaults keyed off the board symbol, so there is one place to look and one
place to correct.

### Building every board at once (release binaries)

`build-all.sh` runs the recipe above for each board and collects two images
per board in `release/` (gitignored):

| File | Use |
|------|-----|
| `capstan_<board>.bin` | App-only image — OTA via Headwaters |
| `capstan_<board>_merged.bin` | Full image flashable at `0x0` — web flasher |

```sh
./build-all.sh                   # all three boards
./build-all.sh crowpanel128      # just one
./build-all.sh --clean           # regenerate sdkconfig.<board> first
```

Use `--clean` for anything that goes on a GitHub release. Without it an
existing `sdkconfig.<board>` is reused, and changes to the tracked defaults
never reach it. The merged image takes its offsets from the build's
`flash_args`, because this partition table puts the app at `0x20000`, not
at `0x10000` like other modules.

The build directories are left in place, so `tools/flash.sh <board>`
flashes the same build over USB afterwards.

There is no board-less `capstan.bin`. Every image is tied to one panel, and
the wrong one boots to a blank screen instead of failing.

**Do a full clean when switching.** The pin map is resolved at compile time
through headers that CMake does not track as dependencies, so an incremental
build after a board change can link stale objects:

```sh
idf.py fullclean && idf.py build
```

## Never edit `sdkconfig`

`sdkconfig` is generated and `.gitignore`d. Edits to it are invisible to
`git status` and to review, and the next regeneration silently discards them.

All configuration goes in **`sdkconfig.defaults`**, which is tracked. That
file only *seeds* a new `sdkconfig` — ESP-IDF does **not** re-apply it to an
existing one, so adding a key there has no effect until the generated file is
recreated:

```sh
rm sdkconfig && idf.py build
```

Then confirm the key actually landed:

```sh
grep CONFIG_THE_KEY_YOU_ADDED sdkconfig
```

If it still reads `# ... is not set` after `=y` in the defaults, the
regeneration did not happen. Reading `sdkconfig` is fine; writing it is not.
A value in `sdkconfig` that disagrees with `sdkconfig.defaults` is a finding
to report, not to correct in place.

## Flashing

**Back up the factory firmware first.** The vendors do not publish
recoverable images for these boards — see
[../DOCS/FactoryFirmware/README.md](../DOCS/FactoryFirmware/README.md).

**Identify the board by MAC, never by port number.** Plugging boards in a
different order renumbers `ttyACM*`, so a port that was the MaTouch an hour
ago may not be now — and flashing a 480×480 build to a 240×240 panel wastes
a debugging session.

```sh
ls -l /dev/serial/by-id/
# usb-Espressif_USB_JTAG_serial_debug_unit_<MAC>-if00 -> ../../ttyACMn
```

Note each of your own boards' MACs somewhere local; they are per-unit, so
there is no table of them here that would be right for anyone else. To
identify an unknown board, see
[hardware.md](hardware.md#identifying-a-connected-board).

```sh
idf.py -p <port> flash monitor
```

All three use the ESP32-S3's native USB-Serial/JTAG, so there is no bridge
driver to install and no auto-reset circuit to fight. If the port does not
appear, the cable is charge-only or the hub is not passing USB 2.0 data —
try a direct host port.

## First bring-up

With no EEZ Studio export present, the firmware shows a **bring-up test
screen** instead of the product UI. It exists to answer three questions in
one glance.

### 1. Does the panel work?

Three full-width bars — red, green, blue — plus the board name and
resolution. If the screen stays black:

- **CrowPanel:** GPIO1 and GPIO2 must be driven HIGH before any panel
  traffic. Handled in `enable_panel_rails()`; if you have been editing
  `board_display.c`, check it still runs.
- Check the backlight actually came on. It is deliberately held off until the
  first frame is drawn, so a crash before that point looks like a dead panel.
- On the MaTouch, the RGB timings or `pclk_hz` may be wrong — start at the
  vendor's effective 12 MHz.

#### The panel smoke test

When the screen is black and the driver reports success, the first question
is whether the fault is *below* LVGL or *in* it. Guessing wastes flash
cycles, so there is a test that answers it directly:

```
Capstan  --->  [*] Panel smoke test at boot
                   (3000) Smoke test: milliseconds per colour
                   (3)    Smoke test: number of R/G/B cycles
```

Immediately after the panel initialises — and before LVGL draws anything —
it fills the screen red, then green, then blue, writing straight through
`esp_lcd`. The backlight is forced on for the duration.

| Result | Meaning |
|---|---|
| Colours appear | Pins, timings and init sequence are **all correct**. The fault is above, in LVGL or the flush path. |
| Colours appear, red/blue transposed | Panel is fine; only the channel order is wrong. Set `CONFIG_CAPSTAN_LCD_SWAP_RB`. |
| Black throughout | LVGL is irrelevant. The fault is the pin map, the RGB timings, or the ST7701 init sequence. |

Defaults give a 27-second run so it cannot be missed — reset the board and
watch. Turn it off for normal builds; it costs that time on every boot.

It also settles the channel-order question without LVGL involved at all,
which is useful on the MaTouch where the vendor's firmware and wiki
disagree.

### 2. Are red and blue the right way round? (MaTouch only)

If the bar labelled **RED** renders blue, the channel order is inverted:

```
Capstan  --->  [*] Swap the red and blue channels
```

Rebuild. **Do not edit the pin map** — it matches the schematic. See
[hardware.md](hardware.md#trap--the-redblue-channel-order-is-genuinely-ambiguous)
for why this is ambiguous in the vendor documentation.

### 3. Does the ring behave?

Below the bars is a wrapping selector, `item N / 9`, and a direction readout.

- **One detent must move it by exactly one.** If it jumps by two or four,
  `CONFIG_CAPSTAN_ENCODER_STEPS_PER_DETENT` is wrong for your unit. Turn on
  `CONFIG_CAPSTAN_ENCODER_DEBUG`, turn the ring one detent, and read the raw
  count from the log.
- **Clockwise must increase.** If not, set `CONFIG_CAPSTAN_ENCODER_INVERT`.
- **Reversal must be instant.** Spin several turns in one direction, then
  reverse — the selection must move on the very first detent back. If it does
  not, something upstream is storing an absolute position, which is
  prohibited. See [architecture.md](architecture.md#2-the-ring-reports-direction-never-position).
- A short press logs nothing yet; a **long press** logs `ring long-press
  (back)`.

The counts-per-detent defaults are per-board and not interchangeable — 1.28"
is 4, 1.46" is 2, MaTouch is 4. The only encoder with a datasheet is the
1.28"'s, and it contradicts both itself and the hardware. Calibrate.

## Building the UI

The product screens come from EEZ Studio. `main/ui/<res>/` is empty until the
first export, and the build warns rather than failing:

```
No generated UI in main/ui/480. Building a placeholder firmware.
```

See [gui.md](gui.md) for the export workflow. In short: edit the
`.eez-project` in `GUI/`, **close and reopen it in EEZ Studio** if it was
changed on disk, Ctrl+B, then `idf.py build`.

Running `idf.py build` before the export produces `undefined reference to
objects` errors that have nothing to do with your C.

## Troubleshooting

| Symptom | Cause |
|---|---|
| `No Capstan board selected` at configure | menuconfig has not been run, or `sdkconfig` was deleted |
| Black screen, no log errors | CrowPanel rails (GPIO1/2), or a crash before the backlight is enabled |
| Red and blue swapped, MaTouch | Channel order is set by data-pin grouping — `CONFIG_CAPSTAN_LCD_SWAP_RB` |
| Red and blue swapped, either CrowPanel | `CONFIG_CAPSTAN_LCD_BGR`. The two boards need opposite values (1.28" `y`, 1.46" `n`); do not copy one to the other |
| Ring moves selection by 2 or 4 per detent | `CONFIG_CAPSTAN_ENCODER_STEPS_PER_DETENT` |
| Ring moves the wrong way | `CONFIG_CAPSTAN_ENCODER_INVERT` |
| Must wind back after overshooting | Something is storing absolute position — a bug, not a setting |
| Touch dead on MaTouch | It has no INT or RST line and is polled; check the I2C bus on 17/18 |
| Screen draws once, then never updates | Nothing is driving `ui_tick()` |
| `undefined reference to objects` | EEZ Studio export has not run since the `.eez-project` changed |
| Builds clean but the board shows nothing | Stale CMake glob — `idf.py reconfigure` |
| A `CONFIG_*` in defaults has no effect | `sdkconfig` was not regenerated — `rm sdkconfig && idf.py build` |
