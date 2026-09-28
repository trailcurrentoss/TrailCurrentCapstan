/*
 * Wi-Fi screen behaviour: run a scan, put the results in the authored rows.
 *
 * Separate from ui_nav.c on purpose. ui_nav owns which screen is showing
 * and what the ring does; this owns what one particular screen contains.
 * Folding per-screen content into the navigator is how a navigator turns
 * into the place everything ends up.
 */
#pragma once

#include <stdbool.h>

#include "capstan_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Called by ui_nav when the Wi-Fi screen becomes visible.
 *
 * Kicks off an asynchronous scan and shows "Scanning..." in the title. A
 * scan already in flight is left alone rather than restarted, so arriving
 * on the screen twice in quick succession does not queue two of them.
 *
 * Must be called with the LVGL lock held (ui_nav calls it from a screen
 * transition, which already holds it).
 */
void ui_wifi_screen_entered(void);

/** Subscribe to connection-state changes. Call once, after
 *  capstan_wifi_init(). */
void ui_wifi_init(void);

/**
 * Paint pending scan results and refresh the title.
 *
 * Called from the LVGL refresh timer. Nothing here is driven from the
 * Wi-Fi state callback any more -- that runs on the system event task,
 * whose stack is too small for LVGL work. See ui_data.c.
 */
void ui_wifi_tick(void);

/**
 * The SSID at a row index, or NULL if that row is empty.
 *
 * Indices match the visible rows, which are the ones the ring can select.
 */
const char *ui_wifi_ssid_at(int index);

/**
 * Remember which network the user picked, by row index.
 *
 * Called when a row is pressed, before navigating to the security
 * picker. The SSID is copied, so a rescan arriving mid-flow cannot
 * change which network the user is halfway through joining.
 */
void ui_wifi_select(int index);

/** The security type chosen on the picker, by row index. */
void ui_wifi_set_security_index(int index);

/**
 * Save the pending SSID / security / passphrase and start connecting.
 *
 * Returns false if there is no pending selection, which is what happens
 * if the keyboard is somehow reached without going through the list.
 */
bool ui_wifi_apply_password(const char *password);

/** SSID currently being joined, or NULL. For the keyboard's prompt. */
const char *ui_wifi_pending_ssid(void);

#ifdef __cplusplus
}
#endif
