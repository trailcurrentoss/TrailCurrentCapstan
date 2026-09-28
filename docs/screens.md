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
otherwise.

## Input model

| Gesture | Effect |
|---|---|
| Rotate | Move selection or adjust a value. One detent, one step. |
| Press | Confirm / open / toggle. |
| Long press (≥ `CONFIG_CAPSTAN_LONG_PRESS_MS`, default 700 ms) | Back. Consumed by the driver, never reaches LVGL. |
| Touch | Only where the policy table allows — today, the keyboard. |

Any input wakes the display from idle and resets the inactivity timer. The
first input after waking is consumed by the wake itself.

## Screen inventory

| Screen | Input | Back goes to | Data source |
|---|---|---|---|
| Idle clock | ring | — | RTC, `local/gps/time`, `os/timezone/current` |
| Menu | ring | Idle | — |
| Climate | ring | Menu | **none — stubbed** |
| Climate mode | ring | Climate | **none — stubbed** |
| Lights | ring | Menu | `local/lights/+/status` |
| Heater | ring | Menu | a PDM/relay channel |
| Energy | ring | Menu | `local/energy/status` |
| Water | ring | Menu | `local/water/status` |
| Air quality | ring | Menu | `local/airquality/*` |
| Levelling | ring | Menu | `local/level/*` |
| Doors | ring | Menu | `local/picket/+/inputs` |
| Settings | ring | Menu | NVS |
| Wi-Fi list | ring | Settings | `esp_wifi_scan_get_ap_records` |
| Wi-Fi security | ring | Wi-Fi list | — |
| MQTT | ring | Settings | NVS |
| Keyboard | **ring + touch** | invoking field | — |
| Alert overlay | ring | previous | derived on-device |

---

## Idle

Analog clock. `lv_scale` in `ROUND_INNER` mode, 60 ticks with 12 major, plus
hour, minute and second needles drawn as lines. The second hand steps once
per second on a 1 s `lv_timer`.

The date sits **high on the face** so the hands do not cover it.

The EEZ Studio export draws the needles as fixed two-point `lv_line`s and the
date as literal text — a static export cannot move a needle — so the face is
driven from `main/ui_clock.c`, which rewrites all three point arrays and the
date label once a second while the idle screen is showing, and once more on
entry so returning to it never shows a stale position.

There is no RTC and no SNTP client. The clock is set from `local/gps/time`
(UTC calendar fields from Milepost's fix) and rendered in the zone from the
retained `os/timezone/current`; both are parsed in `capstan_mqtt` and applied
in `capstan_model`, which owns the system clock and the IANA → POSIX `TZ`
translation. Until a fix arrives the hands park at 12:00 and the date reads
`--`, for the same reason every other unknown reading does.

Shown after `CONFIG_CAPSTAN_IDLE_TIMEOUT_S` of no input. Any input wakes it —
to the menu normally, or straight to Climate if the device is configured as a
thermostat-first dial (the prototype's `wakeScreen`).

Driven by `capstan_board_ms_since_input()`, not LVGL's inactivity timer,
which the second hand's own redraws would otherwise keep resetting forever.

## Menu

Nine faces: Climate, Lights, Heater, Energy, Water Tanks, Air Quality,
Levelling, Doors, Settings. Each shows a one-line summary — `Heating · 72°`,
`3 on`, `Fresh 72%`, `2 open`.

**Wraps, like the prototype.** This screen is the one exception to the
clamping rule — the carousel shows its two neighbouring items and a row of
dots, so the wrap is visible rather than silent. The overshoot is still never
stored: two turns past the end then one detent back moves by exactly one. See
[architecture.md](architecture.md#2-the-ring-reports-direction-never-position).

**Horizontal carousel, identical on all three panels.** The selected app sits
in a circular tile in the middle with its name and a live summary under it,
its two neighbours flank it as muted glyphs, and a row of dots across the
bottom shows position in the list. Rotating the ring moves the items through
the three fixed slots; the slots themselves never move.

Ten items: the nine apps, then **Clock**, which returns to the idle face.
Clock is last so that one detent backwards from Climate reaches it.

Nothing about this layout scales with the panel, which is the point — it shows
three items whether the list has ten or thirty, so the 240 does not need the
list it used to have. The radial ring it replaced could not fit on the 240 at
all.

**Getting back to the clock** has three routes, in order of how likely a user
is to find them: the Clock item in the carousel, the idle timeout
(`CONFIG_CAPSTAN_IDLE_TIMEOUT_S`, 30 s by default), and a long press on the
ring. Only the last of those existed at first, and nothing on the display
mentioned it.

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
- Modes: Heat, Cool, Auto, Off. A press opens the mode screen.
- An ETA is shown while actively heating or cooling.
- °F/°C is a setting; the ring step follows the unit.

The only ambient temperature available today is `tempInC` / `tempInF` from
`local/airquality/temphumid`.

## Lights

On/off only — **no brightness**, even though the topic carries it. The ring
selects, a press toggles, `LV_STATE_CHECKED` is the on state.

Below the lights are scene chips: Evening, Night, All Off. Selection runs
through lights and then scenes as one list, clamping at both ends.

A toggle publishes to `local/lights/<id>/command` and the UI changes **only
when the module confirms** on `local/lights/<id>/status`. This is a
deliberate departure from the handoff's optimistic-update instruction — see
[mqtt.md](mqtt.md#optimistic-update-and-why-capstan-does-not-do-it).

Friendly names and icons come from the retained `local/config/pdm_channels`.

## Heater

Level 0–5, ring-adjusted, press toggles between off and the last non-zero
level. Rendered as an arc of five segments.

Not a distinct platform entity — a heater is a PDM or Switchback channel with
a flame icon, commanded like any other light.

## Air quality

A thin 270° ring near the glass edge with a centred stack inside it, built to
the design prototype's geometry: a centreline radius of 214 px and a 14 px
rounded stroke on a 480 panel (44.6% and 2.9% of the diameter), opening 90° at
the bottom. No cards, no borders, no knob, and the ring is read-only — touch
cannot move it.

Inside, top to bottom: a small tracked `AIR QUALITY`, the hero numeral, its
unit, the status word, and three sub-metrics.

**The hero is eCO2 in ppm, not an AQI.** The prototype shows an Air Quality
Index on a 0–300 scale; there is no AQI on this bus. Borealis publishes eCO2,
TVOC, temperature, humidity and CO, plus its own threshold verdicts on
`local/airquality/safety` — which [mqtt.md](mqtt.md) says to use rather than
re-derive. Computing an index here would have meant inventing a number no
sensor reports and second-guessing thresholds the module has already
evaluated. So the ring spans 400–2000 ppm (outdoor air to full) and the status
word comes from the flags:

| Status | Colour | From |
|---|---|---|
| Good | `Success` | nothing flagged |
| Moderate | `Solar` | `co2_warn` or `co_warn` |
| Unhealthy | `Danger` | `co2_alarm`, `voc_alarm` or `co_alarm` |
| `--` | `TextMuted` | Borealis silent or stale |

LPG is deliberately not folded in: a propane leak is a leak, not air quality,
and it already raises the alert overlay.

The ring and the status word always share one colour, and the ring animates to
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

## Energy, Water, Levelling, Doors

Read-only, laid out as in the prototype. On Energy the ring pages between
Battery / Solar / Load.

All of them show `--` until the first frame arrives, because **nothing on
this platform is retained** — and again once a module goes stale. Timeouts
per module are in [mqtt.md](mqtt.md#nothing-is-retained). A stale number that
looks live is worse than no number.

Doors are reed switches on Picket, arriving as a 12-bit mask in
`local/picket/<addr>/inputs`. Labels live in Headwaters' Mongo, so the device
carries its own until a config topic exposes them.

## Settings

Two rows: Wi-Fi and MQTT Server, each showing current status.

### Wi-Fi

1. Scan (`esp_wifi_scan_get_ap_records`), list SSIDs with signal strength.
2. Select a network.
3. **Choose the security type explicitly** — the prototype infers it from the
   scan record; Capstan asks, so a hidden or mis-reported network can still
   be joined.
4. Enter the passphrase on the keyboard.
5. Connect, showing progress and a clear failure reason.

### MQTT

Host/IP, port, username, password, and Save. Port defaults to **8883** — the
Headwaters broker is TLS-only and has no plaintext listener, contrary to the
prototype's `1883` default. Passwords are masked, with a reveal toggle.

### Credentials and reset

Everything is stored in NVS. Settings carries a **factory reset** that clears
the NVS namespace and reboots, with a confirmation step — it is not
recoverable and it drops the device off the network.

## Keyboard — the only touch screen

Uses LVGL's built-in **`LVGLKeyboardWidget`**, not a custom key grid.

**This departs from the design prototype**, which specifies a hand-built
keyboard with 34x44 px keys. The built-in widget reproduces that mock
exactly while being the same widget Fireside already uses on three screens
(`wifi_setup_kb`, `mqtt_setup_kb`, `kb_button_edit`) and Milepost on two.

Everything in the mock maps onto it:

| Mock element | LVGL |
|---|---|
| the QWERTY rows | a custom map via `lv_keyboard_set_map()` |
| shift arrow | key text `"ABC"` — LVGL switches to `TEXT_UPPER` itself |
| backspace | `LV_SYMBOL_BACKSPACE` — deletes from the bound textarea |
| `123` | key text `"1#"` — switches to the special/number map |
| wide space bar | `LV_KB_BTN(n)` width units in the ctrl map |
| cancel | `LV_SYMBOL_CLOSE` — emits `LV_EVENT_CANCEL` |
| green OK | `LV_SYMBOL_OK` — emits `LV_EVENT_READY`; the green comes from `LV_BUTTONMATRIX_CTRL_CHECKED` on that key plus a `CHECKED` style on the `ITEMS` part, which is exactly how LVGL's own default map marks control keys |

Key dispatch is a string compare on the key's text, so a custom map keeps
the built-in behaviour as long as the control keys carry the expected
strings.

Three things this buys beyond matching the siblings: it auto-sizes to its
container, so the 240x240 panel stops being a problem (a fixed 10-key row at
34 px needs ~340 px and does not fit); special characters, number mode and
cursor handling come for free; and it is a first-class EEZ Studio widget, so
the canvas shows what the device shows.

The **eye reveal** is not part of the keyboard — it is a separate widget over
the textarea. `eye` (`U+F06E`) is already in the shared icon set. Note the
siblings use a plain text button labelled **"Show"** for the same job; either
is consistent, the icon is closer to the mock.

**The one trap:** `lv_keyboard` re-runs its own internal layout and ignores
authored `left/top/width/height` unless the widget's `localStyles` pin
`align: TOP_LEFT` plus matching `min_width`/`max_width`/`min_height`/
`max_height`. Miss that and it renders as a single row at the bottom of the
screen. The fix belongs in the `.eez-project`, not in C — a C-side
`lv_obj_set_size()` would fix the device and leave EEZ Studio's canvas
showing something different.

**The only screen with touch enabled.** Typing a WPA2 passphrase by rotating
to each character is not a real option. Everywhere else touch is off,
because the ring press and a screen touch are the same physical action on
this hardware and would fight — see
[architecture.md](architecture.md#3-touch-is-a-per-screen-policy-declared-in-one-table).

## Alert overlay

Full-screen `lv_obj` over whatever is showing. A press dismisses it; touch
does not, so a stray contact cannot clear an alarm the user has not read.

**Derived on-device.** There is no MQTT alert topic — Headwaters' alarm
service emits over WebSocket only. Capstan mirrors Fireside's `main/alarms.c`
and applies its own armed/label configuration to the raw Picket and Spoor
bitmasks, which also keeps alerts working when the backend is down.

## Theme and formatting

Light theme by default, dark theme toggle, both built as LVGL styles from the
tokens in
[`colors_and_type.css`](../DOCS/TrailCurrent%20rotary%20display%20prototype/colors_and_type.css)
and switchable at runtime without rebuilding screens.

System sans throughout — no serif, no display faces, no emoji. Icons are
Ionicons converted to an LVGL icon font ([icons.md](icons.md)).

Empty values are `--`. Units are written as in the prototype: `72°F`,
`13.4V`, `850 W`.

Keep everything inside the circular safe area — nothing interactive in the
corners of a square frame.
