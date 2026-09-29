/*
 * Simulator stand-in for ESP-IDF's esp_system.h.
 *
 * esp_restart() reloads the simulator page -- the nearest thing to a reboot,
 * and it drops the in-memory settings the same way a factory reset drops NVS.
 */
#pragma once

#include <stdint.h>

void     esp_restart(void);
uint32_t esp_get_free_heap_size(void);
