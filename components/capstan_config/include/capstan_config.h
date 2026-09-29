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
    /**
     * Minutes an acknowledged alarm stays quiet before it is raised again,
     * if it is still active. Set on the device (Settings > Alarm Snooze).
     * An alarm that clears on its own drops its snooze, so the next time it
     * fires it is raised at once rather than when an old window runs out.
     */
    uint16_t alarm_snooze_min;
} capstan_display_cfg_t;

/*
 * Device controls -- which Torrent (PDM) channels and Switchback relays this
 * particular dial is allowed to switch.
 *
 * Configured in the Headwaters PWA (Settings > Network & Modules > the
 * Capstan's Edit dialog) and delivered, retained, on
 *
 *     local/config/capstan/<esp32-XXXXXX>/controls
 *
 * It is stored in NVS rather than being read live off the retained topic,
 * so the dial comes up with its own labelled controls before the broker is
 * reachable -- the same reason the Wi-Fi and broker settings live here. The
 * retained message is the source of truth and overwrites this whenever it
 * changes; the copy is a cache, not a second place to edit.
 *
 * `id` is the unified light id the rest of the platform already uses:
 * PDM channels are 1..N, Switchback relays start at 100. It is what goes in
 * local/lights/<id>/command and what comes back on
 * local/lights/<id>/status, so the source of a control never has to be
 * special-cased -- but it can still be recovered, since id >= 100 means the
 * control is a relay and therefore toggle-only, with no brightness.
 *
 * CAPSTAN_MAX_CONTROLS is bounded by the MQTT message buffer, not by screen
 * real estate. Eight entries of id/name/icon is about 600 bytes at worst,
 * inside MSG_DATA_MAX in capstan_mqtt.c with room to spare. Raising it
 * without checking that limit does not truncate the payload -- an oversized
 * message is DROPPED whole, so the dial simply never gets its controls and
 * says so in one log line. The same constant is mirrored in
 * containers/backend/src/services/capstan-control-sync.js (MAX_CONTROLS) and
 * the PWA (MAX_CAPSTAN_CONTROLS); all three have to move together.
 */
#define CAPSTAN_MAX_CONTROLS       8
#define CAPSTAN_CONTROL_NAME_MAX   25   /**< 24 chars, the PWA's limit, + NUL */
#define CAPSTAN_CONTROL_ICON_MAX   24   /**< icon key, e.g. "power-outlet" */

/** Light ids at or above this are Switchback relays: toggle-only. */
#define CAPSTAN_SWITCHBACK_ID_BASE 100

typedef struct {
    uint16_t id;
    char     name[CAPSTAN_CONTROL_NAME_MAX];
    char     icon[CAPSTAN_CONTROL_ICON_MAX];
} capstan_control_t;

typedef struct {
    uint8_t           count;
    capstan_control_t items[CAPSTAN_MAX_CONTROLS];
} capstan_controls_t;

/** True when this control is a Switchback relay rather than a PDM channel. */
static inline bool capstan_control_is_relay(const capstan_control_t *c)
{
    return c && c->id >= CAPSTAN_SWITCHBACK_ID_BASE;
}

/*
 * Rig mode -- what the vehicle is currently doing.
 *
 * Set in the Headwaters PWA and delivered retained on `local/mode/current`.
 * It used to be a per-browser view preference; it is rig state now precisely
 * so panels can act on it, because what a status event MEANS depends on it.
 *
 * Cached in NVS, unusually for something this volatile. The justification is
 * narrow: alarms are evaluated locally so they keep working when the backend
 * is down, and a dial that rebooted at 3 a.m. with no broker would otherwise
 * fall back to CAMPING and evaluate every alarm against the wrong mode. The
 * last known mode is a far better guess than a compiled-in default. The
 * retained topic still wins the moment it arrives.
 */
typedef enum {
    CAPSTAN_MODE_CAMPING = 0,
    CAPSTAN_MODE_DRIVING = 1,
    CAPSTAN_MODE_STORAGE = 2,
    CAPSTAN_MODE_COUNT
} capstan_mode_t;

/** Wire name for a mode ("camping"), and the reverse. Never NULL. */
const char    *capstan_mode_name(capstan_mode_t mode);
capstan_mode_t capstan_mode_from_name(const char *name, capstan_mode_t fallback);

/*
 * Alarms -- which Picket reed switches and Switchback digital inputs this
 * panel watches, and what each one MEANS in each mode.
 *
 * Configured in the Headwaters PWA beside the device controls and delivered,
 * retained, on
 *
 *     local/config/panel/<esp32-XXXXXX>/alarms
 *
 * (`panel`, not `capstan`: Milepost and Fireside are the same kind of
 * consumer and should adopt this contract rather than growing their own.)
 *
 * NOTHING ON THE BUS IS AN ALARM. Picket publishes a bitmask; Torrent
 * publishes channel state. Whether a given status event constitutes an alarm
 * is an interpretation, and the interpretation differs by panel and by mode.
 * A fridge sense line:
 *
 *     storage  -- voltage present is the alarm. Something switched it on.
 *     driving  -- voltage absent is the alarm. It lost power on the road.
 *     camping  -- neither. It is supposed to cycle.
 *
 * Hence a verdict per mode rather than a single armed flag plus a polarity.
 * The same panel evaluates the same input three different ways depending on
 * what the rig is doing. NONE in every mode is read as HIGH in every mode on
 * arrival (see apply_alarms()): armed, as Headwaters and Milepost mean it. A
 * dial that should not handle a sensor leaves it off its list.
 *
 * WHY THE SOURCE IS ALWAYS A DIGITAL INPUT, never a relay or PDM channel's
 * reported state. A relay can report ON while a failed contact passes no
 * voltage -- exactly the fault the alarm exists to catch. A Picket DI wired to
 * the load's supply sees what the load sees, so it stays correct when the
 * relay lies. Do not add local/lights/<id>/status as an alarm source: that
 * alarms on commanded state rather than on reality.
 *
 * WHY THE NAME AND ICON ARE CARRIED HERE, unlike a control's. A control points
 * at a channel that already has a label and an icon of its own. A sensor has
 * neither -- Headwaters' system_config.alarms.sensors holds an armed flag and
 * a name, nothing more.
 *
 * SENSOR NUMBERING is 1-based on the wire, matching the `type:addr:sensor`
 * keys Headwaters uses throughout. The input bitmasks are 0-based, so the bit
 * to test is `sensor - 1`:
 *
 *     local/picket/<addr>/inputs   12 bits   sensors 1..12
 *     local/spoor/<addr>/inputs     8 bits   sensors 1..8
 *
 * Note the topic for a Switchback alarm is `spoor`, not `switchback`.
 *
 * CAPSTAN_MAX_ALARMS is SIX, and the difference from the controls' eight is
 * the per-mode verdicts. A worst-case entry -- longest sensor key, a
 * 24-character label, a 16-character icon key and all three modes spelled out
 * -- is 149 bytes, so six plus the wrapper is 907, inside MSG_DATA_MAX in
 * capstan_mqtt.c. Eight would be 1205, and an oversized message is DROPPED
 * whole: the panel would have no alarms at all and one log line saying so. If
 * six is too few, the lever is MSG_QUEUE_DEPTH against MSG_DATA_MAX, which are
 * traded off against each other there -- not a quiet bump of this constant.
 */
#define CAPSTAN_MAX_ALARMS       6
#define CAPSTAN_ALARM_NAME_MAX   25   /**< 24 chars, the PWA's limit, + NUL */
#define CAPSTAN_ALARM_ICON_MAX   24

typedef enum {
    CAPSTAN_ALARM_SRC_PICKET     = 0,   /**< local/picket/<addr>/inputs, 12 bits */
    CAPSTAN_ALARM_SRC_SWITCHBACK = 1,   /**< local/spoor/<addr>/inputs,   8 bits */
} capstan_alarm_src_t;

/*
 * What this input means in one mode.
 *
 * NONE is not quite "disarmed" -- the same sensor is usually live in another
 * mode. It means this mode does not care.
 */
typedef enum {
    CAPSTAN_VERDICT_NONE = 0,   /**< this mode does not care */
    CAPSTAN_VERDICT_HIGH = 1,   /**< alarm while the input is asserted */
    CAPSTAN_VERDICT_LOW  = 2,   /**< alarm while the input is not asserted */
} capstan_verdict_t;

/** Wire name for a verdict ("high"), and the reverse. Never NULL. */
const char       *capstan_verdict_name(capstan_verdict_t v);
capstan_verdict_t capstan_verdict_from_name(const char *name);

typedef struct {
    capstan_alarm_src_t src;
    uint8_t             addr;      /**< board address, as in the input topic */
    uint8_t             sensor;    /**< 1-based; bit index is sensor - 1 */
    char                name[CAPSTAN_ALARM_NAME_MAX];
    char                icon[CAPSTAN_ALARM_ICON_MAX];
    capstan_verdict_t   modes[CAPSTAN_MODE_COUNT];
} capstan_alarm_t;

typedef struct {
    uint8_t         count;
    capstan_alarm_t items[CAPSTAN_MAX_ALARMS];
} capstan_alarms_t;

/** Digital inputs on a board of this kind. */
static inline uint8_t capstan_alarm_src_sensor_count(capstan_alarm_src_t src)
{
    return src == CAPSTAN_ALARM_SRC_PICKET ? 12 : 8;
}

/**
 * Is this alarm firing, given the board's raw input word and the rig mode?
 *
 * The one place the verdict rule lives -- callers pass the bitmask straight
 * off the input topic and never shift or invert it themselves.
 */
static inline bool capstan_alarm_is_active(const capstan_alarm_t *a,
                                           uint16_t inputs,
                                           capstan_mode_t mode)
{
    if (!a || mode < 0 || mode >= CAPSTAN_MODE_COUNT) {
        return false;
    }
    if (a->sensor < 1 || a->sensor > capstan_alarm_src_sensor_count(a->src)) {
        return false;
    }

    const bool high = ((inputs >> (a->sensor - 1)) & 1u) != 0;
    switch (a->modes[mode]) {
    case CAPSTAN_VERDICT_HIGH: return high;
    case CAPSTAN_VERDICT_LOW:  return !high;
    default:                   return false;
    }
}

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
 * The configured device controls. An unconfigured dial returns count = 0,
 * which is a legitimate state and not an error -- it is also what a Capstan
 * that has been removed in the PWA is told to become.
 */
void capstan_config_get_controls(capstan_controls_t *out);

/**
 * The configured alarms. count = 0 is a legitimate state: a dial with no
 * alarms configured, and what a Capstan removed in the PWA is told to become.
 */
void capstan_config_get_alarms(capstan_alarms_t *out);

/** Last known rig mode. CAPSTAN_MODE_CAMPING until one has been received. */
capstan_mode_t capstan_config_get_mode(void);

/**
 * Setters write through to NVS immediately and commit before returning, so
 * a power cut straight after "Save" cannot lose the value. That costs a
 * flash write per call, which is why these take whole structs -- save a
 * completed form, not each keystroke.
 */
esp_err_t capstan_config_set_wifi(const capstan_wifi_cfg_t *cfg);
esp_err_t capstan_config_set_mqtt(const capstan_mqtt_cfg_t *cfg);
esp_err_t capstan_config_set_display(const capstan_display_cfg_t *cfg);

/**
 * Replace the control list and commit it.
 *
 * Writes nothing and returns ESP_OK when the list is byte-identical to what
 * is already stored. That check is not an optimisation: the retained topic
 * is redelivered on every broker connect, and in a rig where Wi-Fi drops
 * repeatedly an unconditional write would spend the flash on a value that
 * never changed.
 */
esp_err_t capstan_config_set_controls(const capstan_controls_t *controls);

/**
 * Replace the alarm list and commit it. Writes nothing when the list is
 * byte-identical to what is stored, for the same reason as the controls: the
 * retained topic is redelivered on every broker connect.
 */
esp_err_t capstan_config_set_alarms(const capstan_alarms_t *alarms);

/**
 * Record the rig mode. Writes nothing when it has not changed -- the retained
 * topic is redelivered on every broker connect, and the mode changes rarely
 * enough that an unconditional write would be almost entirely wasted.
 */
esp_err_t capstan_config_set_mode(capstan_mode_t mode);

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
