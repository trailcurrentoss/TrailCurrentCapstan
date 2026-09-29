/* Simulator stand-in for FreeRTOS semphr.h. See FreeRTOS.h. */
#pragma once

#include "FreeRTOS.h"

typedef void *SemaphoreHandle_t;

/* Any non-NULL value: callers only check the create succeeded. */
#define xSemaphoreCreateMutex()      ((SemaphoreHandle_t)1)
#define xSemaphoreTake(sem, ticks)   ((void)(sem), (void)(ticks), pdTRUE)
#define xSemaphoreGive(sem)          ((void)(sem), pdTRUE)
