/*
 * Wi-Fi screen behaviour: run a scan, put the results in the authored rows.
 *
 * Separate from ui_nav.c on purpose. ui_nav owns which screen is showing
 * and what the ring does; this owns what one particular screen contains.
 * Folding per-screen content into the navigator is how a navigator turns
 * into the place everything ends up.
 */
#pragma once

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

/**
 * The SSID at a row index, or NULL if that row is empty.
 *
 * Indices match the visible rows, which are the ones the ring can select.
 */
const char *ui_wifi_ssid_at(int index);

#ifdef __cplusplus
}
#endif
