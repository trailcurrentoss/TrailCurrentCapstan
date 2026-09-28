/*
 * The keyboard, as a shared control. See ui_keyboard.h for why.
 */

#include <string.h>

#include "esp_log.h"

#include "ui_keyboard.h"
#include "ui_nav.h"

#ifndef CAPSTAN_HAVE_UI
#  error "CAPSTAN_HAVE_UI is not defined -- main/CMakeLists.txt must set it"
#endif

#if CAPSTAN_HAVE_UI

#include "screens.h"
#include "ui.h"

static const char *TAG = "ui.kb";

static ui_keyboard_cb_t s_cb;
static void            *s_ctx;
static bool             s_password;

/*
 * Reset the field to a known state.
 *
 * The keyboard page is authored once and reused by every caller, so
 * whatever the last one typed is still in the textarea. For a
 * passphrase that means the next person to open the keyboard -- for an
 * MQTT hostname, say -- would find the Wi-Fi password sitting there,
 * unmasked if the reveal had been toggled. Clearing on the way out AND
 * on the way in means neither a missed exit path nor an unexpected
 * entry path can leak it.
 */
static void reset_field(void)
{
    if (objects.kb_field) {
        lv_textarea_set_text(objects.kb_field, "");
        lv_textarea_set_password_mode(objects.kb_field, true);
    }
    if (objects.kb_eye) {
        lv_obj_remove_state(objects.kb_eye, LV_STATE_CHECKED);
    }
}

void ui_keyboard_open(const char *prompt, const char *initial,
                      bool password, ui_keyboard_cb_t cb, void *ctx,
                      capstan_screen_t ret)
{
    reset_field();

    s_cb = cb;
    s_ctx = ctx;
    s_password = password;

    if (objects.kb_field) {
        lv_textarea_set_placeholder_text(objects.kb_field,
                                         prompt ? prompt : "");
        lv_textarea_set_password_mode(objects.kb_field, password);
        if (initial && *initial) {
            lv_textarea_set_text(objects.kb_field, initial);
        }
    }

    /* Hide the reveal on fields that are not secret -- an eye next to a
     * hostname is a control that does nothing. */
    if (objects.kb_eye) {
        if (password) {
            lv_obj_remove_flag(objects.kb_eye, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(objects.kb_eye, LV_OBJ_FLAG_HIDDEN);
        }
    }

    ui_nav_set_keyboard_return(ret);
    ui_nav_goto(CAPSTAN_SCREEN_KEYBOARD);
}

void ui_keyboard_accept(void)
{
    /* Copy before clearing: the callback may navigate, and the pointer
     * lv_textarea_get_text returns belongs to the widget. */
    char text[129] = {0};
    if (objects.kb_field) {
        const char *t = lv_textarea_get_text(objects.kb_field);
        if (t) {
            strlcpy(text, t, sizeof(text));
        }
    }

    ESP_LOGI(TAG, "accepted %d characters", (int)strlen(text));

    ui_keyboard_cb_t cb = s_cb;
    void *ctx = s_ctx;
    s_cb = NULL;
    s_ctx = NULL;

    reset_field();

    if (cb) {
        cb(text, ctx);
    } else {
        /* Nothing asked for this text. Reached by opening the keyboard
         * some way other than ui_keyboard_open(), which is a wiring bug
         * rather than something the user did. */
        ESP_LOGW(TAG, "text accepted with no callback registered");
    }

    ui_nav_back();
}

void ui_keyboard_cancel(void)
{
    s_cb = NULL;
    s_ctx = NULL;
    reset_field();
    ui_nav_back();
}

void ui_keyboard_toggle_reveal(void)
{
    if (!objects.kb_field || !s_password) {
        return;
    }
    const bool masked = lv_textarea_get_password_mode(objects.kb_field);
    lv_textarea_set_password_mode(objects.kb_field, !masked);

    if (objects.kb_eye) {
        if (masked) {
            lv_obj_add_state(objects.kb_eye, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(objects.kb_eye, LV_STATE_CHECKED);
        }
    }
}

#else

void ui_keyboard_open(const char *p, const char *i, bool pw,
                      ui_keyboard_cb_t cb, void *ctx, capstan_screen_t r)
{ (void)p; (void)i; (void)pw; (void)cb; (void)ctx; (void)r; }
void ui_keyboard_accept(void) { }
void ui_keyboard_cancel(void) { }
void ui_keyboard_toggle_reveal(void) { }

#endif
