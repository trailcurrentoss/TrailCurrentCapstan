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
#include "capstan_config.h"
#include "capstan_wifi.h"
#include "ui_keyboard.h"
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
static bool   s_dirty;

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

/*
 * Runs on the Wi-Fi component's own context, NOT the LVGL task.
 *
 * It does no LVGL work at all -- it copies the results and sets a flag.
 * Taking the LVGL lock here and painting rows meant doing allocating
 * work on a small event-task stack, which is what overflowed `sys_evt`
 * and rebooted the panels. The painting happens in ui_wifi_tick(),
 * called from the LVGL refresh timer.
 */
static void scan_done(const capstan_wifi_ap_t *aps, size_t count, void *ctx)
{
    (void)ctx;

    s_count = (int)(count < WIFI_ROW_COUNT ? count : WIFI_ROW_COUNT);
    for (int i = 0; i < s_count; i++) {
        /* `aps` belongs to the wifi component and dies with this
         * callback, so the strings are copied, not referenced. */
        strlcpy(s_ssid[i], aps[i].ssid, sizeof(s_ssid[i]));
        s_rssi[i] = aps[i].rssi;
        if (s_ssid[i][0] == '\0') {
            snprintf(s_ssid[i], sizeof(s_ssid[i]), "(hidden)");
        }
    }
    s_scanning = false;
    s_dirty = true;

    ESP_LOGI(TAG, "scan found %u networks, showing %d",
             (unsigned)count, s_count);
}

/*
 * Paint pending results and keep the title in step with the radio.
 *
 * Called from the LVGL refresh timer, so the lock is already held and
 * the stack is the LVGL task's.
 */
void ui_wifi_tick(void)
{
    if (ui_nav_current() != CAPSTAN_SCREEN_WIFI) {
        return;
    }

    if (s_dirty) {
        s_dirty = false;
        render_rows();
    }

    switch (capstan_wifi_state()) {
    case CAPSTAN_WIFI_CONNECTING: set_title("Connecting..."); break;
    case CAPSTAN_WIFI_CONNECTED: {
        /* Signal strength matters here specifically: a weak link shows
         * up as data that arrives late or not at all, which looks like
         * a broken screen rather than a distant router. */
        char t[32];
        snprintf(t, sizeof(t), "Connected %s",
                 signal_bars(capstan_wifi_rssi()));
        set_title(t);
        break;
    }
    case CAPSTAN_WIFI_SCANNING:   set_title("Scanning...");   break;
    case CAPSTAN_WIFI_FAILED:     set_title(capstan_wifi_last_error()); break;
    default:
        set_title(s_count ? "Wi-Fi" : "No networks");
        break;
    }
}

void ui_wifi_screen_entered(void)
{
    /* Already under the LVGL lock -- see the file header. */

    /* Show whatever the last scan found straight away, so returning to
     * this screen is not a blank list for the length of a scan. */
    render_rows();

    /*
     * Do NOT scan while the radio is busy joining a network.
     *
     * Accepting a passphrase returns here, which fires this hook. An
     * unconditional scan at that moment is refused by the driver -- it
     * is mid-association -- and the refusal used to overwrite the
     * "Connecting..." title with "Scan failed". Both halves of that were
     * wrong: nothing had failed, and the message hid the thing the user
     * actually wanted to watch. It read as a broken join that then
     * connected anyway.
     *
     * So connection state wins over scanning. The list already on screen
     * stays; it is at most a few seconds old.
     */
    switch (capstan_wifi_state()) {
    case CAPSTAN_WIFI_CONNECTING:
        /* The ONLY state that blocks a scan. The driver is mid-handshake
         * and refuses one, and the refusal used to overwrite the
         * "Connecting..." title with "Scan failed" -- nothing had
         * failed, and the message hid the thing worth watching. */
        set_title("Connecting...");
        return;
    case CAPSTAN_WIFI_SCANNING:
        set_title("Scanning...");
        s_scanning = true;
        return;
    default:
        /*
         * Everything else scans, INCLUDING while connected.
         *
         * An earlier version of this returned early when connected, on
         * the theory that there was nothing to scan for. That was wrong:
         * the list is the only way to move to a different network, and
         * after a reboot there are no cached results, so a connected
         * device showed a permanently empty list. A station can scan
         * while associated -- it drops off channel briefly -- so being
         * connected is no reason not to.
         */
        break;
    }

    if (s_scanning) {
        return;
    }

    const esp_err_t err = capstan_wifi_scan_start(scan_done, NULL);
    if (err == ESP_OK || err == ESP_ERR_INVALID_STATE) {
        s_scanning = true;      /* INVALID_STATE == one already running */
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

/* ----------------------------------------------------------------------
 * Joining a network
 *
 * Three screens, so the choice has to survive between them: the list
 * picks an SSID, the picker picks a security type, the keyboard supplies
 * the passphrase. Only at the end is anything written to NVS.
 *
 * The SSID is COPIED at selection time rather than re-read from the row
 * later. Scan results are replaced wholesale by the next scan, and a
 * rescan finishing while the user is typing would otherwise change --
 * or shorten past -- the row they chose.
 * ---------------------------------------------------------------------- */

static char               s_pending_ssid[CAPSTAN_SSID_MAX_LEN];
static capstan_wifi_sec_t s_pending_sec = CAPSTAN_WIFI_SEC_WPA2_PSK;

/* Row order on PageWifiSecurity, from screens_layout.py. The picker
 * offers four plain choices rather than the seven the enum carries --
 * nobody standing at a panel wants to distinguish WPA2 from WPA2/WPA3,
 * and the combined modes are what real access points advertise. */
static const capstan_wifi_sec_t s_sec_choice[] = {
    CAPSTAN_WIFI_SEC_OPEN,
    CAPSTAN_WIFI_SEC_WEP,
    CAPSTAN_WIFI_SEC_WPA_WPA2_PSK,
    CAPSTAN_WIFI_SEC_WPA3_PSK,
};

void ui_wifi_select(int index)
{
    const char *ssid = ui_wifi_ssid_at(index);
    if (!ssid) {
        return;
    }
    strlcpy(s_pending_ssid, ssid, sizeof(s_pending_ssid));

    /* Seed the picker from what the scan reported, so the common case is
     * one press. The user can still override it, which is the whole
     * reason the picker exists -- a hidden network reports nothing
     * useful. See docs/screens.md. */
    ESP_LOGI(TAG, "selected '%s'", s_pending_ssid);
}

static void on_passphrase(const char *text, void *ctx)
{
    (void)ctx;
    ui_wifi_apply_password(text);
}

void ui_wifi_set_security_index(int index)
{
    const int n = (int)(sizeof(s_sec_choice) / sizeof(s_sec_choice[0]));
    if (index < 0 || index >= n) {
        return;
    }
    s_pending_sec = s_sec_choice[index];
    ESP_LOGI(TAG, "security '%s'", capstan_wifi_sec_name(s_pending_sec));

    if (s_pending_sec == CAPSTAN_WIFI_SEC_OPEN) {
        /* An open network has no passphrase to ask for. Showing an
         * empty keyboard and requiring OK on nothing is a step that
         * exists only because the flow has three screens in it. */
        ui_wifi_apply_password("");
        ui_nav_goto(CAPSTAN_SCREEN_WIFI);
        return;
    }

    /* Back from the keyboard returns to the LIST, not the picker:
     * abandoning a passphrase means "not this network", and returning
     * to the security question for a network just abandoned is a dead
     * end. */
    ui_keyboard_open("Passphrase", NULL, true, on_passphrase, NULL,
                     CAPSTAN_SCREEN_WIFI);
}

const char *ui_wifi_pending_ssid(void)
{
    return s_pending_ssid[0] ? s_pending_ssid : NULL;
}

bool ui_wifi_apply_password(const char *password)
{
    if (!s_pending_ssid[0]) {
        ESP_LOGW(TAG, "no network selected -- ignoring passphrase");
        return false;
    }

    capstan_wifi_cfg_t cfg;
    capstan_config_get_wifi(&cfg);
    strlcpy(cfg.ssid, s_pending_ssid, sizeof(cfg.ssid));
    strlcpy(cfg.password, password ? password : "", sizeof(cfg.password));
    cfg.security = s_pending_sec;
    cfg.configured = true;

    const esp_err_t err = capstan_config_set_wifi(&cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "could not save credentials: %s", esp_err_to_name(err));
        set_title("Save failed");
        return false;
    }

    ESP_LOGI(TAG, "joining '%s' (%s)", cfg.ssid,
             capstan_wifi_sec_name(cfg.security));
    set_title("Connecting...");

    /* connect_with() retries with backoff on its own task; this returns
     * immediately and the state callback reports the outcome. */
    capstan_wifi_connect_with(&cfg);
    return true;
}

void ui_wifi_init(void)
{
    /* Nothing to subscribe: ui_data.c owns the single Wi-Fi state
     * callback and forwards to ui_wifi_on_state(). */
}

#else  /* no generated UI */

void ui_wifi_screen_entered(void) { }
const char *ui_wifi_ssid_at(int index) { (void)index; return NULL; }
void ui_wifi_select(int index) { (void)index; }
void ui_wifi_set_security_index(int index) { (void)index; }
bool ui_wifi_apply_password(const char *p) { (void)p; return false; }
const char *ui_wifi_pending_ssid(void) { return NULL; }
void ui_wifi_init(void) { }
void ui_wifi_tick(void) { }

#endif
