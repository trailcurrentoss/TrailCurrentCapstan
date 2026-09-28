/*
 * Settings screen behaviour.
 *
 * Separate from ui_nav.c for the same reason as ui_wifi.c: the navigator
 * says when a screen appears, the screen's own module says what its
 * controls do.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The Factory Reset row was pressed.
 *
 * First press arms and relabels the row; a second press within the arming
 * window erases the "capstan" NVS namespace and reboots. Call with the
 * LVGL lock held.
 */
void ui_settings_factory_reset_pressed(void);

/**
 * Cancel an armed reset and restore the row's label.
 *
 * Called when the selection moves off the row or the screen is left, so
 * an armed reset cannot outlive the moment the user was looking at it.
 */
void ui_settings_disarm_reset(void);

#ifdef __cplusplus
}
#endif
