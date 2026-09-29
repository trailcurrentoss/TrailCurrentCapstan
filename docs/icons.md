# Icons

Icons are **FontAwesome 6 Solid**, from the same file and the same subset the
other TrailCurrent modules use, so a glyph means the same thing on every
panel in the vehicle.

## Why FontAwesome and not Ionicons

The design handoff specifies Ionicons. This deliberately does not follow it.

Milepost and Fireside both ship FontAwesome Solid, from an identical
`fa-solid-900.otf` with an identical 111-codepoint subset. Adopting Ionicons
would have meant every shared concept — water, battery, Wi-Fi, settings —
rendering as a subtly different glyph from the panel mounted next to it.
Consistency across modules beats consistency with the mock-up.

## The font file

`GUI/ASSETS/fa-solid-900.otf`, extracted from Milepost's own embedded base64
and verified **byte-identical by SHA256** against independent copies
elsewhere in the tree. Not a fresh download — a different FontAwesome release
can move codepoints, which would silently change glyphs.

Subset to **111 codepoints — exactly Milepost's set**, no additions. The
full face is roughly 2000 glyphs and most of the flash cost is glyph data.

Milepost and Fireside are not quite identical to each other: Milepost has
two codepoints Fireside lacks, `temperature-full` (`U+F2C7`) and `wind`
(`U+F72E`), for its air-quality readouts. Capstan follows Milepost, the
superset, so it contains everything either project uses.

Sizes follow the house `fa<size>` convention. The row sizes are
`fa24`/`fa18`/`fa14` on the 480, `fa20`/`fa16`/`fa13` on the 360 and
`fa16`/`fa13` on the 240, plus two larger ones per panel for the **devices
carousel** — `fa56`/`fa36`, `fa42`/`fa28` and `fa30`/`fa20`.

Those two are the FULL 111-codepoint face rather than the app carousel's
reduced `fh` subset, and that is deliberate: the device tile draws whichever
icon the user picked in Headwaters, which is any key in
`main/ui_light_icons.h`. Subsetting it to the ten app glyphs would render an
empty box for every other choice, with nothing to warn about it. It costs
roughly 240 KB of flash on the 480 — only the selected board's variant is
compiled — and that is the price of "every icon a user can choose renders".

## Inherited meanings

These carry the meaning a **named widget** in Milepost or Fireside already
gives them. Same concept, same glyph, across modules.

| Concept | Glyph | Codepoint | Precedent |
|---|---|---|---|
| Settings | `gear` | `U+F013` | `nav_settings_icon` |
| Energy | `bolt` | `U+F0E7` | `nav_power_icon`, `power_volts_icon` |
| Battery | `battery-half` | `U+F242` | `topbar_battery_icon`, `power_soc_icon` |
| Solar | `sun` | `U+F185` | `power_solar_icon`, `home_pwr_solar_icon` |
| Water tanks | `droplet` | `U+F043` | `nav_water_icon`, `water_pump_icon` |
| Air quality | `cloud` | `U+F0C2` | `nav_air_icon`, `air_eco2_icon` |
| Climate / temperature | `temperature-full` | `U+F2C7` | `air_temp_icon` |
| Devices | `lightbulb` | `U+F0EB` | `home_dev4_icon` |
| Wi-Fi | `wifi` | `U+F1EB` | `topbar_wifi_icon` |
| Theme toggle | `moon` | `U+F186` | `topbar_theme_icon` |

## From the shared subset, no direct precedent

Already in the house subset, so they render identically anywhere — they just
have no named sibling widget to inherit a meaning from.

| Concept | Glyph | Codepoint |
|---|---|---|
| Heating call, heater | `fire` | `U+F06D` |
| Cooling call | `snowflake` | `U+F2DC` |
| Levelling | `gauge-high` | `U+F3FD` |
| Doors | `lock` | `U+F023` |
| Alert | `triangle-exclamation` | `U+F071` |
| Confirm / OK | `check` | `U+F00C` |
| Cancel | `xmark` | `U+F00D` |
| Back | `chevron-left` | `U+F053` |
| Scene: all off | `power-off` | `U+F011` |
| Password reveal | `eye` | `U+F06E` |
| Idle clock, alarm snooze | `clock` | `U+F017` |
| Alarm (default icon) | `bell` | `U+F0F3` |

## No Capstan additions

The set is exactly Milepost's. An earlier draft of this file added four
codepoints — `arrow-up`, `delete-left`, `eye-slash`, `server` — on the
assumption that the siblings had no on-screen keyboard or MQTT settings
screen. **That assumption was wrong.** Fireside has `PageMqttSetup` with
host, username and password fields; both have Wi-Fi password entry.

Checking how they actually solve it showed why none of the four is needed:

| Need | How the siblings do it |
|---|---|
| Shift, backspace | `LVGLKeyboardWidget` — LVGL's built-in keyboard, which draws its own symbols from LVGL's symbol font, not FontAwesome |
| Hide/show password | a text button labelled **"Show"** (`wifi_pw_show_btn_lbl`), not an eye-slash icon |
| MQTT broker | `PageMqttSetup` carries **no icon at all** — text labels and text buttons throughout |

Before adding a codepoint, look at how the siblings solve the same problem.
Often the answer is that they do not use an icon.

## Adding an icon

1. Check whether Milepost or Fireside already uses one for that concept. If
   so, use **their** codepoint — that is the whole point.
2. Add the codepoint to `FA_RANGE` in `GUI/tmp/fonts.py`, with a trailing
   comment naming the glyph and what it is for.
3. Re-run `python3 GUI/tmp/gen_eez_project.py`.
4. Re-export from EEZ Studio so the glyph lands in `ui_font_fa*.c`.

**A codepoint not in the subset renders as an empty box** — no build warning,
nothing in EEZ Studio's Check panel. It simply does not draw. If an icon is
missing on device, that is the first thing to check.

## Two traps

**The PUA codepoint, not the emoji one.** FontAwesome maps many glyphs to
*both* a Private Use Area codepoint and a real Unicode emoji point — `fire`
is at `U+F06D` **and** `U+1F525`. Only the PUA one is subset and only the PUA
one is what every TrailCurrent project uses. A naive
`{name: codepoint}` reverse lookup over the font's cmap returns whichever came
last and will hand you the emoji point, which renders as nothing.

**Glyph names come from the font, not from memory.** The names above were
read out of `fa-solid-900.otf` with `fontTools`, not recalled. FontAwesome
renames glyphs between major versions (`tint` became `droplet`,
`exchange-alt` became `arrow-right-arrow-left`), so a name from an older
cheat sheet may not match this file.

```python
from fontTools.ttLib import TTFont
cmap = TTFont('GUI/ASSETS/fa-solid-900.otf').getBestCmap()
print(cmap[0xf06d])          # -> fire
```
