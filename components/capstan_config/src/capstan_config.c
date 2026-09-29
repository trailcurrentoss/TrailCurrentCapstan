/*
 * Persistent settings, stored in NVS. See capstan_config.h for the contract.
 */

#include <string.h>          /* strncpy/memset -- included explicitly rather
                                than inherited from an esp_* header */

#include "esp_check.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "capstan_config.h"

static const char *TAG = "config";

/*
 * Only this namespace is erased by a factory reset, so the Wi-Fi driver's
 * own calibration and PHY data survive it.
 */
#define NS "capstan"

/*
 * NVS keys are capped at 15 characters, so these are abbreviated rather
 * than descriptive. Do not lengthen them without checking the limit -- an
 * over-long key fails at runtime with ESP_ERR_NVS_KEY_TOO_LONG, not at
 * compile time.
 */
#define K_WIFI_SSID  "w_ssid"
#define K_WIFI_PASS  "w_pass"
#define K_WIFI_SEC   "w_sec"
#define K_WIFI_OK    "w_ok"
#define K_MQTT_HOST  "m_host"
#define K_MQTT_PORT  "m_port"
#define K_MQTT_USER  "m_user"
#define K_MQTT_PASS  "m_pass"
#define K_MQTT_OK    "m_ok"
#define K_DISP_C     "d_celsius"
#define K_DISP_LVLMM "d_lvl_mm"
#define K_DISP_24H   "d_24h"
#define K_DISP_HIDEGD "d_hide_gd"
#define K_DISP_FACE  "d_face"
#define K_DISP_DARK  "d_dark"
#define K_DISP_BL    "d_backlight"
#define K_DISP_IDLE  "d_idle"
#define K_DISP_SNOOZE "d_snooze"
#define K_TCAL_XS    "t_xs"
#define K_TCAL_XO    "t_xo"
#define K_TCAL_YS    "t_ys"
#define K_TCAL_YO    "t_yo"
#define K_TCAL_OK    "t_ok"
#define K_CONTROLS   "ctrls"
#define K_ALARMS     "alarms"
#define K_MODE       "mode"

static SemaphoreHandle_t      s_lock;
static capstan_wifi_cfg_t     s_wifi;
static capstan_mqtt_cfg_t     s_mqtt;
static capstan_display_cfg_t  s_display;
static capstan_touch_cal_t    s_tcal;
static capstan_controls_t     s_controls;
static capstan_alarms_t       s_alarms;
static capstan_mode_t         s_mode;

static const char *const s_sec_names[CAPSTAN_WIFI_SEC_COUNT] = {
    [CAPSTAN_WIFI_SEC_OPEN]          = "Open",
    [CAPSTAN_WIFI_SEC_WEP]           = "WEP",
    [CAPSTAN_WIFI_SEC_WPA_PSK]       = "WPA",
    [CAPSTAN_WIFI_SEC_WPA2_PSK]      = "WPA2",
    [CAPSTAN_WIFI_SEC_WPA_WPA2_PSK]  = "WPA/WPA2",
    [CAPSTAN_WIFI_SEC_WPA3_PSK]      = "WPA3",
    [CAPSTAN_WIFI_SEC_WPA2_WPA3_PSK] = "WPA2/WPA3",
};

static const char *const s_mode_names[CAPSTAN_MODE_COUNT] = {
    [CAPSTAN_MODE_CAMPING] = "camping",
    [CAPSTAN_MODE_DRIVING] = "driving",
    [CAPSTAN_MODE_STORAGE] = "storage",
};

const char *capstan_mode_name(capstan_mode_t mode)
{
    if (mode < 0 || mode >= CAPSTAN_MODE_COUNT || !s_mode_names[mode]) {
        return "camping";
    }
    return s_mode_names[mode];
}

capstan_mode_t capstan_mode_from_name(const char *name, capstan_mode_t fallback)
{
    if (!name) {
        return fallback;
    }
    for (int i = 0; i < CAPSTAN_MODE_COUNT; i++) {
        if (s_mode_names[i] && strcmp(name, s_mode_names[i]) == 0) {
            return (capstan_mode_t)i;
        }
    }
    /* An unknown mode keeps whatever we had rather than silently becoming
     * camping. A newer Headwaters adding a fourth mode should not quietly
     * re-interpret every alarm on an older panel. */
    return fallback;
}

static const char *const s_verdict_names[] = {
    [CAPSTAN_VERDICT_NONE] = "none",
    [CAPSTAN_VERDICT_HIGH] = "high",
    [CAPSTAN_VERDICT_LOW]  = "low",
};

const char *capstan_verdict_name(capstan_verdict_t v)
{
    if (v < 0 || v > CAPSTAN_VERDICT_LOW || !s_verdict_names[v]) {
        return "none";
    }
    return s_verdict_names[v];
}

capstan_verdict_t capstan_verdict_from_name(const char *name)
{
    if (name) {
        for (int i = 0; i <= CAPSTAN_VERDICT_LOW; i++) {
            if (s_verdict_names[i] && strcmp(name, s_verdict_names[i]) == 0) {
                return (capstan_verdict_t)i;
            }
        }
    }
    /* Unknown means quiet. A verdict this firmware does not understand must
     * not become an alarm that cannot be turned off from the PWA. */
    return CAPSTAN_VERDICT_NONE;
}

const char *capstan_wifi_sec_name(capstan_wifi_sec_t sec)
{
    if (sec < 0 || sec >= CAPSTAN_WIFI_SEC_COUNT || !s_sec_names[sec]) {
        return "Unknown";
    }
    return s_sec_names[sec];
}

/* ------------------------------------------------------------------ */

static void load_defaults(void)
{
    memset(&s_wifi, 0, sizeof(s_wifi));
    memset(&s_mqtt, 0, sizeof(s_mqtt));

    s_wifi.security = CAPSTAN_WIFI_SEC_WPA2_PSK;   /* the common case */

    /* Host, username and password are deliberately empty. There is no
     * sensible default for "which rig is this" -- the user supplies it, and
     * a baked-in guess would just point the device at somebody else's
     * broker. Only the port has a right answer. */
    s_mqtt.port = CONFIG_CAPSTAN_MQTT_DEFAULT_PORT;

    s_display.celsius = 
#ifdef CONFIG_CAPSTAN_DEFAULT_UNITS_CELSIUS
        true;
#else
        false;
#endif
    /* Metric heights with metric temperatures by default; either can then
     * be changed on its own in Settings > Locale. */
    s_display.level_mm = s_display.celsius;
    s_display.dark_theme =
#ifdef CONFIG_CAPSTAN_DEFAULT_THEME_DARK
        true;
#else
        false;
#endif
    s_display.backlight_percent = 100;
    s_display.idle_timeout_s    = CONFIG_CAPSTAN_IDLE_TIMEOUT_S;
    s_display.alarm_snooze_min  = CONFIG_CAPSTAN_ALARM_SNOOZE_MIN;

    /* No controls and no alarms until Headwaters sends some. An empty dial is
     * a correct first-boot state, not a fault -- and disarmed is the only
     * safe default, since an alarm invented locally would be a false one. */
    memset(&s_controls, 0, sizeof(s_controls));
    memset(&s_alarms, 0, sizeof(s_alarms));
    s_mode = CAPSTAN_MODE_CAMPING;

    /* Identity until calibrated -- raw coordinates pass through. */
    s_tcal.x_scale = 1.0f;  s_tcal.x_offset = 0.0f;
    s_tcal.y_scale = 1.0f;  s_tcal.y_offset = 0.0f;
    s_tcal.valid   = false;
}

static void read_f32(nvs_handle_t h, const char *key, float *dst)
{
    size_t len = sizeof(float);
    float v;
    if (nvs_get_blob(h, key, &v, &len) == ESP_OK && len == sizeof(float)) {
        *dst = v;
    }
}

/* Read a string, leaving the destination untouched if the key is absent so
 * the default survives. */
static void read_str(nvs_handle_t h, const char *key, char *dst, size_t cap)
{
    size_t len = cap;
    if (nvs_get_str(h, key, dst, &len) != ESP_OK) {
        return;
    }
    dst[cap - 1] = '\0';
}

static void read_u8(nvs_handle_t h, const char *key, uint8_t *dst)
{
    uint8_t v;
    if (nvs_get_u8(h, key, &v) == ESP_OK) {
        *dst = v;
    }
}

static void read_u16(nvs_handle_t h, const char *key, uint16_t *dst)
{
    uint16_t v;
    if (nvs_get_u16(h, key, &v) == ESP_OK) {
        *dst = v;
    }
}

static void read_bool(nvs_handle_t h, const char *key, bool *dst)
{
    uint8_t v;
    if (nvs_get_u8(h, key, &v) == ESP_OK) {
        *dst = (v != 0);
    }
}

static esp_err_t load_from_nvs(void)
{
    nvs_handle_t h;
    esp_err_t err = nvs_open(NS, NVS_READONLY, &h);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        /* First boot: the namespace does not exist yet. Not an error. */
        ESP_LOGI(TAG, "no saved settings; using defaults");
        return ESP_OK;
    }
    ESP_RETURN_ON_ERROR(err, TAG, "nvs_open failed");

    read_str(h, K_WIFI_SSID, s_wifi.ssid,     sizeof(s_wifi.ssid));
    read_str(h, K_WIFI_PASS, s_wifi.password, sizeof(s_wifi.password));
    uint8_t sec = (uint8_t)s_wifi.security;
    read_u8(h, K_WIFI_SEC, &sec);
    s_wifi.security = (sec < CAPSTAN_WIFI_SEC_COUNT)
                    ? (capstan_wifi_sec_t)sec
                    : CAPSTAN_WIFI_SEC_WPA2_PSK;
    read_bool(h, K_WIFI_OK, &s_wifi.configured);

    read_str(h, K_MQTT_HOST, s_mqtt.host,     sizeof(s_mqtt.host));
    read_str(h, K_MQTT_USER, s_mqtt.username, sizeof(s_mqtt.username));
    read_str(h, K_MQTT_PASS, s_mqtt.password, sizeof(s_mqtt.password));
    read_u16(h, K_MQTT_PORT, &s_mqtt.port);
    read_bool(h, K_MQTT_OK, &s_mqtt.configured);

    read_f32(h,  K_TCAL_XS, &s_tcal.x_scale);
    read_f32(h,  K_TCAL_XO, &s_tcal.x_offset);
    read_f32(h,  K_TCAL_YS, &s_tcal.y_scale);
    read_f32(h,  K_TCAL_YO, &s_tcal.y_offset);
    read_bool(h, K_TCAL_OK, &s_tcal.valid);

    /* One blob, not a key per control: 15-character keys leave no room for
     * an index plus a field name, and a partial list read back after a
     * power cut mid-write would be worse than none. A size mismatch means
     * the struct changed across a firmware update -- discard rather than
     * reinterpret, the retained topic will refill it within seconds of the
     * broker connecting. */
    size_t ctrl_len = sizeof(s_controls);
    capstan_controls_t ctrls;
    if (nvs_get_blob(h, K_CONTROLS, &ctrls, &ctrl_len) == ESP_OK &&
        ctrl_len == sizeof(s_controls) &&
        ctrls.count <= CAPSTAN_MAX_CONTROLS) {
        s_controls = ctrls;
    }

    size_t alarm_len = sizeof(s_alarms);
    capstan_alarms_t alarms;
    if (nvs_get_blob(h, K_ALARMS, &alarms, &alarm_len) == ESP_OK &&
        alarm_len == sizeof(s_alarms) &&
        alarms.count <= CAPSTAN_MAX_ALARMS) {
        s_alarms = alarms;
    }

    uint8_t mode = (uint8_t)s_mode;
    read_u8(h, K_MODE, &mode);
    if (mode < CAPSTAN_MODE_COUNT) {
        s_mode = (capstan_mode_t)mode;
    }

    read_bool(h, K_DISP_C,    &s_display.celsius);
    read_bool(h, K_DISP_LVLMM, &s_display.level_mm);
    read_bool(h, K_DISP_24H,   &s_display.clock_24h);
    read_bool(h, K_DISP_HIDEGD, &s_display.hide_guide);
    read_u8(h,   K_DISP_FACE, &s_display.clock_face);
    read_bool(h, K_DISP_DARK, &s_display.dark_theme);
    read_u8(h,   K_DISP_BL,   &s_display.backlight_percent);
    read_u16(h,  K_DISP_IDLE, &s_display.idle_timeout_s);
    read_u16(h,  K_DISP_SNOOZE, &s_display.alarm_snooze_min);
    if (s_display.alarm_snooze_min == 0) {
        /* 0 would mean re-raise on the very next refresh -- an alarm that
         * cannot be acknowledged. Treat it as unset. */
        s_display.alarm_snooze_min = CONFIG_CAPSTAN_ALARM_SNOOZE_MIN;
    }

    nvs_close(h);
    return ESP_OK;
}

esp_err_t capstan_config_init(void)
{
    if (!s_lock) {
        s_lock = xSemaphoreCreateMutex();
        ESP_RETURN_ON_FALSE(s_lock, ESP_ERR_NO_MEM, TAG, "mutex alloc failed");
    }

    load_defaults();
    esp_err_t err = load_from_nvs();

    /*
     * Log what was loaded, but NEVER the credentials. A passphrase in a
     * serial log outlives the session it was captured in.
     */
    ESP_LOGI(TAG, "touch cal: %s", s_tcal.valid ? "present" : "NONE (raw)");
    ESP_LOGI(TAG, "device controls: %u, alarms: %u, mode: %s",
             (unsigned)s_controls.count, (unsigned)s_alarms.count,
             capstan_mode_name(s_mode));
    ESP_LOGI(TAG, "wifi: %s (%s), mqtt: %s:%u %s",
             s_wifi.configured ? s_wifi.ssid : "<unset>",
             capstan_wifi_sec_name(s_wifi.security),
             s_mqtt.configured ? s_mqtt.host : "<unset>",
             (unsigned)s_mqtt.port,
             s_mqtt.username[0] ? "(auth)" : "(no user)");
    return err;
}

/* ------------------------------------------------------------------ */

#define WITH_LOCK(body)                                  \
    do {                                                 \
        xSemaphoreTake(s_lock, portMAX_DELAY);           \
        body;                                            \
        xSemaphoreGive(s_lock);                          \
    } while (0)

void capstan_config_get_wifi(capstan_wifi_cfg_t *out)
{
    if (!out) { return; }
    WITH_LOCK(*out = s_wifi);
}

void capstan_config_get_mqtt(capstan_mqtt_cfg_t *out)
{
    if (!out) { return; }
    WITH_LOCK(*out = s_mqtt);
}

void capstan_config_get_display(capstan_display_cfg_t *out)
{
    if (!out) { return; }
    WITH_LOCK(*out = s_display);
}

void capstan_config_get_touch_cal(capstan_touch_cal_t *out)
{
    if (!out) { return; }
    WITH_LOCK(*out = s_tcal);
}

void capstan_config_get_controls(capstan_controls_t *out)
{
    if (!out) { return; }
    WITH_LOCK(*out = s_controls);
}

esp_err_t capstan_config_set_controls(const capstan_controls_t *controls)
{
    ESP_RETURN_ON_FALSE(controls, ESP_ERR_INVALID_ARG, TAG, "null controls");
    ESP_RETURN_ON_FALSE(controls->count <= CAPSTAN_MAX_CONTROLS,
                        ESP_ERR_INVALID_ARG, TAG, "too many controls (%u)",
                        (unsigned)controls->count);

    /*
     * Normalise before comparing. The caller fills only `count` entries, so
     * whatever is in the tail is its business -- but if the tail differs the
     * memcmp below reports a change that isn't one, and the retained topic
     * would then rewrite flash on every single broker reconnect.
     */
    capstan_controls_t want;
    memset(&want, 0, sizeof(want));
    want.count = controls->count;
    for (uint8_t i = 0; i < controls->count; i++) {
        want.items[i].id = controls->items[i].id;
        strncpy(want.items[i].name, controls->items[i].name,
                CAPSTAN_CONTROL_NAME_MAX - 1);
        strncpy(want.items[i].icon, controls->items[i].icon,
                CAPSTAN_CONTROL_ICON_MAX - 1);
    }

    bool unchanged;
    WITH_LOCK(unchanged = (memcmp(&want, &s_controls, sizeof(want)) == 0));
    if (unchanged) {
        return ESP_OK;
    }

    nvs_handle_t h;
    ESP_RETURN_ON_ERROR(nvs_open(NS, NVS_READWRITE, &h), TAG, "nvs_open failed");
    esp_err_t err = nvs_set_blob(h, K_CONTROLS, &want, sizeof(want));
    if (err == ESP_OK) {
        err = nvs_commit(h);
    }
    nvs_close(h);
    ESP_RETURN_ON_ERROR(err, TAG, "control save failed");

    WITH_LOCK(s_controls = want);
    ESP_LOGI(TAG, "device controls saved: %u", (unsigned)want.count);
    return ESP_OK;
}

void capstan_config_get_alarms(capstan_alarms_t *out)
{
    if (!out) { return; }
    WITH_LOCK(*out = s_alarms);
}

esp_err_t capstan_config_set_alarms(const capstan_alarms_t *alarms)
{
    ESP_RETURN_ON_FALSE(alarms, ESP_ERR_INVALID_ARG, TAG, "null alarms");
    ESP_RETURN_ON_FALSE(alarms->count <= CAPSTAN_MAX_ALARMS,
                        ESP_ERR_INVALID_ARG, TAG, "too many alarms (%u)",
                        (unsigned)alarms->count);

    /* Normalised before comparing, exactly as the controls are: the caller
     * fills only `count` entries, and an unzeroed tail would report a change
     * on every retained redelivery and rewrite flash for nothing. */
    capstan_alarms_t want;
    memset(&want, 0, sizeof(want));
    want.count = alarms->count;
    for (uint8_t i = 0; i < alarms->count; i++) {
        want.items[i].src    = alarms->items[i].src;
        want.items[i].addr   = alarms->items[i].addr;
        want.items[i].sensor = alarms->items[i].sensor;
        for (int m = 0; m < CAPSTAN_MODE_COUNT; m++) {
            want.items[i].modes[m] = alarms->items[i].modes[m];
        }
        strncpy(want.items[i].name, alarms->items[i].name,
                CAPSTAN_ALARM_NAME_MAX - 1);
        strncpy(want.items[i].icon, alarms->items[i].icon,
                CAPSTAN_ALARM_ICON_MAX - 1);
    }

    bool unchanged;
    WITH_LOCK(unchanged = (memcmp(&want, &s_alarms, sizeof(want)) == 0));
    if (unchanged) {
        return ESP_OK;
    }

    nvs_handle_t h;
    ESP_RETURN_ON_ERROR(nvs_open(NS, NVS_READWRITE, &h), TAG, "nvs_open failed");
    esp_err_t err = nvs_set_blob(h, K_ALARMS, &want, sizeof(want));
    if (err == ESP_OK) {
        err = nvs_commit(h);
    }
    nvs_close(h);
    ESP_RETURN_ON_ERROR(err, TAG, "alarm save failed");

    WITH_LOCK(s_alarms = want);
    ESP_LOGI(TAG, "alarms saved: %u", (unsigned)want.count);
    return ESP_OK;
}

capstan_mode_t capstan_config_get_mode(void)
{
    capstan_mode_t m;
    WITH_LOCK(m = s_mode);
    return m;
}

esp_err_t capstan_config_set_mode(capstan_mode_t mode)
{
    ESP_RETURN_ON_FALSE(mode >= 0 && mode < CAPSTAN_MODE_COUNT,
                        ESP_ERR_INVALID_ARG, TAG, "bad mode %d", (int)mode);

    bool unchanged;
    WITH_LOCK(unchanged = (s_mode == mode));
    if (unchanged) {
        return ESP_OK;
    }

    nvs_handle_t h;
    ESP_RETURN_ON_ERROR(nvs_open(NS, NVS_READWRITE, &h), TAG, "nvs_open failed");
    esp_err_t err = nvs_set_u8(h, K_MODE, (uint8_t)mode);
    if (err == ESP_OK) {
        err = nvs_commit(h);
    }
    nvs_close(h);
    ESP_RETURN_ON_ERROR(err, TAG, "mode save failed");

    WITH_LOCK(s_mode = mode);
    ESP_LOGI(TAG, "rig mode: %s", capstan_mode_name(mode));
    return ESP_OK;
}

esp_err_t capstan_config_set_touch_cal(const capstan_touch_cal_t *cal)
{
    ESP_RETURN_ON_FALSE(cal, ESP_ERR_INVALID_ARG, TAG, "null cal");

    nvs_handle_t h;
    ESP_RETURN_ON_ERROR(nvs_open(NS, NVS_READWRITE, &h), TAG, "nvs_open failed");
    esp_err_t err = ESP_OK;
    err |= nvs_set_blob(h, K_TCAL_XS, &cal->x_scale,  sizeof(float));
    err |= nvs_set_blob(h, K_TCAL_XO, &cal->x_offset, sizeof(float));
    err |= nvs_set_blob(h, K_TCAL_YS, &cal->y_scale,  sizeof(float));
    err |= nvs_set_blob(h, K_TCAL_YO, &cal->y_offset, sizeof(float));
    err |= nvs_set_u8(h,   K_TCAL_OK, cal->valid ? 1 : 0);
    if (err == ESP_OK) {
        err = nvs_commit(h);
    }
    nvs_close(h);
    ESP_RETURN_ON_ERROR(err, TAG, "touch cal save failed");

    WITH_LOCK(s_tcal = *cal);
    ESP_LOGI(TAG, "touch cal saved: x = %.4f*raw %+.1f,  y = %.4f*raw %+.1f",
             cal->x_scale, cal->x_offset, cal->y_scale, cal->y_offset);
    return ESP_OK;
}

bool capstan_config_is_provisioned(void)
{
    bool ok;
    WITH_LOCK(ok = s_wifi.configured && s_mqtt.configured);
    return ok;
}

/* ------------------------------------------------------------------ */

esp_err_t capstan_config_set_wifi(const capstan_wifi_cfg_t *cfg)
{
    ESP_RETURN_ON_FALSE(cfg, ESP_ERR_INVALID_ARG, TAG, "null cfg");

    nvs_handle_t h;
    ESP_RETURN_ON_ERROR(nvs_open(NS, NVS_READWRITE, &h), TAG, "nvs_open failed");

    esp_err_t err = ESP_OK;
    err |= nvs_set_str(h, K_WIFI_SSID, cfg->ssid);
    err |= nvs_set_str(h, K_WIFI_PASS, cfg->password);
    err |= nvs_set_u8(h,  K_WIFI_SEC,  (uint8_t)cfg->security);
    err |= nvs_set_u8(h,  K_WIFI_OK,   1);
    if (err == ESP_OK) {
        err = nvs_commit(h);   /* commit before returning: a power cut right
                                  after "Save" must not lose the value */
    }
    nvs_close(h);
    ESP_RETURN_ON_ERROR(err, TAG, "wifi save failed");

    WITH_LOCK({
        s_wifi = *cfg;
        s_wifi.configured = true;
    });
    ESP_LOGI(TAG, "wifi saved: %s (%s)", cfg->ssid,
             capstan_wifi_sec_name(cfg->security));
    return ESP_OK;
}

esp_err_t capstan_config_set_mqtt(const capstan_mqtt_cfg_t *cfg)
{
    ESP_RETURN_ON_FALSE(cfg, ESP_ERR_INVALID_ARG, TAG, "null cfg");

    nvs_handle_t h;
    ESP_RETURN_ON_ERROR(nvs_open(NS, NVS_READWRITE, &h), TAG, "nvs_open failed");

    esp_err_t err = ESP_OK;
    err |= nvs_set_str(h,  K_MQTT_HOST, cfg->host);
    err |= nvs_set_u16(h,  K_MQTT_PORT, cfg->port);
    err |= nvs_set_str(h,  K_MQTT_USER, cfg->username);
    err |= nvs_set_str(h,  K_MQTT_PASS, cfg->password);
    err |= nvs_set_u8(h,   K_MQTT_OK,   1);
    if (err == ESP_OK) {
        err = nvs_commit(h);
    }
    nvs_close(h);
    ESP_RETURN_ON_ERROR(err, TAG, "mqtt save failed");

    WITH_LOCK({
        s_mqtt = *cfg;
        s_mqtt.configured = true;
    });
    ESP_LOGI(TAG, "mqtt saved: %s:%u", cfg->host, (unsigned)cfg->port);
    return ESP_OK;
}

esp_err_t capstan_config_set_display(const capstan_display_cfg_t *cfg)
{
    ESP_RETURN_ON_FALSE(cfg, ESP_ERR_INVALID_ARG, TAG, "null cfg");

    nvs_handle_t h;
    ESP_RETURN_ON_ERROR(nvs_open(NS, NVS_READWRITE, &h), TAG, "nvs_open failed");

    esp_err_t err = ESP_OK;
    err |= nvs_set_u8(h,  K_DISP_C,    cfg->celsius ? 1 : 0);
    err |= nvs_set_u8(h,  K_DISP_LVLMM, cfg->level_mm ? 1 : 0);
    err |= nvs_set_u8(h,  K_DISP_24H,   cfg->clock_24h ? 1 : 0);
    err |= nvs_set_u8(h,  K_DISP_HIDEGD, cfg->hide_guide ? 1 : 0);
    err |= nvs_set_u8(h,  K_DISP_FACE, cfg->clock_face);
    err |= nvs_set_u8(h,  K_DISP_DARK, cfg->dark_theme ? 1 : 0);
    err |= nvs_set_u8(h,  K_DISP_BL,   cfg->backlight_percent);
    err |= nvs_set_u16(h, K_DISP_IDLE, cfg->idle_timeout_s);
    err |= nvs_set_u16(h, K_DISP_SNOOZE, cfg->alarm_snooze_min);
    if (err == ESP_OK) {
        err = nvs_commit(h);
    }
    nvs_close(h);
    ESP_RETURN_ON_ERROR(err, TAG, "display save failed");

    WITH_LOCK(s_display = *cfg);
    return ESP_OK;
}

esp_err_t capstan_config_factory_reset(void)
{
    ESP_LOGW(TAG, "factory reset: erasing all saved settings");

    nvs_handle_t h;
    esp_err_t err = nvs_open(NS, NVS_READWRITE, &h);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        /* Nothing saved -- already in the factory state. */
        WITH_LOCK(load_defaults());
        return ESP_OK;
    }
    ESP_RETURN_ON_ERROR(err, TAG, "nvs_open failed");

    err = nvs_erase_all(h);
    if (err == ESP_OK) {
        err = nvs_commit(h);
    }
    nvs_close(h);
    ESP_RETURN_ON_ERROR(err, TAG, "erase failed");

    WITH_LOCK(load_defaults());
    ESP_LOGW(TAG, "factory reset complete -- reboot to apply");
    return ESP_OK;
}
