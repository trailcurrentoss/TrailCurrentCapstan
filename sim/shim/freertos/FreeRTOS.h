/*
 * Simulator stand-in for FreeRTOS.h.
 *
 * The simulator has exactly one thread -- the browser's -- so every lock the
 * firmware takes between its MQTT and LVGL tasks is uncontended here, and the
 * semaphore calls reduce to no-ops that succeed.
 */
#pragma once

#include <stdint.h>

typedef int      BaseType_t;
typedef uint32_t TickType_t;

#define pdTRUE          1
#define pdFALSE         0
#define pdPASS          pdTRUE
#define portMAX_DELAY   ((TickType_t)0xffffffffUL)
#define pdMS_TO_TICKS(ms) ((TickType_t)(ms))
