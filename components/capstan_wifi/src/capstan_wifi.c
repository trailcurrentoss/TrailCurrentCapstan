/*
 * Wi-Fi: scan, join, and stay joined. See capstan_wifi.h for the contract.
 */

#include <string.h>

#include "esp_check.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "capstan_wifi.h"

static const char *TAG = "wifi";

static capstan_wifi_state_t  s_state = CAPSTAN_WIFI_IDLE;
static capstan_wifi_state_cb_t s_state_cb;
static void                 *s_state_ctx;
static capstan_wifi_scan_cb_t s_scan_cb;
static void                 *s_scan_ctx;

static capstan_wifi_ap_t s_aps[CAPSTAN_WIFI_MAX_APS];
static size_t            s_ap_count;

static char    s_last_error[96];
static char    s_ip[16] = "--";
static int8_t  s_rssi;
static bool    s_want_connection;      /* user intent, not radio state */
static uint8_t s_retry;
static TimerHandle_t s_retry_timer;

const char *capstan_wifi_state_name(capstan_wifi_state_t s)
{
    switch (s) {
    case CAPSTAN_WIFI_IDLE:       return "idle";
    case CAPSTAN_WIFI_SCANNING:   return "scanning";
    case CAPSTAN_WIFI_CONNECTING: return "connecting";
    case CAPSTAN_WIFI_CONNECTED:  return "connected";
    case CAPSTAN_WIFI_FAILED:     return "failed";
    default:                      return "?";
    }
}

static void set_state(capstan_wifi_state_t s)
{
    if (s == s_state) {
        return;
    }
    s_state = s;
    ESP_LOGI(TAG, "state -> %s", capstan_wifi_state_name(s));
    if (s_state_cb) {
        s_state_cb(s, s_state_ctx);
    }
}

void capstan_wifi_set_state_callback(capstan_wifi_state_cb_t cb, void *ctx)
{
    s_state_cb  = cb;
    s_state_ctx = ctx;
}

capstan_wifi_state_t capstan_wifi_state(void) { return s_state; }
bool capstan_wifi_is_connected(void) { return s_state == CAPSTAN_WIFI_CONNECTED; }
const char *capstan_wifi_last_error(void) { return s_last_error; }
const char *capstan_wifi_ip(void) { return s_ip; }

int8_t capstan_wifi_rssi(void)
{
    if (s_state != CAPSTAN_WIFI_CONNECTED) {
        return 0;
    }
    wifi_ap_record_t ap;
    if (esp_wifi_sta_get_ap_info(&ap) == ESP_OK) {
        s_rssi = ap.rssi;
    }
    return s_rssi;
}

/* --------------------------------------------------------------------- */

static capstan_wifi_sec_t map_authmode(wifi_auth_mode_t a)
{
    switch (a) {
    case WIFI_AUTH_OPEN:            return CAPSTAN_WIFI_SEC_OPEN;
    case WIFI_AUTH_WEP:             return CAPSTAN_WIFI_SEC_WEP;
    case WIFI_AUTH_WPA_PSK:         return CAPSTAN_WIFI_SEC_WPA_PSK;
    case WIFI_AUTH_WPA2_PSK:        return CAPSTAN_WIFI_SEC_WPA2_PSK;
    case WIFI_AUTH_WPA_WPA2_PSK:    return CAPSTAN_WIFI_SEC_WPA_WPA2_PSK;
    case WIFI_AUTH_WPA3_PSK:        return CAPSTAN_WIFI_SEC_WPA3_PSK;
    case WIFI_AUTH_WPA2_WPA3_PSK:   return CAPSTAN_WIFI_SEC_WPA2_WPA3_PSK;
    default:                        return CAPSTAN_WIFI_SEC_WPA2_PSK;
    }
}

/*
 * Retry backoff.
 *
 * Capped and then held, never abandoned: this is a wall-mounted panel in a
 * vehicle and the access point it wants is quite normally switched off --
 * the rig is parked, the inverter is down, Headwaters is rebooting. A client
 * that gives up permanently would stay dead until someone power-cycles the
 * display, which is not a thing a user should have to know to do.
 */
static uint32_t backoff_ms(uint8_t attempt)
{
    static const uint32_t ladder[] = { 1000, 2000, 5000, 10000, 20000, 30000 };
    const size_t n = sizeof(ladder) / sizeof(ladder[0]);
    return ladder[attempt < n ? attempt : n - 1];
}

static void retry_timer_cb(TimerHandle_t t)
{
    (void)t;
    if (!s_want_connection) {
        return;
    }
    ESP_LOGI(TAG, "retrying association (attempt %u)", (unsigned)s_retry + 1);
    set_state(CAPSTAN_WIFI_CONNECTING);
    esp_wifi_connect();
}

static void schedule_retry(void)
{
    if (!s_want_connection || !s_retry_timer) {
        return;
    }
    const uint32_t ms = backoff_ms(s_retry);
    if (s_retry < 255) {
        s_retry++;
    }
    xTimerChangePeriod(s_retry_timer, pdMS_TO_TICKS(ms), 0);
    xTimerStart(s_retry_timer, 0);
    ESP_LOGI(TAG, "reconnect in %u ms", (unsigned)ms);
}

static const char *disconnect_reason(uint8_t r)
{
    /* Only the ones a user can act on are worth naming; the rest are noise
     * on a settings screen. */
    switch (r) {
    case WIFI_REASON_NO_AP_FOUND:          return "Network not found";
    case WIFI_REASON_AUTH_FAIL:
    case WIFI_REASON_HANDSHAKE_TIMEOUT:
    case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT: return "Wrong password";
    case WIFI_REASON_AUTH_EXPIRE:          return "Authentication expired";
    case WIFI_REASON_BEACON_TIMEOUT:       return "Signal lost";
    case WIFI_REASON_ASSOC_LEAVE:          return "Disconnected";
    default:                               return NULL;
    }
}

static void finish_scan(void)
{
    uint16_t found = 0;
    esp_wifi_scan_get_ap_num(&found);
    if (found == 0) {
        s_ap_count = 0;
        if (s_scan_cb) { s_scan_cb(s_aps, 0, s_scan_ctx); }
        set_state(s_want_connection ? CAPSTAN_WIFI_CONNECTING : CAPSTAN_WIFI_IDLE);
        return;
    }

    uint16_t n = found;
    wifi_ap_record_t *recs = calloc(n, sizeof(wifi_ap_record_t));
    if (!recs) {
        ESP_LOGE(TAG, "no memory for %u scan records", (unsigned)n);
        esp_wifi_clear_ap_list();
        set_state(CAPSTAN_WIFI_IDLE);
        return;
    }
    esp_wifi_scan_get_ap_records(&n, recs);

    /*
     * Collapse duplicate SSIDs, keeping the strongest.
     *
     * A mesh, or one AP with 2.4 and 5 GHz radios, otherwise fills the list
     * with the same name several times over -- which on a screen the user
     * scrolls with a ring is actively hostile. Records arrive sorted by
     * RSSI, so the first sighting of a name is already the best one.
     */
    s_ap_count = 0;
    for (uint16_t i = 0; i < n && s_ap_count < CAPSTAN_WIFI_MAX_APS; i++) {
        const char *ssid = (const char *)recs[i].ssid;
        bool dup = false;
        for (size_t j = 0; j < s_ap_count; j++) {
            if (strncmp(s_aps[j].ssid, ssid, CAPSTAN_SSID_MAX_LEN - 1) == 0) {
                dup = true;
                break;
            }
        }
        if (dup) {
            continue;
        }
        capstan_wifi_ap_t *a = &s_aps[s_ap_count++];
        memset(a, 0, sizeof(*a));
        strncpy(a->ssid, ssid, CAPSTAN_SSID_MAX_LEN - 1);
        a->rssi     = recs[i].rssi;
        a->security = map_authmode(recs[i].authmode);
        a->channel  = recs[i].primary;
        a->hidden   = (a->ssid[0] == '\0');
    }
    free(recs);
    esp_wifi_clear_ap_list();

    ESP_LOGI(TAG, "scan: %u found, %u unique", (unsigned)found,
             (unsigned)s_ap_count);
    if (s_scan_cb) {
        s_scan_cb(s_aps, s_ap_count, s_scan_ctx);
    }
    s_scan_cb = NULL;

    /* A scan while associated does not drop the link, but a scan started
     * before connecting should hand control back to the connect attempt. */
    set_state(s_want_connection ? CAPSTAN_WIFI_CONNECTING : CAPSTAN_WIFI_IDLE);
}

static void wifi_event(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    (void)arg;
    if (base == WIFI_EVENT) {
        switch (id) {
        case WIFI_EVENT_STA_START:
            if (s_want_connection) {
                esp_wifi_connect();
            }
            break;

        case WIFI_EVENT_SCAN_DONE:
            finish_scan();
            break;

        case WIFI_EVENT_STA_DISCONNECTED: {
            const wifi_event_sta_disconnected_t *d = data;
            const char *why = disconnect_reason(d->reason);
            if (why) {
                strncpy(s_last_error, why, sizeof(s_last_error) - 1);
            } else {
                snprintf(s_last_error, sizeof(s_last_error),
                         "Disconnected (reason %u)", (unsigned)d->reason);
            }
            strcpy(s_ip, "--");
            ESP_LOGW(TAG, "disconnected: %s", s_last_error);

            /*
             * A wrong password is reported as a normal disconnect, so the
             * state has to distinguish "still trying" from "this will never
             * work" -- otherwise the settings screen spins forever on a
             * typo. Report FAILED once, then keep retrying quietly in case
             * it was actually a flaky AP.
             */
            if (s_want_connection) {
                set_state(s_retry == 0 ? CAPSTAN_WIFI_CONNECTING
                                       : CAPSTAN_WIFI_FAILED);
                schedule_retry();
            } else {
                set_state(CAPSTAN_WIFI_IDLE);
            }
            break;
        }
        default:
            break;
        }
    } else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        const ip_event_got_ip_t *e = data;
        snprintf(s_ip, sizeof(s_ip), IPSTR, IP2STR(&e->ip_info.ip));
        s_last_error[0] = '\0';
        s_retry = 0;
        ESP_LOGI(TAG, "got IP %s", s_ip);
        set_state(CAPSTAN_WIFI_CONNECTED);
    }
}

/* --------------------------------------------------------------------- */

esp_err_t capstan_wifi_init(void)
{
    ESP_RETURN_ON_ERROR(esp_netif_init(), TAG, "netif init failed");

    /* Tolerate an already-created default loop: main may have made one. */
    esp_err_t err = esp_event_loop_create_default();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_RETURN_ON_ERROR(err, TAG, "event loop failed");
    }

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_RETURN_ON_ERROR(esp_wifi_init(&cfg), TAG, "wifi init failed");

    ESP_RETURN_ON_ERROR(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event, NULL, NULL),
        TAG, "wifi handler failed");
    ESP_RETURN_ON_ERROR(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event, NULL, NULL),
        TAG, "ip handler failed");

    ESP_RETURN_ON_ERROR(esp_wifi_set_storage(WIFI_STORAGE_RAM), TAG,
                        "storage failed");
    ESP_RETURN_ON_ERROR(esp_wifi_set_mode(WIFI_MODE_STA), TAG, "mode failed");

    /*
     * Modem sleep off. The default power save adds latency to every packet,
     * which on a panel showing live MQTT values reads as the UI lagging the
     * rig. This is a mains/house-battery powered wall display, so the radio
     * can stay awake.
     */
    ESP_RETURN_ON_ERROR(esp_wifi_set_ps(WIFI_PS_NONE), TAG, "ps failed");

    ESP_RETURN_ON_ERROR(esp_wifi_start(), TAG, "start failed");

    s_retry_timer = xTimerCreate("wifi_retry", pdMS_TO_TICKS(1000), pdFALSE,
                                 NULL, retry_timer_cb);
    ESP_RETURN_ON_FALSE(s_retry_timer, ESP_ERR_NO_MEM, TAG, "timer failed");

    ESP_LOGI(TAG, "station up");
    return ESP_OK;
}

esp_err_t capstan_wifi_scan_start(capstan_wifi_scan_cb_t cb, void *ctx)
{
    if (s_state == CAPSTAN_WIFI_SCANNING) {
        return ESP_ERR_INVALID_STATE;   /* let the UI ignore a double-press */
    }
    s_scan_cb  = cb;
    s_scan_ctx = ctx;

    const wifi_scan_config_t cfg = {
        .ssid = NULL, .bssid = NULL, .channel = 0,
        .show_hidden = true,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
        .scan_time.active = { .min = 100, .max = 300 },
    };
    ESP_RETURN_ON_ERROR(esp_wifi_scan_start(&cfg, false), TAG, "scan failed");
    set_state(CAPSTAN_WIFI_SCANNING);
    return ESP_OK;
}

esp_err_t capstan_wifi_connect(void)
{
    capstan_wifi_cfg_t c;
    capstan_config_get_wifi(&c);
    ESP_RETURN_ON_FALSE(c.configured && c.ssid[0], ESP_ERR_INVALID_STATE, TAG,
                        "no saved credentials");

    wifi_config_t wc = { 0 };
    strncpy((char *)wc.sta.ssid, c.ssid, sizeof(wc.sta.ssid) - 1);
    strncpy((char *)wc.sta.password, c.password, sizeof(wc.sta.password) - 1);

    /*
     * threshold.authmode is a FLOOR, not a match: the radio refuses anything
     * weaker. Setting it from the user's choice means selecting WPA2 will
     * not silently associate with an open network of the same name -- which
     * is the evil-twin case, and cheap to defend against here.
     */
    switch (c.security) {
    case CAPSTAN_WIFI_SEC_OPEN:
        wc.sta.threshold.authmode = WIFI_AUTH_OPEN; break;
    case CAPSTAN_WIFI_SEC_WEP:
        wc.sta.threshold.authmode = WIFI_AUTH_WEP; break;
    case CAPSTAN_WIFI_SEC_WPA_PSK:
        wc.sta.threshold.authmode = WIFI_AUTH_WPA_PSK; break;
    case CAPSTAN_WIFI_SEC_WPA3_PSK:
        wc.sta.threshold.authmode = WIFI_AUTH_WPA3_PSK; break;
    case CAPSTAN_WIFI_SEC_WPA2_WPA3_PSK:
        wc.sta.threshold.authmode = WIFI_AUTH_WPA2_WPA3_PSK; break;
    default:
        wc.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK; break;
    }

    ESP_RETURN_ON_ERROR(esp_wifi_set_config(WIFI_IF_STA, &wc), TAG,
                        "set config failed");

    s_want_connection = true;
    s_retry = 0;
    s_last_error[0] = '\0';
    set_state(CAPSTAN_WIFI_CONNECTING);
    ESP_LOGI(TAG, "connecting to '%s' (%s)", c.ssid,
             capstan_wifi_sec_name(c.security));

    const esp_err_t err = esp_wifi_connect();
    if (err != ESP_OK && err != ESP_ERR_WIFI_CONN) {
        ESP_RETURN_ON_ERROR(err, TAG, "connect failed");
    }
    return ESP_OK;
}

esp_err_t capstan_wifi_connect_with(const capstan_wifi_cfg_t *cfg)
{
    ESP_RETURN_ON_FALSE(cfg, ESP_ERR_INVALID_ARG, TAG, "null cfg");
    ESP_RETURN_ON_ERROR(capstan_config_set_wifi(cfg), TAG, "save failed");
    return capstan_wifi_connect();
}

esp_err_t capstan_wifi_disconnect(void)
{
    s_want_connection = false;
    if (s_retry_timer) {
        xTimerStop(s_retry_timer, 0);
    }
    strcpy(s_ip, "--");
    set_state(CAPSTAN_WIFI_IDLE);
    return esp_wifi_disconnect();
}
