# Hardware

Three boards, one firmware. This document is the reference for what is
actually on each one, where each number came from, and the handful of
details that will waste an afternoon if you do not know them.

**Provenance rule:** every pin in `components/capstan_board/src/board_pins.h`
was read out of vendor firmware and cross-checked against the vendor wiki;
the 1.46" was additionally verified against its Eagle schematic netlist.
Nothing is inferred from a photograph or a sibling board. If you change a
value, cite where it came from.

## What all three share

Confirmed by reading the silicon over USB, not from the product pages:

```
Chip     ESP32-S3 (QFN56) rev v0.2, dual core + LP core, 240 MHz
PSRAM    8 MB embedded, AP_3v3  ->  R8 part = OCTAL SPI
Flash    16 MB, quad (4 data lines) per eFuse, 3.3 V
Crystal  40 MHz
Console  native USB-Serial/JTAG on the USB-C port -- no CP210x/CH34x bridge
```

Two consequences:

- **Octal PSRAM** drives `CONFIG_SPIRAM_MODE_OCT=y` at 80 MHz. The product
  pages say only "8MB PSRAM". Configuring it as quad costs roughly four times
  the PSRAM bandwidth, which the 480×480 RGB panel cannot absorb.
- **Native USB-JTAG** means `CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG=y`. The UART0
  console default would be silent, and flashing needs no driver.

### Identifying a connected board

They are indistinguishable on the USB bus — all three enumerate as a bare
`Espressif USB JTAG/serial debug unit` with no model in the descriptor. The
symlink does carry each board's MAC, which is a stable per-unit identifier:

```sh
ls -l /dev/serial/by-id/
```

Record your own units' MAC addresses somewhere local and untracked — they
differ for every board, so a table of them in the repository would be wrong
for everyone but its author.

If you have a board in hand and do not know which it is, read it out of the
hardware rather than guessing. The two Elecrow boards are the easy ones to
confuse: same branding, same silicon, and "ESP32" in both product names
despite both being ESP32-S3.

Plugging boards in a different order renumbers `ttyACM*`, so never assume
`ttyACM0` is a particular board.

A factory dump identifies its own board. Both Elecrow images carry an
`ELECROW` string and a BLE device name naming the panel size; the MaTouch
image carries neither:

```sh
strings -n 4 <dump>.bin | grep -iE 'ELECROW|ESP32S3_1\.(28|46)_BLE'
#  -> ESP32S3_1.28_BLE_Server   CrowPanel 1.28"
#  -> ESP32S3_1.46_BLE_Server   CrowPanel 1.46"
#  -> (no match)                MaTouch 2.1"
```

Worth doing before trusting a label: during this project's own bring-up both
Elecrow boards were initially filed under the wrong name, because the board
present was assumed to be the one just mentioned rather than identified.

---

## Makerfabs MaTouch 2.1" — 480×480, ST7701S

Source: `github.com/Makerfabs/MaTouch-ESP32-S3-Rotary-IPS-Display-with-Touch-2.1-ST7701`,
`example/fw_test/fw_test.ino`, plus `wiki.makerfabs.com`. A copy of the vendor
sketch is in [`DOCS/MakerFabMaTouch/`](../DOCS/MakerFabMaTouch).

16-bit RGB565 parallel bus, with a 3-wire SPI sideband used only to push the
init sequence.

| Signal | GPIO | | Signal | GPIO |
|---|---|---|---|---|
| DE | 2 | | G0–G5 | 39, 7, 47, 8, 48, 9 |
| VSYNC | 42 | | (see note) | 11, 15, 12, 16, 21 |
| HSYNC | 3 | | (see note) | 4, 41, 5, 40, 6 |
| PCLK | 45 | | | |
| SPI CS / SCK / SDA | 1 / 46 / 0 | | LCD RST | **not connected** |
| Backlight | 38 | | Touch SDA / SCL | 17 / 18 |
| Encoder A / B | 13 / 10 | | Encoder button | 14 |

RGB timings (vendor): hsync front 10, pulse 8, back 50; vsync front 10,
pulse 8, back 20; both polarities active high.

### Trap — the red/blue channel order is genuinely ambiguous

The vendor's **firmware** labels GPIO 11/15/12/16/21 as `R0..R4` and
4/41/5/40/6 as `B0..B4`. The vendor's **wiki** labels the same pins the exact
opposite way. They are the same physical wires; Arduino_GFX reconciles the
difference by passing a BGR flag, so one of the two labels is simply wrong and
only the panel can say which.

The pin map follows the **wiki** convention. If the bring-up test screen shows
its `RED` bar rendering blue, set `CONFIG_CAPSTAN_LCD_SWAP_RB` and rebuild —
do **not** edit the pin map, which should keep matching the schematic.

### Trap — touch INT and RST do not exist

The touch controller is a **CST826** (not the CST8266 named in the design
brief). Its interrupt and reset lines are unusable, and this is a hardware
fact rather than a documentation gap:

- `TP_INT` reaches GPIO0 only through an **unpopulated** resistor — and GPIO0
  is also the LCD SPI SDA and a boot strapping pin, so it could not be used
  regardless.
- `TP_RST` has a 10K pull-up and **no GPIO drive at all**.

The vendor firmware's `#define TOUCH_RST -1 // 38` and `TOUCH_IRQ -1 // 0`
comments are stale: 38 is the backlight and 0 is the LCD SDA. The controller
is polled over I2C.

### Bring-up notes from first hardware run

Five faults found while bringing this board up, all recorded because none is
obvious from the datasheets and several present as something other than what
they are.

**The stock ST7701 init table blanks this panel.** `esp_lcd_st7701`'s default
sequence leaves the screen completely black: the driver returns success, the
backlight lights, and even direct `esp_lcd_panel_draw_bitmap()` writes show
nothing. The cause is the GIP / gate-driver block (`0xE0`-`0xEF` in Command2
BK1), which programs the panel's gate and source scan waveforms and is
supplied by the *module* maker, not the silicon vendor. Espressif's values
are for different glass -- its `0xEB` is `{00 01 E4 E4 44 88 00}` against
this panel's `{00 00 40 40 00 00 00}`, and it omits `0xCD` entirely. With the
wrong ones the RGB bus clocks pixels in perfectly while the glass never
scans. See `components/capstan_board/src/st7701_matouch_init.h`.

**`full_refresh` is mandatory with `avoid_tearing`.** That combination gives
LVGL the panel's two framebuffers and swaps them, but LVGL's default partial
render mode only draws the changed region into whichever buffer is next. The
two diverge, and every swap shows a mixture: flicker while anything animates,
stale bands in regions nothing redrew, and label updates that appear not to
happen because they landed in the buffer not currently being scanned out.

**Rotation does not reach a plain `lv_obj`.** LVGL sends `LV_EVENT_ROTARY`
only to a focused widget whose *class* is declared editable
(`lv_indev.c`: `if (lv_obj_is_editable(obj))`), and `lv_obj` is
`LV_OBJ_CLASS_EDITABLE_FALSE`. Rotation over one falls through to the scroll
branch and does nothing -- with the driver correctly delivering detents that
nothing consumes. Capstan therefore delivers rotation through
`capstan_board_set_rotate_callback()` instead of LVGL's focus machinery.

**The encoder counts backwards on this board.** Clockwise decremented.
`CONFIG_CAPSTAN_ENCODER_INVERT` now defaults to `y` for the MaTouch,
verified on hardware. 4 counts per detent was correct as assumed.

**Round panel, not square.** The corners of the 480x480 frame do not exist
and the usable width narrows quickly away from the vertical centre -- at
165 px above or below centre only about +/-175 px of width remains. Lay
screens out from the centre and keep content within roughly +/-130 px
vertically.

Two more, both about the panel IO lifetime:

**`auto_del_panel_io` deletes the SPI sideband.** The ST7701 driver tears
down the 3-wire SPI IO once the init sequence has been sent — it exists only
to configure the panel. Any command sent afterwards fails with *"Panel IO is
deleted, cannot send command"*. Calling `esp_lcd_panel_disp_on_off()` after
init therefore aborts board init and boot-loops the board. It is also
unnecessary: the init sequence already issues DISPON.

**`pclk_active_neg` should be 0.** Set to 1, the panel initialises without
error, the backlight lights, and the screen stays pure black — the
controller is configured but sampling the bus on the wrong clock edge, so it
never sees valid video. `esp_lcd_st7701`'s own reference timing macro uses
`false`, and the vendor sketch inherits Arduino_GFX's default of `false`.

### Trap — a reset sequence that ends ASSERTED

The JD9855 showed black with the backlight on, 47 init commands all
returning `ESP_OK`, a clean boot and no errors. The init table had been
verified byte-for-byte against upstream and the panel config matched the
vendor's on every field.

The panel was held in reset the entire time.

`esp_lcd_panel_dev_config_t.flags.reset_active_high` is the level that
HOLDS the panel in reset. On this board it is 0, so asserted is LOW. The
sequence was written as:

```c
gpio_set_level(pin,  reset_level);   /* LOW  — assert  */
gpio_set_level(pin, !reset_level);   /* HIGH — release */
gpio_set_level(pin,  reset_level);   /* LOW  — assert AGAIN */
```

ending with reset asserted. Every subsequent command went to a chip that
was not listening, over a 3-wire bus that is **write-only with no
readback** — so `esp_lcd_panel_io_tx_param()` returned `ESP_OK` for all 47.

Two things made this hard to see:

- **Every observable signal said the driver was fine.** There is no
  diagnostic anywhere that distinguishes "the panel received this" from
  "the SoC transmitted this".
- **The comment above the code was correct and the code was not.** It read
  "high, low, high", matching the vendor sketch. But the vendor's literal
  `HIGH` is this code's `!reset_level`, and the transcription used
  `reset_level` — the asserted level — for it.

A reset sequence must END DEASSERTED. When a panel is black but its driver
reports success, check that before suspecting the init table: it is far
cheaper to verify and produces exactly the same symptom.

### Trap — esp_lvgl_port owns the panel orientation

`lvgl_port_add_disp()` applies its own `cfg.rotation` by calling
`esp_lcd_panel_swap_xy()` and `esp_lcd_panel_mirror()`. Anything set on the
panel BEFORE that call is silently overwritten with the port's defaults,
which are all zero.

The symptom actively misleads. Calling `esp_lcd_panel_mirror(panel, 1, 0)`
after `panel_init()` returns `ESP_OK`, logs happily, and does nothing — the
log says `mirror_x=1` while the panel does `mirror_x=0`. Flipping to the
other axis changes nothing either, because neither value ever reaches the
glass. It reads like the mirror API being broken.

Set orientation in `lvgl_port_display_cfg_t.rotation`, never on the panel
directly:

```c
.rotation = { .swap_xy = ..., .mirror_x = ..., .mirror_y = ... },
```

And keep the TOUCH transform in step (`esp_lcd_touch_config_t.flags`).
Mirror only the display and every touch lands at its reflection — the
display looks perfect, so it presents as a touch fault and sends you into
the touch driver.

Orientation differs per board and cannot be inferred from a sibling: the
CrowPanel 1.28" needs `mirror_x`, the MaTouch needs nothing. The 1.46" is
untested.

### Note — pclk_hz is not a vendor specification

No vendor example passes `prefer_speed`, so the demos inherit Arduino_GFX's
default, which for octal PSRAM is **12 MHz**. That is what the firmware uses.
Treat it as a known-good starting point, not a spec — raise it only with a
frame-rate measurement to justify it, because past the PSRAM bandwidth limit
the panel tears.

### Note — strapping pins on the display bus

The schematic carries the warning "IO45/IO46: pulldown!!!". Both are strapping
pins (45 = VDD_SPI, 46 = boot) and both are used by the display (PCLK and SPI
SCK). Relevant only if something drives them during boot.

No WS2812, no battery, no SD card, no IO expander on this board.

---

## Elecrow CrowPanel 1.46" — 360×360, JD9855

Source: `github.com/Elecrow-RD/CrowPanel-1.46inch-HMI-ESP32-Rotary-Display`
(branch `master`), `example/V1.0/Arduino/RotaryScreen_1_46_Code_Core3_LVGL9/`.
**Every pin below was additionally confirmed against the Eagle schematic
netlist** (`Eagle_SCH&PCB/*.sch`, which is XML and can be parsed directly —
far more reliable than the vector PDF).

Plain 4-wire SPI at 80 MHz. Not QSPI, despite what the form factor suggests.

| Signal | GPIO | | Signal | GPIO |
|---|---|---|---|---|
| SCK / MOSI / MISO | 10 / 11 / — | | Touch SDA / SCL | 6 / 7 |
| DC / CS / RST | 3 / 9 / 14 | | Touch INT / RST | 5 / 13 |
| Backlight | 46 (LEDC 5 kHz, 8-bit) | | Encoder A / B / btn | 45 / 42 / 41 |
| WS2812 ring | 48 (8 LEDs) | | WS2812 power enable | **17** |
| Power LED | 40 (active low) | | Aux I2C SDA / SCL | 38 / 39 |
| Battery sense | 18 (ADC2_CH7) | | Charge status | 15 (active low) |

`rgb_order = true` on this panel — the **opposite** of the 1.28". Do not copy
one board's value to the other.

### Trap — GPIO1 and GPIO2 must be HIGH before any panel traffic

The vendor firmware drives both high in `setup()` with no explanation. Without
them the panel accepts nothing: black screen, no error, no clue. This is the
most likely reason a CrowPanel bring-up appears to do nothing at all. Handled
by `enable_panel_rails()` in `board_display.c`.

### The JD9855 has no driver anywhere

There is no `esp_lcd_jd9855` in esp-iot-solution, and no public repository
implements it. The panel datasheet (`P146B001-IPS-CTP-V2`) has **no
initialisation section**.

Elecrow's own firmware drives it as an **ST77961** via LovyanGFX, so
`components/capstan_board/src/jd9855_init_table.h` is transcribed from
`Panel_ST77961.hpp`. That transcription was verified byte-for-byte against
upstream: **47 commands, 193 parameter bytes, exact match**, including
LovyanGFX's `0x80` delay-flag encoding, which is unpacked here into an
explicit `delay_ms` field.

One gotcha when reading the upstream source: `Panel_ST77961`'s *constructor*
defaults to 360×**390**. That is wrong for this board. The init table's own
`CASET`/`RASET` set `0x0000..0x0167` (0–359), so the table is the authority
and the panel is 360×360.

### Battery is present but unwired

`BAT_CHK` → **GPIO18** through a 1K/1K divider (so the ADC reads Vbat/2), and
`BAT_CHG` → `XTAL_32K_P` = **GPIO15** with a 10K pull-up, active low while
charging. Neither appears in the vendor firmware or wiki; both came from the
schematic netlist.

**GPIO18 is ADC2_CH7, and ADC2 is shared with the Wi-Fi radio on ESP32-S3.** A
read while Wi-Fi is up returns `ESP_ERR_TIMEOUT`. The pin is known; the
sampling strategy is not, so battery monitoring is deliberately not
implemented rather than shipped as something that fails intermittently.

### Note — GPIO43 is a demo LED and also U0TXD

The vendor wires a "bulb" LED to GPIO43, which is UART0 TX. Capstan does not
drive it, keeping a serial console available as a fallback.

---

## Elecrow CrowPanel 1.28" — 240×240, GC9A01

Source: `github.com/Elecrow-RD/CrowPanel-1.28inch-HMI-ESP32-Rotary-Display-240-240-IPS-Round-Touch-Knob-Screen`
(branch `master`), `example/Arduino/RotaryScreen_1_28/`. Wiki and firmware
agree on every value.

**Pin-for-pin identical to the 1.46"** for display, touch, encoder and
backlight. The differences:

| | 1.28" | 1.46" |
|---|---|---|
| Controller | GC9A01 | JD9855 |
| Resolution | 240×240 | 360×360 |
| `invert` | true | false |
| `rgb_order` | false | true |
| WS2812 | 48, 5 LEDs, no enable pin | 48, 8 LEDs, enable on GPIO17 |
| Battery | none | present |

Same GPIO1/GPIO2 rail requirement as the 1.46".

### Encoder — the one part with a datasheet

`Datasheet/EC3501 C15H30P3-规格书2.pdf` in the vendor repo: **30 detents**,
step angle 12±2°, and "output signal is 1 pulse per 2 detents". Contact
chatter ≤ 5 ms.

Under full 4× quadrature decode that would be **2 counts per detent**. The
datasheet contradicts itself — section 4-2 claims "30 pulses/360° for each
phase", which gives 4 — and **section 4-2 is the one that matches the
hardware**. `CONFIG_CAPSTAN_ENCODER_STEPS_PER_DETENT` is **4** for this board,
verified on hardware: at 2, one detent moved the selection by two. The part
number `C15H30P3` (15 pulses, 30 detents) reads like it supports the output
note, and it is wrong. Trust the board.

The 1.46"'s encoder part is **not documented** by the vendor, and it is **not
the same encoder**. Verified on hardware at **2** counts per detent; at 4 it
took two detents to move the selection once. The boards are pin-identical and
that is the whole trap — neither board's value may be derived from the other's.

---

## Addressable RGB LED ring

Both Elecrow CrowPanels carry a ring of WS2812 LEDs around the display. The
MaTouch does not.

| Board | LEDs | Data | Power enable |
|---|---|---|---|
| MaTouch 2.1" | — | — | — |
| CrowPanel 1.46" | 8 | GPIO48 | **GPIO17** — ring stays dark without it |
| CrowPanel 1.28" | 5 | GPIO48 | none |

This is a hardware property, so it is **derived from the board choice** and
cannot be set to something the board cannot do:

| Symbol | Meaning |
|---|---|
| `CONFIG_CAPSTAN_HAS_RGB_LEDS` | auto, no prompt — does this board have a ring |
| `CONFIG_CAPSTAN_RGB_LED_COUNT` | auto — 5, 8 or 0 |
| `CONFIG_CAPSTAN_RGB_LEDS_ENABLED` | user switch, only offered on a board that has one |
| `CONFIG_CAPSTAN_RGB_LEDS_MAX_BRIGHTNESS` | ceiling on every write, default 40% |

At runtime, screens read `capstan_board_info()->has_rgb_leds` and
`->rgb_led_count`. `has_rgb_leds` folds together "the board has a ring" and
"this build uses it", so a caller needs only the one test — and any screen
using the ring must still work on a board without one.

A `_Static_assert` in `capstan_board.c` checks the Kconfig values against
`BOARD_HAS_WS2812` / `BOARD_WS2812_COUNT` in `board_pins.h`, so the two
declarations cannot drift apart.

**Driven** by the RMT peripheral (`board_leds.c`), GRB on the wire, every
write scaled by the brightness ceiling. What each screen shows is in
[screens.md](screens.md#led-ring).

**Positions**, mapped on hardware by lighting each chain index its own
colour. Clock positions seen from the front; the side column is
`BOARD_WS2812_SIDE` in `board_pins.h`, which `capstan_board_leds_set_sides()`
uses to paint the left and right halves (0 = left dark, at the bottom):

| Index | 1.28" | side | 1.46" | side |
|---|---|---|---|---|
| 0 | 4 o'clock | right | 2 o'clock | right |
| 1 | 1 o'clock | right | 4 o'clock | right |
| 2 | 11 o'clock | left | 5 o'clock | 0 |
| 3 | just shy of 9 | left | 7 o'clock | 0 |
| 4 | 6 o'clock | 0 | 8 o'clock | left |
| 5 | — | | 10 o'clock | left |
| 6 | — | | 11 o'clock | left |
| 7 | — | | 1 o'clock | right |

## Memory

All three boards have the same memory: **16 MB flash**, **8 MB octal PSRAM**,
and the ESP32-S3's own **~512 KB internal SRAM**. Flash is not the constraint;
internal RAM is. Measured 2026-09-29, firmware with the four clock faces,
Getting Started, Locale and the Settings carousel (15 screens).

### Flash

`partitions.csv` gives each of two OTA slots 4 MB.

| Board | App image | Slot free |
|---|---|---|
| MaTouch 2.1" (480) | 1.94 MB | 53% |
| CrowPanel 1.46" (360) | 1.70 MB | 58% |
| CrowPanel 1.28" (240) | 1.54 MB | 62% |

The difference is fonts: each panel carries its own sizes, and the 480's are
the largest. A new font size costs roughly 10–50 KB on one board.

### Internal RAM

The `capstan: service alive` log line (every ~11 s) reports internal RAM
free and its largest free block. Free RAM falls for the first minute or
two after boot -- Wi-Fi, MQTT subscriptions and their retained messages,
the first frames of each screen -- then holds within a few hundred bytes.
Six minutes on each board:

| Board | 12 s after boot | Settled (~100 s on) | Largest free block |
|---|---|---|---|
| MaTouch 2.1" | 108.7 KB | ~36.3 KB | 31.7 KB |
| CrowPanel 1.46" | 41.3 KB | ~20.2 KB | 18 KB |
| CrowPanel 1.28" | 40.9 KB | ~19.7 KB | 18 KB |

Not a leak: flat from ~100 s to 6 min on all three. The CrowPanels have less
because their SPI panels draw through two 16 KB DMA buffers that must be in
internal RAM (see the note in `board_display.c`); the MaTouch's RGB
framebuffers live in PSRAM. `CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL` (32 KB)
is held back separately for DMA and internal-only allocations and is not in
these figures.

**Why the UI costs internal RAM.** LVGL allocates through the normal heap
(`CONFIG_LV_USE_CLIB_MALLOC`), and `CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL=16384`
sends every allocation under 16 KB to internal RAM first, spilling to PSRAM
only when internal RAM is short. Every widget is far below 16 KB, so screens
land in internal RAM: EEZ builds all of them at boot (`generated UI
initialised -- 77 KB heap used`), and the ~200 widgets of the clock faces
and their preview took the CrowPanels' settled figure from ~34 KB to ~20 KB.

**What to watch.** The largest free block, not the total. mbedTLS
(`CONFIG_MBEDTLS_EXTERNAL_MEM_ALLOC`) and Wi-Fi / lwIP buffers
(`CONFIG_SPIRAM_TRY_ALLOCATE_WIFI_LWIP`) already use PSRAM, so an MQTT
reconnect does not need a large internal block; task stacks and DMA do. If
a CrowPanel starts failing to create tasks or dropping its connection
after more screens are added, this is the first place to look.

**If it gets tight:** route LVGL's allocations to PSRAM explicitly (a custom
LVGL allocator over `heap_caps_malloc(..., MALLOC_CAP_SPIRAM)`), leaving
internal RAM to Wi-Fi, DMA and stacks. That is a build-config change -- each
board's `sdkconfig` would have to be regenerated -- so it is not made until
it is needed.

## What is verified on hardware

All three boards are validated: display, colour, orientation, touch,
calibration, ring rotation and ring press, confirmed by running the
firmware rather than read from a datasheet.

| | MaTouch 2.1" | CrowPanel 1.46" | CrowPanel 1.28" |
|---|---|---|---|
| Boots clean, no panic | yes | yes | yes |
| Panel output | yes | yes | yes |
| Panel init table | **custom** (stock blanks it) | **hand-written** (no driver exists) | stock GC9A01 |
| Colour channel order | **`SWAP_RB=y`** | **`LCD_BGR=n`** | **`LCD_BGR=y`** |
| Display orientation | all flags off | **all flags off** | **`LCD_MIRROR_X=y`** |
| Touch orientation | all flags off | all flags off | all flags off |
| Touch input + calibration | yes | yes | yes |
| Ring direction | **`INVERT=y`** | **`INVERT=n`** | **`INVERT=n`** |
| Counts per detent | **4** | **2** | **4** (datasheet says 2; hardware says 4) |
| Ring press + long press | yes | yes | yes |
| RGB LED ring | none fitted | 8 LEDs, not driven | 5 LEDs, not driven |
| Factory firmware backed up | yes | yes | yes |

### Colour correctness needs a colour target

Both CrowPanels shipped red-and-blue swapped for most of bring-up, and the
1.28" was recorded in this table as "verified" while it was still wrong.
It was not a careless entry: the board had been through display, touch,
calibration and encoder checks, and a screen full of UI chrome gives the
eye nothing to catch a red/blue swap against. The swap was only noticed
after the same fault was found on the 1.46" and the other board was
re-examined deliberately.

Treat channel order as unverified until a known red, green and blue swatch
has been put on the panel and looked at. "The UI looks right" is not that
test.

### Nothing carries over between boards

Every differing row was a value that looked safe to infer from a sibling
and was not:

- The MaTouch needs `SWAP_RB`; neither CrowPanel does.
- The two CrowPanels need **opposite** colour element orders — 1.28" BGR,
  1.46" RGB — even though both are SPI round panels from the same vendor.
  Neither value can be read off the vendor's LovyanGFX `cfg.rgb_order`,
  which is the exact inverse of what each board actually needs.
- The MaTouch needs `ENCODER_INVERT`; neither CrowPanel does.
- The 1.28" needs `LCD_MIRROR_X`; **the 1.46" does not — despite the two
  being pin-for-pin identical on display, touch and encoder.** That pair is
  the clearest warning in the set: identical wiring does not imply
  identical orientation, because the difference is in how the glass is
  bonded, not how it is connected.
- 4 counts per detent on the MaTouch, 2 on both CrowPanels.
- The MaTouch needs a hand-written init table because the stock one blanks
  it; the 1.28" works on stock; the 1.46" has no stock driver at all.

**Display and touch orientation are also independent of each other.** The
1.28" needs `LCD_MIRROR_X=y` with `TOUCH_MIRROR_X=n`. The display mirror
changes how the framebuffer maps onto the glass; the touch controller is a
separate device fixed by how its own layer is bonded.

Treat every per-board value as unknown until it has been seen on that
board.

## Cross-board summary## Cross-board summary## Cross-board summary

| | MaTouch 2.1" | CrowPanel 1.46" | CrowPanel 1.28" |
|---|---|---|---|
| Resolution | 480×480 | 360×360 | 240×240 |
| Controller | ST7701S | JD9855 (as ST77961) | GC9A01 |
| Bus | RGB565 16-bit + 3-wire SPI | SPI 4-wire 80 MHz | SPI 4-wire 80 MHz |
| esp_lcd driver | `esp_lcd_st7701` | **hand-written** | `esp_lcd_gc9a01` |
| Backlight | 38 | 46, LEDC | 46, LEDC |
| Touch | CST826 @0x15 | CST816T/D @0x15 | CST816D @0x15 |
| Touch SDA/SCL | 17 / 18 | 6 / 7 | 6 / 7 |
| Touch INT/RST | **none** | 5 / 13 | 5 / 13 |
| Encoder A/B/btn | 13 / 10 / 14 | 45 / 42 / 41 | 45 / 42 / 41 |
| Encoder pull-ups | internal | external | external |
| Counts per detent | 4 (assumed) | 2 (assumed) | 2 (datasheet) |
| Mandatory rails | — | GPIO1, GPIO2 HIGH | GPIO1, GPIO2 HIGH |
| RGB LED ring | none | 8 on 48 (+en 17) | 5 on 48 |
| Battery | none | GPIO18 (ADC2) | none |

**No vendor ESP-IDF example exists for any of the three boards.** All vendor
material is Arduino (LovyanGFX or Arduino_GFX), plus ESPHome YAML for the
1.28". Everything in `components/capstan_board/` is a port.
