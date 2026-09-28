/*
 * Settings screen behaviour.
 *
 * Today that is one thing: factory reset, and the confirmation in front
 * of it.
 */

#include <stdbool.h>

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
    if (objects.settings_item3_title) {
        lv_label_set_text(objects.settings_item3_title, text);
    }
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

#else

void ui_settings_factory_reset_pressed(void) { }
void ui_settings_disarm_reset(void) { }

#endif
