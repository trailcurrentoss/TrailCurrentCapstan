/*
 * Headwaters discovery. See discovery.h for the handshake this implements
 * and why the broker connection is dropped for the duration.
 */

#include <string.h>

#include "esp_app_desc.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mdns.h"

#include "capstan_mqtt.h"
#include "discovery.h"

static const char *TAG = "discovery";

/*
 * The `type` TXT record, which is how Overlook decides what it has found and
 * which icon and detail page to give it. It must match the module type the
 * backend knows, not the product name -- lower case, no spaces, same
 * convention as Spotter's "spotter" and Fireside's "fireside".
 */
#define MODULE_TYPE "capstan"

/* mDNS instance name: shown to a human browsing the network, not parsed. */
#define MDNS_INSTANCE "TrailCurrent Capstan"

static volatile bool s_confirmed;
static volatile bool s_running;

void discovery_init(void)
{
    /*
     * Deliberately does NOT start mDNS. See mdns_up() below -- starting it at
     * boot breaks the broker connection on every board.
     */
    ESP_LOGI(TAG, "discovery ready -- will answer local/discovery/trigger");
}

/* ---------------------------------------------------------------------- *
 * mDNS is started per discovery window, NOT at boot.
 *
 * WHY, HONESTLY
 *
 * This is PRECAUTIONARY, not a fix for an observed fault. Read this before
 * changing it, and do not repeat the mistake the first version of this
 * comment made by asserting a cause that had not been established.
 *
 * The broker is reached by an mDNS name -- `mqtts://headwaters.local:8883`.
 * Nothing in this firmware resolves that; lwIP does, through
 * CONFIG_LWIP_DNS_SUPPORT_MDNS_QUERIES, which answers `.local` lookups inside
 * getaddrinfo() by sending its own multicast query. The espressif/mdns
 * component binds UDP 5353 for itself, and the two sharing that port is a
 * known thing to be careful about in ESP-IDF. The component offers no
 * getaddrinfo integration of its own -- no DNS hook, no Kconfig for it.
 *
 * Capstan is the only TrailCurrent device whose BROKER is addressed by a
 * `.local` name, so it is the only one where that would matter. Fireside and
 * Spotter start mDNS at boot and are fine.
 *
 * Confining mDNS to the window costs nothing and removes the question. The
 * window already stops the broker, so for its duration nothing needs to
 * resolve `.local`; outside it, lwIP has 5353 to itself. A wall panel also has
 * no reason to answer multicast queries except while it is being onboarded.
 *
 * WHAT THIS DID NOT FIX
 *
 * All three boards once sat retrying the broker forever with
 *
 *   E esp-tls: couldn't get hostname for :headwaters.local:
 *              getaddrinfo() returns 202, addrinfo=0x0
 *   E mqtt: TLS/TCP error (esp-tls 0x8001)
 *
 * and this arrangement did NOT change that. The cause was external: nothing on
 * the network was answering for `headwaters.local` -- a workstation running
 * none of this code could not resolve it either, and `avahi-browse` saw no
 * mDNS services at all. If this error appears, check that the Headwaters box
 * is up and on the same network BEFORE looking at this file.
 * ---------------------------------------------------------------------- */
static bool mdns_up(void)
{
    const esp_err_t err = mdns_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mdns_init failed (%s) -- cannot be discovered",
                 esp_err_to_name(err));
        return false;
    }

    char hostname[24];
    capstan_mqtt_hostname(hostname, sizeof(hostname));
    mdns_hostname_set(hostname);
    mdns_instance_name_set(MDNS_INSTANCE);
    return true;
}

/* Tears down the service AND the component, handing UDP 5353 back to lwIP. */
static void mdns_down(void)
{
    mdns_free();
}

static void advertise(void)
{
    const esp_app_desc_t *app = esp_app_get_description();

    mdns_txt_item_t txt[] = {
        { "type", MODULE_TYPE },
        { "fw",   app->version },
    };

    const esp_err_t err = mdns_service_add("TrailCurrent Discovery",
                                           "_trailcurrent", "_tcp", 80,
                                           txt, sizeof(txt) / sizeof(txt[0]));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mdns_service_add failed (%s)", esp_err_to_name(err));
        return;
    }

    char hostname[24];
    capstan_mqtt_hostname(hostname, sizeof(hostname));
    ESP_LOGI(TAG, "advertising %s.local type=%s fw=%s",
             hostname, MODULE_TYPE, app->version);
}

static esp_err_t confirm_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "confirmed by Headwaters");
    httpd_resp_sendstr(req, "confirmed\n");
    s_confirmed = true;
    return ESP_OK;
}

static httpd_handle_t start_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    /*
     * Port 80 explicitly, because that is the port advertised in the mDNS
     * record above and the proxy has no way to learn another one. The default
     * already is 80; saying so keeps the two from drifting apart silently if
     * the default ever changes.
     */
    config.server_port = 80;

    httpd_handle_t server = NULL;
    const esp_err_t err = httpd_start(&server, &config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "httpd_start failed (%s) -- cannot be confirmed",
                 esp_err_to_name(err));
        return NULL;
    }

    const httpd_uri_t confirm_uri = {
        .uri     = "/discovery/confirm",
        .method  = HTTP_GET,
        .handler = confirm_handler,
    };
    httpd_register_uri_handler(server, &confirm_uri);
    return server;
}

static void discovery_task(void *arg)
{
    (void)arg;
    ESP_LOGI(TAG, "=== entering discovery mode ===");

    /* Free the TLS session so port 80 can bind. See discovery.h. */
    capstan_mqtt_stop();

    /* Only now -- see the note on mdns_up(). */
    if (!mdns_up()) {
        capstan_mqtt_connect();
        s_running = false;
        vTaskDelete(NULL);
        return;
    }

    advertise();
    httpd_handle_t server = start_server();

    s_confirmed = false;
    const int64_t start = esp_timer_get_time();

    while (!s_confirmed) {
        vTaskDelay(pdMS_TO_TICKS(100));
        if ((esp_timer_get_time() - start) / 1000 >= DISCOVERY_TIMEOUT_MS) {
            ESP_LOGW(TAG, "timed out -- nothing confirmed us");
            break;
        }
    }

    if (server) {
        httpd_stop(server);
    }
    /* mdns_free() drops the service with it, and -- the point of the whole
     * arrangement -- releases UDP 5353 so `headwaters.local` resolves again. */
    mdns_down();

    /*
     * Back to normal on both paths. An unconfirmed window must still
     * reconnect: the alternative is a panel that goes permanently blank on
     * every screen because somebody pressed Scan in Overlook and then closed
     * the tab.
     */
    capstan_mqtt_connect();

    ESP_LOGI(TAG, "=== discovery %s ===",
             s_confirmed ? "complete -- registered" : "ended -- resumed");

    s_running = false;
    vTaskDelete(NULL);
}

bool discovery_is_running(void) { return s_running; }

void discovery_handle_trigger(void)
{
    if (s_running) {
        ESP_LOGW(TAG, "already discovering -- ignoring trigger");
        return;
    }
    s_running = true;

    /*
     * 8 KB, matching the siblings. The HTTP server runs on its own task, so
     * what this stack carries is mDNS calls and the wait loop -- but
     * httpd_start and the mDNS service add both allocate, and the task is
     * created at priority 3 so it stays below the LVGL task and cannot stall
     * a redraw while it waits.
     */
    if (xTaskCreate(discovery_task, "discovery", 8192, NULL, 3, NULL) != pdPASS) {
        ESP_LOGE(TAG, "could not start the discovery task");
        s_running = false;
    }
}
