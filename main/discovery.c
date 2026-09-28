/*
 * Headwaters discovery. See discovery.h for the handshake this implements
 * and why the broker connection is dropped for the duration.
 */

#include <stdio.h>
#include <string.h>

#include "esp_app_desc.h"
#include "esp_heap_caps.h"
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

/*
 * mDNS instance name base.
 *
 * The DNS-SD service instance name -- what goes in the PTR record and keys
 * the whole service in every browser -- MUST be unique on the link. It is
 * NOT decoration, even though nothing on our side parses it: Headwaters'
 * host-side browser (local_code/discovery-mdns.py) reads the hostname from
 * the SRV target and the module type from TXT, but zeroconf indexes the
 * record it resolves by instance name, so two devices claiming the same one
 * collapse into a single entry.
 *
 * Every TrailCurrent module used to advertise the literal
 * "TrailCurrent Discovery". With one of each module type on a rig, the mDNS
 * component's conflict resolution papered over it -- the loser of each
 * collision renames itself to "...-2" and reprobes. With three identical
 * Capstans opening their discovery window off the same broadcast, that
 * degenerates: all three probe at once, the two losers BOTH mangle to the
 * same "-2", collide again, and the result is a multi-round race inside a
 * 30-second window. Any round whose multicast is missed leaves two dials
 * sharing a name, and only one of them reaches Overlook's list.
 *
 * The hostname is appended below to make the name unique by construction so
 * none of that has to happen at all.
 */
#define MDNS_INSTANCE_BASE "TrailCurrent Capstan"

/*
 * Re-announcement cadence.
 *
 * mDNS announcements are unacknowledged UDP multicast. The component sends a
 * burst when the service is added and then goes quiet, so the whole window
 * hangs on that burst surviving the air. On a board whose transmit power has
 * been backed off -- the 1.46" runs at 11 dBm, see
 * boards/crowpanel146.defaults -- that is exactly the traffic that gets lost
 * first, while the broker connection stays up because TCP retransmits and
 * mDNS does not. Symptom: a panel that is plainly online and simply never
 * appears in Overlook's list.
 *
 * So announce again on a timer. mdns_service_txt_item_set() re-announces
 * unconditionally on every call (mdns_responder.c does not gate on the value
 * having changed), so bumping a counter key is the supported way to ask for
 * one without removing and re-adding the service -- a remove would emit a
 * goodbye packet and could evict us from a host that had just resolved us.
 *
 * Stops after REANNOUNCE_UNTIL_MS because Headwaters' browser only listens
 * for BROWSE_TIMEOUT_S = 35 s (local_code/discovery-mdns.py); multicasting
 * into a window nobody is watching buys nothing. The HTTP server stays up for
 * the full DISCOVERY_TIMEOUT_MS regardless.
 */
/*
 * Discovery task stack, in bytes. 8 KB matches the siblings; the task carries
 * the mDNS calls and the wait loop, while the HTTP server runs on its own.
 * Named rather than inlined so the failure log below reports the size that
 * was actually requested.
 */
#define DISCOVERY_TASK_STACK 8192

#define REANNOUNCE_INTERVAL_MS 5000
#define REANNOUNCE_UNTIL_MS    40000

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

    char instance[64];
    snprintf(instance, sizeof(instance), "%s %s", MDNS_INSTANCE_BASE, hostname);
    mdns_instance_name_set(instance);
    return true;
}

/* Tears down the service AND the component, handing UDP 5353 back to lwIP. */
static void mdns_down(void)
{
    mdns_free();
}

/*
 * Internal heap, at each step that can fail for want of it.
 *
 * mdns_init(), httpd_start() and the service add all allocate, and
 * CONFIG_MDNS_MEMORY_ALLOC_INTERNAL=y keeps mDNS out of PSRAM entirely. The
 * larger panels carry bigger draw buffers, so "works on one board, not on
 * another" and "runs out of internal RAM on the bigger board" look identical
 * from the outside. Largest free block is printed alongside each total because
 * a fragmented heap fails an allocation while still reporting plenty free --
 * and a task stack is one contiguous block, so the largest block is the number
 * that actually decides whether xTaskCreate() succeeds.
 */
static void log_heap(const char *when)
{
    ESP_LOGI(TAG, "heap %s: internal %u B free / %u B largest, "
                  "DMA %u B free / %u B largest",
             when,
             (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
             (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL),
             (unsigned)heap_caps_get_free_size(MALLOC_CAP_DMA),
             (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_DMA));
}

/* Force a fresh announcement. See REANNOUNCE_INTERVAL_MS above. */
static void reannounce(unsigned seq)
{
    char val[12];
    snprintf(val, sizeof(val), "%u", seq);

    const esp_err_t err =
        mdns_service_txt_item_set("_trailcurrent", "_tcp", "seq", val);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "re-announce %u failed (%s)", seq, esp_err_to_name(err));
        return;
    }
    ESP_LOGI(TAG, "re-announced (seq=%u)", seq);
}

static void advertise(void)
{
    const esp_app_desc_t *app = esp_app_get_description();

    mdns_txt_item_t txt[] = {
        { "type", MODULE_TYPE },
        { "fw",   app->version },
    };

    char hostname[24];
    capstan_mqtt_hostname(hostname, sizeof(hostname));

    /* Per-device instance name -- see MDNS_INSTANCE_BASE above for why this
     * must not be the same literal on every dial. */
    char instance[64];
    snprintf(instance, sizeof(instance), "TrailCurrent Discovery %s", hostname);

    const esp_err_t err = mdns_service_add(instance,
                                           "_trailcurrent", "_tcp", 80,
                                           txt, sizeof(txt) / sizeof(txt[0]));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mdns_service_add failed (%s)", esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "advertising \"%s\" %s.local type=%s fw=%s",
             instance, hostname, MODULE_TYPE, app->version);
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

    log_heap("entering discovery");

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
    log_heap("after httpd_start");

    s_confirmed = false;
    const int64_t start = esp_timer_get_time();
    int64_t next_announce = start + (int64_t)REANNOUNCE_INTERVAL_MS * 1000;
    unsigned announced = 1; /* mdns_service_add() sent the first one. */

    while (!s_confirmed) {
        vTaskDelay(pdMS_TO_TICKS(100));

        const int64_t now = esp_timer_get_time();
        const int64_t elapsed_ms = (now - start) / 1000;

        if (elapsed_ms < REANNOUNCE_UNTIL_MS && now >= next_announce) {
            next_announce = now + (int64_t)REANNOUNCE_INTERVAL_MS * 1000;
            reannounce(announced++);
        }

        if (elapsed_ms >= DISCOVERY_TIMEOUT_MS) {
            ESP_LOGW(TAG, "timed out -- nothing confirmed us after %u "
                          "announcement(s)", announced);
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
    /*
     * Measured, not assumed. xTaskCreate() has exactly one failure mode --
     * errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY -- and a FreeRTOS stack must be a
     * single contiguous internal-RAM block, so when this fails the only
     * question is how far short the heap was. Logging it before the attempt
     * gives that number on the next scan instead of another theory.
     */
    log_heap("at discovery trigger");

    if (xTaskCreate(discovery_task, "discovery", DISCOVERY_TASK_STACK, NULL, 3,
                    NULL) != pdPASS) {
        ESP_LOGE(TAG, "could not start the discovery task -- wanted %d B of "
                      "contiguous internal stack", DISCOVERY_TASK_STACK);
        log_heap("after the failed create");
        s_running = false;
    }
}
