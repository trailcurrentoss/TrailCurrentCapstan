/*
 * Alarm evaluation and the alert overlay.
 *
 * The alarm list arrives from Headwaters on
 * local/config/panel/<host>/alarms and is stored in NVS by capstan_config;
 * the raw inputs arrive on local/picket/+/inputs and local/spoor/+/inputs and
 * land in capstan_model. This module is the join: every refresh it evaluates
 * each configured alarm against its board's input word and the current rig
 * mode (capstan_alarm_is_active() -- the only place the verdict rule lives),
 * folds in Borealis's gas flags, and raises PageAlert on a rising edge.
 *
 * Content only, like ui_devices: the overlay is authored in EEZ Studio and
 * this module sets label text on it and nothing else.
 *
 * An alarm holds the overlay until the user acknowledges it with a press, or
 * until it clears. If the alarm on the glass clears while another is still
 * firing, the overlay moves to that one; when the last clears, the overlay
 * closes by itself. Either way the dial returns to the screen it was on.
 *
 * Acknowledging snoozes every alarm on the overlay for the interval set under
 * Settings > Alarm Snooze, and the dial is usable meanwhile. An alarm still
 * active when its snooze runs out is raised again. An alarm that clears by a
 * state change drops its snooze timer, so a later re-fire is raised at once
 * and an old timer can never raise an alarm that is no longer there.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/** Evaluate alarms and drive the overlay. LVGL lock held; call per refresh. */
void ui_alerts_tick(void);

/** Close the overlay and return to the screen it interrupted. LVGL lock held. */
void ui_alerts_dismiss(void);

/** The user pressed the overlay: snooze what it shows, then close it. */
void ui_alerts_acknowledge(void);

#ifdef __cplusplus
}
#endif
