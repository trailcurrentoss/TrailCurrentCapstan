/*
 * The bridge from the data model to the authored widgets.
 *
 * One place that knows both sides. Screens do not poll the model and the
 * model knows nothing about LVGL; this sits between them and runs on a
 * timer, so a burst of MQTT traffic cannot turn into a burst of redraws.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Start the refresh timer and connect MQTT once Wi-Fi has an IP.
 *
 * Call after ui_init() and capstan_mqtt_init(), with the LVGL lock held.
 */
void ui_data_init(void);

/** Push the current model values into the widgets. LVGL lock held. */
void ui_data_refresh(void);

/**
 * Connection housekeeping. Call from the service task, NOT from an
 * event callback -- see the note in ui_data.c about sys_evt's stack.
 */
void ui_data_service_tick(void);

#ifdef __cplusplus
}
#endif
