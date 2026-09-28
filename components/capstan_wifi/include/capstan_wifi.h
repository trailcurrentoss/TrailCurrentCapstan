/*
 * Wi-Fi: scan, join, and stay joined.
 *
 * The settings screen drives this: scan, pick an SSID, choose a security
 * type, enter a passphrase, connect. Credentials live in capstan_config
 * (NVS); this component owns only the radio and the connection state.
 *
 * Everything here is asynchronous. Nothing blocks the caller, because the
 * caller is the UI and a blocked UI on a 480x480 panel is immediately
 * obvious. Results arrive through callbacks, which run on the system event
 * task -- NOT the LVGL task. A callback that touches lv_* must take
 * capstan_board_lock() first, or bounce the work over with lv_async_call().
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "capstan_config.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * How many scan results to keep.
 *
 * A campground or RV park can easily show more than 20 networks, and the
 * ones that matter are usually the strongest, so results are sorted by RSSI
 * and the tail is discarded. This is the hard cap on what the UI can ever
 * display -- see the note in docs/screens.md about authoring enough rows.
 */
#define CAPSTAN_WIFI_MAX_APS 20

typedef struct {
    char               ssid[CAPSTAN_SSID_MAX_LEN];
    int8_t             rssi;          /**< dBm, negative */
    capstan_wifi_sec_t security;      /**< as reported by the scan */
    uint8_t            channel;
    bool               hidden;        /**< empty SSID in the beacon */
} capstan_wifi_ap_t;

typedef enum {
    CAPSTAN_WIFI_IDLE = 0,     /**< radio up, not connected, not trying */
    CAPSTAN_WIFI_SCANNING,
    CAPSTAN_WIFI_CONNECTING,
    CAPSTAN_WIFI_CONNECTED,    /**< associated AND has an IP */
    CAPSTAN_WIFI_FAILED,       /**< gave up; see capstan_wifi_last_error() */
} capstan_wifi_state_t;

const char *capstan_wifi_state_name(capstan_wifi_state_t s);

/** Connection state changed. Runs on the system event task. */
typedef void (*capstan_wifi_state_cb_t)(capstan_wifi_state_t state, void *ctx);

/** A scan finished. `aps` is owned by this component and is valid only for
 *  the duration of the callback -- copy anything you need to keep. */
typedef void (*capstan_wifi_scan_cb_t)(const capstan_wifi_ap_t *aps,
                                       size_t count, void *ctx);

/**
 * Bring up the Wi-Fi stack in station mode. Does not connect.
 * Call after capstan_config_init().
 */
esp_err_t capstan_wifi_init(void);

void capstan_wifi_set_state_callback(capstan_wifi_state_cb_t cb, void *ctx);

/**
 * Start an asynchronous scan. The callback fires once, with results sorted
 * strongest first and duplicate SSIDs collapsed to their best signal --
 * a mesh or an AP with several radios would otherwise fill the list with
 * the same name.
 *
 * Returns ESP_ERR_INVALID_STATE if a scan is already running, so the UI can
 * ignore a double-press rather than queueing a second scan.
 */
esp_err_t capstan_wifi_scan_start(capstan_wifi_scan_cb_t cb, void *ctx);

/**
 * Connect using the credentials currently in capstan_config.
 *
 * Retries with backoff and keeps retrying indefinitely: this is a
 * wall-mounted panel in a vehicle, and the access point it wants may simply
 * be switched off right now. Giving up permanently would mean the display
 * stays dead until someone power-cycles it.
 */
esp_err_t capstan_wifi_connect(void);

/** Save credentials, then connect with them. Convenience for the settings
 *  screen, which always does both. */
esp_err_t capstan_wifi_connect_with(const capstan_wifi_cfg_t *cfg);

/** Stop trying and disconnect. Does not clear the saved credentials. */
esp_err_t capstan_wifi_disconnect(void);

capstan_wifi_state_t capstan_wifi_state(void);
bool capstan_wifi_is_connected(void);

/** Human-readable reason for the last failure, for the settings screen.
 *  Empty string if there has not been one. Never NULL. */
const char *capstan_wifi_last_error(void);

/** Current signal strength in dBm, or 0 when not connected. */
int8_t capstan_wifi_rssi(void);

/** Dotted-quad IP, or "--" when not connected. Never NULL. */
const char *capstan_wifi_ip(void);

#ifdef __cplusplus
}
#endif
