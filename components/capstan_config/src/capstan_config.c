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
#define K_DISP_DARK  "d_dark"
#define K_DISP_BL    "d_backlight"
#define K_DISP_IDLE  "d_idle"
#define K_TCAL_XS    "t_xs"
#define K_TCAL_XO    "t_xo"
#define K_TCAL_YS    "t_ys"
#define K_TCAL_YO    "t_yo"
#define K_TCAL_OK    "t_ok"

static SemaphoreHandle_t      s_lock;
static capstan_wifi_cfg_t     s_wifi;
static capstan_mqtt_cfg_t     s_mqtt;
static capstan_display_cfg_t  s_display;
static capstan_touch_cal_t    s_tcal;

static const char *const s_sec_names[CAPSTAN_WIFI_SEC_COUNT] = {
    [CAPSTAN_WIFI_SEC_OPEN]          = "Open",
    [CAPSTAN_WIFI_SEC_WEP]           = "WEP",
    [CAPSTAN_WIFI_SEC_WPA_PSK]       = "WPA",
    [CAPSTAN_WIFI_SEC_WPA2_PSK]      = "WPA2",
    [CAPSTAN_WIFI_SEC_WPA_WPA2_PSK]  = "WPA/WPA2",
    [CAPSTAN_WIFI_SEC_WPA3_PSK]      = "WPA3",
    [CAPSTAN_WIFI_SEC_WPA2_WPA3_PSK] = "WPA2/WPA3",
};

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
    s_display.dark_theme =
#ifdef CONFIG_CAPSTAN_DEFAULT_THEME_DARK
        true;
#else
        false;
#endif
    s_display.backlight_percent = 100;
    s_display.idle_timeout_s    = CONFIG_CAPSTAN_IDLE_TIMEOUT_S;

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

    read_bool(h, K_DISP_C,    &s_display.celsius);
    read_bool(h, K_DISP_DARK, &s_display.dark_theme);
    read_u8(h,   K_DISP_BL,   &s_display.backlight_percent);
    read_u16(h,  K_DISP_IDLE, &s_display.idle_timeout_s);

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
    err |= nvs_set_u8(h,  K_DISP_DARK, cfg->dark_theme ? 1 : 0);
    err |= nvs_set_u8(h,  K_DISP_BL,   cfg->backlight_percent);
    err |= nvs_set_u16(h, K_DISP_IDLE, cfg->idle_timeout_s);
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
