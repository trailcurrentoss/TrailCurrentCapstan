/*
 * TrailCurrent Capstan — rotary touchscreen controller.
 *
 * Boot order matters here:
 *   1. NVS, because the board and every later subsystem reads settings.
 *   2. Settings, which the board uses for backlight and idle timeout.
 *   3. capstan_board_init() -- panel, touch, ring, LVGL. Backlight stays off.
 *   4. First screen drawn.
 *   5. Backlight on, so the panel never shows an unpainted frame.
 */

#include <stdio.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "capstan_board.h"
#include "capstan_config.h"
#include "capstan_touch_cal.h"
#include "capstan_mqtt.h"
#include "capstan_model.h"
#include "capstan_wifi.h"
#include "discovery.h"
#include "ui_nav.h"
#include "ui_clock.h"
#include "ui_data.h"
#include "ui_setup.h"

/*
 * The generated UI may not exist yet -- main/ui/<res>/ is disposable and is
 * empty until the first EEZ Studio export. Guarding on the header keeps the
 * project buildable at every stage instead of failing with missing-symbol
 * errors that look like C bugs.
 */
/*
 * CAPSTAN_HAVE_UI comes from main/CMakeLists.txt, which decides it from the
 * glob over main/ui/<res>/.
 *
 * Do NOT change this back to `#if __has_include("ui.h")`. __has_include
 * records no dependency on a file that is absent, so a build done before
 * the first EEZ Studio export bakes in "no UI" and is never rebuilt when
 * the export appears -- the firmware then ships the placeholder screens
 * indefinitely while the export sits on disk looking correct. See the
 * comment in main/CMakeLists.txt.
 */
#ifndef CAPSTAN_HAVE_UI
#  error "CAPSTAN_HAVE_UI is not defined -- main/CMakeLists.txt must set it"
#endif

#define HAVE_GENERATED_UI CAPSTAN_HAVE_UI

#if HAVE_GENERATED_UI
#  include "ui.h"
#endif

static const char *TAG = "capstan";

#if HAVE_GENERATED_UI

/*
 * ui_tick() reads every expression-bound property through its get_var_*
 * accessor, and nothing calls it for us on the device. Without this timer
 * the screen draws perfectly once and then never changes again -- which
 * reads like a data-layer bug and is not.
 *
 * The callback runs in the LVGL task, which already holds the display
 * lock. Taking it again here would deadlock.
 */
static void ui_tick_timer_cb(lv_timer_t *t)
{
    (void)t;
    ui_tick();
}

#else /* ---------------- bring-up scaffolding only ---------------- */

/*
 * BRING-UP SCAFFOLDING -- TO BE DELETED, NOT MIGRATED.
 *
 * Everything in this branch is compiled only while no EEZ Studio export
 * exists. It brought the panel, the ring and touch up before there was a
 * .eez-project, and it disappears from the binary at the first export.
 *
 * THE CALIBRATION SCREEN HERE IS NOT THE PRODUCT SCREEN.
 *
 * It is a user-visible GUI, so it has to match the brand colours, the type
 * scale and the theme -- all defined in EEZ Studio, where a developer can
 * SEE them. None of that is true here: every colour is a hardcoded hex
 * literal duplicating a palette token, and the layout exists nowhere a
 * designer can inspect it.
 *
 * "It is behind a guard" justifies a jig. It does not justify shipping a
 * screen users look at. When the projects are authored, this code is
 * DELETED and the calibration screen is built there:
 *
 *   EEZ Studio owns  five crosshair widgets at the five fixed target
 *                    positions, title, progress and result labels, using
 *                    project colour tokens and fonts
 *   C owns           toggling LV_OBJ_FLAG_HIDDEN on the active crosshair,
 *                    setting label text, running the fit
 *
 * That split works because the targets are at fixed, known positions and
 * can be authored rather than placed at runtime. Positioning an
 * EEZ-authored widget from C is the forbidden case.
 *
 * The calibration ENGINE is not UI and stays where it is, in
 * components/capstan_board/src/capstan_touch_cal.c.
 */

#define TEST_ITEM_COUNT 9

/* Mutually recursive: calibration rebuilds the test screen when it
 * finishes, and a long press on the test screen re-runs calibration. */
static void build_test_screen(void);
static void build_calibration_screen(void);

/* ---- test screen state ---------------------------------------------- */
static lv_obj_t *s_sel_label;
static lv_obj_t *s_dir_label;
static lv_obj_t *s_count_label;
static lv_obj_t *s_press_label;
static lv_obj_t *s_touch_label;
static lv_obj_t *s_range_label;
static lv_obj_t *s_touch_dot;

static int s_sel;
static int s_total_detents;
static int s_presses;
static int s_touches;
static int s_min_x = 9999, s_max_x = -1, s_min_y = 9999, s_max_y = -1;

/* ---- calibration state ---------------------------------------------- */
static capstan_touch_cal_session_t s_cal;
static lv_obj_t *s_cal_cross_h, *s_cal_cross_v, *s_cal_msg, *s_cal_sub;
static uint32_t  s_cal_last_seq;

/* ---------------------------------------------------------------------- */

static void test_rotary_cb(int diff, void *ctx)
{
    (void)ctx;
    if (diff == 0) {
        return;
    }

    /*
     * CLAMP, DO NOT WRAP -- and never store the overshoot.
     *
     * At either end the selection simply stops; turning further does
     * nothing, which is correct and expected. What must never happen is
     * the excess being remembered, so that reversing requires winding back
     * through everything you overshot. Two full turns past the end then
     * one detent back must move by exactly one.
     *
     * The two lines below are what guarantee it: the delta is applied to
     * the DISPLAYED value and the result clamped. No private counter
     * accumulates anywhere, which is the only way this goes wrong.
     */
    s_sel += diff;
    if (s_sel < 0)                   { s_sel = 0; }
    if (s_sel > TEST_ITEM_COUNT - 1) { s_sel = TEST_ITEM_COUNT - 1; }

    s_total_detents += (diff > 0) ? diff : -diff;

    lv_label_set_text_fmt(s_sel_label, "item %d / %d", s_sel + 1, TEST_ITEM_COUNT);
    lv_label_set_text(s_dir_label, diff > 0 ? "right  >>" : "<<  left");
    lv_label_set_text_fmt(s_count_label, "detents: %d", s_total_detents);
}

static void test_press_cb(void *ctx)
{
    (void)ctx;
    s_presses++;
    lv_label_set_text_fmt(s_press_label, "press %d", s_presses);
    ESP_LOGI(TAG, "short press #%d", s_presses);
}

static void test_long_press_cb(void *ctx)
{
    (void)ctx;
    ESP_LOGW(TAG, "long press -> re-running touch calibration");
    lv_obj_clean(lv_screen_active());
    build_calibration_screen();
}

static void test_touch_cb(lv_event_t *e)
{
    (void)e;
    lv_indev_t *indev = lv_indev_active();
    if (!indev) {
        return;
    }
    lv_point_t p;
    lv_indev_get_point(indev, &p);
    const int x = (int)p.x, y = (int)p.y;

    s_touches++;
    if (x < s_min_x) { s_min_x = x; }
    if (x > s_max_x) { s_max_x = x; }
    if (y < s_min_y) { s_min_y = y; }
    if (y > s_max_y) { s_max_y = y; }

    /*
     * A marker at the reported coordinate plus the observed range. That is
     * the whole touch check here.
     *
     * It replaced a five-target hit-scoring grid for two reasons: the
     * calibration screen already proves touch with its own crosshairs, and
     * on the 240 panel the grid's centre target sat directly on top of the
     * `item N / 9` readout -- hiding the thing this screen exists to show.
     *
     * If the marker does not follow your finger, orientation is wrong and
     * nothing else here matters.
     */
    lv_obj_set_pos(s_touch_dot, x - 6, y - 6);
    lv_obj_clear_flag(s_touch_dot, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text_fmt(s_touch_label, "touch %d,%d", x, y);
    lv_label_set_text_fmt(s_range_label, "x %d-%d  y %d-%d",
                          s_min_x, s_max_x, s_min_y, s_max_y);
}

static void build_test_screen(void)
{
    const capstan_board_info_t *b = capstan_board_info();
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

    /*
     * ROUND PANEL, THREE SIZES.
     *
     * Everything is positioned as a fraction of the RADIUS, never in fixed
     * pixels. An earlier version hard-coded offsets for the 480 and put
     * half the readout outside a 240 panel's glass -- an offset of 128 px
     * from centre does not exist when the radius is 120.
     */
    const int R = b->v_res / 2;
    const int f_touch  = -(R * 62) / 100;
    const int f_swatch = -(R * 40) / 100;
    const int f_info   = -(R * 25) / 100;
    const int f_dir    =  (R * 21) / 100;
    const int f_count  =  (R * 42) / 100;
    const int f_press  =  (R * 55) / 100;

    /* Colour check: three swatches, entirely inside the circle. */
    static const struct { uint32_t hex; const char *name; } sw[] = {
        { 0xFF0000, "R" }, { 0x00FF00, "G" }, { 0x0000FF, "B" },
    };
    const int sz = (b->v_res * 12) / 100, gap = (b->v_res * 3) / 100;
    const int total = 3 * sz + 2 * gap;
    for (int i = 0; i < 3; i++) {
        lv_obj_t *box = lv_obj_create(scr);
        lv_obj_remove_style_all(box);
        lv_obj_set_size(box, sz, sz);
        lv_obj_align(box, LV_ALIGN_CENTER,
                     -total / 2 + i * (sz + gap) + sz / 2, f_swatch);
        lv_obj_set_style_bg_color(box, lv_color_hex(sw[i].hex), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(box, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_radius(box, 4, LV_PART_MAIN);

        lv_obj_t *l = lv_label_create(box);
        lv_label_set_text(l, sw[i].name);
        lv_obj_set_style_text_color(l, lv_color_black(), LV_PART_MAIN);
        lv_obj_center(l);
    }

    s_touch_label = lv_label_create(scr);
    lv_label_set_text(s_touch_label, "touch: none yet");
    lv_obj_set_style_text_color(s_touch_label, lv_color_hex(0xFFC107), LV_PART_MAIN);
    lv_obj_align(s_touch_label, LV_ALIGN_CENTER, 0, f_touch);

    s_range_label = lv_label_create(scr);
    lv_label_set_text(s_range_label, "");
    lv_obj_set_style_text_color(s_range_label, lv_color_hex(0x8A8A8A), LV_PART_MAIN);
    lv_obj_align(s_range_label, LV_ALIGN_CENTER, 0, f_touch + R / 8);

    lv_obj_t *info = lv_label_create(scr);
    lv_label_set_text_fmt(info, "%s  %ux%u", b->name, b->h_res, b->v_res);
    lv_obj_set_style_text_color(info, lv_color_hex(0x8A8A8A), LV_PART_MAIN);
    lv_obj_align(info, LV_ALIGN_CENTER, 0, f_info);

    /* The readout. Dead centre -- the only thing here anyone needs to read.
     * Built-in Montserrat on purpose: scaffolding must not depend on the
     * project's own fonts, which exist only after an export. */
    s_sel_label = lv_label_create(scr);
    lv_label_set_text_fmt(s_sel_label, "item 1 / %d", TEST_ITEM_COUNT);
    lv_obj_set_style_text_color(s_sel_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(s_sel_label,
        b->v_res >= 400 ? &lv_font_montserrat_28 :
        b->v_res >= 320 ? &lv_font_montserrat_24 : &lv_font_montserrat_18,
        LV_PART_MAIN);
    lv_obj_align(s_sel_label, LV_ALIGN_CENTER, 0, 0);

    s_dir_label = lv_label_create(scr);
    lv_label_set_text(s_dir_label, "turn the ring");
    lv_obj_set_style_text_color(s_dir_label, lv_color_hex(0x52A441), LV_PART_MAIN);
    lv_obj_set_style_text_font(s_dir_label,
        b->v_res >= 400 ? &lv_font_montserrat_20 :
        b->v_res >= 320 ? &lv_font_montserrat_16 : &lv_font_montserrat_14,
        LV_PART_MAIN);
    lv_obj_align(s_dir_label, LV_ALIGN_CENTER, 0, f_dir);

    s_count_label = lv_label_create(scr);
    lv_label_set_text(s_count_label, "detents: 0");
    lv_obj_set_style_text_color(s_count_label, lv_color_hex(0x6A6A6A), LV_PART_MAIN);
    lv_obj_align(s_count_label, LV_ALIGN_CENTER, 0, f_count);

    s_press_label = lv_label_create(scr);
    lv_label_set_text(s_press_label, "press the ring");
    lv_obj_set_style_text_color(s_press_label, lv_color_hex(0x48E6FE), LV_PART_MAIN);
    lv_obj_align(s_press_label, LV_ALIGN_CENTER, 0, f_press);

    s_touch_dot = lv_obj_create(scr);
    lv_obj_remove_style_all(s_touch_dot);
    lv_obj_set_size(s_touch_dot, 12, 12);
    lv_obj_set_style_radius(s_touch_dot, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_touch_dot, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(s_touch_dot, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_add_flag(s_touch_dot, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_flag(scr, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(scr, test_touch_cb, LV_EVENT_PRESSING, NULL);

    /* Touch ON for this screen only -- every product screen except the
     * keyboard leaves it off, per ui_nav.c's policy table. */
    capstan_board_touch_set_enabled(true);

    capstan_board_set_rotate_callback(test_rotary_cb, NULL);
    capstan_board_set_press_callback(test_press_cb, NULL);
    capstan_board_set_back_callback(test_long_press_cb, NULL);
    ESP_LOGI(TAG, "test screen ready");
}

/* ---------------------------------------------------------------------- *
 * Touch calibration screen (scaffolding -- see the note above).
 *
 * Samples are read through capstan_board_touch_raw() rather than from
 * LVGL, because LVGL's coordinates already carry the correction and
 * calibrating through the transform being calibrated is circular.
 * ---------------------------------------------------------------------- */

static void cal_draw_current(void)
{
    const capstan_touch_cal_point_t *p = &s_cal.pt[s_cal.current];
    lv_obj_set_pos(s_cal_cross_h, p->expect_x - 20, p->expect_y - 1);
    lv_obj_set_pos(s_cal_cross_v, p->expect_x - 1,  p->expect_y - 20);
    lv_label_set_text_fmt(s_cal_msg, "touch the cross   %d / %d",
                          s_cal.current + 1, CAPSTAN_TOUCH_CAL_POINTS);
    lv_label_set_text_fmt(s_cal_sub, "press %d of %d",
                          s_cal.pt[s_cal.current].samples + 1,
                          CAPSTAN_TOUCH_CAL_SAMPLES);
}

static void cal_press_cb(lv_event_t *e)
{
    (void)e;
    if (s_cal.complete) {
        return;
    }

    int rx, ry;
    uint32_t seq = 0;
    if (!capstan_board_touch_raw(&rx, &ry, &seq)) {
        return;
    }

    /*
     * One sample per PHYSICAL press. LVGL can fire an event several times
     * for a single finger-down, so counting callbacks meant all four
     * samples came from one contact -- averaging nothing, which is the
     * entire reason for taking several.
     */
    if (seq == s_cal_last_seq) {
        return;
    }
    s_cal_last_seq = seq;

    /*
     * Reject a press that clearly missed the crosshair. Pairing a known
     * target with a coordinate the user never aimed at is how a
     * confidently wrong transform gets fitted, and a wrong transform is
     * worse than none because it moves every touch on every screen.
     */
    const capstan_touch_cal_point_t *p = &s_cal.pt[s_cal.current];
    const int dx = rx - p->expect_x, dy = ry - p->expect_y;
    if ((dx * dx + dy * dy) > (140 * 140)) {
        lv_label_set_text(s_cal_sub, "missed - touch the cross itself");
        ESP_LOGW(TAG, "rejected sample %d,%d for target %d,%d (too far)",
                 rx, ry, p->expect_x, p->expect_y);
        return;
    }

    capstan_touch_cal_add_sample(&s_cal, rx, ry);

    if (!s_cal.complete) {
        cal_draw_current();
        return;
    }

    capstan_touch_cal_t cal;
    esp_err_t err = capstan_touch_cal_finish(&s_cal, &cal);
    if (err != ESP_OK) {
        lv_label_set_text(s_cal_msg, "calibration failed");
        lv_label_set_text(s_cal_sub, "long-press the ring to retry");
        ESP_LOGE(TAG, "calibration rejected: %s", esp_err_to_name(err));
        return;
    }
    if (capstan_config_set_touch_cal(&cal) != ESP_OK) {
        lv_label_set_text(s_cal_msg, "could not save");
        return;
    }
    capstan_touch_cal_reload();

    ESP_LOGW(TAG, "calibration stored; returning to test screen");
    lv_obj_clean(lv_screen_active());
    build_test_screen();
}

static void build_calibration_screen(void)
{
    const capstan_board_info_t *b = capstan_board_info();
    capstan_touch_cal_begin(&s_cal, b->h_res, b->v_res);
    s_cal_last_seq = 0;

    const int R = b->v_res / 2;
    lv_obj_t *scr = lv_screen_active();
    lv_obj_clean(scr);
    lv_obj_set_style_bg_color(scr, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_t *t = lv_label_create(scr);
    lv_label_set_text(t, "Touch calibration");
    lv_obj_set_style_text_color(t, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(t, LV_ALIGN_CENTER, 0, -(R * 25) / 100);

    s_cal_msg = lv_label_create(scr);
    lv_obj_set_style_text_color(s_cal_msg, lv_color_hex(0x52A441), LV_PART_MAIN);
    lv_obj_align(s_cal_msg, LV_ALIGN_CENTER, 0, -(R * 8) / 100);

    s_cal_sub = lv_label_create(scr);
    lv_obj_set_style_text_color(s_cal_sub, lv_color_hex(0x8A8A8A), LV_PART_MAIN);
    lv_obj_align(s_cal_sub, LV_ALIGN_CENTER, 0, (R * 6) / 100);

    /* A thin crosshair, not a dot -- a big target invites aiming at its
     * edge, which is exactly the error we are trying to measure out. */
    s_cal_cross_h = lv_obj_create(scr);
    lv_obj_remove_style_all(s_cal_cross_h);
    lv_obj_set_size(s_cal_cross_h, 40, 2);
    lv_obj_set_style_bg_color(s_cal_cross_h, lv_color_hex(0xFFC107), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(s_cal_cross_h, LV_OPA_COVER, LV_PART_MAIN);

    s_cal_cross_v = lv_obj_create(scr);
    lv_obj_remove_style_all(s_cal_cross_v);
    lv_obj_set_size(s_cal_cross_v, 2, 40);
    lv_obj_set_style_bg_color(s_cal_cross_v, lv_color_hex(0xFFC107), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(s_cal_cross_v, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_add_flag(scr, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(scr, cal_press_cb, LV_EVENT_PRESSED, NULL);

    capstan_board_touch_set_enabled(true);
    capstan_board_set_rotate_callback(NULL, NULL);
    capstan_board_set_press_callback(NULL, NULL);
    capstan_board_set_back_callback(test_long_press_cb, NULL);

    cal_draw_current();
    ESP_LOGW(TAG, "touch calibration: touch each cross %d times",
             CAPSTAN_TOUCH_CAL_SAMPLES);
}

#endif /* HAVE_GENERATED_UI */

/* ----------------------------------------------------------------------
 * Service task
 *
 * Drains the MQTT queue into the data model and expires readings whose
 * module has gone quiet.
 *
 * 6 KB because the JSON parsing in capstan_mqtt_process() happens on
 * this stack -- deliberately, so it is off the MQTT task and cannot
 * stall the network stack. Priority 4 keeps it below the LVGL task:
 * dropping a frame to parse a payload would be the wrong trade on a
 * display whose job is to look calm.
 * ---------------------------------------------------------------------- */
#define CAPSTAN_SERVICE_TASK_STACK 6144
#define CAPSTAN_SERVICE_TASK_PRIO  4

/* Set at boot when NVS holds no network; acted on once the UI is up. */
static bool s_needs_setup;

static void on_discovery_trigger(void *ctx)
{
    (void)ctx;
    /* Runs on the service task, from capstan_mqtt_process(). The handler
     * spawns its own worker, so this returns immediately. */
    discovery_handle_trigger();
}

static void rf_health_cb(const capstan_wifi_ap_t *aps, size_t count, void *ctx)
{
    (void)ctx;
    int8_t best = -127;
    for (size_t i = 0; i < count; i++) {
        if (aps[i].rssi > best) {
            best = aps[i].rssi;
        }
    }
    if (count) {
        ESP_LOGI(TAG, "RF health: hears %u networks, strongest %d dBm",
                 (unsigned)count, (int)best);
    } else {
        ESP_LOGW(TAG, "RF health: hears NOTHING -- check the antenna");
    }
}

static void service_task(void *arg)
{
    (void)arg;
    uint32_t ticks = 0;
    while (true) {
        /*
         * While a discovery window is open the broker is deliberately down --
         * discovery_task() stopped it so the HTTP server could bind port 80,
         * and it reconnects on the way out. Without this guard
         * ui_data_service_tick() would see "online but not connected", rebuild
         * the client five seconds later, and undo the teardown in the middle
         * of the handshake.
         */
        if (!discovery_is_running()) {
            ui_data_service_tick();     /* connect the broker when online */
        }
        capstan_mqtt_process();

        /* Nothing on this platform is retained, so a reading is only as
         * good as its last frame. Without this a dead module's number
         * sits on the display looking live. */
        capstan_mqtt_check_watchdogs();

        /*
         * Heartbeat, every 10 s.
         *
         * capstan_mqtt_process() drains the WHOLE queue on each call, so
         * a full inbound queue can only mean this loop is not running --
         * and "inbound queue full, dropped 551" looks like a throughput
         * problem, not an absent consumer. This line is what tells the
         * two apart at a glance.
         */
        /* Every 10 s: proof the loop runs AND that data is reaching the
         * model. Between them these separate "consumer stalled" from
         * "consumer fine, nothing arriving" -- two failures that look
         * identical on a screen full of `--`. */
        if (++ticks % 500 == 0) {
            const capstan_value_t v = capstan_model_battery_volts();
            ESP_LOGI(TAG, "service alive: %u B stack free, battery %s",
                     (unsigned)uxTaskGetStackHighWaterMark(NULL),
                     v.valid ? "live" : "--");
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

/* ---------------------------------------------------------------------- */

void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS needs erasing (%s)", esp_err_to_name(err));
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    /* Settings load before the board, which reads the saved backlight and
     * idle timeout. A failure is not fatal -- the compiled-in defaults are
     * serviceable and the user can re-enter settings. */
    const esp_err_t cfg_err = capstan_config_init();
    if (cfg_err != ESP_OK) {
        ESP_LOGE(TAG, "settings failed to load (%s) -- using defaults",
                 esp_err_to_name(cfg_err));
    }

    ESP_ERROR_CHECK(capstan_board_init());

    /*
     * Bring the radio up, but do NOT connect here.
     *
     * The Wi-Fi screen needs a scan the moment it is opened, and
     * esp_wifi_scan_start fails if the driver was never started -- which
     * presented as an empty network list with nothing in the log to say
     * why. Starting the stack at boot and leaving association to the
     * settings flow keeps the two separable.
     */
    const esp_err_t wifi_err = capstan_wifi_init();
    if (wifi_err != ESP_OK) {
        ESP_LOGE(TAG, "wifi init failed (%s) -- scanning and connecting "
                      "will not work", esp_err_to_name(wifi_err));
    }

    /*
     * Rejoin the saved network.
     *
     * capstan_wifi_init() only brings the radio up. Association used to
     * happen ONLY through the settings flow, which meant the panel came
     * back from every reboot offline with perfectly good credentials in
     * NVS, and the only way back on was to retype the passphrase. A
     * wall-mounted display has to come back by itself after a power cut.
     *
     * Nothing is retried here: capstan_wifi_connect() backs off and
     * keeps trying on its own, which is right for a vehicle whose
     * access point may simply be switched off.
     */
    if (capstan_config_is_provisioned() || wifi_err == ESP_OK) {
        capstan_wifi_cfg_t wcfg;
        capstan_config_get_wifi(&wcfg);
        if (wcfg.configured && wcfg.ssid[0]) {
            ESP_LOGI(TAG, "rejoining saved network '%s'", wcfg.ssid);
            capstan_wifi_connect();
        } else {
            /*
             * Nothing saved: this is a new or factory-reset device, so
             * it goes straight into phone-based setup rather than
             * sitting on a menu with no way to get on the network.
             * Entering it is deferred until the UI exists -- the
             * instructions are the whole point, and starting the AP
             * before there is a screen to print them on would leave a
             * nameless network in the air.
             */
            ESP_LOGI(TAG, "no saved network -- entering setup mode");
            s_needs_setup = true;
        }
    }

    /*
     * One-shot RF health check.
     *
     * Logs how many networks this board can hear and how strong the
     * best one is. Identical across all three panels, so the numbers
     * are directly comparable -- which is the only way to tell a board
     * with a poor antenna from a firmware problem. A board that hears
     * far less than its siblings sitting on the same bench has a
     * hardware fault, and no amount of driver configuration will fix
     * it.
     */
    capstan_wifi_scan_start(rf_health_cb, NULL);

    /* The data layer, then the broker client. Neither connects here:
     * capstan_mqtt_connect() needs an IP, so it is driven off the Wi-Fi
     * state callback in ui_data.c. */
    capstan_model_init();
    const esp_err_t mqtt_err = capstan_mqtt_init();
    if (mqtt_err != ESP_OK) {
        ESP_LOGE(TAG, "mqtt init failed (%s) -- no data will arrive",
                 esp_err_to_name(mqtt_err));
    }

    /*
     * Discovery.
     *
     * Overlook's device list is built from an mDNS browse that follows an MQTT
     * broadcast, so a panel that never answers the broadcast is invisible
     * there no matter how healthy it looks otherwise. The trigger arrives
     * through the broker, which is why the callback is registered next to the
     * client rather than with the rest of the UI.
     */
    discovery_init();
    capstan_mqtt_set_discovery_callback(on_discovery_trigger, NULL);

    if (capstan_board_lock(0)) {
#if HAVE_GENERATED_UI
        /*
         * ui_init() -> create_screens() builds EVERY screen up front,
         * because EEZ Studio's export has screensLifetimeSupport off.
         * That is a large, single allocation burst, so the heap is logged
         * either side of it.
         *
         * Worth keeping: when LVGL runs out of memory it does not return
         * an error, it trips LV_ASSERT_MALLOC and spins forever, which
         * presents as a task-watchdog timeout with a backtrace pointing at
         * whatever allocation happened to be unlucky. These two numbers
         * are what tell you it was memory and not the code in the trace.
         */
        const size_t heap_before = esp_get_free_heap_size();

        ui_init();
        lv_timer_create(ui_tick_timer_cb, 20, NULL);
        ui_nav_init();          /* owns screen transitions AND touch policy */
        ui_data_init();         /* model -> widgets, and mqtt connect-on-IP */

        if (s_needs_setup) {
            /* Raise the setup AP now that the Setup screen exists to
             * show its name and password. */
            ui_setup_enter();
        }
        ui_clock_init();        /* idle face -> GNSS time, once a second */

        const size_t heap_after = esp_get_free_heap_size();
        ESP_LOGI(TAG, "generated UI initialised -- %u KB heap used, "
                      "%u KB free",
                 (unsigned)((heap_before - heap_after) / 1024),
                 (unsigned)(heap_after / 1024));
#else
        capstan_touch_cal_t tc;
        capstan_config_get_touch_cal(&tc);
        if (!tc.valid) {
            /* Never calibrated. Touch is off by ~22 px mean on an
             * uncalibrated panel, which is most of a keyboard key. */
            build_calibration_screen();
        } else {
            build_test_screen();
        }
        ESP_LOGW(TAG, "no generated UI -- showing bring-up screens. "
                      "Export from EEZ Studio (Ctrl+B) to replace them.");
#endif
        capstan_board_unlock();
    } else {
        ESP_LOGE(TAG, "could not take the LVGL lock at boot");
    }

    /* First frame is drawn by now; light the panel to the saved level. */
    vTaskDelay(pdMS_TO_TICKS(50));
    capstan_display_cfg_t disp;
    capstan_config_get_display(&disp);
    ESP_ERROR_CHECK(capstan_board_backlight_set(disp.backlight_percent));

    ESP_LOGI(TAG, "running");

    /*
     * Hand the service loop to its own task and let app_main return.
     *
     * It ran inline here first, which overflowed the main task's stack
     * and rebooted the panel every few seconds:
     *
     *   vApplicationStackOverflowHook  <- panic
     *   Backtrace: ... |<-CORRUPTED
     *
     * CONFIG_ESP_MAIN_TASK_STACK_SIZE is 3584 bytes, and
     * capstan_mqtt_process() parses JSON, which does not fit in what is
     * left of it. Raising the main task's stack would have worked and
     * would have been the wrong fix: it makes one number in sdkconfig
     * responsible for whatever anyone later adds to app_main, and the
     * next overflow would look exactly as mysterious as this one.
     *
     * A named task with its own stack says what it needs and keeps the
     * cost next to the code that incurs it.
     */
    BaseType_t ok = xTaskCreate(service_task, "capstan_svc",
                                CAPSTAN_SERVICE_TASK_STACK, NULL,
                                CAPSTAN_SERVICE_TASK_PRIO, NULL);
    if (ok != pdPASS) {
        ESP_LOGE(TAG, "could not start the service task -- no MQTT data "
                      "will be parsed");
    }
}
