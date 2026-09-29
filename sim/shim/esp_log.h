/*
 * Simulator stand-in for ESP-IDF's esp_log.h. Output goes to the simulator's
 * console panel in EEZ Studio, in the same "L (tag) message" shape the
 * serial monitor shows, so a log line reads the same in both places.
 */
#pragma once

#include <stdio.h>

#include "esp_err.h"

#define SIM_LOG_(letter, tag, fmt, ...) \
    printf(letter " (%s) " fmt "\n", (tag), ##__VA_ARGS__)

#define ESP_LOGE(tag, fmt, ...) SIM_LOG_("E", tag, fmt, ##__VA_ARGS__)
#define ESP_LOGW(tag, fmt, ...) SIM_LOG_("W", tag, fmt, ##__VA_ARGS__)
#define ESP_LOGI(tag, fmt, ...) SIM_LOG_("I", tag, fmt, ##__VA_ARGS__)
/* Debug and verbose are off, as in the firmware's default log level. The
 * arguments are still type-checked. */
#define ESP_LOGD(tag, fmt, ...) do { if (0) SIM_LOG_("D", tag, fmt, ##__VA_ARGS__); } while (0)
#define ESP_LOGV(tag, fmt, ...) do { if (0) SIM_LOG_("V", tag, fmt, ##__VA_ARGS__); } while (0)
