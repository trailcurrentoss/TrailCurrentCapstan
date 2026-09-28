/*
 * MQTT client for the Headwaters gateway. See capstan_mqtt.h and
 * docs/mqtt.md.
 */

#include <stdio.h>
#include <string.h>

#include "cJSON.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "mqtt_client.h"

#include "capstan_config.h"
#include "capstan_model.h"
#include "capstan_mqtt.h"

static const char *TAG = "mqtt";

/*
 * Inbound queue.
 *
 * esp-mqtt's event task must return quickly; anything slow there stalls
 * keepalives. Messages are copied onto this queue and parsed by
 * capstan_mqtt_process() from the main loop. Depth 24 covers a burst at the
 * roughly 200 msg/s this broker carries without allocating much -- on
 * overflow the OLDEST is dropped, because with no retained state the newest
 * reading is the only one that matters.
 */
/*
 * 48, and read every 20 ms.
 *
 * Headwaters publishes local/energy/status on EVERY CAN frame -- see
 * can-bridge.js, which has no rate limit -- so a live bus delivers
 * hundreds of messages a second. The queue is not a buffer for work
 * that is falling behind; it is a place for state to land between
 * reads, and the oldest is discarded first so the newest always wins.
 */
#define MSG_QUEUE_DEPTH 48
#define MSG_TOPIC_MAX   96
#define MSG_DATA_MAX    512

typedef struct {
    char topic[MSG_TOPIC_MAX];
    char data[MSG_DATA_MAX];
    int  len;
} msg_t;

static esp_mqtt_client_handle_t s_client;
static QueueHandle_t            s_queue;
static SemaphoreHandle_t        s_lock;
static capstan_mqtt_state_t     s_state;
static capstan_mqtt_state_cb_t  s_state_cb;
static void                    *s_state_ctx;
static capstan_mqtt_trigger_cb_t s_discovery_cb;
static void                    *s_discovery_ctx;
static char                     s_last_error[96];
static char                     s_lwt_topic[64];
static int64_t                  s_started_us;
static uint32_t                 s_superseded;

const char *capstan_mqtt_state_name(capstan_mqtt_state_t s)
{
    switch (s) {
    case CAPSTAN_MQTT_IDLE:       return "idle";
    case CAPSTAN_MQTT_CONNECTING: return "connecting";
    case CAPSTAN_MQTT_CONNECTED:  return "connected";
    case CAPSTAN_MQTT_FAILED:     return "failed";
    default:                      return "?";
    }
}

void capstan_mqtt_set_state_callback(capstan_mqtt_state_cb_t cb, void *ctx)
{
    s_state_cb = cb;
    s_state_ctx = ctx;
}

void capstan_mqtt_set_discovery_callback(capstan_mqtt_trigger_cb_t cb, void *ctx)
{
    s_discovery_cb  = cb;
    s_discovery_ctx = ctx;
}

capstan_mqtt_state_t capstan_mqtt_state(void) { return s_state; }
bool capstan_mqtt_is_connected(void) { return s_state == CAPSTAN_MQTT_CONNECTED; }
const char *capstan_mqtt_last_error(void) { return s_last_error; }

static void set_state(capstan_mqtt_state_t s)
{
    if (s == s_state) {
        return;
    }
    s_state = s;
    ESP_LOGI(TAG, "state -> %s", capstan_mqtt_state_name(s));
    if (s_state_cb) {
        s_state_cb(s, s_state_ctx);
    }
}

const char *capstan_mqtt_hostname(char *out, size_t len)
{
    uint8_t mac[6] = { 0 };
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    snprintf(out, len, "esp32-%02X%02X%02X", mac[3], mac[4], mac[5]);
    return out;
}

/*
 * Client ID uses the FULL six-byte MAC.
 *
 * A shortened form caused broker session eviction and connection flapping
 * when two boards collided on Fireside. With several dials in one rig that
 * is a certainty, not a risk.
 */
static void client_id(char *out, size_t len)
{
    uint8_t mac[6] = { 0 };
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    snprintf(out, len, "tc-capstan-%02x%02x%02x%02x%02x%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

/* Topics the display needs. Everything here is documented in docs/mqtt.md. */
static const char *const SUBSCRIPTIONS[] = {
    "local/energy/status",
    "local/airquality/temphumid",
    "local/airquality/status",
    "local/airquality/safety",
    "local/water/status",
    "local/level/tilt",
    "local/level/corners",
    "local/level/status",
    "local/lights/+/status",
    "local/relays/+/status",
    "local/picket/+/inputs",
    "local/spoor/+/inputs",
    "local/config/pdm_channels",
    "local/config/relay_channels",
    "local/gps/time",
    "os/timezone/current",
    "local/discovery/trigger",
    "local/ota/trigger",
};

static void mqtt_event(void *handler_args, esp_event_base_t base,
                       int32_t event_id, void *event_data)
{
    (void)handler_args; (void)base;
    esp_mqtt_event_handle_t e = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        s_last_error[0] = '\0';
        for (size_t i = 0; i < sizeof(SUBSCRIPTIONS) / sizeof(*SUBSCRIPTIONS); i++) {
            esp_mqtt_client_subscribe(s_client, SUBSCRIPTIONS[i], 0);
        }
        /* Retained "online" overrides the retained LWT the broker will have
         * published on the previous drop. */
        esp_mqtt_client_publish(s_client, s_lwt_topic, "online", 6, 1, 1);
        ESP_LOGI(TAG, "connected; %u subscriptions",
                 (unsigned)(sizeof(SUBSCRIPTIONS) / sizeof(*SUBSCRIPTIONS)));
        set_state(CAPSTAN_MQTT_CONNECTED);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "disconnected");
        set_state(CAPSTAN_MQTT_CONNECTING);   /* esp-mqtt retries itself */
        break;

    case MQTT_EVENT_ERROR:
        if (e->error_handle) {
            if (e->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                snprintf(s_last_error, sizeof(s_last_error),
                         "TLS/TCP error (esp-tls 0x%x)",
                         e->error_handle->esp_tls_last_esp_err);
            } else if (e->error_handle->connect_return_code) {
                snprintf(s_last_error, sizeof(s_last_error),
                         "Broker refused connection (%d)",
                         e->error_handle->connect_return_code);
            } else {
                strncpy(s_last_error, "Connection error",
                        sizeof(s_last_error) - 1);
            }
        }
        ESP_LOGE(TAG, "%s", s_last_error);
        set_state(CAPSTAN_MQTT_FAILED);
        break;

    case MQTT_EVENT_DATA: {
        /*
         * Copy and queue only. Parsing here would run on the MQTT event
         * task and stall keepalives; see the header.
         *
         * Fragmented payloads (data_len < total_data_len) are dropped
         * rather than reassembled -- no topic in this contract approaches
         * the 1 KB buffer, so a fragment means something unexpected and
         * silently keeping half of it would be worse than losing it.
         */
        if (e->total_data_len != e->data_len) {
            ESP_LOGW(TAG, "dropping fragmented payload on %.*s (%d of %d)",
                     e->topic_len, e->topic, e->data_len, e->total_data_len);
            break;
        }
        if (e->topic_len >= MSG_TOPIC_MAX || e->data_len >= MSG_DATA_MAX) {
            ESP_LOGW(TAG, "dropping oversized message (%d/%d)",
                     e->topic_len, e->data_len);
            break;
        }
        msg_t m;
        memcpy(m.topic, e->topic, e->topic_len);
        m.topic[e->topic_len] = '\0';
        memcpy(m.data, e->data, e->data_len);
        m.data[e->data_len] = '\0';
        m.len = e->data_len;

        if (xQueueSend(s_queue, &m, 0) != pdTRUE) {
            /* Full: discard the oldest and keep the newest. With nothing
             * retained, the latest reading is the only useful one. */
            msg_t discard;
            if (xQueueReceive(s_queue, &discard, 0) == pdTRUE) {
                xQueueSend(s_queue, &m, 0);
            }
            /*
             * DEBUG, and rare. This is not data loss in any sense that
             * matters: the discarded message is an older copy of state
             * that has already been superseded, and a display only ever
             * wants the latest. Logged as a warning it read as a fault
             * and sent someone looking for a stalled consumer that was
             * running perfectly.
             */
            if ((++s_superseded % 500) == 1) {
                ESP_LOGD(TAG, "coalesced %u superseded updates",
                         (unsigned)s_superseded);
            }
        }
        break;
    }
    default:
        break;
    }
}

/* --------------------------------------------------------------------- */

esp_err_t capstan_mqtt_init(void)
{
    if (!s_queue) {
        s_queue = xQueueCreate(MSG_QUEUE_DEPTH, sizeof(msg_t));
        ESP_RETURN_ON_FALSE(s_queue, ESP_ERR_NO_MEM, TAG, "queue alloc failed");
    }
    if (!s_lock) {
        s_lock = xSemaphoreCreateMutex();
        ESP_RETURN_ON_FALSE(s_lock, ESP_ERR_NO_MEM, TAG, "mutex alloc failed");
    }

    uint8_t mac[6] = { 0 };
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    snprintf(s_lwt_topic, sizeof(s_lwt_topic),
             "local/capstan/%02x%02x%02x%02x%02x%02x/status",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    ESP_LOGI(TAG, "LWT topic %s", s_lwt_topic);
    return ESP_OK;
}

static esp_err_t build_and_start(void)
{
    capstan_mqtt_cfg_t c;
    capstan_config_get_mqtt(&c);
    ESP_RETURN_ON_FALSE(c.configured && c.host[0], ESP_ERR_INVALID_STATE, TAG,
                        "broker not configured");

    char uri[CAPSTAN_HOST_MAX_LEN + 32];
    snprintf(uri, sizeof(uri), "mqtts://%s:%u", c.host, (unsigned)c.port);

    char cid[40];
    client_id(cid, sizeof(cid));

    esp_mqtt_client_config_t cfg = {
        .broker.address.uri = uri,
        .credentials = {
            .username = c.username[0] ? c.username : NULL,
            .client_id = cid,
            .authentication.password = c.password[0] ? c.password : NULL,
        },
        .session = {
            /*
             * 60 s, not the default. The IDF default produced spurious
             * "No PING_RESP, disconnected" drops on Fireside whenever
             * Wi-Fi degraded -- which in a vehicle is routine.
             */
            .keepalive = 60,
            .last_will = {
                .topic  = s_lwt_topic,
                .msg    = "offline",
                .msg_len = 7,
                .qos    = 1,
                .retain = 1,
            },
        },
        .network.timeout_ms = 20000,
        .buffer.size = 1024,
        /*
         * Headwaters ships a self-signed certificate and no CA is
         * provisioned here, so the certificate is not verified. Without
         * this the handshake fails with mbedtls 0x8017, which reads like a
         * broker fault and is not. Requires CONFIG_ESP_TLS_INSECURE and
         * CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY, both in sdkconfig.defaults.
         */
        .broker.verification.skip_cert_common_name_check = true,
    };

    s_client = esp_mqtt_client_init(&cfg);
    ESP_RETURN_ON_FALSE(s_client, ESP_FAIL, TAG, "client init failed");
    ESP_RETURN_ON_ERROR(esp_mqtt_client_register_event(
        s_client, ESP_EVENT_ANY_ID, mqtt_event, NULL), TAG, "register failed");
    ESP_RETURN_ON_ERROR(esp_mqtt_client_start(s_client), TAG, "start failed");

    s_started_us = esp_timer_get_time();
    set_state(CAPSTAN_MQTT_CONNECTING);
    ESP_LOGI(TAG, "connecting to %s as %s", uri, cid);
    return ESP_OK;
}

esp_err_t capstan_mqtt_connect(void)
{
    xSemaphoreTake(s_lock, portMAX_DELAY);
    esp_err_t err = ESP_OK;

    if (s_client) {
        const bool fresh = (esp_timer_get_time() - s_started_us) < 5000000LL;
        if (s_state == CAPSTAN_MQTT_CONNECTED || fresh) {
            /* Genuinely nothing to do: the two boot callers fire about
             * 36 ms apart on Fireside and both are legitimate. */
            xSemaphoreGive(s_lock);
            return ESP_OK;
        }
        /* Down for a while: the socket is dead. Rebuild rather than hope. */
        esp_mqtt_client_stop(s_client);
        esp_mqtt_client_destroy(s_client);
        s_client = NULL;
    }
    err = build_and_start();
    xSemaphoreGive(s_lock);
    return err;
}

esp_err_t capstan_mqtt_reconnect(void)
{
    xSemaphoreTake(s_lock, portMAX_DELAY);
    if (s_client) {
        esp_mqtt_client_stop(s_client);
        esp_mqtt_client_destroy(s_client);
        s_client = NULL;
    }
    const esp_err_t err = build_and_start();
    xSemaphoreGive(s_lock);
    return err;
}

esp_err_t capstan_mqtt_stop(void)
{
    xSemaphoreTake(s_lock, portMAX_DELAY);
    if (s_client) {
        esp_mqtt_client_stop(s_client);
        esp_mqtt_client_destroy(s_client);
        s_client = NULL;
    }
    set_state(CAPSTAN_MQTT_IDLE);
    xSemaphoreGive(s_lock);
    return ESP_OK;
}

int capstan_mqtt_publish(const char *topic, const char *payload, int len)
{
    /*
     * Bounded take, never portMAX_DELAY: an unbounded wait here deadlocks
     * against a concurrent reconnect, which holds the lock while tearing
     * the client down.
     */
    if (xSemaphoreTake(s_lock, pdMS_TO_TICKS(100)) != pdTRUE) {
        ESP_LOGW(TAG, "publish skipped, client busy");
        return -1;
    }
    int id = -1;
    if (s_client && s_state == CAPSTAN_MQTT_CONNECTED) {
        id = esp_mqtt_client_publish(s_client, topic, payload, len, 0, 0);
    } else {
        ESP_LOGW(TAG, "not connected, cannot publish to %s", topic);
    }
    xSemaphoreGive(s_lock);
    return id;
}

/* --------------------------------------------------------------------- */
/* Parsing. Runs on the caller's task, NOT the MQTT event task.           */
/* --------------------------------------------------------------------- */

static bool num(const cJSON *o, const char *key, double *out)
{
    const cJSON *j = cJSON_GetObjectItemCaseSensitive(o, key);
    if (cJSON_IsNumber(j)) {
        *out = j->valuedouble;
        return true;
    }
    return false;
}

/*
 * Is this trigger addressed to us?
 *
 * Headwaters broadcasts `*` when the user asks Overlook to scan, and sends a
 * single `esp32-XXXXXX` hostname when it wants one specific device. Every
 * device on the broker sees both, so a device that does not check the payload
 * would enter discovery every time any other device was targeted -- dropping
 * its broker connection for three minutes each time.
 *
 * Matched with strncmp over the payload length, the way Spotter and Fireside
 * do, because the payload is not guaranteed to be terminated the same way our
 * own hostname buffer is.
 */
static bool trigger_is_for_us(const msg_t *m)
{
    if (m->len == 1 && m->data[0] == '*') {
        return true;
    }
    char me[24];
    capstan_mqtt_hostname(me, sizeof(me));
    return m->len > 0 && strncmp(m->data, me, (size_t)m->len) == 0;
}

static void dispatch_trigger(const msg_t *m)
{
    const bool discovery = strcmp(m->topic, "local/discovery/trigger") == 0;

    if (!trigger_is_for_us(m)) {
        ESP_LOGD(TAG, "%s for '%s' -- not us", m->topic, m->data);
        return;
    }

    if (discovery) {
        if (s_discovery_cb) {
            ESP_LOGI(TAG, "discovery trigger accepted (payload '%s')", m->data);
            s_discovery_cb(s_discovery_ctx);
        } else {
            /* Loud, because the consequence is invisible: the panel simply
             * never appears in Overlook's list and nothing else looks wrong. */
            ESP_LOGW(TAG, "discovery trigger for us, but no handler is "
                          "registered -- this panel will not be discoverable");
        }
        return;
    }

    /* OTA. No implementation yet; say so rather than dropping it silently. */
    capstan_model_note_trigger(m->topic, m->data);
}

static void apply(const msg_t *m)
{
    /* Every field on every topic is optional: local/energy/status is an
     * accumulator fed by three separate CAN frames, so a message routinely
     * carries only part of it. */
    /*
     * The triggers are handled BEFORE the JSON parse, not in its failure
     * branch.
     *
     * They are bare strings -- `*`, or a hostname -- so parsing them fails and
     * falling through to the failure branch happens to work today. It is the
     * wrong place for it: a trigger is a trigger whatever its payload parses
     * as, and hanging the discovery handshake off "cJSON gave up" makes it
     * fragile to a payload that happens to be valid JSON. `*` is not, but
     * `"*"` with quotes would be, and nothing on our side controls that.
     */
    if (strcmp(m->topic, "local/discovery/trigger") == 0 ||
        strcmp(m->topic, "local/ota/trigger") == 0) {
        dispatch_trigger(m);
        return;
    }

    cJSON *root = cJSON_ParseWithLength(m->data, m->len);
    if (!root) {
        return;
    }

    double v;
    if (strcmp(m->topic, "local/energy/status") == 0) {
        if (num(root, "battery_voltage", &v))       capstan_model_set_battery_volts(v);
        if (num(root, "battery_percent", &v))       capstan_model_set_battery_pct(v);
        if (num(root, "consumption_watts", &v))     capstan_model_set_load_watts(v);
        if (num(root, "solar_watts", &v))           capstan_model_set_solar_watts(v);
        if (num(root, "time_remaining_minutes", &v))capstan_model_set_runtime_min(v);
        const cJSON *ct = cJSON_GetObjectItemCaseSensitive(root, "charge_type");
        if (cJSON_IsString(ct)) capstan_model_set_charge_type(ct->valuestring);
    } else if (strcmp(m->topic, "local/airquality/temphumid") == 0) {
        if (num(root, "tempInF", &v))  capstan_model_set_temp_f(v);
        if (num(root, "tempInC", &v))  capstan_model_set_temp_c(v);
        if (num(root, "humidity", &v)) capstan_model_set_humidity(v);
    } else if (strcmp(m->topic, "local/airquality/status") == 0) {
        if (num(root, "tvoc_ppb", &v)) capstan_model_set_tvoc(v);
        if (num(root, "eco2_ppm", &v)) capstan_model_set_eco2(v);
    } else if (strcmp(m->topic, "local/airquality/safety") == 0) {
        if (num(root, "co_ppm", &v)) capstan_model_set_co(v);
        capstan_model_set_safety_flags(
            cJSON_IsTrue(cJSON_GetObjectItem(root, "co_alarm")),
            cJSON_IsTrue(cJSON_GetObjectItem(root, "co_warn")),
            cJSON_IsTrue(cJSON_GetObjectItem(root, "lpg_alarm")),
            cJSON_IsTrue(cJSON_GetObjectItem(root, "lpg_warn")));
    } else if (strcmp(m->topic, "local/water/status") == 0) {
        if (num(root, "fresh", &v)) capstan_model_set_tank(CAPSTAN_TANK_FRESH, v);
        if (num(root, "grey", &v))  capstan_model_set_tank(CAPSTAN_TANK_GREY, v);
        if (num(root, "black", &v)) capstan_model_set_tank(CAPSTAN_TANK_BLACK, v);
    } else if (strcmp(m->topic, "local/level/tilt") == 0) {
        double fb, ss;
        if (num(root, "front_back", &fb) && num(root, "side_to_side", &ss)) {
            capstan_model_set_tilt(fb, ss);
        }
    } else if (strncmp(m->topic, "local/lights/", 13) == 0) {
        int id = atoi(m->topic + 13);
        double st = 0, br = 0;
        num(root, "state", &st);
        num(root, "brightness", &br);
        capstan_model_set_light(id, st != 0, (int)br);
    } else if (strcmp(m->topic, "local/gps/time") == 0) {
        /* UTC calendar fields from Milepost's GNSS fix -- the only clock
         * source on the rig. Every field is required; a partial date is
         * not a date. */
        double y, mo, d, h, mi, sec;
        if (num(root, "year", &y)   && num(root, "month", &mo) &&
            num(root, "day", &d)    && num(root, "hour", &h)   &&
            num(root, "minute", &mi)&& num(root, "second", &sec)) {
            capstan_model_set_gps_time((int)y, (int)mo, (int)d,
                                       (int)h, (int)mi, (int)sec);
        }
    } else if (strcmp(m->topic, "os/timezone/current") == 0) {
        /* Retained, so this arrives once on connect and again whenever
         * the user changes it in the Headwaters PWA. */
        const cJSON *tz = cJSON_GetObjectItemCaseSensitive(root, "tz");
        if (cJSON_IsString(tz)) {
            capstan_model_set_timezone(tz->valuestring);
        }
    } else if (strncmp(m->topic, "local/picket/", 13) == 0) {
        double addr = 0, inputs = 0;
        num(root, "addr", &addr);
        num(root, "inputs", &inputs);
        capstan_model_set_picket_inputs((int)addr, (uint16_t)inputs);
    }

    cJSON_Delete(root);
}

void capstan_mqtt_process(void)
{
    msg_t m;
    while (s_queue && xQueueReceive(s_queue, &m, 0) == pdTRUE) {
        apply(&m);
    }
}

void capstan_mqtt_check_watchdogs(void)
{
    capstan_model_expire_stale();
}
