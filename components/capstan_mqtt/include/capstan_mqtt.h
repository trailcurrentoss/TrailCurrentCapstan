/*
 * MQTT client for the Headwaters gateway.
 *
 * Subscribes to the rig's `local/...` topics, parses them into the data
 * model, and publishes commands. See docs/mqtt.md for the full contract --
 * including the three domains that have no topic yet.
 *
 * THREADING
 *
 * esp-mqtt delivers on its own event task. Parsing happens THERE, and only
 * the resulting setter calls take the LVGL lock. Doing it the other way
 * round -- taking the lock and then parsing JSON inside it -- produced
 * roughly ten seconds of UI lag on Fireside at this broker's ~200 msg/s.
 *
 * Callbacks registered here run on the MQTT task, not the LVGL task.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CAPSTAN_MQTT_IDLE = 0,
    CAPSTAN_MQTT_CONNECTING,
    CAPSTAN_MQTT_CONNECTED,
    CAPSTAN_MQTT_FAILED,
} capstan_mqtt_state_t;

const char *capstan_mqtt_state_name(capstan_mqtt_state_t s);

typedef void (*capstan_mqtt_state_cb_t)(capstan_mqtt_state_t s, void *ctx);
void capstan_mqtt_set_state_callback(capstan_mqtt_state_cb_t cb, void *ctx);

/** Prepare the client. Does not connect. Call after capstan_config_init(). */
esp_err_t capstan_mqtt_init(void);

/**
 * Connect using the broker details in capstan_config. Call once Wi-Fi has
 * an IP -- there is no point before that, and esp-mqtt's own retry would
 * just burn cycles failing to resolve.
 *
 * Idempotent where that is actually correct: a duplicate request is ignored
 * while connected or just started, but a client that has been down for a
 * while is rebuilt. That distinction matters after a Wi-Fi reassociation,
 * when the old client holds a dead socket and treating its mere existence
 * as "nothing to do" strands the display offline.
 */
esp_err_t capstan_mqtt_connect(void);

/** Tear down and reconnect, ignoring the idempotency check. Use after the
 *  broker settings change. */
esp_err_t capstan_mqtt_reconnect(void);

/** Stop and free the client, releasing its TLS socket. */
esp_err_t capstan_mqtt_stop(void);

capstan_mqtt_state_t capstan_mqtt_state(void);
bool capstan_mqtt_is_connected(void);
const char *capstan_mqtt_last_error(void);

/** Publish. Returns the message id, or -1. */
int capstan_mqtt_publish(const char *topic, const char *payload, int len);

/**
 * Drain the inbound queue and apply messages to the data model.
 * Call from the main loop. Parsing happens here, off the MQTT task.
 */
void capstan_mqtt_process(void);

/**
 * Expire values whose module has gone quiet.
 *
 * NOTHING on this platform is retained, so a value is only ever as good as
 * the last frame. A stale number that still looks live is worse than no
 * number -- the whole point of the `--` convention. Call every loop; it
 * costs nothing until a module actually times out.
 */
void capstan_mqtt_check_watchdogs(void);

/** This device's hostname, esp32-XXXXXX from the last 3 MAC bytes. `out`
 *  needs at least 14 bytes. Returns out. */
const char *capstan_mqtt_hostname(char *out, size_t len);

#ifdef __cplusplus
}
#endif
