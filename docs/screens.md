# Screens

Derived from the design prototype in
[`DOCS/TrailCurrent rotary display prototype/`](../DOCS/TrailCurrent%20rotary%20display%20prototype).
Open `Rotary Dial.dc.html` in a browser — it is the source of truth for
layout, copy and behaviour, and its `class Component` holds the state,
thresholds and navigation rules.

Every screen obeys the input rules in
[architecture.md](architecture.md#2-the-ring-reports-direction-never-position):
the ring reports direction only, clamping never accumulates overshoot, and
touch is off unless that screen's row in the `ui_nav.c` policy table says
otherwise. Today it is off everywhere, and the touch controller is not
started at all (`CONFIG_CAPSTAN_TOUCH_ENABLED`, off by default).

Where Capstan deliberately differs from the prototype is listed in
[Departures from the prototype](#departures-from-the-prototype) at the end.

## Input model

| Gesture | Effect |
|---|---|
| Rotate | Move selection or adjust a value. One detent, one step. |
| Press | Confirm / open / toggle. |
| Long press (≥ `CONFIG_CAPSTAN_LONG_PRESS_MS`, default 700 ms) | Back, from every screen -- the only way back; there is no on-screen Back control. Consumed by the driver, never reaches LVGL. |
| Touch | Off. Nothing on any screen is touchable. |

Any input wakes the display from idle and resets the inactivity timer. The
first input after waking is consumed by the wake itself.

## Screen inventory

Every screen is ring-only. "Back" is where a long press goes.

| Screen | Back goes to | Data source |
|---|---|---|
| Idle clock | — | `local/gps/time`, `os/timezone/current` |
| Menu | Idle | — |
| Climate | Menu | inside temperature only; setpoint and mode **stubbed** in RAM |
| Climate mode | Climate | — |
| Devices | Menu | the controls Headwaters assigned to this dial |
| Energy | Menu | `local/energy/status` |
| Water | Menu | `local/water/status` |
| Air quality | Menu | `local/airquality/*` |
| Levelling | Menu | `local/level/*` |
| Settings | Menu | NVS, Wi-Fi and MQTT state |
| Setup | Settings | phone portal (first boot and after a factory reset) |
| Locale | Settings | NVS |
| Getting Started | Menu (after stepping back to step 1) | — |
| Alert overlay | previous screen | derived on-device from Picket / Spoor inputs |

---

## Idle

Four clock faces from the newer prototype (`DOCS/GettingStarted/`), one
showing — **Settings > Clock Face** picks it, saved to NVS:

| Face | What it shows |
|---|---|
| Classic | 60 ticks, hour / minute / second hands (the second hand and cap in brand green), day and date, "Inside 67°", the climate line, battery and fresh-water % |
| Digital | a 60-tick seconds ring whose elapsed ticks are a green Scale **section**, the date, a 150 px time with AM/PM, inside temperature and the climate line |
| TrailCurrent | the TrailCurrent logo (Marketing's `trailcurrent-icon.svg`, embedded as a bitmap), a minute arc from 12 o'clock with a dot at its end (the arc's knob), the time, date, inside temperature and the climate line |
| Climate Ring | the twelve hour numerals (the current one larger), a minute arc, and the inside temperature large in the middle with the time and the climate line |

The climate line is the Climate screen's state — "Heating to 72°",
"Cooling to 72°", "Holding 72°", "Climate off" — in its readable colour. All
four faces are authored on the Idle page (Classic visible) and `main/ui_clock.c`
shows the chosen one and repaints it once a second. Time is 12-hour with
AM/PM as designed, or 24-hour per Settings > Locale. Without a time fix
every field reads `--`.

The picker is its own screen: a 60 % live preview of the face (the same face
built again and drawn through `transform_scale`, the design's own note), its
name, "Current face" or "Press to set", and four dots. Turn to preview (it
wraps), press to set, hold to leave.

Departures from that design: the tick rings and minute arcs sit 12 / 11 / 5
px from the glass on the 2.1" / 1.46" / 1.28" (2.5 / 3 / 2 %) rather than
the design's 18 px, which read as an odd margin on the hardware; no openings
count on Classic (Doors is removed);
the TrailCurrent face's time uses the 120 px numeral face rather than a new
112 px one; weights other than the design's medium are the nearest existing
regular sizes.

Shown after the Clock Timeout set in Settings (`CONFIG_CAPSTAN_IDLE_TIMEOUT_S`,
30 s, is the default; "Never" turns it off). Any input wakes it to the menu.
The prototype's `wakeScreen` option (wake straight to Climate) is not
implemented.

Driven by `capstan_board_ms_since_input()`, not LVGL's inactivity timer,
which the second hand's own redraws would otherwise keep resetting forever.

## Menu

Eight faces plus Clock: Climate, Devices, Energy, Water Tanks, Air Quality,
Levelling, Getting Started, Settings. (The prototype also has Diesel Heater and Doors faces.
Capstan has one thermostat control -- how heating and cooling are carried out
is handled outside it -- and Doors is removed for now: nothing feeds it, and
open inputs already raise the full-screen alert.) Each shows a one-line
summary — `Heating · 72°`, `3 on`, `Fresh 72%`, `82% · 14h 20m`.

**Wraps, like the prototype.** This screen is the one exception to the
clamping rule — the carousel shows its two neighbouring items and a row of
dots, so the wrap is visible rather than silent. The overshoot is still never
stored: two turns past the end then one detent back moves by exactly one. See
[architecture.md](architecture.md#2-the-ring-reports-direction-never-position).

Its row of page dots is authored in pixels rather than percent, for the reason
described under [Devices](#devices) — in percent the gaps came out uneven and
the arc stair-stepped.

**Horizontal carousel, identical on all three panels.** The selected app sits
in a circular tile in the middle with its name and a live summary under it,
its two neighbours flank it as muted glyphs, and a row of dots across the
bottom shows position in the list. Rotating the ring moves the items through
the three fixed slots; the slots themselves never move.

Nine items: the eight apps, then **Clock**, which returns to the idle face.
Clock is last so that one detent backwards from Climate reaches it.

Nothing about this layout scales with the panel, which is the point — it shows
three items whether the list has ten or thirty, so the 240 does not need the
list it used to have. The radial ring it replaced could not fit on the 240 at
all.

**Getting back to the clock** has three routes, in order of how likely a user
is to find them: the Clock item in the carousel, the Clock Timeout (Settings),
and a long press on the ring. The long press is taught by
[Getting Started](#getting-started).

## Climate — GUI only, backend stubbed

The flagship screen, and the one with no platform support.

**There is no `local/thermostat/*` topic anywhere in Headwaters.** It is not
implemented in `mqtt.js` or `cloud-bridge.js`; the platform documentation
describes one that does not exist. The screen is built and the backend is
stubbed until the CAN bus and MQTT design lands. **No speculative schema has
been invented** — see [mqtt.md](mqtt.md#thermostat--no-topic-exists).

Behaviour, from the prototype:

- The ring adjusts the setpoint, 50–90 °F, 1° per detent (0.5° in Celsius).
  **Clamps at the ends** — and reversing moves immediately, with no wind-back.
- State is derived from setpoint vs. current, with a 0.2° deadband:
  - setpoint > current + 0.2 → **Heating**, `#FF5453`, flame
  - setpoint < current − 0.2 → **Cooling**, `#48E6FE`, snowflake
  - otherwise → **Holding**, `#52A441`, check
- Modes: Heat, Cool, Auto, Off. A press opens the mode screen. As in the
  prototype, only Off changes what the screen shows; heating vs cooling is
  decided outside Capstan.
- An ETA is shown while actively heating or cooling.
- °F/°C is a setting; the ring step follows the unit.

The only ambient temperature available today is `tempInC` / `tempInF` from
`local/airquality/temphumid`, and it is the only live value on the screen.
The setpoint and mode are held in RAM by `main/ui_climate.c` and published
nowhere: the screen is visual for now.

Layout is the prototype's face A: an 81-tick Scale (50–90 °F at 0.5°, 270°)
with the inside→target range coloured by Scale **sections** (Heating Danger,
Cooling Info, Holding AccentPrimary), rim marks for inside and target, the
mode line, a 150 px setpoint and "Inside 67°  (clock) 28 min".

## Devices

**Called Devices, not Lights.** What Headwaters assigns to a dial is a set of
switchable channels — a PDM output, a Switchback relay — and those drive
awning lights, a fan, a pump, a fridge socket. Calling the screen Lights
named one of them and misdescribed the rest.

The MQTT topics are still `local/lights/*`. That is the platform's contract,
shared with Fireside and the PWA; renaming a topic on the dial alone would
just stop it talking to the rig. See [mqtt.md](mqtt.md#commands).

**A carousel, identical on all three panels — the same shape as the menu.**
The selected device sits in a circular tile in the middle with its name and
state under it, its two neighbours flank it as muted glyphs, and a row of
dots across the bottom shows position in the list. Rotating moves the devices
through the three fixed slots; the slots never move.

This replaced a vertical scrolling list, for two reasons:

- **It was not the design.** The prototype's devices face (`lv:'focus'`, its
  default) is this carousel. The list was the alternative it offers and does
  not choose.
- **The list could not be reviewed.** Its rows were created, positioned and
  sized from C at runtime, so EEZ Studio's canvas showed an empty box where
  the devices would be — the canvas-device divergence
  [gui.md](gui.md#the-rule-the-canvas-must-match-the-device) exists to
  prevent, sitting in the tree as a documented exception. A carousel shows
  three items whatever the list length, so the exception is no longer needed:
  the only per-device content is a glyph, a name and a word, all of which are
  `lv_label_set_text`.

On/off only — **no brightness**, even though the topic carries it. The ring
selects, a press toggles, and `LV_STATE_CHECKED` on the tile and its glyph is
the on state.

**Wraps, like the menu.** It is the second exception to the clamping rule and
for the same reason: the neighbouring glyphs and the dots make the wrap
visible before the user reaches it. The overshoot is still never stored. See
[architecture.md](architecture.md#2-the-ring-reports-direction-never-position).

**The tile is not touchable.** On both CrowPanels the display *is* the encoder
button, so a tap firm enough to register would also close the ring button and
toggle the device twice. Touch is off on every screen now (there is no Back
chip; a long press goes back), so nothing on the glass reacts to a tap.

**There are no scene chips.** The previous version authored Evening, Night and
All Off. No scenes topic exists anywhere in Headwaters, so all three were
placeholders for a feature with no backend.

A toggle publishes to `local/lights/<id>/command` and the UI changes **only
when the module confirms** on `local/lights/<id>/status`. This is a
deliberate departure from the handoff's optimistic-update instruction — see
[mqtt.md](mqtt.md#optimistic-update-and-why-capstan-does-not-do-it).

Friendly names and icons come from the retained controls payload, per device.

**Unconfigured dial**: the tile sits in its off look and the two labels read
`No devices` / `Use Headwaters`. There is no separate message widget — a
round 240 px face has no free band for one, and authoring it over the tile
would make the canvas show the message and the carousel at once.

**The dots are one per device, on an arc at 6 o'clock.** They answer two
questions at a glance — how many devices this dial has, and which one you are
on — so the number shown is the device count and the lit one is the selection.
The run grows outwards from the bottom and stays centred there.

**It is exactly the menu's dot row**: same 6 o'clock centre, same 44.6%
radius, same 6° spacing, same 2.1% dot — Settings uses it too, so all three
carousels put their dots in one place. Thirty degrees
apart was tried first, because that is what "12, then 11 and 1, then 10 and 2"
implies, and it was wrong: eight dots a clock hour apart read as eight separate
marks rather than a row you can count.

The rim rather than a row under the labels, because a row is bounded by the
width of the face — enough devices and its dots either run off the glass or
shrink until they stop being countable. An arc is not bounded that way.

The widgets cannot move to keep the run centred, so the arc is
`2 * CAPSTAN_MAX_CONTROLS - 1` slots at **half** the visible spacing, and *n*
devices light every other slot starting at slot `MAX - n`. An odd count sits
one dot on 6 o'clock with the rest either side; an even count straddles it.
One slot per device can only ever centre one of those two cases. The
interstitial slots are authored hidden, so the canvas shows the eight-dot full
house — a state the device really renders.

**Both dot rows are authored in pixels, not percent.** This is the one place
in the GUI where percent is the wrong unit, and it is worth knowing why: EEZ
Studio exports positions as whole-number `LV_PCT`, so a dot authored at 30.417%
ships as `LV_PCT(30)`. On a 240 px panel one percent is 2.4 px — half the
diameter of a 5 px dot — so the menu's row came out with gaps alternating
9.6 px and 12 px, and the arc's vertical sag, a fraction of a pixel between
neighbours near the centre, collapsed into 2.4 px stair-steps. The dots were
always authored on a true circle; the unit was throwing the curve away. In
pixels the 240's full house has uniform 11 px gaps and a smooth sag of
18, 14, 12, 11, 11, 12, 14, 18. See `dot()` in `GUI/tmp/layout.py`, and
[gui.md](gui.md#percent-positions-are-exported-as-whole-numbers).

The ceiling is the MQTT buffer limit, mirrored from Headwaters' own
`MAX_CONTROLS`; the generator refuses to build a project where the two
disagree, and there is a `_Static_assert` in `ui_devices.c` for the case it
cannot see.

## Air quality

A thin 270° ring near the glass edge with a centred stack inside it, built to
the design prototype's geometry: a centreline radius of 214 px and a 14 px
rounded stroke on a 480 panel (44.6% and 2.9% of the diameter), opening 90° at
the bottom. No cards, no borders, no knob, and the ring is read-only — touch
cannot move it.

Inside, top to bottom: a small tracked `AIR QUALITY`, the hero numeral, its
unit, the status word, and three sub-metrics. The column is centred as a group
— the bands are measured rather than assumed — so it stays optically centred
on all three panels.

**The numeral's band is sized from the font, not from its point size.** The
`rn` faces are a digits-only subset, so `lv_font_conv` recomputes their line
height from the digits alone: 44 px at size 60, where a full charset would
declare about 74. Sizing that band at the usual 1.20 em reserved 30 px of
nothing on the 240 — which showed up at both ends of the stack at once, as an
obvious hole under the value and an `AIR QUALITY` pushed up far enough to
overlap the ring. The band is now the font's real line height plus 2 px, with
an explicit 20 px (at 480) gap under the numeral to replace the slack it no
longer carries, and the title sits 21 px lower on the 240, 17 px on the 360 and
20 px on the 480.

The generator **checks both ends of that**: it diffs the line heights against
the exported `ui_font_rn*.c` when an export exists, and it refuses to write a
project in which the eyebrow's box crosses the ring's inner edge. The overlap
was found by eye on hardware; it will not need to be found that way again.

**The hero is eCO2 in ppm, not an AQI.** The prototype shows an Air Quality
Index on a 0–300 scale; there is no AQI on this bus. Borealis publishes eCO2,
TVOC, temperature, humidity and CO, plus its own threshold verdicts on
`local/airquality/safety` — which [mqtt.md](mqtt.md) says to use rather than
re-derive. Computing an index here would have meant inventing a number no
sensor reports and second-guessing thresholds the module has already
evaluated. So the ring spans 400–2000 ppm (outdoor air to full) and the status
word comes from the flags:

| Status | Ring | Word | From |
|---|---|---|---|
| Good | `AccentPrimary` | `AccentText` | nothing flagged |
| Moderate | `Solar` | `SolarText` | `co2_warn` or `co_warn` |
| Unhealthy | `Danger` | `DangerText` | `co2_alarm`, `voc_alarm` or `co_alarm` |
| `--` | hidden | `TextMuted` | Borealis silent or stale |

The ring takes the house fill colour and the word its readable text twin
(see [Theme and formatting](#theme-and-formatting)); green is always the brand
green, never the house `Success`.

LPG is deliberately not folded in: a propane leak is a leak, not air quality,
and it already raises the alert overlay.

The ring and the status word always share one hue, and the ring animates to
a new reading over 300 ms rather than jumping. Because the ring reads eCO2
while its colour reads the verdict, the two can legitimately disagree — a VOC
alarm at 500 ppm draws a short red arc, and the status word is what names
which is which.

The three sub-metrics are **VOC / Humidity / Temp**. The prototype's third
column is PM2.5; there is no particulate sensor on this bus, so VOC takes it.
Temperature is here because `local/airquality/temphumid` is the only ambient
temperature anywhere on the rig.

All four severity looks are authored in the `.eez-project` as LVGL states, so
they are visible on EEZ Studio's canvas. `main/ui_data.c` only calls
`lv_obj_add_state` / `lv_obj_remove_state`; it sets no colours. See the note on
the `ArcThin` style in `GUI/tmp/gen_eez_project.py` for the state mapping and
why it is what it is.

## Energy, Water, Levelling

Read-only, laid out as in the prototype.

**Energy** pages between Battery / Solar Input / Loads on the ring. There is
no per-device load data, so Loads is the total, derived as solar minus the
signed battery power (`battery_watts`: + charging, − discharging). Time
remaining adapts its unit (`45 min`, `14h 20m`, `2d 4h`) and reads
`Charging` only when the battery is known to be charging.

**Water** is three upright tanks (Fresh / Grey / Black) in the prototype's
geometry: a card-coloured vessel with a rounded border, the level square-topped
inside it, the percentage above and the name below. Fresh is the house
`TankFreshLight` → `TankFreshDark` gradient; grey and black are `GreyWater`
and `BlackWater`. The prototype's glow on the fresh tank is left out, because
a glow means "on" or "selected" elsewhere.

**Levelling** moves the bubble on both axes from the two tilt readings; the
status word ("Level", "Tilted right") uses the same 24 px status style as
Air. Under it is how much higher or lower each side is —
`Side +0.8" · Front -0.3"`, or mm, per Settings > Locale — from Plateau's
`front_back_diff_mm` / `left_right_diff_mm`, signed as Headwaters and Milepost
show them. **Never degrees**: nobody levelling a trailer can turn 2° into
"raise the driver's side 2 inches" without its track and wheelbase, which
Plateau knows. `--` if the gateway sends no heights.

All of them show `--` until the first frame arrives, because **nothing on
this platform is retained** — and again once a module goes stale. Timeouts
per module are in [mqtt.md](mqtt.md#nothing-is-retained). A stale number that
looks live is worse than no number.

## Settings

A carousel, built like the app menu: the centred item in a glowing tile with
its neighbours either side, the name and current value beneath, one dot per
item on the rim at 6 o'clock. The ring wraps, as on the other carousels, and
a press acts on the centred item:

| Item | Value | Press |
|---|---|---|
| Wi-Fi | network name, or what is wrong | nothing -- status only |
| MQTT | Connected / Offline / Not set | nothing -- status only |
| Theme | Light / Dark | toggles; saved to NVS, restored at boot |
| Locale | e.g. `°F · in · 12 h` | opens the Locale screen |
| Clock Face | Classic / Digital / TrailCurrent / Climate Ring | opens the face picker |
| Getting Started | Shown / Hidden | toggles whether it appears in the app menu; saved to NVS, so a factory reset shows it again |
| Alarm Snooze | 5 / 10 / 15 / 30 / 60 min | steps to the next |
| Clock Timeout | 15 s / 30 s / 1 / 2 / 5 min / Never | steps to the next |
| Factory Reset | Press twice | first press arms (tile turns red), second within 5 s resets |

Wi-Fi and MQTT values are green when connected and amber when not; on the
boards with LEDs the ring shows the same, green or orange, while either is
centred.

Wi-Fi and MQTT are set up from a phone, not on the dial; the way back into
setup is a factory reset.

Clock Timeout is how long an app stays up without input before the dial
returns to the clock face. Setup and an open alarm are never timed out.

### Locale

Units, on a short list opened from Settings (a list rather than a carousel:
with two items a carousel would show the same item on both sides). The ring
selects, a press flips the selected row, a long press returns to Settings.
Both are saved to NVS and apply everywhere on the next refresh.

| Row | Values | Used by |
|---|---|---|
| Temperature | °F / °C | Climate, the Air temperature, menu summaries |
| Leveling | in / mm | the Level screen's height differences and its menu summary |
| Clock | 12 h / 24 h | every clock face: "7:42 PM" or "19:42" (no AM/PM) |

Metric heights default on with metric temperatures (`CONFIG_CAPSTAN_DEFAULT_UNITS_CELSIUS`).
More units (distance, 12/24 h) belong here as further rows.

### Credentials and reset

Everything is stored in NVS. Factory reset clears the NVS namespace and
reboots -- it is not recoverable and it drops the device off the network,
which is why it is last and needs a second press.

## Getting Started

Six steps that teach the ring, now that nothing on the glass is touchable:
what controls the dial, turn, press, hold to go back, the idle clock, and
Ready. From the newer prototype (`DOCS/GettingStarted/`) — the only thing
taken from that download. "STEP n OF 6" at the top; an accent circle with the
step's icon, the title, the body and a green hint in a centred column; six
progress dots at 6 o'clock, completed steps green.

Turn or press to step forward (turn also goes back); a hold steps back one,
and on the first step or Ready leaves to the menu. It always opens at step 1.
Once someone has been through it, Settings > Getting Started can hide it from
the menu; the carousel then skips it and its dot, and the remaining dots stay
centred (the menu uses the devices carousel's half-pitch dot slots).

The column is centred between the STEP heading and the dots, and the 1.28"
draws the step's circle at 48 px rather than a scaled 56, so the longest step
fits without reaching the heading.

Adapted from the design: the copy names what is actually pressed — **the
screen** on the Elecrow CrowPanels (1.28", 1.46"), where the display is the
button, and **the ring** on the MaTouch 2.1" — rather than "the ring or
screen", which also shortens it; the hold step does not mention an edge
filling green (there is no hold-progress feedback); the idle step reads the Clock Timeout
setting, including Never; the icons are the nearest in the house set (the
design's hand, circle-dot, undo arrow and check-circle are not in it); the
dots are at the bottom like every carousel here. The menu summary "Rotate,
press, hold" is Capstan's own — the design gives none.

## Setup

Shown at first boot and after a factory reset, and nowhere else: it is not
reachable from Settings, because it has no way back (on a new device there is
nowhere to go back to), and an accidental entry with Clock Timeout at Never
would strand the dial. The screen shows the setup network's name and password
and a URL; everything else happens in the phone's browser, which sets Wi-Fi and
the MQTT broker. There is no on-device keyboard — typing a WPA2 passphrase with
a ring was built, tried on the bench, and did not work on a panel this size.

## Alert overlay

Full screen over whatever is showing, with a red ring on the glass edge. It
appears when an alarm configured in Headwaters becomes active and stays until
it is acknowledged or its input clears. A press (or a long press) acknowledges
it: the overlay closes and that alarm is snoozed for the Alarm Snooze interval
in Settings, after which it returns if still active. A snooze timer is
cancelled the moment its input clears, so an alarm that clears and re-opens is
raised fresh rather than suppressed.

**Derived on-device.** There is no MQTT alert topic — Headwaters' alarm
service emits over WebSocket only. Capstan applies its own copy of the alarm
configuration (retained `local/config/panel/<host>/alarms`, saved to NVS on
every update) to the raw Picket and Spoor input bitmasks, which also keeps
alerts working when the backend is down.

## LED ring

The 1.46" and 1.28" boards have an RGB LED ring; the 2.1" does not. One
function (`apply_leds()` in `main/ui_alerts.c`) decides its colour, in this
order, so features cannot fight over it:

| Condition | Colour |
|---|---|
| any alarm active (snoozed or not) | red |
| Climate | left half blue, right half red, always — which way to turn for cooler or warmer; the bottom LED(s) dark |
| Devices: the centred device is on | brand green |
| Energy, Battery page | green > 75 %, yellow 40–75 %, red < 40 %, dimmer toward the bottom of each band |
| Water | orange if fresh < 40 % or grey/black > 60 %; green if fresh > 40 % and grey/black < 50 %; in between, unchanged |
| Settings: Wi-Fi or MQTT centred | green connected, orange not |
| anything else, including the Menu | off |

Brightness is capped by `CONFIG_CAPSTAN_RGB_LEDS_MAX_BRIGHTNESS`.

## Theme and formatting

Light by default; Theme in Settings switches to dark, saved to NVS and applied
at boot. Every colour in the project is a named token from
`GUI/tmp/palette.py`: Milepost's house palette verbatim, plus four readable
text variants (`AccentText`, `SolarText`, `InfoText`, `DangerText`) for accent
colours used as text, which the house fills fail to meet contrast for on the
light theme. The design's hex values are read as intent, not copied — its
bright green `#74FE00` is the brand green `AccentPrimary`.

Roboto (Light / Regular / Medium) throughout, with digits-only subsets for the
large numerals. Icons are the FontAwesome subset shared with Milepost, plus
`server` ([icons.md](icons.md)).

Empty values are `--`. Units are written as in the prototype: `72°F`,
`13.4V`, `850 W`.

Keep everything inside the circular safe area — nothing interactive in the
corners of a square frame. The generator checks every widget against the
round mask.

## Departures from the prototype

Deliberate, and kept deliberately. Check here before "fixing" one of them to
match the design.

| Prototype | Capstan | Why |
|---|---|---|
| Diesel Heater face, heating/cooling faces | one **Climate** screen | Capstan is one climate control; how heat or cooling is produced is handled outside it |
| Doors face | removed for now | nothing feeds it; open inputs already raise the alert overlay |
| one analog clock face | four faces, chosen in Settings | the newer prototype's clock faces (see [Idle](#idle)) |
| Air Quality Index, 0–300 | eCO2 in ppm, 400–2000, status from Borealis's flags | no AQI exists on the bus; see [Air quality](#air-quality) |
| PM2.5 column | VOC | no particulate sensor |
| per-device loads on Energy | total load (solar − battery) | nothing measures per-device loads |
| bright green `#74FE00`, accent-coloured text | brand green, `*Text` tokens | brand consistency and legibility on the light theme |
| glow on the fresh tank | no glow | glow means "on" (Devices) or "selected" (carousels) |
| Devices face with arc and scene chips | carousel of assigned controls; glow only while the device is on | the tile is a control, so its glow means on |
| Settings list with rows | Settings **carousel**, wraps like the menu | consistency with the other carousels |
| tilt in degrees on Leveling ("Side 1.2° · Front 0.4°") | height differences in in / mm | what a person actually does is raise a side by a height |
| hand-built keyboard, on-device Wi-Fi / MQTT editors | phone setup portal | ring text entry does not work on these panels |
| back affordances on screen | none; **long press** goes back everywhere | clutter; [Getting Started](#getting-started) teaches it |
| touch | off, controller not started | nothing needs it; the whole glass is the ring's button |
| `wakeScreen` (wake to Climate) | always wakes to the menu | not implemented |
| Alert on the clock face | always full-screen overlay | preferred full-screen |
| Devices dots at 12 o'clock | dots at 6 o'clock on every carousel | one place for all three |
