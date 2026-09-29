/*
 * Settings screen behaviour.
 *
 * Separate from ui_nav.c for the same reason as ui_wifi.c: the navigator
 * says when a screen appears, the screen's own module says what its
 * controls do.
 */
#pragma once

#include <stddef.h>

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

/**
 * The Alarm Snooze row was pressed: step to the next interval and save it.
 *
 * The ring is already busy moving the selection, so the row cycles through
 * a short fixed list on each press instead of entering an edit mode.
 * LVGL lock held.
 */
void ui_settings_snooze_pressed(void);

/** "10 min" -- the value text for the Alarm Snooze row. */
void ui_settings_snooze_text(char *out, size_t len);

/**
 * The Clock Timeout row was pressed: step how long an app stays up without
 * input before returning to the clock (15 s .. 5 min, then Never) and save
 * it. The idle timer reads the saved value, so it applies at once.
 * LVGL lock held.
 */
void ui_settings_timeout_pressed(void);

/** "30 s", "2 min", "Never" -- the value text for the Clock Timeout row. */
void ui_settings_timeout_text(char *out, size_t len);

/**
 * Theme row: a press flips light <-> dark, saves it, and repaints every
 * screen. LVGL lock held.
 */
void ui_settings_theme_pressed(void);

/** Apply the saved theme. Called once after ui_init(); LVGL lock held. */
void ui_settings_apply_theme(void);

/** "Light" or "Dark" -- the value text for the Theme row. */
const char *ui_settings_theme_text(void);

#ifdef __cplusplus
}
#endif
