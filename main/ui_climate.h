/*
 * The thermostat screen's state and content.
 *
 * There is no thermostat topic in Headwaters yet (docs/mqtt.md), so the
 * setpoint and mode live here, in RAM, set by the ring and the Mode screen.
 * The one live input is the inside temperature, from Borealis via
 * capstan_model. Nothing is published. When a topic exists, this is the one
 * module that has to learn about it.
 *
 * Content only, like ui_devices: labels, flags, states and the two needle
 * lines' points. The active-range ticks are Scale sections whose ranges EEZ
 * reads through get_var_climate_*() in vars.c every ui_tick().
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UI_CLIMATE_HEAT = 0,     /* order matches the Mode screen's rows */
    UI_CLIMATE_COOL,
    UI_CLIMATE_AUTO,
    UI_CLIMATE_OFF,
    UI_CLIMATE_MODE_COUNT
} ui_climate_mode_t;

/** Ring on the Climate screen: one detent = 1 F (0.5 C when showing C). */
void ui_climate_rotate(int diff);

void              ui_climate_set_mode(ui_climate_mode_t mode);
ui_climate_mode_t ui_climate_mode(void);

/** Paint the Climate screen. LVGL lock held. Cheap; called per refresh. */
void ui_climate_refresh(void);

/* What the climate is doing, for the clock faces' mode line. */
typedef enum {
    UI_CLIMATE_LINE_HOLD = 0,   /* "Holding 72°"    -- green   */
    UI_CLIMATE_LINE_HEAT,       /* "Heating to 72°" -- danger  */
    UI_CLIMATE_LINE_COOL,       /* "Cooling to 72°" -- info    */
    UI_CLIMATE_LINE_OFF,        /* "Climate off"    -- muted   */
} ui_climate_line_t;

/** The clock faces' climate line (the prototype's idleLine), and which of
 *  the four it is, for its colour and icon. */
ui_climate_line_t ui_climate_idle_line(char *out, size_t len);

/** A temperature held in Fahrenheit, in the Locale's unit, digits only:
 *  "72" or "22.5". */
void ui_climate_format_temp(char *out, size_t len, float f);

/** The menu carousel's summary line, e.g. "Heating · 72°" or "Off". */
void ui_climate_summary(char *out, size_t len);

/* The three active-range colours, one Scale section each. */
typedef enum {
    UI_CLIMATE_SEC_HEAT = 0,   /* Danger */
    UI_CLIMATE_SEC_COOL,       /* Info */
    UI_CLIMATE_SEC_HOLD,       /* AccentPrimary */
    UI_CLIMATE_SEC_COUNT
} ui_climate_section_t;

/** Section range in scale units (half degrees above 50 F); -1/-1 when that
 *  colour is not showing. Read by vars.c on every ui_tick(). */
void ui_climate_section(ui_climate_section_t sec, int32_t *min, int32_t *max);

#ifdef __cplusplus
}
#endif
