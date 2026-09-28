/*
 * MQTT settings.
 *
 * Each row opens the shared keyboard for one field and writes the result
 * back to the in-memory copy; nothing reaches NVS until Save. That way
 * abandoning the screen half-edited leaves the working config alone,
 * which matters because the broker settings are how the panel gets its
 * data -- a partial edit that took effect immediately would disconnect
 * it with no obvious way back.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"

#include "capstan_config.h"
#include "ui_keyboard.h"
#include "ui_mqtt.h"
#include "ui_nav.h"

#ifndef CAPSTAN_HAVE_UI
#  error "CAPSTAN_HAVE_UI is not defined -- main/CMakeLists.txt must set it"
#endif

#if CAPSTAN_HAVE_UI

#include "screens.h"
#include "ui.h"

static const char *TAG = "ui.mqtt";

/* Working copy, loaded on entry and written on Save. */
static capstan_mqtt_cfg_t s_cfg;
static bool               s_loaded;

enum { ROW_HOST = 0, ROW_PORT, ROW_USER, ROW_PASS, ROW_SAVE };

static lv_obj_t *row_value(int i)
{
    switch (i) {
    case ROW_HOST: return objects.mqtt_item0_value;
    case ROW_PORT: return objects.mqtt_item1_value;
    case ROW_USER: return objects.mqtt_item2_value;
    case ROW_PASS: return objects.mqtt_item3_value;
    default:       return NULL;
    }
}

static void render(void)
{
    char port[8];
    snprintf(port, sizeof(port), "%u", (unsigned)s_cfg.port);

    /* A blank field reads as "--", the same placeholder every other
     * screen uses for "nothing here yet". */
    const char *host = s_cfg.host[0] ? s_cfg.host : "--";
    const char *user = s_cfg.username[0] ? s_cfg.username : "--";

    /* The password is never shown, not even truncated: this screen is on
     * a wall in a shared space. Whether one is set is still useful, so
     * that much is shown and no more. */
    const char *pass = s_cfg.password[0] ? "********" : "--";

    if (row_value(ROW_HOST)) lv_label_set_text(row_value(ROW_HOST), host);
    if (row_value(ROW_PORT)) lv_label_set_text(row_value(ROW_PORT), port);
    if (row_value(ROW_USER)) lv_label_set_text(row_value(ROW_USER), user);
    if (row_value(ROW_PASS)) lv_label_set_text(row_value(ROW_PASS), pass);
}

void ui_mqtt_screen_entered(void)
{
    /*
     * Reload from NVS only on a FRESH visit. Coming back from the
     * keyboard is also a screen entry, and reloading there would throw
     * away the field the user just typed.
     */
    if (!s_loaded) {
        capstan_config_get_mqtt(&s_cfg);
        s_loaded = true;
    }
    render();
}

static void on_host(const char *text, void *ctx)
{
    (void)ctx;
    strlcpy(s_cfg.host, text, sizeof(s_cfg.host));
}

static void on_port(const char *text, void *ctx)
{
    (void)ctx;
    const long v = strtol(text, NULL, 10);
    /* Out-of-range input is ignored rather than clamped: silently
     * turning 99999 into 65535 would look like it was accepted. */
    if (v > 0 && v <= 65535) {
        s_cfg.port = (uint16_t)v;
    } else {
        ESP_LOGW(TAG, "ignoring out-of-range port '%s'", text);
    }
}

static void on_user(const char *text, void *ctx)
{
    (void)ctx;
    strlcpy(s_cfg.username, text, sizeof(s_cfg.username));
}

static void on_pass(const char *text, void *ctx)
{
    (void)ctx;
    strlcpy(s_cfg.password, text, sizeof(s_cfg.password));
}

void ui_mqtt_row_pressed(int index)
{
    char port[8];

    switch (index) {
    case ROW_HOST:
        ui_keyboard_open("Broker host", s_cfg.host, false,
                         on_host, NULL, CAPSTAN_SCREEN_MQTT);
        return;

    case ROW_PORT:
        snprintf(port, sizeof(port), "%u", (unsigned)s_cfg.port);
        ui_keyboard_open("Port", port, false,
                         on_port, NULL, CAPSTAN_SCREEN_MQTT);
        return;

    case ROW_USER:
        ui_keyboard_open("Username", s_cfg.username, false,
                         on_user, NULL, CAPSTAN_SCREEN_MQTT);
        return;

    case ROW_PASS:
        /* Not pre-filled. Handing the existing password back into an
         * editable field, on a wall panel, to be revealed by one press,
         * is not worth saving the retype. */
        ui_keyboard_open("Password", NULL, true,
                         on_pass, NULL, CAPSTAN_SCREEN_MQTT);
        return;

    case ROW_SAVE: {
        s_cfg.configured = true;
        const esp_err_t err = capstan_config_set_mqtt(&s_cfg);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "saved broker %s:%u", s_cfg.host, s_cfg.port);
            /* Drop the working copy so the next visit reloads what was
             * actually stored, rather than trusting this copy. */
            s_loaded = false;
            ui_nav_back();
        } else {
            ESP_LOGE(TAG, "save failed: %s", esp_err_to_name(err));
        }
        return;
    }

    default:
        return;
    }
}

#else

void ui_mqtt_screen_entered(void) { }
void ui_mqtt_row_pressed(int index) { (void)index; }

#endif
