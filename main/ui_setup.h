/*
 * Setup mode: raise the portal, show the instructions, wait for a phone.
 */
#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enter setup mode: start the portal and show the Setup screen.
 *
 * Call with the LVGL lock held.
 */
void ui_setup_enter(void);

/** Leave setup mode and tear the portal down. LVGL lock held. */
void ui_setup_exit(void);

/** True while the portal is up. */
bool ui_setup_active(void);

/**
 * Refresh the Setup screen and finish provisioning when credentials
 * arrive. Called from the LVGL refresh timer.
 */
void ui_setup_tick(void);

#ifdef __cplusplus
}
#endif
