/*
 * capstan_wifi, capstan_mqtt and capstan_portal for the EEZ Studio simulator.
 *
 * The panel is on the rig's network with the broker connected -- the state a
 * working install spends its life in. Readings do not travel over MQTT here;
 * sim_feed.c hands them to the same capstan_model setters the MQTT parser
 * calls, which is the boundary capstan_model.h draws on purpose.
 *
 * Commands do go through the publish call, and the one the UI sends -- a
 * light on or off -- is answered like the rig answers it: a status for that
 * id a moment later. ui_devices.c never updates optimistically, so a tile
 * that changes proves the whole press -> command -> status loop ran.
 *
 * Simulator only -- see docs/simulator.md.
 */
#ifdef EEZ_LVGL_SIMULATOR

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lvgl.h"

#include "capstan_config.h"
#include "capstan_model.h"
#include "capstan_mqtt.h"
#include "capstan_portal.h"
#include "capstan_wifi.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "sim.h"

static const char *TAG = "sim_net";

/* RFC 5737 documentation range: plainly not anybody's real network. */
#define SIM_IP     "192.0.2.47"
#define SIM_AP_IP  "192.0.2.1"

static bool s_connected = true;

/* ---- Wi-Fi ---------------------------------------------------------- */

static const char *const s_wifi_state_names[] = {
    "idle", "scanning", "connecting", "connected", "failed",
};

const char *capstan_wifi_state_name(capstan_wifi_state_t s)
{
    return (unsigned)s < sizeof(s_wifi_state_names) / sizeof(*s_wifi_state_names)
         ? s_wifi_state_names[s] : "?";
}

esp_err_t capstan_wifi_init(void)       { return ESP_OK; }
esp_err_t capstan_wifi_ap_start(void)   { return ESP_OK; }
esp_err_t capstan_wifi_ap_stop(void)    { return ESP_OK; }
const char *capstan_wifi_ap_ssid(void)     { return "Capstan-Setup"; }
const char *capstan_wifi_ap_password(void) { return "trailcurrent"; }
const char *capstan_wifi_ap_ip(void)       { return SIM_AP_IP; }

void capstan_wifi_set_state_callback(capstan_wifi_state_cb_t cb, void *ctx)
{
    if (cb) {
        cb(CAPSTAN_WIFI_CONNECTED, ctx);
    }
}

/* A handful of neighbours, the way a campground sounds. Delivered a beat
 * later, as a real scan would be. */
static capstan_wifi_scan_cb_t s_scan_cb;
static void                  *s_scan_ctx;

static void scan_done(lv_timer_t *t)
{
    lv_timer_delete(t);
    static const capstan_wifi_ap_t aps[] = {
        { "TrailCurrent-Rig",   -48, CAPSTAN_WIFI_SEC_WPA2_PSK,  6, false },
        { "Pinecrest_Guest",    -71, CAPSTAN_WIFI_SEC_OPEN,      1, false },
        { "Site 14",            -77, CAPSTAN_WIFI_SEC_WPA2_PSK, 11, false },
        { "Starlink",           -80, CAPSTAN_WIFI_SEC_WPA2_WPA3_PSK, 36, false },
    };
    if (s_scan_cb) {
        s_scan_cb(aps, sizeof(aps) / sizeof(*aps), s_scan_ctx);
    }
}

esp_err_t capstan_wifi_scan_start(capstan_wifi_scan_cb_t cb, void *ctx)
{
    s_scan_cb  = cb;
    s_scan_ctx = ctx;
    lv_timer_create(scan_done, 1200, NULL);
    return ESP_OK;
}

esp_err_t capstan_wifi_connect(void)                              { s_connected = true; return ESP_OK; }
esp_err_t capstan_wifi_connect_with(const capstan_wifi_cfg_t *c)  { (void)c; s_connected = true; return ESP_OK; }
esp_err_t capstan_wifi_try(const capstan_wifi_cfg_t *c)           { (void)c; return ESP_OK; }
esp_err_t capstan_wifi_disconnect(void)                           { s_connected = false; return ESP_OK; }

capstan_wifi_state_t capstan_wifi_state(void)
{
    return s_connected ? CAPSTAN_WIFI_CONNECTED : CAPSTAN_WIFI_IDLE;
}

bool        capstan_wifi_is_connected(void) { return s_connected; }
const char *capstan_wifi_last_error(void)   { return ""; }
const char *capstan_wifi_ip(void)           { return s_connected ? SIM_IP : ""; }

int8_t capstan_wifi_rssi(void)
{
    /* A parked rig a few metres from its router: strong, never still. */
    return (int8_t)(-54 - (int)((esp_timer_get_time() / 3000000) % 5));
}

/* ---- MQTT ----------------------------------------------------------- */

static const char *const s_mqtt_state_names[] = {
    "idle", "connecting", "connected", "failed",
};

const char *capstan_mqtt_state_name(capstan_mqtt_state_t s)
{
    return (unsigned)s < sizeof(s_mqtt_state_names) / sizeof(*s_mqtt_state_names)
         ? s_mqtt_state_names[s] : "?";
}

void capstan_mqtt_set_state_callback(capstan_mqtt_state_cb_t cb, void *ctx)
{
    if (cb) {
        cb(CAPSTAN_MQTT_CONNECTED, ctx);
    }
}

esp_err_t capstan_mqtt_init(void)      { return ESP_OK; }
esp_err_t capstan_mqtt_connect(void)   { return ESP_OK; }
esp_err_t capstan_mqtt_reconnect(void) { return ESP_OK; }
esp_err_t capstan_mqtt_stop(void)      { return ESP_OK; }
esp_err_t capstan_mqtt_try(const capstan_mqtt_cfg_t *c) { (void)c; return ESP_OK; }
void capstan_mqtt_process(void)         { }
void capstan_mqtt_check_watchdogs(void) { }
void capstan_mqtt_set_discovery_callback(capstan_mqtt_trigger_cb_t cb, void *ctx)
{ (void)cb; (void)ctx; }

capstan_mqtt_state_t capstan_mqtt_state(void)
{
    return s_connected ? CAPSTAN_MQTT_CONNECTED : CAPSTAN_MQTT_IDLE;
}

bool        capstan_mqtt_is_connected(void) { return s_connected; }
const char *capstan_mqtt_last_error(void)   { return ""; }

const char *capstan_mqtt_hostname(char *out, size_t len)
{
    snprintf(out, len, "capstan-%s", "sim");
    return out;
}

/* The rig's answer to a light command: its status, ~150 ms later. */
typedef struct {
    int  id;
    bool on;
} light_reply_t;

static void light_reply(lv_timer_t *t)
{
    light_reply_t *r = lv_timer_get_user_data(t);
    lv_timer_delete(t);
    capstan_model_set_light(r->id, r->on, r->on ? 255 : 0);
    free(r);
}

int capstan_mqtt_publish(const char *topic, const char *payload, int len)
{
    if (!s_connected) {
        return -1;
    }
    ESP_LOGI(TAG, "publish %s %.*s", topic, len, payload ? payload : "");

    int id = 0, state = 0;
    if (topic && payload &&
        sscanf(topic, "local/lights/%d/command", &id) == 1 &&
        sscanf(payload, "{\"state\":%d", &state) == 1) {
        light_reply_t *r = malloc(sizeof(*r));
        if (r) {
            r->id = id;
            r->on = state != 0;
            lv_timer_create(light_reply, 150, r);
        }
    }
    return 1;   /* a message id, as esp-mqtt returns */
}

/* ---- setup portal --------------------------------------------------- */

esp_err_t capstan_portal_start(void)       { return ESP_OK; }
esp_err_t capstan_portal_stop(void)        { return ESP_OK; }
bool      capstan_portal_is_running(void)  { return false; }
bool      capstan_portal_got_credentials(void) { return false; }
void      capstan_portal_tick(void)        { }
int       capstan_portal_client_count(void) { return 0; }

void sim_net_init(void)
{
    ESP_LOGI(TAG, "Wi-Fi up (%s), broker connected", SIM_IP);
}

#endif /* EEZ_LVGL_SIMULATOR */
