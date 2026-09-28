/*
 * Wi-Fi screen: scan, then fill the authored rows.
 *
 * THREADING
 *
 * capstan_wifi's scan callback runs on the system event task, which does
 * NOT hold the LVGL lock. Touching a widget from there without taking it
 * races the LVGL task mid-render, and the corruption that follows appears
 * nowhere near the Wi-Fi code. Every widget access below is inside
 * capstan_board_lock().
 *
 * The one exception is ui_wifi_screen_entered(), which ui_nav calls during
 * a screen transition and therefore already runs under the lock -- taking
 * it again there would deadlock a non-recursive mutex.
 */

#include <string.h>

#include "esp_log.h"

#include "capstan_board.h"
#include "capstan_wifi.h"
#include "ui_nav.h"
#include "ui_wifi.h"

#ifndef CAPSTAN_HAVE_UI
#  error "CAPSTAN_HAVE_UI is not defined -- main/CMakeLists.txt must set it"
#endif

#if CAPSTAN_HAVE_UI

#include "screens.h"
#include "ui.h"

static const char *TAG = "ui.wifi";

/*
 * Rows authored on the page. MUST match WIFI_ROW_COUNT in
 * GUI/tmp/screens_layout.py -- there is no way to read it back out of the
 * export, so the assert below is the only thing keeping them in step.
 */
#define WIFI_ROW_COUNT 10

/* The scan can return more than we can show; the rest are dropped. */
static char   s_ssid[WIFI_ROW_COUNT][CAPSTAN_SSID_MAX_LEN];
static int8_t s_rssi[WIFI_ROW_COUNT];
static int    s_count;
static bool   s_scanning;

static lv_obj_t *wifi_row(int i)
{
    switch (i) {
    case 0: return objects.wifi_item0;
    case 1: return objects.wifi_item1;
    case 2: return objects.wifi_item2;
    case 3: return objects.wifi_item3;
    case 4: return objects.wifi_item4;
    case 5: return objects.wifi_item5;
    case 6: return objects.wifi_item6;
    case 7: return objects.wifi_item7;
    case 8: return objects.wifi_item8;
    case 9: return objects.wifi_item9;
    default: return NULL;
    }
}

static lv_obj_t *wifi_row_title(int i)
{
    switch (i) {
    case 0: return objects.wifi_item0_title;
    case 1: return objects.wifi_item1_title;
    case 2: return objects.wifi_item2_title;
    case 3: return objects.wifi_item3_title;
    case 4: return objects.wifi_item4_title;
    case 5: return objects.wifi_item5_title;
    case 6: return objects.wifi_item6_title;
    case 7: return objects.wifi_item7_title;
    case 8: return objects.wifi_item8_title;
    case 9: return objects.wifi_item9_title;
    default: return NULL;
    }
}

static lv_obj_t *wifi_row_value(int i)
{
    switch (i) {
    case 0: return objects.wifi_item0_value;
    case 1: return objects.wifi_item1_value;
    case 2: return objects.wifi_item2_value;
    case 3: return objects.wifi_item3_value;
    case 4: return objects.wifi_item4_value;
    case 5: return objects.wifi_item5_value;
    case 6: return objects.wifi_item6_value;
    case 7: return objects.wifi_item7_value;
    case 8: return objects.wifi_item8_value;
    case 9: return objects.wifi_item9_value;
    default: return NULL;
    }
}

/*
 * Signal as bars rather than dBm.
 *
 * -67 dBm means nothing to most people and the sign invites reading it
 * backwards. Thresholds are the usual ones: -55 excellent, -67 good
 * (the floor for reliable video), -75 workable, below that marginal.
 */
static const char *signal_bars(int8_t rssi)
{
    if (rssi >= -55) { return "||||"; }
    if (rssi >= -67) { return "|||"; }
    if (rssi >= -75) { return "||"; }
    return "|";
}

/* Repaint the rows from the arrays. Caller holds the LVGL lock. */
static void render_rows(void)
{
    for (int i = 0; i < WIFI_ROW_COUNT; i++) {
        lv_obj_t *row = wifi_row(i);
        lv_obj_t *ttl = wifi_row_title(i);
        if (!row || !ttl) {
            continue;
        }
        if (i < s_count) {
            lv_label_set_text(ttl, s_ssid[i]);
            lv_obj_t *val = wifi_row_value(i);
            if (val) {
                lv_label_set_text(val, signal_bars(s_rssi[i]));
            }
            lv_obj_remove_flag(row, LV_OBJ_FLAG_HIDDEN);
        } else {
            /*
             * Hidden, not blanked. ui_nav counts only VISIBLE rows, so
             * hiding is what stops the ring selecting empty space below
             * the last network -- clearing the text would leave a
             * selectable blank row behind.
             */
            lv_obj_add_flag(row, LV_OBJ_FLAG_HIDDEN);
        }
    }
    ui_nav_selection_changed();
}

static void set_title(const char *text)
{
    if (objects.wifi_title) {
        lv_label_set_text(objects.wifi_title, text);
    }
}

/* Runs on the system event task -- no LVGL lock held. */
static void scan_done(const capstan_wifi_ap_t *aps, size_t count, void *ctx)
{
    (void)ctx;

    s_count = (int)(count < WIFI_ROW_COUNT ? count : WIFI_ROW_COUNT);
    for (int i = 0; i < s_count; i++) {
        /* `aps` belongs to the wifi component and dies with this callback,
         * so the strings are copied, not referenced. */
        strlcpy(s_ssid[i], aps[i].ssid, sizeof(s_ssid[i]));
        s_rssi[i] = aps[i].rssi;
        if (s_ssid[i][0] == '\0') {
            snprintf(s_ssid[i], sizeof(s_ssid[i]), "(hidden)");
        }
    }
    s_scanning = false;

    ESP_LOGI(TAG, "scan found %u networks, showing %d",
             (unsigned)count, s_count);

    if (!capstan_board_lock(0)) {
        ESP_LOGW(TAG, "could not take the LVGL lock; results not shown");
        return;
    }
    if (ui_nav_current() == CAPSTAN_SCREEN_WIFI) {
        render_rows();
        set_title(s_count ? "Wi-Fi" : "No networks");
    }
    capstan_board_unlock();
}

void ui_wifi_screen_entered(void)
{
    /* Already under the LVGL lock -- see the file header. */
    if (s_scanning) {
        return;
    }

    /* Show whatever the last scan found straight away, so returning to
     * this screen is not a blank list for the length of a scan. */
    render_rows();

    const esp_err_t err = capstan_wifi_scan_start(scan_done, NULL);
    if (err == ESP_OK) {
        s_scanning = true;
        set_title("Scanning...");
    } else if (err == ESP_ERR_INVALID_STATE) {
        s_scanning = true;      /* one was already running */
        set_title("Scanning...");
    } else {
        ESP_LOGW(TAG, "scan refused: %s", esp_err_to_name(err));
        set_title("Scan failed");
    }
}

const char *ui_wifi_ssid_at(int index)
{
    if (index < 0 || index >= s_count) {
        return NULL;
    }
    return s_ssid[index];
}

#else  /* no generated UI */

void ui_wifi_screen_entered(void) { }
const char *ui_wifi_ssid_at(int index) { (void)index; return NULL; }

#endif
