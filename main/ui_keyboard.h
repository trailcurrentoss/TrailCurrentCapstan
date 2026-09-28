/*
 * The keyboard, as a shared control rather than one screen's property.
 *
 * WHY A MEDIATOR
 *
 * There is one keyboard page and several things that need text: a Wi-Fi
 * passphrase, an MQTT host, a username, a broker password. The obvious
 * shortcut is to let the OK handler work out who asked -- and that
 * handler then grows a branch per caller, which is where a screen's
 * knowledge of every other screen starts.
 *
 * Instead a caller says what it wants and supplies a callback. The
 * keyboard knows nothing about Wi-Fi or MQTT, and adding the next field
 * that needs typing touches nothing here.
 */
#pragma once

#include <stdbool.h>

#include "ui_nav.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Called with the accepted text. Runs with the LVGL lock held. */
typedef void (*ui_keyboard_cb_t)(const char *text, void *ctx);

/**
 * Show the keyboard for one field.
 *
 * `prompt`   shown as the field's placeholder, so the user can see what
 *            they are being asked for -- the keyboard fills the screen
 *            and there is no room for a separate label.
 * `initial`  pre-filled text, or NULL. Editing an existing host should
 *            not mean retyping it.
 * `password` mask the text and offer the reveal.
 * `ret`      screen to return to on OK or cancel.
 *
 * Call with the LVGL lock held.
 */
void ui_keyboard_open(const char *prompt, const char *initial,
                      bool password, ui_keyboard_cb_t cb, void *ctx,
                      capstan_screen_t ret);

/** OK pressed. Invokes the callback, clears the field, navigates back. */
void ui_keyboard_accept(void);

/** Cancel pressed. Clears the field and navigates back, no callback. */
void ui_keyboard_cancel(void);

/** Toggle masking on the field. Bound to the reveal chip. */
void ui_keyboard_toggle_reveal(void);

#ifdef __cplusplus
}
#endif
