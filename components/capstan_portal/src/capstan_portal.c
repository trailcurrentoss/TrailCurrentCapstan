/*
 * Setup portal: soft AP + captive DNS + a one-page HTTP form.
 *
 * See capstan_portal.h for why this exists at all.
 */

#include <string.h>

#include "cJSON.h"
#include "esp_http_server.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lwip/inet.h"

#include "capstan_config.h"
#include "capstan_mqtt.h"
#include "capstan_portal.h"
#include "capstan_wifi.h"
#include "portal_dns.h"
#include "portal_page.h"

static const char *TAG = "portal";

static httpd_handle_t s_server;
static bool           s_got_creds;

/* ---------------------------------------------------------------- */

static esp_err_t root_get(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    /* The page is regenerated on every visit and must never be cached:
     * a phone that cached it during a previous setup would show a stale
     * form after a firmware update changed the fields. */
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    return httpd_resp_send(req, PORTAL_HTML, HTTPD_RESP_USE_STRLEN);
}

/*
 * Captive-portal probes.
 *
 * Each OS fetches a known URL and decides the network is captive if the
 * answer is not exactly what it expects. A 302 to our root is what makes
 * the setup page appear on its own, instead of the user having to be
 * told an IP address.
 */
static esp_err_t probe_get(httpd_req_t *req)
{
    char url[40];
    snprintf(url, sizeof(url), "http://%s/", capstan_wifi_ap_ip());
    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", url);
    return httpd_resp_send(req, NULL, 0);
}

static const char *bars_for(int8_t rssi)
{
    if (rssi >= -55) { return "||||"; }
    if (rssi >= -67) { return "|||"; }
    if (rssi >= -75) { return "||"; }
    return "|";
}

/*
 * Scan results, as JSON. RETURNS IMMEDIATELY.
 *
 * Two lessons are baked into this endpoint.
 *
 * 1. It goes through capstan_wifi, never esp_wifi directly. The
 *    component also handles WIFI_EVENT_SCAN_DONE and fetching the
 *    records CONSUMES them, so two owners meant whichever ran first
 *    won -- and it was never the portal. That produced an empty list
 *    with no error anywhere.
 *
 * 2. It does NOT block on the scan. The first version held the HTTP
 *    response open until results arrived. curl waits happily; iOS's
 *    captive-portal browser does not -- it abandons a slow request, so
 *    the page showed nothing on an iPhone while working perfectly from
 *    a laptop on the same AP. A scan also takes longer than usual here
 *    because the radio has to keep returning to its own channel to
 *    stay associated with the very phone that is waiting.
 *
 * So a request starts a scan if none is running, returns whatever is
 * cached right now, and says whether more is coming. The page polls.
 */
static capstan_wifi_ap_t s_cache[24];
static size_t            s_cache_n;
static volatile bool     s_scanning;

static void scan_cb(const capstan_wifi_ap_t *aps, size_t count, void *ctx)
{
    (void)ctx;
    const size_t max = sizeof(s_cache) / sizeof(s_cache[0]);
    s_cache_n = count < max ? count : max;
    /* Copied, not referenced: `aps` belongs to the component and does
     * not outlive this callback. */
    memcpy(s_cache, aps, s_cache_n * sizeof(s_cache[0]));
    s_scanning = false;
    ESP_LOGI(TAG, "portal scan cached %u networks", (unsigned)s_cache_n);
}

static esp_err_t scan_get(httpd_req_t *req)
{
    if (!s_scanning) {
        const esp_err_t err = capstan_wifi_scan_start(scan_cb, NULL);
        if (err == ESP_OK) {
            s_scanning = true;
        } else if (err != ESP_ERR_INVALID_STATE) {
            ESP_LOGW(TAG, "scan refused: %s", esp_err_to_name(err));
        }
    }

    cJSON *root = cJSON_CreateObject();
    cJSON_AddBoolToObject(root, "scanning", s_scanning);
    cJSON *arr = cJSON_AddArrayToObject(root, "nets");

    for (size_t i = 0; i < s_cache_n; i++) {
        if (s_cache[i].ssid[0] == '\0') {
            continue;       /* hidden: nothing useful to show */
        }
        cJSON *o = cJSON_CreateObject();
        cJSON_AddStringToObject(o, "ssid", s_cache[i].ssid);
        cJSON_AddStringToObject(o, "bars", bars_for(s_cache[i].rssi));
        /* Map the component's security enum onto the four choices the
         * form offers; anything WPA-ish becomes WPA/WPA2. */
        int sec = 4;
        switch (s_cache[i].security) {
        case CAPSTAN_WIFI_SEC_OPEN: sec = 0; break;
        case CAPSTAN_WIFI_SEC_WEP:  sec = 1; break;
        case CAPSTAN_WIFI_SEC_WPA3_PSK:
        case CAPSTAN_WIFI_SEC_WPA2_WPA3_PSK: sec = 5; break;
        default: break;
        }
        cJSON_AddNumberToObject(o, "sec", sec);
        cJSON_AddItemToArray(arr, o);
    }

    char *body = cJSON_PrintUnformatted(root);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_sendstr(req, body ? body : "{\"scanning\":false,\"nets\":[]}");
    cJSON_free(body);
    cJSON_Delete(root);
    return ESP_OK;
}


/* ----------------------------------------------------------------------
 * Provisioning validation.
 *
 * Credentials reach NVS at exactly one place -- PORTAL_VAL_OK below -- and
 * only after the radio has associated and, when a broker was given, the
 * broker has accepted us. Anything else leaves NVS untouched, keeps the setup
 * AP up, and tells the user which half failed.
 *
 * WHY THIS EXISTS
 *
 * /save used to write first and find out afterwards. A wrong passphrase, or a
 * broker host that never resolves, was committed anyway; the panel then booted
 * believing it was provisioned, retried forever, and gave no sign of what was
 * wrong. Recovering meant a factory reset. A MaTouch in the field had Wi-Fi in
 * NVS and no broker at all for exactly this reason.
 *
 * It is a polled state machine rather than a long HTTP request for the reason
 * already recorded for /scan: iOS's captive-portal browser abandons a slow
 * fetch, and associating plus a TLS handshake takes far longer than it waits.
 * ---------------------------------------------------------------------- */
typedef enum {
    PORTAL_VAL_IDLE = 0,
    PORTAL_VAL_WIFI,
    PORTAL_VAL_MQTT,
    PORTAL_VAL_OK,
    PORTAL_VAL_FAIL,
} portal_val_t;

#define VAL_WIFI_TIMEOUT_MS 25000
#define VAL_MQTT_TIMEOUT_MS 20000

static portal_val_t        s_val;
static int64_t             s_val_started;
static char                s_val_msg[160];
static capstan_wifi_cfg_t  s_pending_w;
static capstan_mqtt_cfg_t  s_pending_m;
static bool                s_pending_mqtt;

static void val_fail(const char *why)
{
    /* Nothing was persisted, so there is nothing to roll back. Stop trying,
     * so the radio is not hammering a wrong passphrase while the user
     * retypes it. */
    capstan_wifi_disconnect();
    capstan_mqtt_stop();
    s_val = PORTAL_VAL_FAIL;
    strlcpy(s_val_msg, why, sizeof(s_val_msg));
    ESP_LOGW(TAG, "provisioning check failed: %s (nothing saved)", why);
}

void capstan_portal_tick(void)
{
    if (s_val != PORTAL_VAL_WIFI && s_val != PORTAL_VAL_MQTT) {
        return;
    }
    const int64_t elapsed_ms = (esp_timer_get_time() - s_val_started) / 1000;

    if (s_val == PORTAL_VAL_WIFI) {
        if (capstan_wifi_is_connected()) {
            if (!s_pending_mqtt) {
                if (capstan_config_set_wifi(&s_pending_w) != ESP_OK) {
                    val_fail("Joined, but could not save settings");
                    return;
                }
                s_val = PORTAL_VAL_OK;
                strlcpy(s_val_msg,
                        "Wi-Fi saved. No broker host was given, so this "
                        "display will show no data until one is set.",
                        sizeof(s_val_msg));
                s_got_creds = true;
                return;
            }
            s_val = PORTAL_VAL_MQTT;
            s_val_started = esp_timer_get_time();
            snprintf(s_val_msg, sizeof(s_val_msg), "Joined. Checking %s...",
                     s_pending_m.host);
            if (capstan_mqtt_try(&s_pending_m) != ESP_OK) {
                val_fail("Joined Wi-Fi, but the broker could not be reached");
            }
            return;
        }
        if (capstan_wifi_state() == CAPSTAN_WIFI_FAILED) {
            const char *e = capstan_wifi_last_error();
            val_fail((e && e[0]) ? e : "Could not join that network");
            return;
        }
        if (elapsed_ms > VAL_WIFI_TIMEOUT_MS) {
            val_fail("Timed out joining that network -- check the password");
        }
        return;
    }

    /* PORTAL_VAL_MQTT */
    if (capstan_mqtt_is_connected()) {
        if (capstan_config_set_wifi(&s_pending_w) != ESP_OK ||
            capstan_config_set_mqtt(&s_pending_m) != ESP_OK) {
            val_fail("Everything worked, but settings could not be saved");
            return;
        }
        s_val = PORTAL_VAL_OK;
        strlcpy(s_val_msg, "Saved. The display is connected -- you can close "
                           "this page and rejoin your normal Wi-Fi.",
                sizeof(s_val_msg));
        ESP_LOGI(TAG, "provisioning verified and saved: '%s' -> %s:%u",
                 s_pending_w.ssid, s_pending_m.host,
                 (unsigned)s_pending_m.port);
        s_got_creds = true;
        return;
    }
    if (elapsed_ms > VAL_MQTT_TIMEOUT_MS) {
        const char *e = capstan_mqtt_last_error();
        char why[160];
        snprintf(why, sizeof(why),
                 "Joined Wi-Fi, but the broker did not answer%s%s",
                 (e && e[0]) ? " -- " : "", (e && e[0]) ? e : "");
        val_fail(why);
    }
}

static esp_err_t save_post(httpd_req_t *req)
{
    char buf[512];
    int total = 0;
    while (total < req->content_len && total < (int)sizeof(buf) - 1) {
        const int r = httpd_req_recv(req, buf + total,
                                     sizeof(buf) - 1 - total);
        if (r <= 0) { break; }
        total += r;
    }
    buf[total > 0 ? total : 0] = '\0';

    cJSON *j = cJSON_Parse(buf);
    if (!j) {
        httpd_resp_set_type(req, "application/json");
        httpd_resp_sendstr(req, "{\"ok\":false,\"error\":\"Bad request\"}");
        return ESP_OK;
    }

    const cJSON *ssid = cJSON_GetObjectItem(j, "ssid");
    if (!cJSON_IsString(ssid) || !ssid->valuestring[0]) {
        cJSON_Delete(j);
        httpd_resp_set_type(req, "application/json");
        httpd_resp_sendstr(req,
            "{\"ok\":false,\"error\":\"Network name is required\"}");
        return ESP_OK;
    }

    /* ------------------------------------------------------------------
     * Parse into PENDING buffers. NOTHING is written to NVS here.
     *
     * Provisioning used to save first and find out afterwards. A wrong
     * passphrase, or a broker host that does not resolve, was committed
     * anyway -- and the panel then booted believing it was provisioned,
     * retried forever, and showed no route back to setup. Recovering meant
     * a factory reset.
     *
     * So /save now only STARTS a check. The credentials are applied to the
     * radio and the broker, and they reach NVS in portal_validate_tick()
     * only once both have actually worked.
     * ------------------------------------------------------------------ */
    capstan_wifi_cfg_t w;
    capstan_config_get_wifi(&w);
    strlcpy(w.ssid, ssid->valuestring, sizeof(w.ssid));

    const cJSON *pw = cJSON_GetObjectItem(j, "pw");
    strlcpy(w.password, cJSON_IsString(pw) ? pw->valuestring : "",
            sizeof(w.password));

    const cJSON *sec = cJSON_GetObjectItem(j, "sec");
    w.security = cJSON_IsNumber(sec) &&
                 sec->valueint >= 0 && sec->valueint < CAPSTAN_WIFI_SEC_COUNT
                 ? (capstan_wifi_sec_t)sec->valueint
                 : CAPSTAN_WIFI_SEC_WPA_WPA2_PSK;
    w.configured = true;

    /* The broker stays optional -- a panel on the network can be pointed at
     * one later -- but "optional" now means "skipped deliberately", not
     * "silently dropped": an empty host is reported on the page. */
    capstan_mqtt_cfg_t m;
    capstan_config_get_mqtt(&m);
    bool want_mqtt = false;
    const cJSON *mh = cJSON_GetObjectItem(j, "mh");
    if (cJSON_IsString(mh) && mh->valuestring[0]) {
        strlcpy(m.host, mh->valuestring, sizeof(m.host));
        const cJSON *mp = cJSON_GetObjectItem(j, "mp");
        m.port = cJSON_IsNumber(mp) && mp->valueint > 0 && mp->valueint < 65536
                 ? (uint16_t)mp->valueint : 8883;
        const cJSON *mu = cJSON_GetObjectItem(j, "mu");
        strlcpy(m.username, cJSON_IsString(mu) ? mu->valuestring : "",
                sizeof(m.username));
        const cJSON *mpw = cJSON_GetObjectItem(j, "mpw");
        strlcpy(m.password, cJSON_IsString(mpw) ? mpw->valuestring : "",
                sizeof(m.password));
        m.configured = true;
        want_mqtt = true;
    } else {
        ESP_LOGW(TAG, "no broker host submitted -- will save Wi-Fi only");
    }

    cJSON_Delete(j);

    s_pending_w    = w;
    s_pending_m    = m;
    s_pending_mqtt = want_mqtt;
    s_val_started  = esp_timer_get_time();
    s_val          = PORTAL_VAL_WIFI;
    snprintf(s_val_msg, sizeof(s_val_msg), "Joining %s...", w.ssid);

    const esp_err_t terr = capstan_wifi_try(&s_pending_w);
    if (terr != ESP_OK) {
        s_val = PORTAL_VAL_FAIL;
        snprintf(s_val_msg, sizeof(s_val_msg), "Could not start Wi-Fi (%s)",
                 esp_err_to_name(terr));
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"ok\":true,\"checking\":true}");
    return ESP_OK;
}

/* Progress of the check, polled by the page. */
static esp_err_t status_handler(httpd_req_t *req)
{
    const char *st = "idle";
    switch (s_val) {
    case PORTAL_VAL_WIFI:
    case PORTAL_VAL_MQTT: st = "checking"; break;
    case PORTAL_VAL_OK:   st = "ok";       break;
    case PORTAL_VAL_FAIL: st = "fail";     break;
    default:              st = "idle";     break;
    }

    cJSON *o = cJSON_CreateObject();
    cJSON_AddStringToObject(o, "state", st);
    cJSON_AddStringToObject(o, "msg", s_val_msg);
    char *txt = cJSON_PrintUnformatted(o);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, txt ? txt : "{\"state\":\"idle\",\"msg\":\"\"}");
    cJSON_free(txt);
    cJSON_Delete(o);
    return ESP_OK;
}



/* ---------------------------------------------------------------- */

static const httpd_uri_t URIS[] = {
    { .uri = "/",       .method = HTTP_GET,  .handler = root_get },
    { .uri = "/scan",   .method = HTTP_GET,  .handler = scan_get },
    { .uri = "/save",   .method = HTTP_POST, .handler = save_post },
    { .uri = "/status", .method = HTTP_GET,  .handler = status_handler },
    /* The probe URLs each OS uses to decide a network is captive. */
    { .uri = "/generate_204",        .method = HTTP_GET, .handler = probe_get },
    { .uri = "/gen_204",             .method = HTTP_GET, .handler = probe_get },
    { .uri = "/hotspot-detect.html", .method = HTTP_GET, .handler = probe_get },
    { .uri = "/library/test/success.html",
                                     .method = HTTP_GET, .handler = probe_get },
    { .uri = "/ncsi.txt",            .method = HTTP_GET, .handler = probe_get },
    { .uri = "/connecttest.txt",     .method = HTTP_GET, .handler = probe_get },
    { .uri = "/redirect",            .method = HTTP_GET, .handler = probe_get },
};

esp_err_t capstan_portal_start(void)
{
    if (s_server) {
        return ESP_OK;
    }
    s_got_creds = false;
    s_val = PORTAL_VAL_IDLE;
    s_val_msg[0] = '\0';

    ESP_RETURN_ON_ERROR(capstan_wifi_ap_start(), TAG, "ap failed");

    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    cfg.max_uri_handlers = sizeof(URIS) / sizeof(URIS[0]) + 2;
    cfg.lru_purge_enable = true;
    /* A blocking scan can take a couple of seconds and the phone must
     * not be dropped while it waits. */
    cfg.recv_wait_timeout = 10;
    cfg.send_wait_timeout = 10;

    esp_err_t err = httpd_start(&s_server, &cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "http server failed: %s", esp_err_to_name(err));
        capstan_wifi_ap_stop();
        s_server = NULL;
        return err;
    }
    for (size_t i = 0; i < sizeof(URIS) / sizeof(URIS[0]); i++) {
        httpd_register_uri_handler(s_server, &URIS[i]);
    }

    portal_dns_start(inet_addr(capstan_wifi_ap_ip()));

    ESP_LOGI(TAG, "setup portal up: join '%s' and browse to http://%s/",
             capstan_wifi_ap_ssid(), capstan_wifi_ap_ip());
    return ESP_OK;
}

esp_err_t capstan_portal_stop(void)
{
    if (!s_server) {
        return ESP_OK;
    }
    portal_dns_stop();
    httpd_stop(s_server);
    s_server = NULL;
    capstan_wifi_ap_stop();
    ESP_LOGI(TAG, "setup portal down");
    return ESP_OK;
}

bool capstan_portal_is_running(void)      { return s_server != NULL; }
bool capstan_portal_got_credentials(void) { return s_got_creds; }

int capstan_portal_client_count(void)
{
    wifi_sta_list_t list = { 0 };
    if (esp_wifi_ap_get_sta_list(&list) != ESP_OK) {
        return 0;
    }
    return list.num;
}
