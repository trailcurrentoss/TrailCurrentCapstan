/*
 * The Lights screen: rows built from the controls Headwaters assigned to
 * this dial. See ui_lights.h for why these rows are not authored.
 */

#include <stdio.h>
#include <string.h>

#include "esp_log.h"

#include "capstan_config.h"
#include "capstan_model.h"
#include "capstan_mqtt.h"
#include "ui_lights.h"
#include "ui_light_icons.h"
#include "ui_nav.h"

#ifndef CAPSTAN_HAVE_UI
#  error "CAPSTAN_HAVE_UI is not defined -- main/CMakeLists.txt must set it"
#endif

#if CAPSTAN_HAVE_UI

#include "screens.h"
#include "styles.h"
#include "ui.h"

static const char *TAG = "ui.lights";

/*
 * Row geometry, as a fraction of the viewport.
 *
 * The authored lists use `visible = 3` below 360 px and 4 at or above it --
 * see scroll_list() in GUI/tmp/screens_layout.py -- and these rows have to
 * match, or the one screen built from code looks different from every screen
 * that is not. The viewport height is read from the widget rather than
 * assumed, so this follows the authored panel if it ever moves.
 */
static int32_t row_height(int32_t viewport_h, int32_t screen_w)
{
    const int visible = (screen_w < 360) ? 3 : 4;
    return viewport_h / visible;
}

/*
 * What the list was built from last time.
 *
 * Rebuilding every refresh would destroy and recreate every widget four times
 * a second, which throws away the scroll position and the selection with it.
 * The controls only change when Headwaters republishes, so the rebuild is
 * keyed on the payload rather than on the clock.
 */
static uint32_t s_built_sig;
static uint8_t  s_built_count;

static uint32_t controls_signature(const capstan_controls_t *c)
{
    /* FNV-1a over the fields that affect what is drawn. Not a checksum for
     * integrity -- just "is this the same list I already built". */
    uint32_t h = 2166136261u;
    h = (h ^ c->count) * 16777619u;
    for (uint8_t i = 0; i < c->count; i++) {
        h = (h ^ (c->items[i].id & 0xFF)) * 16777619u;
        h = (h ^ (c->items[i].id >> 8)) * 16777619u;
        for (const char *p = c->items[i].name; *p; p++) {
            h = (h ^ (uint8_t)*p) * 16777619u;
        }
        for (const char *p = c->items[i].icon; *p; p++) {
            h = (h ^ (uint8_t)*p) * 16777619u;
        }
    }
    return h;
}

/*
 * One row.
 *
 * Every style comes from the generated getters, so a row built here is the
 * same object a row authored in EEZ Studio would be -- same Card background,
 * same CHECKED look for the ring highlight, same icon and text styles. That
 * is what keeps this screen looking like the rest of the product despite not
 * being drawn on the canvas.
 */
static void build_row(lv_obj_t *parent, const capstan_control_t *ctl,
                      int index, int32_t vp_w, int32_t row_h)
{
    lv_obj_t *row = lv_obj_create(parent);
    lv_obj_remove_style_all(row);
    lv_obj_add_style(row, get_style_card_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(row, get_style_card_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_pos(row, 0, index * row_h);
    lv_obj_set_size(row, vp_w, row_h - 2);
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

    /* Icon, resolved from the key Headwaters sent. */
    lv_obj_t *icon = lv_label_create(row);
    lv_obj_remove_style_all(icon);
    lv_obj_add_style(icon, get_style_label_icon_sm_MAIN_DEFAULT(),
                     LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(icon, ui_light_icon(ctl->icon));
    lv_obj_set_style_align(icon, LV_ALIGN_LEFT_MID, LV_PART_MAIN);
    lv_obj_set_style_translate_x(icon, (vp_w * 3) / 100, LV_PART_MAIN);

    /* Name. */
    lv_obj_t *title = lv_label_create(row);
    lv_obj_remove_style_all(title);
    lv_obj_add_style(title, get_style_label_body_MAIN_DEFAULT(),
                     LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_long_mode(title, LV_LABEL_LONG_DOT);
    lv_obj_set_width(title, (vp_w * 42) / 100);
    lv_label_set_text(title, ctl->name);
    lv_obj_set_style_align(title, LV_ALIGN_LEFT_MID, LV_PART_MAIN);
    lv_obj_set_style_translate_x(title, (vp_w * 24) / 100, LV_PART_MAIN);

    /* State, filled in by ui_lights_refresh(). */
    lv_obj_t *value = lv_label_create(row);
    lv_obj_remove_style_all(value);
    lv_obj_add_style(value, get_style_label_muted_MAIN_DEFAULT(),
                     LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(value, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
    lv_obj_set_width(value, (vp_w * 28) / 100);
    lv_label_set_text(value, "--");
    lv_obj_set_style_align(value, LV_ALIGN_RIGHT_MID, LV_PART_MAIN);
    lv_obj_set_style_translate_x(value, -(vp_w * 3) / 100, LV_PART_MAIN);
}

static bool rebuild(const capstan_controls_t *c)
{
    lv_obj_t *list = objects.lights_list;
    if (!list) {
        return false;
    }

    /*
     * Force layout before measuring.
     *
     * EEZ builds every screen at ui_init(), but LVGL only computes geometry
     * for a screen when it is shown -- so a percentage-sized child of a
     * screen the user has not visited yet reports width 0. Reading that and
     * giving up is how this screen stayed empty while the controls were
     * sitting in NVS: the payload arrives long before anyone opens Lights.
     */
    lv_obj_update_layout(list);

    const int32_t vp_w = lv_obj_get_width(list);
    const int32_t vp_h = lv_obj_get_height(list);
    const int32_t scr_w = objects.page_lights ? lv_obj_get_width(objects.page_lights) : 360;
    if (vp_w <= 0 || vp_h <= 0) {
        /* Still not measurable. Do NOT record this as built -- the caller
         * keys the rebuild on a signature, and recording it here would mean
         * never trying again. */
        return false;
    }

    lv_obj_clean(list);
    const int32_t row_h = row_height(vp_h, scr_w);

    for (uint8_t i = 0; i < c->count; i++) {
        build_row(list, &c->items[i], i, vp_w, row_h);
    }

    if (objects.lights_empty) {
        if (c->count == 0) {
            lv_obj_clear_flag(objects.lights_empty, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(objects.lights_empty, LV_OBJ_FLAG_HIDDEN);
        }
    }

    ESP_LOGI(TAG, "built %u control row(s)", (unsigned)c->count);

    /* The list length changed under the ring, so the selection has to be
     * re-clamped and the highlight repainted. */
    ui_nav_selection_changed();
    return true;
}

void ui_lights_refresh(void)
{
    capstan_controls_t c;
    capstan_config_get_controls(&c);
    if (c.count > CAPSTAN_MAX_CONTROLS) {
        c.count = CAPSTAN_MAX_CONTROLS;
    }

    /*
     * Record the signature only once the rebuild has actually happened.
     * Setting it first meant a rebuild that bailed -- because the screen had
     * never been shown and so had no measurable size -- was remembered as
     * done, and the rows never appeared however long the dial ran.
     */
    const uint32_t sig = controls_signature(&c);
    if (sig != s_built_sig && rebuild(&c)) {
        s_built_sig   = sig;
        s_built_count = c.count;
    }

    /* State is live, so it is written every pass rather than at build time.
     * Rows and controls are in the same order by construction. */
    lv_obj_t *list = objects.lights_list;
    if (!list) {
        return;
    }
    const uint32_t rows = lv_obj_get_child_count(list);
    for (uint32_t i = 0; i < rows && i < c.count; i++) {
        lv_obj_t *row = lv_obj_get_child(list, i);
        /* icon, title, value -- value is the last child, see build_row(). */
        if (lv_obj_get_child_count(row) < 3) {
            continue;
        }
        lv_obj_t *value = lv_obj_get_child(row, 2);

        /*
         * A relay reports on/off and nothing else; a PDM channel could report
         * brightness, but this screen is deliberately on/off only -- see
         * page_lights() in screens_layout.py.
         *
         * Keyed per light rather than on the module being alive: one channel
         * that has never reported should read `--` even while its neighbours
         * are reporting happily. Asking the module instead would show a
         * confident "Off" for a light nothing has ever said anything about.
         */
        const uint16_t id = c.items[i].id;
        if (!capstan_model_light_known(id)) {
            lv_label_set_text(value, "--");
        } else {
            lv_label_set_text(value, capstan_model_light_on(id) ? "On" : "Off");
        }
    }
}

/*
 * Ring press on a control row.
 *
 * Publishes the intended state rather than a "toggle" verb, because the
 * contract is `{"state":0|1}` on local/lights/<id>/command -- see
 * handleLightCommand() in Headwaters' mqtt.js. A Switchback relay is toggled
 * by the backend regardless of the value sent (it has no addressable state),
 * so sending the intended state is right for PDM channels and harmless for
 * relays.
 *
 * Nothing is updated optimistically. The row changes when
 * local/lights/<id>/status comes back, so what the dial shows is what the rig
 * reports, not what it hoped for -- the same rule every other reading follows.
 */
void ui_lights_press(void)
{
    capstan_controls_t c;
    capstan_config_get_controls(&c);
    if (c.count > CAPSTAN_MAX_CONTROLS) {
        c.count = CAPSTAN_MAX_CONTROLS;
    }

    const int sel = ui_nav_selection_of(CAPSTAN_SCREEN_LIGHTS);
    if (sel < 0 || sel >= (int)c.count) {
        return;
    }
    const capstan_control_t *ctl = &c.items[sel];
    const bool want_on = !capstan_model_light_on(ctl->id);

    char topic[56], payload[24];
    snprintf(topic, sizeof(topic), "local/lights/%u/command",
             (unsigned)ctl->id);
    const int n = snprintf(payload, sizeof(payload), "{\"state\":%d}",
                           want_on ? 1 : 0);

    if (capstan_mqtt_publish(topic, payload, n) < 0) {
        ESP_LOGW(TAG, "'%s' not sent -- broker unavailable", ctl->name);
        return;
    }
    ESP_LOGI(TAG, "'%s' (id %u) -> %s", ctl->name, (unsigned)ctl->id,
             want_on ? "on" : "off");
}

#else

void ui_lights_refresh(void) { }
void ui_lights_press(void) { }

#endif
