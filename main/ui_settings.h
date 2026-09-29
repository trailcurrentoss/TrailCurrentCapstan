/*
 * Settings screen behaviour.
 *
 * Separate from ui_nav.c for the same reason as ui_wifi.c: the navigator
 * says when a screen appears, the screen's own module says what its
 * controls do.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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

/* The carousel's items, in ring order -- SETTINGS_ITEMS in
 * GUI/tmp/screens_layout.py. ui_nav_press() switches on these. */
enum {
    UI_SETTINGS_WIFI = 0,
    UI_SETTINGS_MQTT,
    UI_SETTINGS_THEME,
    UI_SETTINGS_LOCALE,
    UI_SETTINGS_FACE,
    UI_SETTINGS_GUIDE,
    UI_SETTINGS_SNOOZE,
    UI_SETTINGS_TIMEOUT,
    UI_SETTINGS_RESET,
    UI_SETTINGS_ITEM_COUNT
};

/**
 * Paint the carousel for the current selection: the three glyphs, the name,
 * the value (coloured by state: green connected, amber not, red for an
 * armed reset) and the dots. Cheap and change-only; called on every
 * selection change and on the data refresh. LVGL lock held.
 */
void ui_settings_refresh(void);

/* The Locale screen's rows -- LOCALE_ITEMS in GUI/tmp/screens_layout.py. */
enum {
    UI_LOCALE_TEMPERATURE = 0,
    UI_LOCALE_LEVELING,
    UI_LOCALE_CLOCK,
    UI_LOCALE_ITEM_COUNT
};

/**
 * A press on Locale row `row`: flip that unit (°F/°C, in/mm, 12/24 h), save it to
 * NVS and repaint. Everything that shows the unit reads the saved setting,
 * so the change shows everywhere on the next refresh. LVGL lock held.
 */
void ui_settings_locale_pressed(int row);

/** Show or hide Getting Started in the app menu; saved to NVS. */
void ui_settings_guide_pressed(void);

/** "°F · in · 12 h" -- the Locale item's value in the Settings carousel. */
void ui_settings_locale_text(char *out, size_t len);

/**
 * The LED ring colour Settings wants: on Wi-Fi or MQTT, green when
 * connected and orange when not. False (ring dark) on the other items.
 * Consulted by ui_alerts.c, where an alarm's red takes priority.
 */
bool ui_settings_led(uint8_t *r, uint8_t *g, uint8_t *b);

#ifdef __cplusplus
}
#endif
