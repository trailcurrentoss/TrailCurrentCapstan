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
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lwip/inet.h"

#include "capstan_config.h"
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

    /* MQTT is optional here: a panel that is on the network can be
     * pointed at a broker later from Overlook, and refusing to save
     * Wi-Fi because the broker field was left blank would strand the
     * user on the setup AP. */
    capstan_mqtt_cfg_t m;
    capstan_config_get_mqtt(&m);
    const cJSON *mh = cJSON_GetObjectItem(j, "mh");
    /* Say either way. A silent skip here is what made an empty host look
     * like a save that had worked. */
    if (!cJSON_IsString(mh) || !mh->valuestring[0]) {
        ESP_LOGW(TAG, "no broker host submitted -- MQTT left unconfigured");
    }
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
        const esp_err_t merr = capstan_config_set_mqtt(&m);
        if (merr != ESP_OK) {
            ESP_LOGE(TAG, "broker save failed: %s", esp_err_to_name(merr));
        } else {
            ESP_LOGI(TAG, "broker saved: %s:%u", m.host, (unsigned)m.port);
        }
    }

    cJSON_Delete(j);

    const esp_err_t err = capstan_config_set_wifi(&w);
    httpd_resp_set_type(req, "application/json");
    if (err != ESP_OK) {
        httpd_resp_sendstr(req,
            "{\"ok\":false,\"error\":\"Could not save settings\"}");
        return ESP_OK;
    }

    ESP_LOGI(TAG, "credentials saved for '%s'", w.ssid);
    httpd_resp_sendstr(req, "{\"ok\":true}");

    /*
     * Flag only -- the teardown happens elsewhere.
     *
     * Stopping the AP here would kill the socket this response is still
     * being written to, so the phone would see the request fail and the
     * user would assume setup did not work. The owner of the portal
     * polls capstan_portal_got_credentials() and tears down once the
     * reply has gone out.
     */
    s_got_creds = true;
    return ESP_OK;
}

/* ---------------------------------------------------------------- */

static const httpd_uri_t URIS[] = {
    { .uri = "/",       .method = HTTP_GET,  .handler = root_get },
    { .uri = "/scan",   .method = HTTP_GET,  .handler = scan_get },
    { .uri = "/save",   .method = HTTP_POST, .handler = save_post },
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
