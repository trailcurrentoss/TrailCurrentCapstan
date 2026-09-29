/*
 * EEZ Studio action handlers.
 *
 * Every action declared in the .eez-project surfaces here as
 * `void action_<Name>(lv_event_t *e)`. This file is hand-written and lives in
 * main/ -- NOT in main/ui/, which is disposable generated output.
 *
 * What C is allowed to do to an EEZ-authored widget:
 *   content  (lv_label_set_text, lv_slider_set_value, ...)
 *   state    (lv_obj_add_state / clear_state, LV_STATE_CHECKED ...)
 *   flags    (LV_OBJ_FLAG_HIDDEN ...)
 *   events   (lv_obj_add_event_cb)
 *   screens  (loadScreen)
 *
 * What it must never do: geometry, alignment, size, fonts or colours. Those
 * live in the .eez-project. Setting them here makes the device disagree with
 * what EEZ Studio's canvas shows, which is the failure mode this project is
 * organised to prevent.
 */

/* From main/CMakeLists.txt. Never __has_include -- it records no
 * dependency on an absent file, so a unit compiled before the first EEZ
 * Studio export is never rebuilt when the export arrives. See main.c. */
#ifndef CAPSTAN_HAVE_UI
#  error "CAPSTAN_HAVE_UI is not defined -- main/CMakeLists.txt must set it"
#endif

#if CAPSTAN_HAVE_UI

#include <string.h>

#include "esp_log.h"

#include "actions.h"
#include "screens.h"
#include "ui.h"

#include "ui_nav.h"

/*
 * EEZ Studio emits an `extern void action_<name>(lv_event_t *)` for every
 * entry in the project's actions list, and wires it to whichever widget
 * events reference it. Every one of them must exist somewhere or the link
 * fails, which is why the unimplemented ones are stubbed at the bottom
 * rather than left out.
 *
 * Keep these thin. An action runs inside LVGL's event dispatch with the
 * lock held; anything that blocks, allocates heavily or talks to the
 * network belongs in a task, with the action only posting to it.
 */

void action_nav_back(lv_event_t *e)
{
    (void)e;
    /* The Back chip on every screen. ui_nav_back() applies the policy
     * table and swallows the ring press that a touch here also makes. */
    ui_nav_back();
}

/* ----------------------------------------------------------------------
 * Not implemented yet.
 *
 * These are declared by the export because the project lists them, but no
 * widget references most of them yet. They must still link. Each is a
 * no-op rather than an abort: an unimplemented control that does nothing
 * is recoverable, one that panics takes the panel down.
 * ---------------------------------------------------------------------- */
#define CAPSTAN_ACTION_TODO(fn)                                           \
    void fn(lv_event_t *e) { (void)e; }

CAPSTAN_ACTION_TODO(action_nav_open)
CAPSTAN_ACTION_TODO(action_nav_home)
CAPSTAN_ACTION_TODO(action_climate_setpoint_up)
CAPSTAN_ACTION_TODO(action_climate_setpoint_down)
CAPSTAN_ACTION_TODO(action_climate_mode_select)
CAPSTAN_ACTION_TODO(action_device_toggle)
CAPSTAN_ACTION_TODO(action_scene_apply)
CAPSTAN_ACTION_TODO(action_energy_page)
CAPSTAN_ACTION_TODO(action_settings_toggle_units)
CAPSTAN_ACTION_TODO(action_settings_toggle_theme)
CAPSTAN_ACTION_TODO(action_touch_calibrate_start)
CAPSTAN_ACTION_TODO(action_touch_calibrate_sample)
CAPSTAN_ACTION_TODO(action_factory_reset)
CAPSTAN_ACTION_TODO(action_alert_dismiss)

#else
/* No export yet -- keep the translation unit non-empty so it compiles. */
typedef int capstan_actions_placeholder;
#endif
