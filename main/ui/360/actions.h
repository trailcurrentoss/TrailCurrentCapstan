#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_nav_back(lv_event_t * e);
extern void action_nav_open(lv_event_t * e);
extern void action_nav_home(lv_event_t * e);
extern void action_climate_setpoint_up(lv_event_t * e);
extern void action_climate_setpoint_down(lv_event_t * e);
extern void action_climate_mode_select(lv_event_t * e);
extern void action_light_toggle(lv_event_t * e);
extern void action_scene_apply(lv_event_t * e);
extern void action_heater_level(lv_event_t * e);
extern void action_energy_page(lv_event_t * e);
extern void action_wifi_scan(lv_event_t * e);
extern void action_wifi_select(lv_event_t * e);
extern void action_wifi_connect(lv_event_t * e);
extern void action_wifi_security_select(lv_event_t * e);
extern void action_mqtt_field_edit(lv_event_t * e);
extern void action_mqtt_save(lv_event_t * e);
extern void action_keyboard_key(lv_event_t * e);
extern void action_keyboard_ok(lv_event_t * e);
extern void action_keyboard_cancel(lv_event_t * e);
extern void action_settings_toggle_units(lv_event_t * e);
extern void action_settings_toggle_theme(lv_event_t * e);
extern void action_touch_calibrate_start(lv_event_t * e);
extern void action_touch_calibrate_sample(lv_event_t * e);
extern void action_factory_reset(lv_event_t * e);
extern void action_alert_dismiss(lv_event_t * e);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/