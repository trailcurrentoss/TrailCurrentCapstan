/*
 * The idle clock faces and the Clock Face picker. See ui_clock.h.
 *
 * Each face is authored twice by the same builder in screens_layout.py:
 * once on the Idle screen and once, at 60 %, in the picker's preview window.
 * A face_t holds one copy's widgets, so one paint function serves both.
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "esp_log.h"

#include "capstan_config.h"
#include "capstan_model.h"
#include "ui_climate.h"
#include "ui_clock.h"

#ifndef CAPSTAN_HAVE_UI
#  error "CAPSTAN_HAVE_UI is not defined -- main/CMakeLists.txt must set it"
#endif

static int s_pick;       /* the face previewed in the picker */
static int s_second;     /* 0..59, read by the Digital face's section */

static const char *const FACE_NAMES[UI_CLOCK_FACE_COUNT] = {
    "Classic", "Digital", "TrailCurrent", "Climate Ring",
};

int ui_clock_face(void)
{
    capstan_display_cfg_t d;
    capstan_config_get_display(&d);
    return d.clock_face < UI_CLOCK_FACE_COUNT ? d.clock_face : 0;
}

const char *ui_clock_face_name(int face)
{
    return (face >= 0 && face < UI_CLOCK_FACE_COUNT) ? FACE_NAMES[face] : "--";
}

int32_t ui_clock_second(void) { return s_second; }

#if CAPSTAN_HAVE_UI

#include "screens.h"
#include "ui.h"
#include "ui_lv.h"

static const char *TAG = "ui.clock";

/* Mode-line glyphs, from the house subset: fire, snowflake, check, power. */
#define G_HEAT  "\xEF\x81\xAD"   /* 0xF06D */
#define G_COOL  "\xEF\x8B\x9C"   /* 0xF2DC */
#define G_HOLD  "\xEF\x80\x8C"   /* 0xF00C */
#define G_OFF   "\xEF\x80\x91"   /* 0xF011 */

typedef struct {
    lv_obj_t *root[UI_CLOCK_FACE_COUNT];
    /* Classic */
    lv_obj_t *cl_day, *cl_date, *cl_inside, *cl_mode_icon, *cl_mode_text;
    lv_obj_t *cl_batt, *cl_water, *cl_hand[3];
    /* Digital */
    lv_obj_t *dg_ticks, *dg_date, *dg_time, *dg_ampm, *dg_inside;
    lv_scale_section_t *dg_sec;     /* the elapsed-seconds section */
    lv_obj_t *dg_mode_icon, *dg_mode_text;
    /* TrailCurrent */
    lv_obj_t *br_arc, *br_time, *br_ampm, *br_date, *br_inside;
    lv_obj_t *br_mode_icon, *br_mode_text;
    /* Climate Ring */
    lv_obj_t *rg_arc, *rg_h[12], *rg_temp, *rg_time;
    lv_obj_t *rg_mode_icon, *rg_mode_text;
    /* LVGL keeps a pointer to a line's points, so they live here. */
    lv_point_precise_t pts[3][2];
} face_t;

static face_t s_idle, s_prev;
static bool   s_bound;

#define BIND(f, P) do {                                                      \
    (f)->root[0] = objects.P##_cl;  (f)->root[1] = objects.P##_dg;           \
    (f)->root[2] = objects.P##_br;  (f)->root[3] = objects.P##_rg;           \
    (f)->cl_day = objects.P##_cl_day; (f)->cl_date = objects.P##_cl_date;    \
    (f)->cl_inside = objects.P##_cl_inside;                                  \
    (f)->cl_mode_icon = objects.P##_cl_mode_icon;                            \
    (f)->cl_mode_text = objects.P##_cl_mode_text;                            \
    (f)->cl_batt = objects.P##_cl_batt_value;                                \
    (f)->cl_water = objects.P##_cl_water_value;                              \
    (f)->cl_hand[0] = objects.P##_cl_hand_h;                                 \
    (f)->cl_hand[1] = objects.P##_cl_hand_m;                                 \
    (f)->cl_hand[2] = objects.P##_cl_hand_s;                                 \
    (f)->dg_ticks = objects.P##_dg_ticks; (f)->dg_date = objects.P##_dg_date; \
    (f)->dg_time = objects.P##_dg_time; (f)->dg_ampm = objects.P##_dg_ampm;  \
    (f)->dg_inside = objects.P##_dg_inside;                                  \
    (f)->dg_mode_icon = objects.P##_dg_mode_icon;                            \
    (f)->dg_mode_text = objects.P##_dg_mode_text;                            \
    (f)->br_arc = objects.P##_br_arc; (f)->br_time = objects.P##_br_time;    \
    (f)->br_ampm = objects.P##_br_ampm; (f)->br_date = objects.P##_br_date;  \
    (f)->br_inside = objects.P##_br_inside;                                  \
    (f)->br_mode_icon = objects.P##_br_mode_icon;                            \
    (f)->br_mode_text = objects.P##_br_mode_text;                            \
    (f)->rg_arc = objects.P##_rg_arc; (f)->rg_temp = objects.P##_rg_temp;    \
    (f)->rg_time = objects.P##_rg_time;                                      \
    (f)->rg_mode_icon = objects.P##_rg_mode_icon;                            \
    (f)->rg_mode_text = objects.P##_rg_mode_text;                            \
    (f)->rg_h[0] = objects.P##_rg_h1;   (f)->rg_h[1] = objects.P##_rg_h2;    \
    (f)->rg_h[2] = objects.P##_rg_h3;   (f)->rg_h[3] = objects.P##_rg_h4;    \
    (f)->rg_h[4] = objects.P##_rg_h5;   (f)->rg_h[5] = objects.P##_rg_h6;    \
    (f)->rg_h[6] = objects.P##_rg_h7;   (f)->rg_h[7] = objects.P##_rg_h8;    \
    (f)->rg_h[8] = objects.P##_rg_h9;   (f)->rg_h[9] = objects.P##_rg_h10;   \
    (f)->rg_h[10] = objects.P##_rg_h11; (f)->rg_h[11] = objects.P##_rg_h12;  \
} while (0)

static void bind(void)
{
    if (!s_bound) {
        BIND(&s_idle, idle);
        BIND(&s_prev, cfp);
        s_idle.dg_sec = screen_page_idle_state.scale_section;
        s_prev.dg_sec = screen_page_clock_face_state.scale_section;
        s_bound = true;
    }
}

/* One reading of everything the faces show, taken once per paint. */
typedef struct {
    bool  time_ok;
    struct tm lt;
    char  hm[8], ampm[4];
    char  day[16], date_long[32], date_short[40];
    char  inside[24], inside_num[12];
    char  mode[32];
    ui_climate_line_t line;
    char  batt[8], water[8];
} snapshot_t;

static void take(snapshot_t *s)
{
    static const char *const MONTHS[12] = {
        "January", "February", "March", "April", "May", "June", "July",
        "August", "September", "October", "November", "December" };
    static const char *const DAYS[7] = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
        "Saturday" };

    memset(s, 0, sizeof(*s));
    s->time_ok = capstan_model_time_valid();
    if (s->time_ok) {
        const time_t now = time(NULL);
        localtime_r(&now, &s->lt);
        capstan_display_cfg_t dc;
        capstan_config_get_display(&dc);
        if (dc.clock_24h) {
            /* "19:42", and no AM/PM: the label is left empty. */
            snprintf(s->hm, sizeof(s->hm), "%02d:%02d", s->lt.tm_hour,
                     s->lt.tm_min);
        } else {
            const int h12 = (s->lt.tm_hour % 12) ? (s->lt.tm_hour % 12) : 12;
            snprintf(s->hm, sizeof(s->hm), "%d:%02d", h12, s->lt.tm_min);
            snprintf(s->ampm, sizeof(s->ampm), "%s",
                     s->lt.tm_hour < 12 ? "AM" : "PM");
        }
        const char *m = MONTHS[s->lt.tm_mon % 12];
        const char *d = DAYS[s->lt.tm_wday % 7];
        snprintf(s->day, sizeof(s->day), "%s", d);
        snprintf(s->date_long, sizeof(s->date_long), "%s %d, %d", m,
                 s->lt.tm_mday, s->lt.tm_year + 1900);
        snprintf(s->date_short, sizeof(s->date_short),
                 "%s \xC2\xB7 %s %d", d, m, s->lt.tm_mday);
    } else {
        /* No fix yet: `--` like every other unknown reading. */
        snprintf(s->hm, sizeof(s->hm), "--:--");
        snprintf(s->day, sizeof(s->day), "--");
        snprintf(s->date_long, sizeof(s->date_long), "--");
        snprintf(s->date_short, sizeof(s->date_short), "--");
    }

    const capstan_value_t in = capstan_model_temp_f();
    if (in.valid) {
        char t[8];
        ui_climate_format_temp(t, sizeof(t), in.value);
        snprintf(s->inside, sizeof(s->inside), "Inside %s\xC2\xB0", t);
        snprintf(s->inside_num, sizeof(s->inside_num), "%s\xC2\xB0", t);
    } else {
        snprintf(s->inside, sizeof(s->inside), "Inside --\xC2\xB0");
        snprintf(s->inside_num, sizeof(s->inside_num), "--");
    }
    s->line = ui_climate_idle_line(s->mode, sizeof(s->mode));

    const capstan_value_t b = capstan_model_battery_pct();
    const capstan_value_t w = capstan_model_tank(CAPSTAN_TANK_FRESH);
    if (b.valid) { snprintf(s->batt, sizeof(s->batt), "%.0f%%", (double)b.value); }
    else         { snprintf(s->batt, sizeof(s->batt), "--"); }
    if (w.valid) { snprintf(s->water, sizeof(s->water), "%.0f%%", (double)w.value); }
    else         { snprintf(s->water, sizeof(s->water), "--"); }
}

/* The mode line's text, glyph and colour state (see FaceMode). */
static void paint_mode(lv_obj_t *icon, lv_obj_t *text, const snapshot_t *s)
{
    const char *g;
    lv_state_t st;
    switch (s->line) {
    case UI_CLIMATE_LINE_HEAT: g = G_HEAT; st = LV_STATE_CHECKED;  break;
    case UI_CLIMATE_LINE_COOL: g = G_COOL; st = LV_STATE_PRESSED;  break;
    case UI_CLIMATE_LINE_OFF:  g = G_OFF;  st = LV_STATE_DISABLED; break;
    default:                   g = G_HOLD; st = 0;                 break;
    }
    const lv_state_t mask = LV_STATE_CHECKED | LV_STATE_PRESSED |
                            LV_STATE_DISABLED;
    ui_lv_set_text(icon, g);
    ui_lv_set_text(text, s->mode);
    ui_lv_set_state_in(icon, mask, st);
    ui_lv_set_state_in(text, mask, st);
}

/* A hand from the centre, `turns` of a revolution clockwise from 12, with an
 * optional tail behind the pivot. Only re-set when it moved. */
static void set_hand(lv_obj_t *line, lv_point_precise_t *pts, float c,
                     float len, float tail, float turns)
{
    if (!line) {
        return;
    }
    const float a = turns * 2.0f * (float)M_PI;
    const lv_point_precise_t p0 = {
        (lv_value_precise_t)lroundf(c - tail * sinf(a)),
        (lv_value_precise_t)lroundf(c + tail * cosf(a)) };
    const lv_point_precise_t p1 = {
        (lv_value_precise_t)lroundf(c + len * sinf(a)),
        (lv_value_precise_t)lroundf(c - len * cosf(a)) };
    if (lv_line_get_point_count(line) == 2 &&
        pts[0].x == p0.x && pts[0].y == p0.y &&
        pts[1].x == p1.x && pts[1].y == p1.y) {
        return;
    }
    pts[0] = p0;
    pts[1] = p1;
    lv_line_set_points(line, pts, 2);
}

/* Seconds into the hour for the minute arcs, never under 0.2 minutes so the
 * arc always shows a stub (the design's max(min, 0.2)). */
static int32_t arc_value(const snapshot_t *s)
{
    if (!s->time_ok) {
        return 12;
    }
    const int32_t v = s->lt.tm_min * 60 + s->lt.tm_sec;
    return v < 12 ? 12 : v;
}

static void paint(face_t *f, int face, const snapshot_t *s)
{
    for (int i = 0; i < UI_CLOCK_FACE_COUNT; i++) {
        ui_lv_set_hidden(f->root[i], i != face);
    }

    switch (face) {
    case UI_CLOCK_FACE_CLASSIC: {
        ui_lv_set_text(f->cl_day, s->day);
        ui_lv_set_text(f->cl_date, s->date_long);
        ui_lv_set_text(f->cl_inside, s->inside);
        paint_mode(f->cl_mode_icon, f->cl_mode_text, s);
        ui_lv_set_text(f->cl_batt, s->batt);
        ui_lv_set_text(f->cl_water, s->water);

        /* The design's hand lengths on 480: 110 / 170 / 190 (tail 30). */
        const float res = (float)lv_display_get_horizontal_resolution(NULL);
        const float k = res / 480.0f, c = res / 2.0f;
        float th = 0, tm = 0, ts = 0;
        if (s->time_ok) {
            ts = s->lt.tm_sec / 60.0f;
            tm = (s->lt.tm_min + s->lt.tm_sec / 60.0f) / 60.0f;
            th = ((s->lt.tm_hour % 12) + s->lt.tm_min / 60.0f) / 12.0f;
        }
        set_hand(f->cl_hand[0], f->pts[0], c, 110 * k, 0, th);
        set_hand(f->cl_hand[1], f->pts[1], c, 170 * k, 0, tm);
        set_hand(f->cl_hand[2], f->pts[2], c, 190 * k, 30 * k, ts);
        break;
    }
    case UI_CLOCK_FACE_DIGITAL:
        ui_lv_set_text(f->dg_date, s->date_short);
        ui_lv_set_text(f->dg_time, s->hm);
        ui_lv_set_text(f->dg_ampm, s->ampm);
        ui_lv_set_hidden(f->dg_ampm, !s->ampm[0]);   /* 24 h: re-centres */
        ui_lv_set_text(f->dg_inside, s->inside);
        paint_mode(f->dg_mode_icon, f->dg_mode_text, s);
        /* EEZ also applies this range from get_var_clock_sec_max() in its
         * tick, but lv_scale_section_set_range() does not invalidate and the
         * tick may run after the frame -- so set it here and repaint, and
         * the ring moves on the second, not a second late. */
        if (f->dg_sec && f->dg_ticks) {
            lv_scale_section_set_range(f->dg_sec, 0, s_second);
            lv_obj_invalidate(f->dg_ticks);
        }
        break;
    case UI_CLOCK_FACE_BRAND:
        if (f->br_arc && lv_arc_get_value(f->br_arc) != arc_value(s)) {
            lv_arc_set_value(f->br_arc, arc_value(s));
        }
        ui_lv_set_text(f->br_time, s->hm);
        ui_lv_set_text(f->br_ampm, s->ampm);
        ui_lv_set_hidden(f->br_ampm, !s->ampm[0]);
        ui_lv_set_text(f->br_date, s->date_short);
        ui_lv_set_text(f->br_inside, s->inside);
        paint_mode(f->br_mode_icon, f->br_mode_text, s);
        break;
    case UI_CLOCK_FACE_RING: {
        if (f->rg_arc && lv_arc_get_value(f->rg_arc) != arc_value(s)) {
            lv_arc_set_value(f->rg_arc, arc_value(s));
        }
        const int h12 = s->time_ok ?
            ((s->lt.tm_hour % 12) ? (s->lt.tm_hour % 12) : 12) : 0;
        for (int i = 0; i < 12; i++) {
            ui_lv_set_state_in(f->rg_h[i], LV_STATE_CHECKED,
                               (i + 1 == h12) ? LV_STATE_CHECKED : 0);
        }
        ui_lv_set_text(f->rg_temp, s->inside_num);
        char t[16];
        if (s->time_ok) {
            snprintf(t, sizeof(t), s->ampm[0] ? "%s %s" : "%s", s->hm,
                     s->ampm);
        } else {
            snprintf(t, sizeof(t), "--:--");
        }
        ui_lv_set_text(f->rg_time, t);
        paint_mode(f->rg_mode_icon, f->rg_mode_text, s);
        break;
    }
    default:
        break;
    }
}

void ui_clock_refresh(void)
{
    bind();
    snapshot_t s;
    take(&s);
    s_second = s.time_ok ? s.lt.tm_sec : 0;
    paint(&s_idle, ui_clock_face(), &s);
}

/* ---------------------------------------------------------------------- */
/* The picker                                                              */
/* ---------------------------------------------------------------------- */

static void paint_picker(void)
{
    bind();
    snapshot_t s;
    take(&s);
    s_second = s.time_ok ? s.lt.tm_sec : 0;
    paint(&s_prev, s_pick, &s);

    const bool current = (s_pick == ui_clock_face());
    ui_lv_set_text(objects.cfp_name, FACE_NAMES[s_pick]);
    ui_lv_set_text(objects.cfp_sub, current ? "Current face" : "Press to set");
    ui_lv_set_state_in(objects.cfp_sub, LV_STATE_CHECKED,
                       current ? LV_STATE_CHECKED : 0);
    lv_obj_t *const dots[UI_CLOCK_FACE_COUNT] = {
        objects.cfp_dot0, objects.cfp_dot1, objects.cfp_dot2,
        objects.cfp_dot3 };
    for (int i = 0; i < UI_CLOCK_FACE_COUNT; i++) {
        ui_lv_set_state_in(dots[i], LV_STATE_CHECKED,
                           i == s_pick ? LV_STATE_CHECKED : 0);
    }
}

void ui_clock_pick_enter(void)
{
    s_pick = ui_clock_face();
    paint_picker();
}

void ui_clock_pick_rotate(int diff)
{
    /* Wraps, as the design's picker does. */
    s_pick = ((s_pick + diff) % UI_CLOCK_FACE_COUNT + UI_CLOCK_FACE_COUNT) %
             UI_CLOCK_FACE_COUNT;
    paint_picker();
}

void ui_clock_pick_press(void)
{
    capstan_display_cfg_t d;
    capstan_config_get_display(&d);
    d.clock_face = (uint8_t)s_pick;
    const esp_err_t err = capstan_config_set_display(&d);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "clock face save failed: %s", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(TAG, "clock face -> %s", FACE_NAMES[s_pick]);
}

/*
 * Say out loud, once, why the face is blank.
 *
 * "The clock is not getting data" is the single most reported thing about this
 * screen, and from the panel it is indistinguishable from a bug in this file.
 * It almost never is: `local/gps/time` is published at 1 Hz only while the
 * GNSS module has a fix, it is NOT retained, and there is no other time source
 * on the rig. So after half a minute of a live broker and no time, one line
 * naming the topic saves reading this code.
 *
 * Also reports the zone, because a clock that is exactly some whole number of
 * hours out is a missing `os/timezone/current` and nothing else.
 */
static void report_time_source_once(void)
{
    static bool s_reported;
    static bool s_reported_tz;

    if (!s_reported && !capstan_model_time_valid() &&
        lv_tick_get() > 30000) {
        s_reported = true;
        ESP_LOGW(TAG, "no time after 30s -- nothing has published "
                      "local/gps/time. It is 1 Hz, not retained, and only "
                      "flows while the GNSS module has a fix; there is no "
                      "other clock source on the rig.");
    }

    if (!s_reported_tz && capstan_model_time_valid() &&
        !capstan_model_timezone_known()) {
        s_reported_tz = true;
        ESP_LOGW(TAG, "clock is set but no known timezone -- rendering UTC. "
                      "Expect os/timezone/current (retained) from Headwaters.");
    }
}

static void clock_timer_cb(lv_timer_t *t)
{
    (void)t;

    report_time_source_once();

    /* Once a second, and only for a face that is showing: the idle screen,
     * or the picker's preview. A background screen repainted every second
     * is a needless redraw forever. */
    lv_obj_t *const scr = lv_screen_active();
    if (scr == objects.page_idle) {
        ui_clock_refresh();
    } else if (scr == objects.page_clock_face) {
        paint_picker();
    }
}

void ui_clock_init(void)
{
    lv_timer_create(clock_timer_cb, 1000, NULL);
    ui_clock_refresh();     /* park the hands before the first tick */
}

#else

void ui_clock_init(void) { }
void ui_clock_refresh(void) { }
void ui_clock_pick_enter(void) { }
void ui_clock_pick_rotate(int diff) { (void)diff; }
void ui_clock_pick_press(void) { }

#endif
