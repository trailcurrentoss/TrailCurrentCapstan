# Claude Code prompt: Air Quality screen (EEZ Studio 0.29, LVGL 9)

Paste everything below the line into Claude Code.

---

The Air Quality screen you built only puts basic labels on the display. Rebuild it in **EEZ Studio 0.29** (LVGL project, LVGL 9.x) so it matches the prototype in `design/Rotary Dial.dc.html` to within a few pixels. The spec below has every measurement, color, and font. Do not guess, and do not fall back to LVGL defaults.

## 0. Before you start
1. Open `design/Rotary Dial.dc.html` in a browser, rotate the ring to **Air Quality**, and press it. Screenshot the screen in both light and dark themes (use the theme toggle). These screenshots are the acceptance reference.
2. In the HTML, the air screen is the `<sc-if value="{{ isAir }}">` block. Its arc geometry comes from `arc(A0, A0+SPAN*aqi/300, 214)`, where `A0=135` and `SPAN=270`, drawn around center (240,240).
3. Open the `.eez-project` file and check that **Settings → General → LVGL version = 9.x** and that the display size is 480×480. Fix both if they are wrong.

## 1. What the screen looks like
It is a round 480×480 face with no header, cards, or borders.
- A **thin 270° gauge ring** sits near the edge of the glass. The gap is at the bottom, and the ring starts at the 7:30 position and runs clockwise to the 4:30 position. A grey track covers the full 270°. A colored indicator fills from the start in proportion to the AQI (0–300 scale). Both ends are rounded caps. There is no knob.
- Everything else is one **centered vertical stack**:
  1. `AIR QUALITY INDEX`: a small, muted, uppercase label with wide tracking
  2. `32`: a very large, light-weight numeral, tightly tracked
  3. `Good`: the status word, drawn in the status color (the same color as the arc indicator)
  4. A row of three **sub-metrics**, each a muted label with a value underneath: `CO2 / 640 ppm`, `Humidity / 44%`, `PM2.5 / 8 µg`

What makes it look right: large type contrast (120px light vs 14px labels), plenty of empty space, a single accent color shared by the arc and the status word, and no containers.

## 2. Exact geometry (px, 480×480 screen, origin top-left)
Arc (`lv_arc`, name it `aq_arc`):
- The centerline radius is 214 and the stroke width is 14. LVGL draws `arc_width` inward from the object's edge, so the **object is 442×442** (2 × (214 + 7)), aligned `LV_ALIGN_CENTER` with offset (0,0). Its outer edge is at r = 221 and its inner edge at r = 207.
- LVGL angles match the prototype (0° = 3 o'clock, increasing clockwise). Set `rotation = 135`, `bg_start_angle = 0`, `bg_end_angle = 270`, `mode = NORMAL`, `range 0..300`, `value = aqi`.

Center stack (`lv_obj` container, name it `aq_stack`):
- Size `LV_SIZE_CONTENT` × `LV_SIZE_CONTENT`, aligned `LV_ALIGN_CENTER`.
- Flex column, `main_place = CENTER`, `cross_place = CENTER`, `track_place = CENTER`, **row gap 6**.
- The metrics row (`aq_metrics`) has content size, flex row, **column gap 22**, and **margin-top 16**. LVGL 9 supports `margin_top`. If it doesn't work inside the flex layout, add a 10px spacer instead, since 6 of the 16px already comes from the stack gap. Each metric is a content-sized flex column with row gap 0 and centered cross axis.

## 3. Typography
Use the **Roboto** family (TrailCurrent uses system sans, which is Roboto on Android). Add the TTFs to `assets/fonts/` and create each font in EEZ Studio's **Fonts** panel at **4 bpp**. Never use the default Montserrat.

| EEZ font name | TTF | Size | Glyph ranges | Used by |
| --- | --- | --- | --- | --- |
| `ui_font_light_120` | Roboto-Light | 120 | `0-9` `-` only (U+0030–0039, U+002D) | AQI value |
| `ui_font_reg_24` | Roboto-Regular | 24 | Basic Latin U+0020–007E | status word |
| `ui_font_reg_20` | Roboto-Regular | 20 | U+0020–007E, `µ` U+00B5, `°` U+00B0, `%` | metric values |
| `ui_font_reg_17` | Roboto-Regular | 17 | U+0020–007E | screen title |
| `ui_font_reg_14` | Roboto-Regular | 14 | U+0020–007E | metric labels |

Reuse any of these that already exist from other screens instead of making duplicates. Keep the digits-only range on the 120px font, because the full range costs hundreds of KB.

Text styles:
- Title: `ui_font_reg_17`, `text_letter_space = 2`, and the text written in uppercase in the source (`AIR QUALITY INDEX`, since LVGL has no text-transform). Color is text-muted.
- Value: `ui_font_light_120`, `text_letter_space = -4`, color text-primary. It is `LV_SIZE_CONTENT` with no fixed width. If the line box leaves extra space above the digits compared with the prototype, set `pad_top`/`pad_bottom` negative (about −6) on this label only.
- Status: `ui_font_reg_24`, colored with the status color.
- Metric label: `ui_font_reg_14`, text-muted. Metric value: `ui_font_reg_20`, text-primary. Both centered (`text_align = CENTER`).

## 4. Color
Use hex values from the TrailCurrent tokens (`design/colors_and_type.css`) and nothing else.

| Role | Light (default) | Dark |
| --- | --- | --- |
| Screen bg | `#f5f5f5` | `#000000` |
| Arc track (MAIN) | `#dddddd` | `#333333` |
| Text primary | `#1a1a1a` | `#ffffff` |
| Text muted | `#888888` | `#666666` |

Status color ladder (drives both the arc INDICATOR and the status label):
- 0–50 → `Good`, `#74FE00`. This is what the prototype shows at AQI 32.
- 51–100 → `Moderate`, `#FFC107`
- 101–150 → `Unhealthy`, `#FF5453`
- \>150 → `Critical`, `#FF5453`

When the data is missing, the value shows `--`, the status shows `--` in text-muted, the arc indicator is set to 0 and hidden (`LV_PART_INDICATOR` `arc_opa = 0`), and each metric value shows `--`.

Build the theme colors as EEZ **Styles** (or theme colors) with a light and dark variant, so the global theme toggle switches this screen with no per-screen code.

## 5. Strip the LVGL defaults
The basic version most likely failed here. Apply these on every widget:
- `aq_stack`, `aq_metrics`, and each metric column: `bg_opa = 0`, `border_width = 0`, `pad_all = 0`, `radius = 0`, `shadow_width = 0`, flag `SCROLLABLE` cleared, `scrollbar_mode = OFF`. The default `lv_obj` has a white background, border, padding, and scrollbars.
- `aq_arc`:
  - MAIN: `arc_width 14`, `arc_rounded true`, `arc_color` = track color, `bg_opa 0`, `pad_all 0`, `border_width 0`
  - INDICATOR: `arc_width 14`, `arc_rounded true`, `arc_color` = status color
  - KNOB: `bg_opa 0`, `pad_all 0`, `border_width 0`, `shadow_width 0`, or remove the knob style entirely
  - Clear the `CLICKABLE` flag, and do **not** add it to the encoder group. It is read-only, and the ring must not change the value.
- Screen: bg = screen bg, `pad_all 0`, no scroll.

## 6. Data binding (EEZ Flow / native variables)
Define these global variables in EEZ (as Native variables backed by the C model, or Flow globals if the project uses Flow):
`air_aqi` (integer, −1 = no data), `air_co2_ppm` (integer), `air_humidity_pct` (integer), `air_pm25_ug` (integer).

- Bind `aq_arc` **Value** → `air_aqi`, clamped to 0..300.
- Bind label texts through expressions or computed native getters: `"640 ppm"`, `"44%"`, `"8 µg"`, and `--` when the value is missing. Format these in C getters (`get_var_air_co2_text()` and so on) instead of building long EEZ expressions.
- Put status text and color in native getters (`get_var_air_status_text()`, `get_var_air_status_color()`). Apply the color in a `ui_tick`/observer callback with `lv_obj_set_style_arc_color(aq_arc, c, LV_PART_INDICATOR)` and `lv_obj_set_style_text_color(aq_status, c, 0)`, because EEZ style bindings for colors are limited.
- When the AQI changes, animate the arc from its old value to the new one over **300 ms** with `lv_anim_path_ease_out`, so it doesn't jump. There is no other animation: no bounce and no pulse.
- The MQTT layer writes the model only. The UI never touches MQTT.

## 7. Separate generated code from hand-written code
- Treat everything EEZ generates in `src/ui/` (`screens.c`, `styles.c`, `fonts.h`, `vars.h`, …) as generated. Never hand-edit it; change the `.eez-project` and rebuild.
- Put native getters and setters in `src/ui_logic/air_quality.c`, and animation and color updates in the same file.
- If a property you need isn't exposed in the EEZ 0.29 UI, set it in a screen-created hook (for example a `ui_air_quality_post_create()` called after `create_screens()`), not in generated files. Record each workaround in `docs/eez-notes.md`.

## 8. Acceptance
Run the EEZ Studio simulator or the LVGL PC simulator (SDL) at 480×480, and screenshot the screen at AQI 32, 85, 160, and no-data, in both themes. Put them next to the prototype screenshots and check that:
- the arc radius, stroke, rounded caps, bottom gap, and start angle match
- the `32` looks thin and large (about 120px cap-to-baseline scale), not bold or small
- the title is small, muted, uppercase, and letter-spaced
- the arc and the status word share the same color
- there are no stray backgrounds, borders, scrollbars, or a knob
- the three metrics are centered as a group with even 22px gaps
- the theme toggle switches every color on this screen

Commit the screenshots to `docs/screens/air_quality/`. Report any difference you could not close, with the reason.

Once this screen passes, use the same patterns (stripped containers, flex stack, the thin 214/14 arc, and the font set) for the Energy screen, which shares the same arc and type scale.
