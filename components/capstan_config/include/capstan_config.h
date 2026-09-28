/*
 * Persistent settings, stored in NVS.
 *
 * Everything the user can change on the device and expects to survive a
 * power cycle: which network to join, how to reach the broker, and display
 * preferences.
 *
 * THREAD SAFETY. Every getter returns a copy under an internal mutex, and
 * every setter takes it. Callers never hold a pointer into the store, so a
 * concurrent write cannot tear a string out from under a reader. Getters are
 * cheap enough to call from a UI tick.
 *
 * WHAT IS NOT HERE. Nothing derived and nothing volatile -- no connection
 * state, no signal strength, no last-seen timestamps. Those live in
 * capstan_model, which is rebuilt from scratch on every boot. If a value
 * would be wrong after a reboot, it does not belong in NVS.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Buffer sizes. These are the NVS limits, not guesses:
 *   - an SSID is at most 32 bytes and is NOT required to be NUL-terminated
 *     on the wire, hence 33 here
 *   - a WPA2 passphrase is 8..63 characters, or a 64-character raw PSK
 *   - a hostname can reach 253 characters; 128 is the practical ceiling for
 *     a rig-local mDNS name and keeps the struct small
 */
#define CAPSTAN_SSID_MAX_LEN      33
#define CAPSTAN_WIFI_PASS_MAX_LEN 65
#define CAPSTAN_HOST_MAX_LEN      128
#define CAPSTAN_USER_MAX_LEN      65
#define CAPSTAN_PASS_MAX_LEN      65

/*
 * Wi-Fi security type.
 *
 * The design prototype infers this from the scan record. Capstan asks the
 * user instead, because a scan can be wrong or absent: a hidden network does
 * not appear at all, and some access points misreport their auth mode. The
 * scan result is used to preselect the most likely entry, not to decide.
 */
typedef enum {
    CAPSTAN_WIFI_SEC_OPEN = 0,
    CAPSTAN_WIFI_SEC_WEP,
    CAPSTAN_WIFI_SEC_WPA_PSK,
    CAPSTAN_WIFI_SEC_WPA2_PSK,
    CAPSTAN_WIFI_SEC_WPA_WPA2_PSK,
    CAPSTAN_WIFI_SEC_WPA3_PSK,
    CAPSTAN_WIFI_SEC_WPA2_WPA3_PSK,
    CAPSTAN_WIFI_SEC_COUNT
} capstan_wifi_sec_t;

/** Human-readable name, for the security picker. Never NULL. */
const char *capstan_wifi_sec_name(capstan_wifi_sec_t sec);

typedef struct {
    char               ssid[CAPSTAN_SSID_MAX_LEN];
    char               password[CAPSTAN_WIFI_PASS_MAX_LEN];
    capstan_wifi_sec_t security;
    bool               configured;   /**< false until the user has saved once */
} capstan_wifi_cfg_t;

typedef struct {
    char     host[CAPSTAN_HOST_MAX_LEN];
    uint16_t port;
    char     username[CAPSTAN_USER_MAX_LEN];
    char     password[CAPSTAN_PASS_MAX_LEN];
    bool     configured;
} capstan_mqtt_cfg_t;

/*
 * Touch calibration: a per-axis affine correction, screen = a * raw + b.
 *
 * Needed because esp_lcd_touch applies NO scaling -- x_max/y_max are used
 * only for mirroring, so whatever range the controller reports goes
 * straight to LVGL. On the MaTouch that measured as a systematic ~19 px Y
 * bias and roughly 10% X stretch: a mean error of 22 px and a worst case of
 * 30 px, against keyboard keys 34 px wide. Unusable for text entry, fine
 * for anything large.
 *
 * Stored per unit rather than compiled in, because the error is a property
 * of how one particular touch layer is bonded to one particular panel.
 * Baking one board's numbers in as a default is the assumption most likely
 * to be wrong on the next board.
 *
 * `valid` is false until a calibration has been run; the correction is then
 * the identity and raw coordinates pass through unchanged.
 */
typedef struct {
    float x_scale;
    float x_offset;
    float y_scale;
    float y_offset;
    bool  valid;
} capstan_touch_cal_t;

typedef struct {
    bool celsius;
    bool dark_theme;
    uint8_t backlight_percent;
    uint16_t idle_timeout_s;
} capstan_display_cfg_t;

/**
 * Open the NVS namespace and load everything into the in-memory cache.
 * Call after nvs_flash_init() and before any getter.
 *
 * Missing keys are not an error -- a first boot simply yields the defaults
 * from Kconfig with `configured` false.
 */
esp_err_t capstan_config_init(void);

void capstan_config_get_wifi(capstan_wifi_cfg_t *out);
void capstan_config_get_mqtt(capstan_mqtt_cfg_t *out);
void capstan_config_get_display(capstan_display_cfg_t *out);

/**
 * Setters write through to NVS immediately and commit before returning, so
 * a power cut straight after "Save" cannot lose the value. That costs a
 * flash write per call, which is why these take whole structs -- save a
 * completed form, not each keystroke.
 */
esp_err_t capstan_config_set_wifi(const capstan_wifi_cfg_t *cfg);
esp_err_t capstan_config_set_mqtt(const capstan_mqtt_cfg_t *cfg);
esp_err_t capstan_config_set_display(const capstan_display_cfg_t *cfg);

/** Touch calibration. An uncalibrated unit returns valid=false and the
 *  identity transform, so callers never need to special-case it. */
void      capstan_config_get_touch_cal(capstan_touch_cal_t *out);
esp_err_t capstan_config_set_touch_cal(const capstan_touch_cal_t *cal);

/**
 * Erase every Capstan key and restore the compiled-in defaults.
 *
 * Destroys the Wi-Fi and broker credentials -- the device drops off the
 * network and has to be set up again by hand. The caller is responsible for
 * confirming with the user first; this function does not ask.
 *
 * Only the "capstan" namespace is erased, not the whole of NVS, so the
 * Wi-Fi driver's own calibration data and any PHY data survive.
 *
 * The in-memory cache is reset too, so getters are correct immediately. A
 * reboot afterwards is still the cleanest way to apply it, since subsystems
 * already holding a connection will not drop it on their own.
 */
esp_err_t capstan_config_factory_reset(void);

/** True if both Wi-Fi and MQTT have been configured at least once. */
bool capstan_config_is_provisioned(void);

#ifdef __cplusplus
}
#endif
