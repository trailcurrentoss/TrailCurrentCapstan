/*
 * The Lights screen.
 *
 * WHY THESE ROWS ARE BUILT FROM CODE
 *
 * Every other screen in this project is authored in EEZ Studio, and the rule
 * is that the canvas shows what the device shows. This screen is the one
 * exception, taken deliberately:
 *
 *   - The list has NO BOUND. Headwaters lets the user pick which of the
 *     rig's channels this particular dial drives, and a rig may have thirty.
 *     Authoring a fixed number of rows means a silent cap: exceed it and the
 *     extra controls simply never appear, with nothing to say so.
 *   - The label AND the icon both come from Headwaters, per control. There is
 *     nothing for a designer to choose per row -- the row is data.
 *
 * What IS authored, and therefore still honest on the canvas: the title, the
 * scroll viewport, and the "Use Headwaters to configure" message. Opening the
 * page in EEZ Studio shows the real screen for a dial with no controls.
 *
 * Rows use the generated style getters (get_style_card_*, get_style_label_*),
 * so a row built here is indistinguishable from an authored one.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Rebuild the rows if the control list changed, then write the live on/off
 * state. Call from the data refresh; it is cheap when nothing has changed.
 *
 * Must be called with the LVGL lock held.
 */
void ui_lights_refresh(void);

/**
 * Ring press on the selected control: command it to the opposite of its last
 * reported state. No-op when the selection is out of range or the broker is
 * down. Must be called with the LVGL lock held.
 */
void ui_lights_press(void);

#ifdef __cplusplus
}
#endif
