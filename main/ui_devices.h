/*
 * The Devices screen: the carousel of controls Headwaters assigned to this
 * dial.
 *
 * WHAT THIS MODULE IS AND IS NOT
 *
 * It is content only. The tile, the two neighbour slots, the name, the state
 * word and the row of dots are all authored in EEZ Studio -- see
 * page_devices() in GUI/tmp/screens_layout.py -- and everything here is
 * lv_label_set_text, lv_obj_add_state and LV_OBJ_FLAG_HIDDEN. No geometry,
 * no styles, no widget creation.
 *
 * That is the whole reason this screen was rebuilt. Its previous version
 * created a scrolling list of rows in C with lv_obj_set_pos() and
 * lv_obj_set_size(), so EEZ Studio's canvas showed an empty box where the
 * device rows would be, and nobody could review the one screen users touch
 * most. The carousel shows THREE items whatever the list length, so an
 * unbounded list no longer needs an unbounded number of widgets and the
 * exception that justified building it from code is gone.
 *
 * WHAT IS STILL DATA, AND WHERE IT COMES FROM
 *
 *   name, icon   the retained local/config/capstan/<host>/controls payload,
 *                via capstan_config -- the user picks both in Headwaters
 *   on / off     local/lights/<id>/status, via capstan_model
 *
 * The MQTT topics are still `lights`, because that is the platform's
 * contract and renaming a topic on the dial alone would simply stop it
 * talking to the rig. "Devices" is what this dial calls them, which is the
 * only name a user sees.
 */
#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * How many devices the ring can move through -- the count Headwaters sent,
 * clamped to CAPSTAN_MAX_CONTROLS. Zero on an unconfigured dial, and the
 * ring is then inert on this screen.
 */
int ui_devices_count(void);

/**
 * Write the carousel: the selected device's glyph, name and state, its two
 * neighbours' glyphs, and the dots. Cheap enough for the 250 ms data pass,
 * and also called by ui_nav on rotation.
 *
 * Must be called with the LVGL lock held.
 */
void ui_devices_refresh(void);

/**
 * Ring press on the selected device: command it to the opposite of its last
 * reported state. No-op when there are no devices or the broker is down.
 * Must be called with the LVGL lock held.
 */
void ui_devices_press(void);

/**
 * True when the selected carousel item's last REPORTED state is on -- not
 * the commanded state, so it follows the device when it is switched from
 * elsewhere or fails to switch. False with no devices or no report yet.
 * Drives the LED ring's green on this screen (see ui_alerts.c).
 */
bool ui_devices_selected_on(void);

#ifdef __cplusplus
}
#endif
