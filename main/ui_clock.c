/*
 * The idle analog clock. See ui_clock.h.
 *
 * WHY THIS FILE EXISTS
 *
 * The idle face is fully authored in EEZ Studio, and it looked finished:
 * a tick ring, three needles, a date label reading "SAT 27 SEP". None of
 * it moved. The export draws each needle as an `lv_line` with two fixed
 * points and the date as literal text, and nothing in the firmware ever
 * touched them -- the panel showed the same frozen time and the same
 * hard-coded date that happened to be the day the layout was drawn.
 *
 * WHERE THE TIME COMES FROM
 *
 * There is no RTC on any of the three panels and no SNTP client in this
 * firmware. The clock is set from `local/gps/time` and rendered in the
 * zone from `os/timezone/current`; both are parsed in capstan_mqtt and
 * applied in capstan_model, which owns the system clock. This file only
 * reads localtime_r().
 *
 * Until a fix arrives the hands park at 12:00 and the date reads `--`,
 * for the same reason every other reading renders `--` when it is not
 * known: a clock confidently showing the wrong time is worse than a clock
 * visibly showing none.
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "capstan_model.h"
#include "ui_clock.h"

#ifndef CAPSTAN_HAVE_UI
#  error "CAPSTAN_HAVE_UI is not defined -- main/CMakeLists.txt must set it"
#endif

#if CAPSTAN_HAVE_UI

#include "screens.h"
#include "ui.h"

/*
 * Needle lengths as a fraction of the face, matching the authored
 * geometry on all three panels.
 *
 * They are fractions rather than pixels because the same file drives the
 * 240, 360 and 480 exports, and the three differ only in scale: the 480
 * face draws its hour needle from (240,240) to (240,105), which is
 * 135/480 of the width, and the other two exports use the same ratio. A
 * table of per-resolution constants here would be a second place to
 * forget when a face is re-authored, so the geometry is derived from the
 * screen instead.
 */
#define LEN_HOUR_F    0.28125f
#define LEN_MIN_F     0.40000f
#define LEN_SEC_F     0.44167f

/*
 * LVGL does not copy the point array handed to lv_line_set_points() -- it
 * keeps the pointer. These must outlive every redraw, so they are static
 * rather than locals in the tick, which would leave the line objects
 * pointing into a dead stack frame.
 */
static lv_point_precise_t s_pts_hour[2];
static lv_point_precise_t s_pts_min[2];
static lv_point_precise_t s_pts_sec[2];

static void set_hand(lv_obj_t *line, lv_point_precise_t *pts,
                     int32_t cx, int32_t cy, float len, float turns)
{
    if (!line) {
        return;
    }
    /* `turns` is a fraction of a full revolution clockwise from 12. */
    const float a = turns * 2.0f * (float)M_PI;
    pts[0].x = cx;
    pts[0].y = cy;
    pts[1].x = cx + (int32_t)lroundf(len * sinf(a));
    pts[1].y = cy - (int32_t)lroundf(len * cosf(a));
    /* Re-setting the same array is what invalidates the line; mutating it
     * in place without this leaves the old shape on screen until some
     * unrelated redraw happens to cover it. */
    lv_line_set_points(line, pts, 2);
}

void ui_clock_refresh(void)
{
    if (!objects.page_idle) {
        return;
    }

    const int32_t w  = lv_obj_get_width(objects.page_idle);
    const int32_t h  = lv_obj_get_height(objects.page_idle);
    if (w <= 0 || h <= 0) {
        return;     /* not laid out yet */
    }
    const int32_t cx = w / 2;
    const int32_t cy = h / 2;

    float turns_h = 0.0f, turns_m = 0.0f, turns_s = 0.0f;
    char  date[16] = "--";

    if (capstan_model_time_valid()) {
        const time_t now = time(NULL);
        struct tm lt;
        localtime_r(&now, &lt);

        /* Continuous, not stepped: an hour hand that jumps on the hour
         * reads as broken next to a moving minute hand. */
        turns_m = (lt.tm_min + lt.tm_sec / 60.0f) / 60.0f;
        turns_h = ((lt.tm_hour % 12) + lt.tm_min / 60.0f) / 12.0f;
        turns_s = lt.tm_sec / 60.0f;

        strftime(date, sizeof(date), "%a %d %b", &lt);
        for (char *p = date; *p; p++) {
            if (*p >= 'a' && *p <= 'z') *p -= 32;
        }
    }

    set_hand(objects.idle_hand_hour,   s_pts_hour, cx, cy, w * LEN_HOUR_F, turns_h);
    set_hand(objects.idle_hand_minute, s_pts_min,  cx, cy, w * LEN_MIN_F,  turns_m);
    set_hand(objects.idle_hand_second, s_pts_sec,  cx, cy, w * LEN_SEC_F,  turns_s);

    if (objects.idle_date) {
        lv_label_set_text(objects.idle_date, date);
    }
}

static void clock_timer_cb(lv_timer_t *t)
{
    (void)t;
    /*
     * The second hand moves once a second, so this fires once a second --
     * but only the idle screen shows it. Redrawing a background screen
     * costs a needless invalidation every second forever, on a panel that
     * spends most of its life on this very screen and the rest on another
     * one entirely.
     */
    if (lv_screen_active() != objects.page_idle) {
        return;
    }
    ui_clock_refresh();
}

void ui_clock_init(void)
{
    lv_timer_create(clock_timer_cb, 1000, NULL);
    ui_clock_refresh();     /* park the hands before the first tick */
}

#else

void ui_clock_init(void) { }
void ui_clock_refresh(void) { }

#endif
