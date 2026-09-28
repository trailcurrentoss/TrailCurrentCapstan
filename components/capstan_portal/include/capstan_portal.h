/*
 * Setup portal — phone-based provisioning.
 *
 * WHY THIS EXISTS
 *
 * Capstan's only input is a rotary ring and a round touchscreen no wider
 * than a coaster. Entering a WPA2 passphrase on that means turning to
 * each character in turn, and an MQTT hostname is worse. It was built
 * and it does not work in practice.
 *
 * So the device raises its own Wi-Fi network, shows the name and
 * password on the glass, and serves a captive portal. The phone that
 * already has a keyboard does the typing.
 *
 * Once the panel is on the network and registered with Headwaters, the
 * rest of its configuration -- which lights it shows, what they are
 * called -- comes from the Overlook PWA, which has a real screen. This
 * portal deliberately handles only what is needed to GET there: Wi-Fi
 * and the broker.
 */
#pragma once

#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Raise the setup AP and start the portal.
 *
 * Idempotent: calling it while already running is ignored, so entering
 * setup twice cannot leave two servers bound to port 80.
 */
esp_err_t capstan_portal_start(void);

/** Tear down the portal and the setup AP. */
esp_err_t capstan_portal_stop(void);

/** True while the portal is serving. */
bool capstan_portal_is_running(void);

/** True once credentials have been submitted and saved. */
bool capstan_portal_got_credentials(void);

/**
 * Advance the provisioning check. Call regularly while the portal is up.
 *
 * /save does not save. It applies the submitted credentials to the radio and
 * the broker and returns immediately; this drives the check to a verdict and
 * writes to NVS only if both actually worked. Without it being called, a
 * submission never completes.
 */
void capstan_portal_tick(void);

/** Number of phones currently associated with the setup AP. */
int capstan_portal_client_count(void);

#ifdef __cplusplus
}
#endif
