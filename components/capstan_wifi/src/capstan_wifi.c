/*
 * Wi-Fi: scan, join, and stay joined. See capstan_wifi.h for the contract.
 */

#include <string.h>

#include "esp_check.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_random.h"
#include "nvs.h"
#include "nvs_flash.h"
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

        case WIFI_EVENT_AP_STACONNECTED: {
            const wifi_event_ap_staconnected_t *e = data;
            ESP_LOGI(TAG, "setup AP: station " MACSTR " joined (aid %u)",
                     MAC2STR(e->mac), (unsigned)e->aid);
            break;
        }

        case WIFI_EVENT_AP_STADISCONNECTED: {
            /* The reason code is the only place that distinguishes a
             * wrong passphrase from a full AP or a phone that simply
             * wandered off, and none of them look different on the
             * phone itself. */
            const wifi_event_ap_stadisconnected_t *e = data;
            ESP_LOGW(TAG, "setup AP: station " MACSTR " left, reason %u",
                     MAC2STR(e->mac), (unsigned)e->reason);
            break;
        }

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

/* ----------------------------------------------------------------------
 * Soft AP, for phone-based setup
 * ---------------------------------------------------------------------- */

static esp_netif_t *s_ap_netif;
static char s_ap_ssid[33];
static char s_ap_pass[16];

/*
 * The AP name must be unique per device.
 *
 * Two panels being set up side by side -- on a production line, or by an
 * owner who bought a pair -- would otherwise advertise the same SSID, and
 * a phone would silently join whichever was stronger. The suffix is the
 * last three bytes of the AP MAC, the same identifier the MQTT client
 * uses for its hostname, so a device is called the same thing everywhere.
 */
static void build_ap_ssid(void)
{
    uint8_t mac[6] = {0};
    esp_wifi_get_mac(WIFI_IF_AP, mac);
    snprintf(s_ap_ssid, sizeof(s_ap_ssid), "Capstan-%02X%02X%02X",
             mac[3], mac[4], mac[5]);
}

/*
 * A random passphrase, generated once and then KEPT.
 *
 * Not derived from the MAC: the MAC is broadcast in every beacon, so
 * anything computed from it is public and a neighbour could join the
 * setup network and hand the panel their own credentials. Random means
 * possession of the display is what grants access.
 *
 * But it is PERSISTED, where the first version regenerated it on every
 * entry to setup. That was wrong in a way only hardware showed: a phone
 * remembers the network, and on the second visit silently retries the
 * password it cached, which no longer matched. The phone reports
 * "incorrect password" and blames the user, who is reading the correct
 * password off the glass in front of them. There is no way to work that
 * out from either side.
 *
 * Stored in the same NVS namespace the rest of the settings use, so a
 * factory reset clears it and the next setup gets a fresh one -- which
 * is the one moment a stale phone profile is expected and acceptable.
 */
#define AP_PASS_NVS_KEY "ap_pass"

static void build_ap_password(void)
{
    nvs_handle_t h;
    size_t len = sizeof(s_ap_pass);

    if (nvs_open("capstan", NVS_READWRITE, &h) == ESP_OK) {
        if (nvs_get_str(h, AP_PASS_NVS_KEY, s_ap_pass, &len) == ESP_OK &&
            strlen(s_ap_pass) >= 8) {
            nvs_close(h);
            return;             /* reuse -- the phone may have it cached */
        }

        /* No 0/O/1/I/l: this gets read off a round 240 px panel and
         * typed into a phone, and those are the characters people get
         * wrong. */
        static const char alphabet[] = "23456789ABCDEFGHJKLMNPQRSTUVWXYZ";
        for (int i = 0; i < 8; i++) {
            s_ap_pass[i] = alphabet[esp_random() % (sizeof(alphabet) - 1)];
        }
        s_ap_pass[8] = '\0';

        nvs_set_str(h, AP_PASS_NVS_KEY, s_ap_pass);
        nvs_commit(h);
        nvs_close(h);
        ESP_LOGI(TAG, "generated a new setup passphrase");
        return;
    }

    /* NVS unavailable: still raise an AP rather than none, but it will
     * differ next boot -- which is worth a warning, because that is the
     * exact failure this function exists to avoid. */
    static const char alphabet[] = "23456789ABCDEFGHJKLMNPQRSTUVWXYZ";
    for (int i = 0; i < 8; i++) {
        s_ap_pass[i] = alphabet[esp_random() % (sizeof(alphabet) - 1)];
    }
    s_ap_pass[8] = '\0';
    ESP_LOGW(TAG, "NVS unavailable -- setup passphrase will not persist");
}

esp_err_t capstan_wifi_ap_start(void)
{
    build_ap_ssid();
    build_ap_password();

    wifi_config_t ap = { 0 };
    strlcpy((char *)ap.ap.ssid, s_ap_ssid, sizeof(ap.ap.ssid));
    ap.ap.ssid_len = strlen(s_ap_ssid);
    strlcpy((char *)ap.ap.password, s_ap_pass, sizeof(ap.ap.password));
    ap.ap.authmode = WIFI_AUTH_WPA2_PSK;
    ap.ap.channel = 1;
    /*
     * Four, not one.
     *
     * One phone at a time is the right POLICY, but enforcing it in the
     * radio is not how to express it. A station entry lingers after a
     * phone walks away or retries, so the next association is refused
     * for lack of a slot -- and Android reports a refused association
     * as "incorrect password", which sends the user to re-read a
     * passphrase that was never the problem. Headroom costs nothing and
     * removes a failure that is impossible to diagnose from the phone.
     */
    ap.ap.max_connection = 4;

    /*
     * Be explicit about the cipher rather than leaving it zeroed.
     *
     * A zeroed pairwise_cipher is WIFI_CIPHER_TYPE_NONE, and relying on
     * the driver to substitute something sane for a WPA2 AP is exactly
     * the kind of assumption that works on one phone and not another.
     */
    ap.ap.pairwise_cipher = WIFI_CIPHER_TYPE_CCMP;
    ap.ap.beacon_interval = 100;

    /*
     * APSTA, not AP. The portal has to SCAN for networks to offer, and
     * scanning needs the station interface: in pure AP mode
     * esp_wifi_scan_start() fails, which would leave the portal with an
     * empty list and no way to explain why.
     */
    ESP_RETURN_ON_ERROR(esp_wifi_set_mode(WIFI_MODE_APSTA), TAG,
                        "apsta mode failed");
    ESP_RETURN_ON_ERROR(esp_wifi_set_config(WIFI_IF_AP, &ap), TAG,
                        "ap config failed");

    /*
     * Read the config BACK from the driver.
     *
     * esp_wifi_set_config() returning ESP_OK only means the call was
     * accepted. On the MaTouch the AP reported "up" and never appeared
     * in a scan, and there was no way to tell from the log whether the
     * SSID, channel and authmode had actually taken. Reading them back
     * separates "we asked for the wrong thing" from "we asked for the
     * right thing and the radio is not transmitting it".
     */
    wifi_config_t back = { 0 };
    wifi_mode_t mode = WIFI_MODE_NULL;
    int8_t txp = 0;
    esp_wifi_get_config(WIFI_IF_AP, &back);
    esp_wifi_get_mode(&mode);
    esp_wifi_get_max_tx_power(&txp);

    uint8_t prim = 0;
    wifi_second_chan_t sec = WIFI_SECOND_CHAN_NONE;
    esp_wifi_get_channel(&prim, &sec);

    ESP_LOGI(TAG, "setup AP up: SSID '%s' ch %u auth %d hidden %u "
                  "max_conn %u | mode %d, radio ch %u, tx %.1f dBm",
             (char *)back.ap.ssid, (unsigned)back.ap.channel,
             (int)back.ap.authmode, (unsigned)back.ap.ssid_hidden,
             (unsigned)back.ap.max_connection,
             (int)mode, (unsigned)prim, txp / 4.0f);
    return ESP_OK;
}

esp_err_t capstan_wifi_ap_stop(void)
{
    ESP_RETURN_ON_ERROR(esp_wifi_set_mode(WIFI_MODE_STA), TAG,
                        "sta mode failed");
    ESP_LOGI(TAG, "setup AP down");
    return ESP_OK;
}

const char *capstan_wifi_ap_ssid(void) { return s_ap_ssid; }
const char *capstan_wifi_ap_password(void) { return s_ap_pass; }

const char *capstan_wifi_ap_ip(void)
{
    static char ip[16] = "192.168.4.1";
    esp_netif_ip_info_t info;
    if (s_ap_netif && esp_netif_get_ip_info(s_ap_netif, &info) == ESP_OK) {
        snprintf(ip, sizeof(ip), IPSTR, IP2STR(&info.ip));
    }
    return ip;
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
    /* The AP netif is created up front even though the AP is usually off:
     * creating it later, after esp_wifi_start(), needs the driver stopped
     * and restarted, which drops any station association. Setup mode can
     * then be entered from a running device without a reconnect. */
    s_ap_netif = esp_netif_create_default_wifi_ap();

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

    /* Must be set AFTER esp_wifi_start(); before it the call is accepted
     * and then reset to the default by the driver's own init. */
    if (CONFIG_CAPSTAN_WIFI_TX_POWER_QDBM < 80) {
        esp_wifi_set_max_tx_power(CONFIG_CAPSTAN_WIFI_TX_POWER_QDBM);
        int8_t got = 0;
        esp_wifi_get_max_tx_power(&got);
        ESP_LOGW(TAG, "tx power limited to %.1f dBm (asked %.1f)",
                 got / 4.0f, CONFIG_CAPSTAN_WIFI_TX_POWER_QDBM / 4.0f);
    }

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
