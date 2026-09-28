/*
 * MQTT settings screen: show the saved broker settings, edit them
 * through the shared keyboard, save to NVS.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/** Refresh the rows from NVS. Call on screen entry, LVGL lock held. */
void ui_mqtt_screen_entered(void);

/** A row was pressed: 0..3 edit a field, 4 saves. LVGL lock held. */
void ui_mqtt_row_pressed(int index);

#ifdef __cplusplus
}
#endif
