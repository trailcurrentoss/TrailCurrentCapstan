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
     * mdns_init() is idempotent per boot but not re-entrant, and nothing else
     * in this firmware starts it, so this is the only call.
     */
    const esp_err_t err = mdns_init();
    if (err != ESP_OK) {
        /* Not fatal. Everything else -- Wi-Fi, the broker, the whole UI --
         * works without mDNS; the one thing that will not is being found by
         * Overlook, which is exactly what this warning is about. */
        ESP_LOGE(TAG, "mdns_init failed (%s) -- this panel will not be "
                      "discoverable from Headwaters", esp_err_to_name(err));
        return;
    }

    char hostname[24];
    capstan_mqtt_hostname(hostname, sizeof(hostname));
    mdns_hostname_set(hostname);
    mdns_instance_name_set(MDNS_INSTANCE);

    ESP_LOGI(TAG, "mDNS up -- this device is %s.local; waiting for "
                  "local/discovery/trigger", hostname);
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
    mdns_service_remove("_trailcurrent", "_tcp");

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
