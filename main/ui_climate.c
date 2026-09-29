/*
 * The thermostat screen. See ui_climate.h, and page_climate() in
 * GUI/tmp/screens_layout.py for the layout this fills in.
 */

#include <math.h>
#include <stdio.h>

#include "esp_log.h"

#include "capstan_config.h"
#include "capstan_model.h"
#include "ui_climate.h"

#ifndef CAPSTAN_HAVE_UI
#  error "CAPSTAN_HAVE_UI is not defined -- main/CMakeLists.txt must set it"
#endif

/* Setpoint range and the prototype's "holding" band (P:477-482). */
#define T_MIN_F   50.0f
#define T_MAX_F   90.0f
#define HOLD_F    0.2f

static float             s_target_f = 72.0f;          /* prototype default */
static ui_climate_mode_t s_mode     = UI_CLIMATE_HEAT;

static int32_t s_sec_min[UI_CLIMATE_SEC_COUNT] = { -1, -1, -1 };
static int32_t s_sec_max[UI_CLIMATE_SEC_COUNT] = { -1, -1, -1 };

typedef enum { ACT_NONE, ACT_HEAT, ACT_COOL } act_t;

static bool celsius(void)
{
    capstan_display_cfg_t d;
    capstan_config_get_display(&d);
    return d.celsius;
}

/* The prototype's fmt(): whole F, or C to the nearest half degree. */
static void fmt_temp(char *out, size_t len, float f)
{
    if (celsius()) {
        const float c = roundf((f - 32.0f) * 5.0f / 9.0f * 2.0f) / 2.0f;
        if (c == floorf(c)) {
            snprintf(out, len, "%.0f", (double)c);
        } else {
            snprintf(out, len, "%.1f", (double)c);
        }
    } else {
        snprintf(out, len, "%.0f", (double)roundf(f));
    }
}

/*
 * What the system is doing, exactly as the prototype derives it (P:477-482):
 * target vs inside with a 0.2 deg band, and only Off overrides. Capstan is
 * one climate control; how heating or cooling is actually carried out is
 * handled outside it, so the mode does not restrict the direction.
 */
static act_t activity(bool inside_ok, float inside)
{
    if (s_mode == UI_CLIMATE_OFF || !inside_ok) {
        return ACT_NONE;
    }
    if (s_target_f > inside + HOLD_F) {
        return ACT_HEAT;
    }
    if (s_target_f < inside - HOLD_F) {
        return ACT_COOL;
    }
    return ACT_NONE;
}

void ui_climate_rotate(int diff)
{
    if (s_mode == UI_CLIMATE_OFF) {
        return;                       /* as the prototype: Off has no setpoint */
    }
    const float step = celsius() ? 0.9f : 1.0f;
    float t = s_target_f + (float)diff * step;
    if (t < T_MIN_F) { t = T_MIN_F; }
    if (t > T_MAX_F) { t = T_MAX_F; }
    s_target_f = t;
    ui_climate_refresh();             /* the ring should feel immediate */
}

void ui_climate_set_mode(ui_climate_mode_t mode)
{
    if (mode >= 0 && mode < UI_CLIMATE_MODE_COUNT) {
        s_mode = mode;
    }
}

ui_climate_mode_t ui_climate_mode(void) { return s_mode; }

void ui_climate_section(ui_climate_section_t sec, int32_t *min, int32_t *max)
{
    if (sec < 0 || sec >= UI_CLIMATE_SEC_COUNT) {
        *min = *max = -1;
        return;
    }
    *min = s_sec_min[sec];
    *max = s_sec_max[sec];
}

bool ui_climate_led(uint8_t *r, uint8_t *g, uint8_t *b)
{
    const capstan_value_t in = capstan_model_temp_f();
    switch (activity(in.valid, in.value)) {
    case ACT_HEAT: *r = 255; *g = 96;  *b = 0;   return true;   /* orange */
    case ACT_COOL: *r = 0;   *g = 64;  *b = 255; return true;   /* blue   */
    default:       return false;
    }
}

void ui_climate_summary(char *out, size_t len)
{
    if (s_mode == UI_CLIMATE_OFF) {
        snprintf(out, len, "Off");
        return;
    }
    const capstan_value_t in = capstan_model_temp_f();
    const act_t act = activity(in.valid, in.value);
    char t[8];
    fmt_temp(t, sizeof(t), s_target_f);
    snprintf(out, len, "%s \xC2\xB7 %s\xC2\xB0",
             act == ACT_HEAT ? "Heating" : act == ACT_COOL ? "Cooling"
                                                           : "Holding", t);
}

#if CAPSTAN_HAVE_UI

#include "screens.h"
#include "ui_lv.h"

/* Mode glyphs, from the house subset: fire, snowflake, check, power. */
#define G_HEAT  "\xEF\x81\xAD"   /* 0xF06D */
#define G_COOL  "\xEF\x8B\x9C"   /* 0xF2DC */
#define G_HOLD  "\xEF\x80\x8C"   /* 0xF00C */
#define G_OFF   "\xEF\x80\x91"   /* 0xF011 */

/* LVGL keeps a pointer to a line's points, so they must outlive the call. */
static lv_point_precise_t s_pts_inside[2];
static lv_point_precise_t s_pts_target[2];

/* A rim mark at temperature `f`, radii in the prototype's 480 px units. */
static void set_rim(lv_obj_t *line, lv_point_precise_t *pts, float f,
                    float r1, float r2)
{
    if (!line) {
        return;
    }
    const float res = (float)lv_display_get_horizontal_resolution(NULL);
    const float k = res / 480.0f, c = res / 2.0f;
    float frac = (f - T_MIN_F) / (T_MAX_F - T_MIN_F);
    frac = frac < 0.0f ? 0.0f : frac > 1.0f ? 1.0f : frac;
    const float a = (135.0f + frac * 270.0f) * (float)M_PI / 180.0f;
    const lv_point_precise_t p0 = {
        (lv_value_precise_t)(c + r1 * k * cosf(a)),
        (lv_value_precise_t)(c + r1 * k * sinf(a)) };
    const lv_point_precise_t p1 = {
        (lv_value_precise_t)(c + r2 * k * cosf(a)),
        (lv_value_precise_t)(c + r2 * k * sinf(a)) };
    /* Setting points redraws the line, so only when it has moved. */
    if (lv_line_get_point_count(line) == 2 &&
        pts[0].x == p0.x && pts[0].y == p0.y &&
        pts[1].x == p1.x && pts[1].y == p1.y) {
        return;
    }
    pts[0] = p0;
    pts[1] = p1;
    lv_line_set_points(line, pts, 2);
}

static void set_text(lv_obj_t *label, const char *text)
{
    ui_lv_set_text(label, text);
}

static void set_hidden(lv_obj_t *obj, bool hidden)
{
    ui_lv_set_hidden(obj, hidden);
}

/* Mode colour as a STATE (see ClimateModeText in gen_eez_project.py). */
static void set_mode_state(lv_obj_t *obj, lv_state_t st)
{
    ui_lv_set_state_in(obj, LV_STATE_CHECKED | LV_STATE_PRESSED |
                            LV_STATE_DISABLED, st);
}

void ui_climate_refresh(void)
{
    const capstan_value_t in = capstan_model_temp_f();
    const act_t act = activity(in.valid, in.value);
    char buf[24];

    /* Mode line. */
    const char *word, *glyph;
    lv_state_t st;
    if (s_mode == UI_CLIMATE_OFF) {
        word = "OFF";     glyph = G_OFF;  st = LV_STATE_DISABLED;
    } else if (act == ACT_HEAT) {
        word = "HEATING"; glyph = G_HEAT; st = LV_STATE_CHECKED;
    } else if (act == ACT_COOL) {
        word = "COOLING"; glyph = G_COOL; st = LV_STATE_PRESSED;
    } else {
        word = "HOLDING"; glyph = G_HOLD; st = 0;
    }
    set_text(objects.climate_mode, word);
    set_text(objects.climate_mode_icon, glyph);
    set_mode_state(objects.climate_mode, st);
    set_mode_state(objects.climate_mode_icon, st);

    /* Setpoint. */
    if (s_mode == UI_CLIMATE_OFF) {
        set_text(objects.climate_setpoint, "--");
    } else {
        fmt_temp(buf, sizeof(buf), s_target_f);
        set_text(objects.climate_setpoint, buf);
    }

    /* Inside, and the time-to-target the prototype estimates at 6 min per
     * degree (P:611) -- only while actually heating or cooling. */
    if (in.valid) {
        char t[8];
        fmt_temp(t, sizeof(t), in.value);
        snprintf(buf, sizeof(buf), "Inside %s\xC2\xB0", t);
    } else {
        snprintf(buf, sizeof(buf), "Inside --\xC2\xB0");
    }
    set_text(objects.climate_inside, buf);
    if (act != ACT_NONE) {
        long mins = lroundf(fabsf(s_target_f - in.value) * 6.0f);
        snprintf(buf, sizeof(buf), "%ld min", mins < 1 ? 1L : mins);
        set_text(objects.climate_eta, buf);
    }
    set_hidden(objects.climate_eta_group, act == ACT_NONE);

    /* Needles. */
    set_rim(objects.climate_needle_target, s_pts_target, s_target_f,
            176.0f, 226.0f);
    set_hidden(objects.climate_needle_target, s_mode == UI_CLIMATE_OFF);
    if (in.valid) {
        set_rim(objects.climate_needle_inside, s_pts_inside, in.value,
                190.0f, 222.0f);
    }
    set_hidden(objects.climate_needle_inside, !in.valid);

    /* Active range, in scale units: half degrees above 50 F, ticks whose
     * temperature lies within [lo-0.25, hi+0.25] as in P:626. */
    int32_t prev_min[UI_CLIMATE_SEC_COUNT], prev_max[UI_CLIMATE_SEC_COUNT];
    for (int i = 0; i < UI_CLIMATE_SEC_COUNT; i++) {
        prev_min[i] = s_sec_min[i];
        prev_max[i] = s_sec_max[i];
        s_sec_min[i] = s_sec_max[i] = -1;
    }
    if (s_mode != UI_CLIMATE_OFF && in.valid) {
        const float lo = fminf(in.value, s_target_f);
        const float hi = fmaxf(in.value, s_target_f);
        int32_t a = (int32_t)ceilf((lo - 0.25f - T_MIN_F) * 2.0f);
        int32_t b = (int32_t)floorf((hi + 0.25f - T_MIN_F) * 2.0f);
        a = a < 0 ? 0 : a > 80 ? 80 : a;
        b = b < 0 ? 0 : b > 80 ? 80 : b;
        const ui_climate_section_t sec =
            act == ACT_HEAT ? UI_CLIMATE_SEC_HEAT :
            act == ACT_COOL ? UI_CLIMATE_SEC_COOL : UI_CLIMATE_SEC_HOLD;
        if (a <= b) {
            s_sec_min[sec] = a;
            s_sec_max[sec] = b;
        }
    }

    /*
     * EEZ applies these ranges in its tick, but lv_scale_section_set_range()
     * does not invalidate, so the ticks only repainted when something else
     * redrew the screen. Now that nothing redraws without a change, apply
     * them here and invalidate the scale -- once, when a range moves.
     */
    bool moved = false;
    for (int i = 0; i < UI_CLIMATE_SEC_COUNT; i++) {
        moved |= prev_min[i] != s_sec_min[i] || prev_max[i] != s_sec_max[i];
    }
    if (moved && objects.climate_ticks) {
        screen_page_climate_state_t *st = &screen_page_climate_state;
        lv_scale_section_t *secs[UI_CLIMATE_SEC_COUNT] = {
            st->scale_section, st->scale_section1, st->scale_section2 };
        for (int i = 0; i < UI_CLIMATE_SEC_COUNT; i++) {
            if (secs[i]) {
                lv_scale_section_set_range(secs[i], s_sec_min[i], s_sec_max[i]);
            }
        }
        lv_obj_invalidate(objects.climate_ticks);
    }
}

#else

void ui_climate_refresh(void) { }

#endif
