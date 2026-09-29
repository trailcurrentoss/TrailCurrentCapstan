/*
 * Settings screen behaviour: the alarm snooze interval, and factory reset
 * with the confirmation in front of it.
 */

#include <stdbool.h>
#include <stdio.h>

#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "capstan_config.h"
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

static void set_row_label(const char *text)
{
    ui_lv_set_text(objects.settings_item5_title, text);
}

static bool armed(void)
{
    return s_armed_us &&
           (esp_timer_get_time() - s_armed_us) < ARM_TIMEOUT_US;
}

void ui_settings_disarm_reset(void)
{
    if (s_armed_us) {
        s_armed_us = 0;
        set_row_label("Factory Reset");
    }
}

void ui_settings_factory_reset_pressed(void)
{
    if (!armed()) {
        s_armed_us = esp_timer_get_time();
        set_row_label("Confirm Reset?");
        ESP_LOGW(TAG, "factory reset armed -- press again to confirm");
        return;
    }

    s_armed_us = 0;
    ESP_LOGW(TAG, "factory reset confirmed -- erasing settings");
    set_row_label("Resetting...");

    const esp_err_t err = capstan_config_factory_reset();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "factory reset failed: %s", esp_err_to_name(err));
        set_row_label("Reset failed");
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

#else

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
