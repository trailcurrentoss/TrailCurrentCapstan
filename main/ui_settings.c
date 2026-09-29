/*
 * The settings carousel: what each item shows, what a press on it does, and
 * the LED colour it wants. Factory reset keeps its confirmation step.
 */

#include <stdbool.h>
#include <stdio.h>

#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "capstan_config.h"
#include "capstan_mqtt.h"
#include "capstan_wifi.h"
#include "ui_nav.h"
#include "ui_settings.h"

#ifndef CAPSTAN_HAVE_UI
#  error "CAPSTAN_HAVE_UI is not defined -- main/CMakeLists.txt must set it"
#endif

#if CAPSTAN_HAVE_UI

#include "screens.h"
#include "ui_lv.h"
#include "ui.h"

static const char *TAG = "ui.settings";

/*
 * Factory reset is armed by one press and performed by a second.
 *
 * It erases the saved Wi-Fi and MQTT credentials and reboots, which
 * drops the panel off the network and cannot be undone. It also sits in
 * a list the user is scrolling through with a ring, one row below
 * things they legitimately want to press. A single press is far too
 * little between "turning the knob" and "losing the credentials".
 *
 * Two presses on the same row, with the label changing in between, is
 * the cheapest confirmation that is still a real one: it needs no extra
 * screen, and the changed label means the second press is made
 * knowingly rather than by momentum.
 *
 * The arming times out, so a reset cannot be left half-committed for
 * someone else to finish by accident.
 */
#define ARM_TIMEOUT_US 5000000   /* 5 s */

static int64_t s_armed_us;

/* What the Factory Reset item says once a reset has been confirmed
 * ("Resetting...", "Reset failed"); NULL for the usual armed/idle text. */
static const char *s_reset_note;

static void set_reset_note(const char *text)
{
    s_reset_note = text;
    ui_settings_refresh();
}

static bool armed(void)
{
    return s_armed_us &&
           (esp_timer_get_time() - s_armed_us) < ARM_TIMEOUT_US;
}

void ui_settings_disarm_reset(void)
{
    if (s_armed_us || s_reset_note) {
        s_armed_us = 0;
        set_reset_note(NULL);
    }
}

void ui_settings_factory_reset_pressed(void)
{
    if (!armed()) {
        s_armed_us = esp_timer_get_time();
        set_reset_note(NULL);   /* repaint as armed */
        ESP_LOGW(TAG, "factory reset armed -- press again to confirm");
        return;
    }

    s_armed_us = 0;
    ESP_LOGW(TAG, "factory reset confirmed -- erasing settings");
    set_reset_note("Resetting...");

    const esp_err_t err = capstan_config_factory_reset();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "factory reset failed: %s", esp_err_to_name(err));
        set_reset_note("Reset failed");
        return;
    }

    /*
     * Let the label paint before rebooting.
     *
     * esp_restart() from inside an LVGL event handler cuts the frame off
     * mid-render, so the user gets an instant blank screen with no
     * confirmation that anything happened -- indistinguishable from a
     * crash. A short delay on a timer lets "Resetting..." reach the
     * glass first.
     *
     * The delay runs on the esp_timer task, NOT here: this is called
     * with the LVGL lock held, and sleeping while holding it stalls the
     * display for the duration.
     */
    const esp_timer_create_args_t args = {
        .callback = (esp_timer_cb_t)esp_restart,
        .name = "factory_reset_reboot",
    };
    esp_timer_handle_t t;
    if (esp_timer_create(&args, &t) == ESP_OK) {
        esp_timer_start_once(t, 600000);   /* 600 ms */
    } else {
        esp_restart();   /* could not defer; reboot now rather than not */
    }
}

/*
 * Alarm snooze. A handful of steps rather than a free value: this is set
 * with one ring press at a time, and nobody needs 17 minutes.
 */
static const uint16_t SNOOZE_STEPS_MIN[] = { 5, 10, 15, 30, 60 };
#define SNOOZE_STEP_COUNT (sizeof(SNOOZE_STEPS_MIN) / sizeof(*SNOOZE_STEPS_MIN))

void ui_settings_snooze_text(char *out, size_t len)
{
    capstan_display_cfg_t d;
    capstan_config_get_display(&d);
    snprintf(out, len, "%u min", (unsigned)d.alarm_snooze_min);
}

void ui_settings_snooze_pressed(void)
{
    capstan_display_cfg_t d;
    capstan_config_get_display(&d);

    /* Next step above the current value, wrapping. A value that is not on
     * the list (a different Kconfig default) steps to the first one above
     * it, so the cycle is always reachable. */
    uint16_t next = SNOOZE_STEPS_MIN[0];
    for (size_t i = 0; i < SNOOZE_STEP_COUNT; i++) {
        if (SNOOZE_STEPS_MIN[i] > d.alarm_snooze_min) {
            next = SNOOZE_STEPS_MIN[i];
            break;
        }
    }
    d.alarm_snooze_min = next;

    const esp_err_t err = capstan_config_set_display(&d);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "snooze save failed: %s", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(TAG, "alarm snooze -> %u min", (unsigned)next);
}

/*
 * Clock timeout: how long an app stays up without input before the idle
 * timer in ui_nav.c returns to the clock. 0 is "Never" -- that timer already
 * treats 0 as disabled -- and it sits last so the cycle passes through every
 * real value first.
 */
static const uint16_t TIMEOUT_STEPS_S[] = { 15, 30, 60, 120, 300, 0 };
#define TIMEOUT_STEP_COUNT (sizeof(TIMEOUT_STEPS_S) / sizeof(*TIMEOUT_STEPS_S))

void ui_settings_timeout_text(char *out, size_t len)
{
    capstan_display_cfg_t d;
    capstan_config_get_display(&d);
    const unsigned s = d.idle_timeout_s;
    if (s == 0) {
        snprintf(out, len, "Never");
    } else if (s < 60) {
        snprintf(out, len, "%u s", s);
    } else if (s % 60 == 0) {
        snprintf(out, len, "%u min", s / 60);
    } else {
        snprintf(out, len, "%um %us", s / 60, s % 60);
    }
}

void ui_settings_timeout_pressed(void)
{
    capstan_display_cfg_t d;
    capstan_config_get_display(&d);

    /* The step after the current one, wrapping. A value not on the list
     * (another Kconfig default) steps to the first real value above it. */
    size_t next = 0;
    bool found = false;
    for (size_t i = 0; i < TIMEOUT_STEP_COUNT; i++) {
        if (TIMEOUT_STEPS_S[i] == d.idle_timeout_s) {
            next = (i + 1) % TIMEOUT_STEP_COUNT;
            found = true;
            break;
        }
    }
    if (!found) {
        for (size_t i = 0; i < TIMEOUT_STEP_COUNT; i++) {
            if (TIMEOUT_STEPS_S[i] > d.idle_timeout_s) {
                next = i;
                break;
            }
        }
    }
    d.idle_timeout_s = TIMEOUT_STEPS_S[next];

    const esp_err_t err = capstan_config_set_display(&d);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "timeout save failed: %s", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(TAG, "clock timeout -> %u s (0 = never)",
             (unsigned)d.idle_timeout_s);
}

/*
 * Theme. EEZ's change_color_theme() rewrites the shared styles' colours
 * from the palette's light or dark column; every colour on every screen is
 * a palette token, so that is the whole theme. LVGL's default theme, which
 * EEZ initialises dark whatever the palette says, is switched with it: it
 * still colours the few parts no Capstan style overrides, and a dark
 * scrollbar or focus ring left on a light screen would be a contrast bug of
 * our own making. report_style_change() then makes every object re-read
 * its styles.
 */
void ui_settings_apply_theme(void)
{
    capstan_display_cfg_t d;
    capstan_config_get_display(&d);

    change_color_theme(d.dark_theme ? THEME_ID_DARK : THEME_ID_DEFAULT);

    lv_display_t *disp = lv_display_get_default();
    lv_theme_t *th = lv_theme_default_init(disp,
                                           lv_palette_main(LV_PALETTE_BLUE),
                                           lv_palette_main(LV_PALETTE_RED),
                                           d.dark_theme, LV_FONT_DEFAULT);
    lv_display_set_theme(disp, th);
    lv_obj_report_style_change(NULL);
    ESP_LOGI(TAG, "theme -> %s", d.dark_theme ? "dark" : "light");
}

void ui_settings_theme_pressed(void)
{
    capstan_display_cfg_t d;
    capstan_config_get_display(&d);
    d.dark_theme = !d.dark_theme;

    const esp_err_t err = capstan_config_set_display(&d);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "theme save failed: %s", esp_err_to_name(err));
        return;
    }
    ui_settings_apply_theme();
}

const char *ui_settings_theme_text(void)
{
    capstan_display_cfg_t d;
    capstan_config_get_display(&d);
    return d.dark_theme ? "Dark" : "Light";
}

/* ----------------------------------------------------------------------
 * The carousel
 * ---------------------------------------------------------------------- */

/* Glyphs from the full `fa` face (not ui_icons.h, which is for the menu's
 * reduced `fh` subset). Server is Capstan's one addition to the house set. */
#define G_WIFI    "\xEF\x87\xAB"   /* 0xF1EB wifi                 */
#define G_SERVER  "\xEF\x88\xB3"   /* 0xF233 server               */
#define G_THEME   "\xEF\x86\x86"   /* 0xF186 moon                 */
#define G_LOCALE  "\xEF\x82\xAC"   /* 0xF0AC globe                */
#define G_GUIDE   "\xEF\x81\x9A"   /* 0xF05A circle-info          */
#define G_BELL    "\xEF\x83\xB3"   /* 0xF0F3 bell                 */
#define G_CLOCK   "\xEF\x80\x97"   /* 0xF017 clock                */
#define G_ALERT   "\xEF\x81\xB1"   /* 0xF071 triangle-exclamation */

/* SETTINGS_ITEMS in GUI/tmp/screens_layout.py, index for index; the press
 * switch in ui_nav_press() acts on the same indices. */
static const struct {
    const char *icon;
    const char *title;
} s_items[UI_SETTINGS_ITEM_COUNT] = {
    { G_WIFI,   "Wi-Fi"         },
    { G_SERVER, "MQTT"          },
    { G_THEME,  "Theme"         },
    { G_LOCALE, "Locale"        },
    { G_GUIDE,  "Getting Started" },
    { G_BELL,   "Alarm Snooze"  },
    { G_CLOCK,  "Clock Timeout" },
    { G_ALERT,  "Factory Reset" },
};

/* Value colours, as SettingsValue states. */
#define ST_OK     LV_STATE_CHECKED    /* connected: green   */
#define ST_WARN   LV_STATE_PRESSED    /* not connected: amber */
#define ST_DANGER LV_STATE_DISABLED   /* reset armed: red   */
#define ST_MASK   (ST_OK | ST_WARN | ST_DANGER)

static int wrap(int i)
{
    const int n = UI_SETTINGS_ITEM_COUNT;
    return ((i % n) + n) % n;
}

/* Wi-Fi's value: the network name when connected, otherwise what is wrong.
 * The failure reason, not just "failed": a wrong passphrase and an AP that
 * is switched off need different things from the user, and the retry
 * backoff means the state lasts long enough to read. */
static const char *wifi_value(bool *ok)
{
    /* The SSID is returned, so it cannot live in `c` on this stack frame --
     * that pointer dangled and the value line showed garbage. */
    static char s_ssid[sizeof(((capstan_wifi_cfg_t *)0)->ssid)];
    capstan_wifi_cfg_t c;
    capstan_config_get_wifi(&c);
    *ok = false;
    switch (capstan_wifi_state()) {
    case CAPSTAN_WIFI_CONNECTED:
        *ok = true;
        if (!c.ssid[0]) {
            return "Connected";
        }
        snprintf(s_ssid, sizeof(s_ssid), "%s", c.ssid);
        return s_ssid;
    case CAPSTAN_WIFI_CONNECTING: return "Connecting...";
    case CAPSTAN_WIFI_SCANNING:   return "Scanning...";
    case CAPSTAN_WIFI_FAILED:     return capstan_wifi_last_error();
    default:                      return c.configured ? "Offline" : "Not set";
    }
}

static const char *mqtt_value(bool *ok)
{
    *ok = capstan_mqtt_is_connected();
    if (*ok) {
        return "Connected";
    }
    capstan_mqtt_cfg_t m;
    capstan_config_get_mqtt(&m);
    return m.configured ? "Offline" : "Not set";
}

static void locale_refresh(void);

void ui_settings_refresh(void)
{
    locale_refresh();   /* change-only, so free when nothing moved */

    const int sel = wrap(ui_nav_selection_of(CAPSTAN_SCREEN_SETTINGS));

    ui_lv_set_text(objects.settings_hero_icon, s_items[sel].icon);
    ui_lv_set_text(objects.settings_prev_icon, s_items[wrap(sel - 1)].icon);
    ui_lv_set_text(objects.settings_next_icon, s_items[wrap(sel + 1)].icon);
    ui_lv_set_text(objects.settings_name, s_items[sel].title);

    char buf[24];
    const char *value = "";
    lv_state_t st = 0;
    bool danger = false;
    bool ok;
    switch (sel) {
    case UI_SETTINGS_WIFI:
        value = wifi_value(&ok);
        st = ok ? ST_OK : ST_WARN;
        break;
    case UI_SETTINGS_MQTT:
        value = mqtt_value(&ok);
        st = ok ? ST_OK : ST_WARN;
        break;
    case UI_SETTINGS_THEME:
        value = ui_settings_theme_text();
        break;
    case UI_SETTINGS_LOCALE:
        ui_settings_locale_text(buf, sizeof(buf));
        value = buf;
        break;
    case UI_SETTINGS_GUIDE: {
        capstan_display_cfg_t d;
        capstan_config_get_display(&d);
        value = d.hide_guide ? "Hidden" : "Shown";
        break;
    }
    case UI_SETTINGS_SNOOZE:
        ui_settings_snooze_text(buf, sizeof(buf));
        value = buf;
        break;
    case UI_SETTINGS_TIMEOUT:
        ui_settings_timeout_text(buf, sizeof(buf));
        value = buf;
        break;
    case UI_SETTINGS_RESET:
        if (s_reset_note) {
            value = s_reset_note;
            st = ST_DANGER;
            danger = true;
        } else if (armed()) {
            value = "Press again to reset";
            st = ST_DANGER;
            danger = true;
        } else {
            value = "Press twice";
        }
        break;
    default:
        break;
    }
    ui_lv_set_text(objects.settings_value, value);
    ui_lv_set_state_in(objects.settings_value, ST_MASK, st);
    ui_lv_set_state_in(objects.settings_hero, LV_STATE_CHECKED,
                       danger ? LV_STATE_CHECKED : 0);
    ui_lv_set_state_in(objects.settings_hero_icon, LV_STATE_CHECKED,
                       danger ? LV_STATE_CHECKED : 0);

    lv_obj_t *const dots[UI_SETTINGS_ITEM_COUNT] = {
        objects.settings_dot0, objects.settings_dot1, objects.settings_dot2,
        objects.settings_dot3, objects.settings_dot4, objects.settings_dot5,
        objects.settings_dot6, objects.settings_dot7,
    };
    for (int i = 0; i < UI_SETTINGS_ITEM_COUNT; i++) {
        ui_lv_set_state_in(dots[i], LV_STATE_CHECKED,
                           i == sel ? LV_STATE_CHECKED : 0);
    }
}

/* ----------------------------------------------------------------------
 * Locale
 * ---------------------------------------------------------------------- */

void ui_settings_locale_text(char *out, size_t len)
{
    capstan_display_cfg_t d;
    capstan_config_get_display(&d);
    snprintf(out, len, "%s \xC2\xB7 %s",
             d.celsius ? "\xC2\xB0" "C" : "\xC2\xB0" "F",
             d.level_mm ? "mm" : "in");
}

static void locale_refresh(void)
{
    capstan_display_cfg_t d;
    capstan_config_get_display(&d);
    ui_lv_set_text(objects.locale_item0_value,
                   d.celsius ? "\xC2\xB0" "C" : "\xC2\xB0" "F");
    ui_lv_set_text(objects.locale_item1_value, d.level_mm ? "mm" : "in");
}

/*
 * Getting Started in the app menu: shown or hidden. Once someone has been
 * through it they need not see it again; a factory reset erases NVS and so
 * brings it back for the next person.
 */
void ui_settings_guide_pressed(void)
{
    capstan_display_cfg_t d;
    capstan_config_get_display(&d);
    d.hide_guide = !d.hide_guide;
    const esp_err_t err = capstan_config_set_display(&d);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "guide save failed: %s", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(TAG, "getting started -> %s", d.hide_guide ? "hidden" : "shown");
}

void ui_settings_locale_pressed(int row)
{
    capstan_display_cfg_t d;
    capstan_config_get_display(&d);
    switch (row) {
    case UI_LOCALE_TEMPERATURE: d.celsius  = !d.celsius;  break;
    case UI_LOCALE_LEVELING:    d.level_mm = !d.level_mm; break;
    default: return;
    }
    const esp_err_t err = capstan_config_set_display(&d);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "locale save failed: %s", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(TAG, "locale -> %s, %s", d.celsius ? "C" : "F",
             d.level_mm ? "mm" : "in");
    locale_refresh();
}

bool ui_settings_led(uint8_t *r, uint8_t *g, uint8_t *b)
{
    bool ok;
    switch (wrap(ui_nav_selection_of(CAPSTAN_SCREEN_SETTINGS))) {
    case UI_SETTINGS_WIFI: (void)wifi_value(&ok); break;
    case UI_SETTINGS_MQTT: (void)mqtt_value(&ok); break;
    default:               return false;
    }
    if (ok) {
        *r = 82;  *g = 164; *b = 65;   /* AccentPrimary #52a441, brand green */
    } else {
        *r = 255; *g = 96;  *b = 0;    /* the Climate screen's orange */
    }
    return true;
}

#else

void ui_settings_refresh(void) { }
void ui_settings_locale_pressed(int row) { (void)row; }
void ui_settings_guide_pressed(void) { }
void ui_settings_locale_text(char *out, size_t len) { if (len) out[0] = '\0'; }
bool ui_settings_led(uint8_t *r, uint8_t *g, uint8_t *b)
{
    (void)r; (void)g; (void)b;
    return false;
}
void ui_settings_theme_pressed(void) { }
void ui_settings_timeout_pressed(void) { }
void ui_settings_timeout_text(char *out, size_t len) { if (len) out[0] = '\0'; }
void ui_settings_apply_theme(void) { }
const char *ui_settings_theme_text(void) { return ""; }
void ui_settings_factory_reset_pressed(void) { }
void ui_settings_disarm_reset(void) { }
void ui_settings_snooze_pressed(void) { }
void ui_settings_snooze_text(char *out, size_t len) { if (len) out[0] = '\0'; }

#endif
