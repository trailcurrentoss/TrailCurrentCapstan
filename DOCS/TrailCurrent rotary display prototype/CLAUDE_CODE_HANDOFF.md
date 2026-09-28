# Claude Code prompt — TrailCurrent rotary display firmware (EEZ Studio + LVGL 9)

Paste everything below the line into Claude Code, from the root of the firmware repo, with this zip extracted into `design/`.

---

You are implementing firmware for the **TrailCurrent round rotary display**: a 480×480 round touchscreen inside a rotatable, pressable brushed-aluminum ring. It is a wall-mounted controller for an RV/trailer on the TrailCurrent platform (ESP32 modules, CAN bus, MQTT via the Headwaters gateway).

## Reference material
- `design/Rotary Dial.dc.html` — the interactive prototype and the source of truth for layout, behavior, and copy. Open it in a browser. The side panel lists which LVGL 9 widgets build each screen; follow those choices unless there is a concrete reason not to.
- `design/colors_and_type.css` — TrailCurrent design tokens (colors, type scale). Use these hex values for LVGL styles.
- Read the prototype's logic class (`class Component`) for state, thresholds, and navigation rules before writing code.

## Stack
- **EEZ Studio** (LVGL project type) for screens, styles, fonts, and images. Generate UI code into `src/ui/` and never hand-edit generated files; put logic in separate files.
- **LVGL 9.x**, **ESP-IDF** (PlatformIO acceptable if the repo already uses it).
- Hardware: **[FILL IN: MCU (e.g. ESP32-S3 + PSRAM), display driver IC and bus (e.g. ST7701 RGB / GC9A01 SPI), touch controller (e.g. CST816 / GT911), encoder pins, push-button pin]**.

## Inputs
- Ring rotation → LVGL encoder indev (`LV_INDEV_TYPE_ENCODER`), `enc_diff` per detent. Debounce in the driver.
- Ring press → encoder `LV_KEY_ENTER`. Long press → back to the menu (match prototype).
- Touch → pointer indev. A tap on the screen confirms, the same as a ring press.
- Put every focusable widget in one `lv_group_t` per screen and switch the group when changing screens.

## Screens (match the prototype)
1. **Idle**: analog clock. `lv_scale` ROUND_INNER, 60 ticks with 12 major, hour/minute/second line needles. The second hand steps once per second on a 1 s `lv_timer`. The date sits high on the face so the hands don't cover it. Show idle after an inactivity timeout; any input wakes the display.
2. **Menu/home**: ring rotation selects, press or tap opens.
3. **Thermostat**: the ring adjusts the setpoint. Heating (red `#FF5453`, flame) when setpoint > current; cooling (cyan `#48E6FE`, snowflake) when setpoint < current; holding (green `#52A441`) when within 0.2°. °F/°C setting.
4. **Lights**: on/off only, no brightness. Ring selects, press toggles. Use `LV_STATE_CHECKED` for on.
5. **Heater**, 6. **Energy**, 7. **Water tanks**, 8. **Air quality**, 9. **Leveling / doors**: read-only status as laid out in the prototype.
10. **Settings**: Wi-Fi (scan with `esp_wifi_scan_get_ap_records`, pick SSID, enter password) and MQTT (host/IP, port, username, password, Save). Use the custom on-screen keyboard from the prototype. Store credentials in NVS. Mask passwords.
- **Alert overlay**: full-screen `lv_obj` overlay. Press or tap dismisses.

## Theme
- Light theme by default with a dark theme toggle. Build both as LVGL styles from the token hex values and switch at runtime without rebuilding screens.
- System sans look, with no serif or display faces. Icons are Ionicons converted to an LVGL icon font. No emoji.
- Empty values show `--`. Units are written as in the prototype (`72°F`, `13.4V`, `850 W`).

## Data (MQTT)
- Put all live values behind a small data layer (`src/model/`) that the UI observes, using LVGL 9 `lv_subject_t` / observers or EEZ Flow variables. The UI must not touch MQTT directly.
- Subscribe to the TrailCurrent topics for thermostat, lights, heater, energy, tanks, air quality, leveling, and doors. **[FILL IN: topic list and payload schemas from Headwaters/Node-RED]**. If they aren't provided, define them in `docs/mqtt.md` and stub them.
- Light toggles and setpoint changes publish commands. Update the UI optimistically and roll back if no retained state confirms within a timeout.
- The prototype's weather, energy, and tank values are placeholders. Keep a mock data source behind a build flag for bench testing.

## Known firmware work
- Large numerals (~160px) and lighter weights need custom fonts generated with `lv_font_conv` or EEZ Studio's font tool. Only include the glyph ranges you need (digits, °, F, C, ., -, :) so flash use stays low.
- Convert the Ionicons used in the prototype into one icon font, and list the codepoints in `docs/icons.md`.
- Keyboard keys are 34×44 px in the prototype. Check them on real hardware against the round mask and enlarge them if taps are missed.
- Keep content inside the circular safe area. Nothing interactive should sit in the corners of the 480×480 frame.
- Budget memory: draw buffers in PSRAM when using RGB panels, and target ≥30 fps while the ring is turning.

## Deliverables and order
1. Display, touch, and encoder drivers, plus an LVGL port with a test screen. Stop and report.
2. EEZ Studio project with the theme styles, fonts, and icon font.
3. Screens in this order: idle → menu → thermostat → lights → the status screens → settings/keyboard → alert.
4. Data layer with mock data, then MQTT integration.
5. `README.md` covering build, flash, pin map, and how to regenerate EEZ output.

Work incrementally, with each step compiling and committed. Ask before guessing hardware details or MQTT schemas.
