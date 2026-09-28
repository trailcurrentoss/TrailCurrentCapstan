/*
 * The Devices carousel. See ui_devices.h for what this module may and may
 * not do to the authored screen.
 */

#include <stdio.h>
#include <string.h>

#include "esp_log.h"

#include "capstan_config.h"
#include "capstan_model.h"
#include "capstan_mqtt.h"
#include "ui_devices.h"
#include "ui_icons.h"
#include "ui_light_icons.h"
#include "ui_nav.h"

#ifndef CAPSTAN_HAVE_UI
#  error "CAPSTAN_HAVE_UI is not defined -- main/CMakeLists.txt must set it"
#endif

#if CAPSTAN_HAVE_UI

#include "screens.h"
#include "styles.h"
#include "ui.h"

static const char *TAG = "ui.devices";

/*
 * The dot arc.
 *
 * The dots answer two questions at a glance -- how many devices there are,
 * and which one this is -- so the number SHOWN is the number of devices and
 * the lit one is the selected device. Both change at runtime; the widgets
 * cannot move, because moving an authored widget from C is what makes EEZ
 * Studio's canvas disagree with the panel.
 *
 * They sit on an arc at 12 o'clock, growing outwards from there -- the menu's
 * dot row at the same radius and the same spacing, moved to the top because
 * the Back chip owns the bottom of this screen. A row across the face would be
 * bounded by its width and would eventually either run off the glass or shrink
 * until the dots stopped being countable; an arc is not bounded that way.
 *
 * So there are 2*MAX-1 slots at HALF the visible spacing, and a list of n
 * devices lights every OTHER slot starting at slot (MAX - n). Half-spacing
 * slots are what let an EVEN count straddle 12 o'clock and an ODD count sit
 * one dot on it -- one slot per device can only ever centre one of the two,
 * and the other lands half a step round the arc.
 *
 * MAX_DEVICES in GUI/tmp/screens_layout.py authors the arc from the same
 * ceiling, and the generator refuses to write a project where it disagrees
 * with CAPSTAN_MAX_CONTROLS -- see check_device_dots(). The assert is the
 * third leg of that: it catches the header changing and the project never
 * being regenerated, which is the case the generator cannot see.
 */
#define DEVICE_DOT_SLOTS (2 * CAPSTAN_MAX_CONTROLS - 1)

_Static_assert(CAPSTAN_MAX_CONTROLS == 8,
               "the devices dot arc is authored at 2*MAX-1 slots from "
               "MAX_DEVICES in GUI/tmp/screens_layout.py -- change that, "
               "regenerate the projects and re-export before changing this");

static lv_obj_t *device_dot(int slot)
{
    switch (slot) {
    case  0: return objects.devices_dot0;
    case  1: return objects.devices_dot1;
    case  2: return objects.devices_dot2;
    case  3: return objects.devices_dot3;
    case  4: return objects.devices_dot4;
    case  5: return objects.devices_dot5;
    case  6: return objects.devices_dot6;
    case  7: return objects.devices_dot7;
    case  8: return objects.devices_dot8;
    case  9: return objects.devices_dot9;
    case 10: return objects.devices_dot10;
    case 11: return objects.devices_dot11;
    case 12: return objects.devices_dot12;
    case 13: return objects.devices_dot13;
    case 14: return objects.devices_dot14;
    default: return NULL;
    }
}


static void show(lv_obj_t *o, bool visible)
{
    if (!o) {
        return;
    }
    if (visible) {
        lv_obj_clear_flag(o, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(o, LV_OBJ_FLAG_HIDDEN);
    }
}

static void set_checked(lv_obj_t *o, bool on)
{
    if (!o) {
        return;
    }
    if (on) {
        lv_obj_add_state(o, LV_STATE_CHECKED);
    } else {
        lv_obj_remove_state(o, LV_STATE_CHECKED);
    }
}

static void set_text(lv_obj_t *o, const char *s)
{
    if (o) {
        lv_label_set_text(o, s);
    }
}

/*
 * Light the dots for `n` devices with `sel` selected, or clear the arc
 * entirely when there are none.
 *
 * Every slot is written on every pass, including the ones that stay hidden.
 * Touching only the slots in use is how an arc gets left with a stale dot
 * from a longer list still showing beside a shorter one.
 */
static void draw_dots(int n, int sel)
{
    const int first = CAPSTAN_MAX_CONTROLS - n;   /* 0 at a full house */

    for (int slot = 0; slot < DEVICE_DOT_SLOTS; slot++) {
        lv_obj_t *d = device_dot(slot);

        /* Interstitial slots exist only so the run can stay centred on 12
         * o'clock; a device never lands on one. */
        const bool on_rail = ((slot - first) % 2 == 0);
        const int  item    = (slot - first) / 2;
        const bool used    = on_rail && n > 0 && item >= 0 && item < n;

        show(d, used);
        set_checked(d, used && item == sel);
    }
}

static void controls(capstan_controls_t *c)
{
    capstan_config_get_controls(c);
    if (c->count > CAPSTAN_MAX_CONTROLS) {
        /* A payload longer than the dial can hold is truncated rather than
         * refused: driving the first eight is more useful than driving none,
         * and Headwaters caps at the same number. */
        c->count = CAPSTAN_MAX_CONTROLS;
    }
}

int ui_devices_count(void)
{
    capstan_controls_t c;
    controls(&c);
    return (int)c.count;
}

/* Index `i` places along the ring, wrapping. The carousel shows its
 * neighbours, so the wrap is visible -- see ui_nav_rotate(). */
static int wrap(int i, int n)
{
    return ((i % n) + n) % n;
}

/*
 * The empty face.
 *
 * No separate message widget: the name and state labels say it, and the tile
 * sits in its off look. A dedicated label would have to be authored on top
 * of the tile -- a round 240 px face has no free band left -- and EEZ
 * Studio's canvas would then show the message and the carousel overlapping,
 * which is a worse lie than the one this screen was rebuilt to remove.
 */
static void draw_empty(void)
{
    set_text(objects.devices_hero_icon, UI_ICON_DEVICES);
    set_checked(objects.devices_hero, false);
    set_checked(objects.devices_hero_icon, false);
    set_text(objects.devices_name, "No devices");
    set_text(objects.devices_value, "Use Headwaters");

    show(objects.devices_prev, false);
    show(objects.devices_next, false);
    draw_dots(0, -1);
}

void ui_devices_refresh(void)
{
    capstan_controls_t c;
    controls(&c);

    if (c.count == 0) {
        draw_empty();
        return;
    }

    /*
     * Clamp the selection HERE rather than writing it back to ui_nav.
     *
     * The device list can shrink under the ring when Headwaters republishes,
     * so the stored selection can be past the end for one pass. Asking the
     * navigator to re-clamp from inside a refresh it can call means a
     * re-entrant path that only terminates if the clamp always succeeds --
     * and it cannot when the user is looking at a different screen. Reading
     * the index and clamping it locally has neither problem: ui_nav_rotate()
     * clamps its own copy the next time the ring moves.
     */
    const int n = (int)c.count;
    int sel = ui_nav_selection_of(CAPSTAN_SCREEN_DEVICES);
    if (sel >= n) { sel = n - 1; }
    if (sel < 0)  { sel = 0; }

    const capstan_control_t *ctl = &c.items[sel];

    set_text(objects.devices_hero_icon, ui_light_icon(ctl->icon));
    set_text(objects.devices_name, ctl->name);

    /*
     * On/off only, and keyed per device rather than on the module being
     * alive: a channel nothing has ever reported reads `--` while its
     * neighbours report happily. Asking whether the module is up instead
     * would show a confident "Off" for a device nobody has heard from.
     *
     * The tile and its glyph both carry CHECKED. Text colour is inheritable
     * in LVGL, but DeviceIcon sets its own, so the glyph would not follow
     * the tile's state on its own -- and a muted glyph inside an accent ring
     * reads as a control caught mid-switch.
     */
    const bool known = capstan_model_light_known(ctl->id);
    const bool on    = known && capstan_model_light_on(ctl->id);
    set_text(objects.devices_value, !known ? "--" : (on ? "On" : "Off"));
    set_checked(objects.devices_hero, on);
    set_checked(objects.devices_hero_icon, on);

    /*
     * The neighbours show that the list continues, so with nothing to
     * continue to they are hidden. At two devices they are the same device
     * on both sides, which is what a two-item ring looks like and is not
     * worth a special case.
     */
    if (n < 2) {
        show(objects.devices_prev, false);
        show(objects.devices_next, false);
    } else {
        show(objects.devices_prev, true);
        show(objects.devices_next, true);
        set_text(objects.devices_prev_icon,
                 ui_light_icon(c.items[wrap(sel - 1, n)].icon));
        set_text(objects.devices_next_icon,
                 ui_light_icon(c.items[wrap(sel + 1, n)].icon));
    }

    /* n dots on the rim, the sel-th lit, centred on 12 o'clock. */
    draw_dots(n, sel);
}

/*
 * Ring press on the selected device.
 *
 * Publishes the intended state rather than a "toggle" verb, because the
 * contract is `{"state":0|1}` on local/lights/<id>/command -- see
 * handleLightCommand() in Headwaters' mqtt.js. A Switchback relay is toggled
 * by the backend regardless of the value sent (it has no addressable state),
 * so sending the intended state is right for PDM channels and harmless for
 * relays.
 *
 * Nothing is updated optimistically. The tile changes when
 * local/lights/<id>/status comes back, so what the dial shows is what the
 * rig reports, not what it hoped for -- the rule every other reading follows.
 */
void ui_devices_press(void)
{
    capstan_controls_t c;
    controls(&c);

    const int sel = ui_nav_selection_of(CAPSTAN_SCREEN_DEVICES);
    if (sel < 0 || sel >= (int)c.count) {
        return;
    }
    const capstan_control_t *ctl = &c.items[sel];
    const bool want_on = !capstan_model_light_on(ctl->id);

    char topic[56], payload[24];
    snprintf(topic, sizeof(topic), "local/lights/%u/command",
             (unsigned)ctl->id);
    const int len = snprintf(payload, sizeof(payload), "{\"state\":%d}",
                             want_on ? 1 : 0);

    if (capstan_mqtt_publish(topic, payload, len) < 0) {
        ESP_LOGW(TAG, "'%s' not sent -- broker unavailable", ctl->name);
        return;
    }
    ESP_LOGI(TAG, "'%s' (id %u) -> %s", ctl->name, (unsigned)ctl->id,
             want_on ? "on" : "off");
}

#else

int  ui_devices_count(void)   { return 0; }
void ui_devices_refresh(void) { }
void ui_devices_press(void)   { }

#endif
