/*
 * Model -> widgets.
 *
 * WHY A TIMER AND NOT A CALLBACK PER VALUE
 *
 * MQTT arrives in bursts -- a module waking up republishes everything it
 * has. Redrawing on each value would turn one burst into dozens of
 * invalidations in the same frame, on a panel whose whole job is to look
 * calm. A fixed refresh decouples the two: however chaotic the traffic,
 * the display updates at a steady rate.
 *
 * 250 ms is chosen against human perception rather than data rate.
 * Nothing here is a control loop; these are readings a person glances
 * at, and four updates a second already looks instant.
 */

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_timer.h"

#include "capstan_config.h"
#include "capstan_model.h"
#include "capstan_mqtt.h"
#include "capstan_wifi.h"
#include "ui_alerts.h"
#include "ui_climate.h"
#include "ui_data.h"
#include "ui_devices.h"
#include "ui_nav.h"
#include "ui_settings.h"
#include "ui_setup.h"

#ifndef CAPSTAN_HAVE_UI
#  error "CAPSTAN_HAVE_UI is not defined -- main/CMakeLists.txt must set it"
#endif

#if CAPSTAN_HAVE_UI

#include "screens.h"
#include "ui_lv.h"
#include "ui.h"

static const char *TAG = "ui.data";

#define REFRESH_MS 250

/*
 * Render a reading, or the empty placeholder.
 *
 * `valid` is false both for "never received" and "received but the
 * module has since gone quiet", and both must read as `--`. That is the
 * entire reason capstan_value_t carries the flag instead of using a
 * sentinel float: a stale number that still looks live is worse than no
 * number, and NaN-as-missing is the kind of convention that survives
 * exactly until someone formats it without checking.
 */
static void set_value(lv_obj_t *label, capstan_value_t v,
                      const char *fmt)
{
    if (!label) {
        return;
    }
    if (!v.valid) {
        ui_lv_set_text(label, "--");
        return;
    }
    char buf[24];
    snprintf(buf, sizeof(buf), fmt, v.value);
    ui_lv_set_text(label, buf);
}

static void set_text(lv_obj_t *label, const char *text)
{
    ui_lv_set_text(label, text ? text : "--");
}

/*
 * Energy: the prototype's three pages (P:653-657), one at a time on the ring.
 * ENERGY_PAGES in GUI/tmp/screens_layout.py and ENERGY_PAGE_COUNT in
 * ui_nav.c must list the same three.
 *
 * Watt signs are Victron's, relayed by Solstice: battery_watts is + while
 * the battery charges and - while it discharges (docs/mqtt.md). Loads are not
 * measured per device, or at all -- the Loads page derives the total as
 * solar - net, exactly what the prototype's "Net" line assumes, and exact
 * while solar is the only charger.
 */
#define ENERGY_FULL_SCALE_W 600.0f   /* the prototype's arc scale for W */

/* Signed battery power, or false if unknown. Prefers battery_watts; with a
 * Headwaters that predates it, consumption_watts > 0 still means a draw of
 * that size, but 0 cannot be told apart from charging. */
static bool battery_net_watts(float *out)
{
    const capstan_value_t bw = capstan_model_battery_watts();
    if (bw.valid) {
        *out = bw.value;
        return true;
    }
    const capstan_value_t cw = capstan_model_load_watts();
    if (cw.valid && cw.value > 0.0f) {
        *out = -cw.value;
        return true;
    }
    return false;
}

/* "45 min", "14h 20m", "2d 4h". False past 99 days: that is not draining in
 * any meaningful sense, and a four-digit day count is noise. */
static bool fmt_runtime(char *out, size_t len, float mins)
{
    const float m = mins < 0.0f ? 0.0f : mins;
    const unsigned t = (unsigned)lroundf(m);
    if (m < 60.0f) {
        snprintf(out, len, "%u min", t);
    } else if (m < 48.0f * 60.0f) {
        snprintf(out, len, "%uh %um", t / 60u, t % 60u);
    } else if (m <= 99.0f * 24.0f * 60.0f) {
        snprintf(out, len, "%ud %uh", t / 1440u, (t % 1440u) / 60u);
    } else {
        return false;
    }
    return true;
}

/* The charger's state word, capitalised, or NULL when there is nothing
 * worth saying ("--" before the MPPT reports, "unknown"). */
static const char *charge_word(char *buf, size_t len)
{
    const char *ct = capstan_model_charge_type();
    if (!ct || !ct[0] || strcmp(ct, "--") == 0 || strcmp(ct, "unknown") == 0) {
        return NULL;
    }
    snprintf(buf, len, "%s", ct);
    if (buf[0] >= 'a' && buf[0] <= 'z') {
        buf[0] = (char)(buf[0] - 'a' + 'A');
    }
    return buf;
}

static void show_line(lv_obj_t *label, const char *text)
{
    if (!label) {
        return;
    }
    if (text && text[0]) {
        ui_lv_set_text(label, text);
        ui_lv_set_hidden(label, false);   /* flex re-centres */
    } else {
        ui_lv_set_hidden(label, true);
    }
}

/* Page colour as a state on the arc and the eyebrow (EnergyArc/EnergyHead):
 * DEFAULT battery, CHECKED solar, PRESSED loads, DISABLED (arc) no data. */
static void energy_state(lv_obj_t *o, lv_state_t st)
{
    ui_lv_set_state_in(o, LV_STATE_CHECKED | LV_STATE_PRESSED |
                          LV_STATE_DISABLED, st);
}

static void refresh_energy(void)
{
    const int page = ui_nav_selection_of(CAPSTAN_SCREEN_ENERGY);
    char val[16] = "--", sub1[48] = "", sub2[48] = "", cw[16], rt[16];
    const char *title, *unit, *glyph;
    lv_state_t st;
    float frac = -1.0f;                          /* < 0: no data, track only */
    float net;
    const bool net_ok = battery_net_watts(&net);

    /* Trace, at most every 10 s: what the energy fields are, so "why is a
     * line missing" can be answered from a serial log. */
    {
        static int64_t s_last_trace;
        const int64_t now = esp_timer_get_time();
        if (now - s_last_trace > 10000000) {
            s_last_trace = now;
            const capstan_value_t bw = capstan_model_battery_watts();
            const capstan_value_t cw = capstan_model_load_watts();
            const capstan_value_t rm = capstan_model_runtime_min();
            const capstan_value_t sw = capstan_model_solar_watts();
            ESP_LOGI(TAG, "energy: battery_watts=%s%.0f consumption=%s%.0f "
                          "solar=%s%.0f runtime_min=%s%.0f charge=%s",
                     bw.valid ? "" : "?", (double)bw.value,
                     cw.valid ? "" : "?", (double)cw.value,
                     sw.valid ? "" : "?", (double)sw.value,
                     rm.valid ? "" : "?", (double)rm.value,
                     capstan_model_charge_type());
        }
    }

    switch (page) {
    default:
    case 0: {   /* Battery */
        const capstan_value_t pct = capstan_model_battery_pct();
        const capstan_value_t volts = capstan_model_battery_volts();
        title = "BATTERY"; unit = "%"; st = 0;
        /* The battery glyph follows the charge, from the house subset. */
        const float p = pct.valid ? pct.value : 50.0f;
        glyph = p > 87.5f ? "\xEF\x89\x80" : p > 62.5f ? "\xEF\x89\x81" :
                p > 37.5f ? "\xEF\x89\x82" : p > 12.5f ? "\xEF\x89\x83" :
                            "\xEF\x89\x84";           /* F240..F244 */
        if (pct.valid) {
            snprintf(val, sizeof(val), "%.0f", (double)pct.value);
            frac = pct.value / 100.0f;
        }
        /* "13.4 V · Float" */
        const char *w = charge_word(cw, sizeof(cw));
        if (volts.valid && w) {
            snprintf(sub1, sizeof(sub1), "%.1f V \xC2\xB7 %s", (double)volts.value, w);
        } else if (volts.valid) {
            snprintf(sub1, sizeof(sub1), "%.1f V", (double)volts.value);
        }
        /* Time remaining whenever it is reported -- EXCEPT while the battery
         * is known to be charging, when Headwaters' figure is stale (it never
         * clears it) and "Charging" is the true answer. "Known" needs
         * battery_watts > 0; without that field (an older Headwaters) a zero
         * consumption cannot be told from idle, and the reported runtime is
         * shown as before. */
        const capstan_value_t mins = capstan_model_runtime_min();
        if (net_ok && net > 0.0f) {
            snprintf(sub2, sizeof(sub2), "Charging");
        } else if (mins.valid && fmt_runtime(rt, sizeof(rt), mins.value)) {
            snprintf(sub2, sizeof(sub2), "Time Remaining %s", rt);
        }
        break;
    }

    case 1: {   /* Solar Input */
        const capstan_value_t w = capstan_model_solar_watts();
        title = "SOLAR INPUT"; unit = "W"; st = LV_STATE_CHECKED;
        glyph = "\xEF\x86\x85";                        /* F185 sun */
        if (w.valid) {
            snprintf(val, sizeof(val), "%.0f", (double)w.value);
            frac = w.value / ENERGY_FULL_SCALE_W;
        }
        const char *c = charge_word(cw, sizeof(cw));
        if (c) {
            snprintf(sub1, sizeof(sub1), "Charge Status \xC2\xB7 %s", c);
        }
        /* The prototype's "Today 1.8 kWh" has no data on the bus: Solstice
         * reads the MPPT's yield (H20) but does not transmit it. */
        break;
    }

    case 2: {   /* Loads = solar - net */
        const capstan_value_t sw = capstan_model_solar_watts();
        title = "LOADS"; unit = "W"; st = LV_STATE_PRESSED;
        glyph = "\xEF\x83\xA7";                        /* F0E7 bolt */
        if (sw.valid && net_ok) {
            const float loads = sw.value - net;
            /* Negative means another charger (shore, alternator) is
             * feeding the battery: the derivation no longer holds. */
            if (loads >= 0.0f) {
                snprintf(val, sizeof(val), "%.0f", (double)loads);
                frac = loads / ENERGY_FULL_SCALE_W;
            }
        }
        /* The prototype's first line is a per-device load, which does not
         * exist here; its "Net" line moves up and the second is hidden. */
        if (net_ok) {
            snprintf(sub1, sizeof(sub1), "Net %s%.0f W",
                     net >= 0.0f ? "+" : "-", (double)fabsf(net));
        }
        break;
    }
    }

    set_text(objects.energy_title, title);
    set_text(objects.energy_head_icon, glyph);
    energy_state(objects.energy_title, st);
    energy_state(objects.energy_head_icon, st);
    set_text(objects.energy_value, val);
    set_text(objects.energy_unit, unit);
    show_line(objects.energy_sub1, sub1);
    show_line(objects.energy_sub2, sub2);

    if (objects.energy_arc) {
        if (frac < 0.0f) {
            energy_state(objects.energy_arc, LV_STATE_DISABLED);
            lv_arc_set_value(objects.energy_arc, 0);
        } else {
            energy_state(objects.energy_arc, st);
            lv_arc_set_value(objects.energy_arc,
                             (int32_t)lroundf(fminf(frac, 1.0f) * 100.0f));
        }
    }
}

bool ui_data_energy_led(uint8_t *r, uint8_t *g, uint8_t *b)
{
    /*
     * Battery page only: green above 75 %, yellow 40-75 %, red below 40 %,
     * each brightening as the charge rises within its band -- so a nearly
     * flat battery is a dim red and a full one a bright green. The ring's
     * global cap (CONFIG_CAPSTAN_RGB_LEDS_MAX_BRIGHTNESS) still applies.
     */
    if (ui_nav_selection_of(CAPSTAN_SCREEN_ENERGY) != 0) {
        return false;
    }
    const capstan_value_t pct = capstan_model_battery_pct();
    if (!pct.valid) {
        return false;
    }
    const float p = fminf(fmaxf(pct.value, 0.0f), 100.0f);
    /* The house colours, as the palette defines them: AccentPrimary
     * #52a441 (the brand green -- never the bright `Success`), Solar
     * #ffc107, Danger #ff5453. */
    float t;
    uint8_t R, G, B;
    if (p > 75.0f)       { t = (p - 75.0f) / 25.0f; R = 82;  G = 164; B = 65; }
    else if (p >= 40.0f) { t = (p - 40.0f) / 35.0f; R = 255; G = 193; B = 7;  }
    else                 { t = p / 40.0f;           R = 255; G = 84;  B = 83; }
    const float k = 0.25f + 0.75f * t;           /* 25 % .. 100 % */
    *r = (uint8_t)lroundf(R * k);
    *g = (uint8_t)lroundf(G * k);
    *b = (uint8_t)lroundf(B * k);
    return true;
}

bool ui_data_water_led(uint8_t *r, uint8_t *g, uint8_t *b)
{
    /*
     * Orange when any tank needs attention: fresh below 40 %, or grey or
     * black above 60 %. Green when all are fine: fresh above 40 %, grey and
     * black below 50 %. Between those (a waste tank at 50-60 %, fresh at
     * exactly 40 %) the ring keeps its last colour, so a level wobbling on
     * a threshold does not flicker it. A tank with no reading is left out;
     * with none at all the ring is dark.
     */
    static enum { W_NONE, W_GREEN, W_ORANGE } s_state = W_NONE;

    const capstan_value_t fresh = capstan_model_tank(CAPSTAN_TANK_FRESH);
    const capstan_value_t grey  = capstan_model_tank(CAPSTAN_TANK_GREY);
    const capstan_value_t black = capstan_model_tank(CAPSTAN_TANK_BLACK);
    if (!fresh.valid && !grey.valid && !black.valid) {
        s_state = W_NONE;
        return false;
    }

    const bool attention = (fresh.valid && fresh.value < 40.0f) ||
                           (grey.valid  && grey.value  > 60.0f) ||
                           (black.valid && black.value > 60.0f);
    const bool fine = (!fresh.valid || fresh.value > 40.0f) &&
                      (!grey.valid  || grey.value  < 50.0f) &&
                      (!black.valid || black.value < 50.0f);
    if (attention) {
        s_state = W_ORANGE;
    } else if (fine) {
        s_state = W_GREEN;
    } else if (s_state == W_NONE) {
        s_state = W_GREEN;         /* first reading lands in the band */
    }

    if (s_state == W_ORANGE) {
        *r = 255; *g = 96; *b = 0;   /* the Climate screen's heating orange */
    } else {
        *r = 82; *g = 164; *b = 65;  /* AccentPrimary #52a441, brand green */
    }
    return true;
}

/*
 * Levelling's status word, shared by the Level screen and its menu summary
 * so the two can never disagree. `worst` is the larger absolute tilt.
 */
#define LEVEL_FULL_DEG 5.0f
#define LEVEL_OK_DEG   0.5f

static const char *level_word(float fb, float ss, float *worst)
{
    const float afb = fabsf(fb), ass = fabsf(ss);
    *worst = afb > ass ? afb : ass;
    if (*worst < LEVEL_OK_DEG) {
        return "Level";
    }
    if (ass >= afb) {
        return ss > 0 ? "Tilted right" : "Tilted left";
    }
    return fb > 0 ? "Tilted forward" : "Tilted back";
}

/* A height difference in the Locale's unit: +0.8" or -35 mm, "--" if the
 * gateway sends none (an older one publishes only the angles). */
static void fmt_height(char *out, size_t len, capstan_value_t mm)
{
    if (!mm.valid) {
        snprintf(out, len, "--");
        return;
    }
    capstan_display_cfg_t d;
    capstan_config_get_display(&d);
    const char *sign = mm.value > 0.0f ? "+" : "";
    if (d.level_mm) {
        snprintf(out, len, "%s%.0f mm", sign, (double)mm.value);
    } else {
        snprintf(out, len, "%s%.1f\"", sign, (double)(mm.value / 25.4f));
    }
}

/* A temperature held in Fahrenheit, in the Locale's unit: "68°F" / "20°C". */
static void fmt_temp(char *out, size_t len, float f)
{
    capstan_display_cfg_t d;
    capstan_config_get_display(&d);
    if (d.celsius) {
        snprintf(out, len, "%.0f\xC2\xB0" "C", (double)((f - 32.0f) * 5.0f / 9.0f));
    } else {
        snprintf(out, len, "%.0f\xC2\xB0" "F", (double)f);
    }
}

/*
 * The carousel's centre readout.
 *
 * ui_nav owns the glyphs, the name and the dots -- everything that depends
 * only on which item is selected. The summary is the one part that depends
 * on the MODEL, so it is written here, on the same 250 ms pass as every
 * other reading, rather than from the rotation handler which has no data
 * behind it.
 *
 * Every line is a glance, not a report: one number and its unit, or one
 * word. Anything that does not fit in the width the layout gives it belongs
 * on the app's own screen.
 *
 * The order of the switch follows MENU_ITEMS in GUI/tmp/screens_layout.py
 * and s_menu[] in ui_nav.c. It has a `default` so an item added there
 * without a line here reads as "--" rather than falling through to the
 * previous item's value.
 */
static void refresh_menu(void)
{
    if (!objects.menu_summary) {
        return;
    }

    const int sel = ui_nav_selection_of(CAPSTAN_SCREEN_MENU);
    char buf[32] = "";

    switch (sel) {
    case 0:     /* Climate -- local setpoint; see ui_climate.h. */
        ui_climate_summary(buf, sizeof(buf));
        break;

    case 1: {   /* Devices */
        if (capstan_model_module_alive(CAPSTAN_MOD_LIGHTS)) {
            snprintf(buf, sizeof(buf), "%d on",
                     capstan_model_lights_on_count());
        }
        break;
    }

    case 2: {   /* Energy -- the prototype's "82% · 14h 20m" */
        const capstan_value_t pct = capstan_model_battery_pct();
        const capstan_value_t mins = capstan_model_runtime_min();
        float net;
        char rt[16];
        if (pct.valid) {
            /* Same rule as the Battery page's time-remaining line. */
            if (battery_net_watts(&net) && net > 0.0f) {
                snprintf(buf, sizeof(buf), "%.0f%% \xC2\xB7 Charging",
                         (double)pct.value);
            } else if (mins.valid && fmt_runtime(rt, sizeof(rt), mins.value)) {
                snprintf(buf, sizeof(buf), "%.0f%% \xC2\xB7 %s",
                         (double)pct.value, rt);
            } else {
                snprintf(buf, sizeof(buf), "%.0f%%", (double)pct.value);
            }
        }
        break;
    }

    case 3: {   /* Water -- fresh is the tank people care about. */
        const capstan_value_t v = capstan_model_tank(CAPSTAN_TANK_FRESH);
        if (v.valid) {
            snprintf(buf, sizeof(buf), "Fresh %.0f%%", v.value);
        }
        break;
    }

    case 4: {   /* Air */
        const capstan_value_t t = capstan_model_temp_f();
        if (t.valid) {
            fmt_temp(buf, sizeof(buf), t.value);
        }
        break;
    }

    case 5: {   /* Level -- the larger of the two tilts is the actionable one. */
        const capstan_value_t fb = capstan_model_tilt_front_back();
        const capstan_value_t ss = capstan_model_tilt_side_to_side();
        if (fb.valid && ss.valid) {
            /* The prototype's "Tilted right 1.2°", with the height on the
             * dominant axis instead of the angle -- see refresh_level(). */
            float worst;
            const char *word = level_word(fb.value, ss.value, &worst);
            const capstan_value_t h = fabsf(ss.value) >= fabsf(fb.value)
                ? capstan_model_tilt_diff_left_right()
                : capstan_model_tilt_diff_front_back();
            if (worst < LEVEL_OK_DEG || !h.valid) {
                snprintf(buf, sizeof(buf), "%s", word);
            } else {
                char hb[16];
                capstan_value_t mag = h;
                mag.value = fabsf(h.value);
                fmt_height(hb, sizeof(hb), mag);
                snprintf(buf, sizeof(buf), "%s %s", word, hb);
            }
        }
        break;
    }

    case 6:     /* Settings */
        snprintf(buf, sizeof(buf), "%s",
                 capstan_mqtt_is_connected() ? "Connected" : "Offline");
        break;

    case 7:     /* Clock. Says what pressing does, because nothing else does. */
        snprintf(buf, sizeof(buf), "Back to the clock");
        break;

    default:
        break;
    }

    /* An empty summary reads "--", exactly as a missing reading does
     * anywhere else on this device -- see set_value(). */
    set_text(objects.menu_summary, buf[0] ? buf : "--");
}

static void refresh_water(void)
{
    static const struct {
        capstan_tank_t tank;
        lv_obj_t **bar;
        lv_obj_t **value;
    } rows[] = {
        { CAPSTAN_TANK_FRESH, &objects.water_fresh_bar,
          &objects.water_fresh_value },
        { CAPSTAN_TANK_GREY,  &objects.water_grey_bar,
          &objects.water_grey_value },
        { CAPSTAN_TANK_BLACK, &objects.water_black_bar,
          &objects.water_black_value },
    };

    for (size_t i = 0; i < sizeof(rows) / sizeof(rows[0]); i++) {
        const capstan_value_t v = capstan_model_tank(rows[i].tank);
        if (rows[i].value && *rows[i].value) {
            if (v.valid) {
                char buf[16];
                snprintf(buf, sizeof(buf), "%.0f%%", v.value);
                ui_lv_set_text(*rows[i].value, buf);
            } else {
                ui_lv_set_text(*rows[i].value, "--");
            }
        }
        if (rows[i].bar && *rows[i].bar) {
            /* An unknown tank reads EMPTY, not "last known". A tank bar
             * is glanced at, not read, so leaving the old height up
             * would be actively misleading. */
            lv_bar_set_value(*rows[i].bar,
                             v.valid ? (int32_t)v.value : 0, LV_ANIM_OFF);
        }
    }
}

/*
 * Air: one ring, one number, one word, three captions.
 *
 * WHAT THIS DOES NOT DO
 *
 * It sets no colours and no geometry. The four looks the ring and the status
 * word take -- good, moderate, unhealthy, no data -- are all AUTHORED in the
 * .eez-project as LVGL states, so the screen a designer opens in EEZ Studio
 * is the screen the panel draws. All that happens here is add_state /
 * clear_state, the same device the carousel's page dots use. The mapping
 * from severity to state, and why four built-in states are borrowed rather
 * than LV_STATE_USER_1, is documented on the ArcThin style in
 * GUI/tmp/gen_eez_project.py. Do not "fix" anything on this screen with
 * lv_obj_set_style_*.
 */

/* The ring's span, matching rmin/rmax on air_arc in screens_layout.py.
 * 400 ppm is outdoor air; 2000 ppm fills the ring. */
#define AIR_PPM_MIN  400
#define AIR_PPM_MAX  2000

/* The design's 300 ms ease-out. eCO2 arrives every 2 s and this timer runs
 * four times a second, so without it the ring steps in visible jumps. */
#define AIR_ARC_ANIM_MS 300

static void air_arc_anim_cb(void *obj, int32_t v)
{
    lv_arc_set_value((lv_obj_t *)obj, v);
}

/*
 * Severity -> the state that carries its look.
 *
 * CHECKED, DISABLED and PRESSED are borrowed as plain style selectors. That
 * is only safe because neither widget is interactive -- air_arc has
 * CLICKABLE cleared and a label never had it -- so nothing but this function
 * can ever put them into one of these states.
 */
static void air_set_level(lv_obj_t *obj, capstan_air_level_t level)
{
    lv_state_t st = 0;                   /* GOOD: DEFAULT */
    switch (level) {
    case CAPSTAN_AIR_MODERATE:  st = LV_STATE_CHECKED;  break;
    case CAPSTAN_AIR_UNHEALTHY: st = LV_STATE_DISABLED; break;
    case CAPSTAN_AIR_UNKNOWN:   st = LV_STATE_PRESSED;  break;
    case CAPSTAN_AIR_GOOD:      break;
    }
    ui_lv_set_state_in(obj, LV_STATE_CHECKED | LV_STATE_DISABLED |
                            LV_STATE_PRESSED, st);
}

static void refresh_air(void)
{
    const capstan_value_t eco2 = capstan_model_eco2();
    const capstan_air_level_t level = capstan_model_air_level();

    set_value(objects.air_value, eco2, "%.0f");

    static const char *const WORD[] = {
        [CAPSTAN_AIR_UNKNOWN]   = "--",
        [CAPSTAN_AIR_GOOD]      = "Good",
        [CAPSTAN_AIR_MODERATE]  = "Moderate",
        [CAPSTAN_AIR_UNHEALTHY] = "Unhealthy",
    };
    set_text(objects.air_status, WORD[level]);
    air_set_level(objects.air_status, level);

    /*
     * The ring reads eCO2, but its COLOUR reads the verdict, so the two can
     * legitimately disagree: Borealis can flag a VOC alarm while eCO2 sits
     * at 500 ppm, and the ring then draws a short red arc. That is the
     * intended behaviour -- the ring is the number, the colour is the
     * warning -- and it is why the status word is there to name which.
     */
    if (objects.air_arc) {
        int32_t target = AIR_PPM_MIN;
        if (eco2.valid) {
            target = (int32_t)eco2.value;
            if (target < AIR_PPM_MIN) { target = AIR_PPM_MIN; }
            if (target > AIR_PPM_MAX) { target = AIR_PPM_MAX; }
        }
        /* An unknown reading parks the ring at its start, where the
         * indicator has no length. The PRESSED look also takes its opacity
         * to zero; both, because a zero-length rounded arc still paints a
         * dot at the 7:30 position. */
        air_set_level(objects.air_arc, level);

        if (lv_arc_get_value(objects.air_arc) != target) {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, objects.air_arc);
            lv_anim_set_exec_cb(&a, air_arc_anim_cb);
            lv_anim_set_values(&a, lv_arc_get_value(objects.air_arc), target);
            lv_anim_set_duration(&a, AIR_ARC_ANIM_MS);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
            /* Restarting an animation on the same var+cb replaces it, so a
             * reading that lands mid-sweep re-aims rather than queueing. */
            lv_anim_start(&a);
        }
    }

    /*
     * The three captions. VOC takes the column the prototype gives PM2.5:
     * there is no particulate sensor on this bus. Temperature is here rather
     * than on its own screen because local/airquality/temphumid is the only
     * ambient temperature anywhere on the rig.
     */
    set_value(objects.air_voc, capstan_model_tvoc(), "%.0f ppb");
    set_value(objects.air_humidity, capstan_model_humidity(), "%.0f%%");
    {
        const capstan_value_t t = capstan_model_temp_f();
        char tb[12];
        if (t.valid) {
            fmt_temp(tb, sizeof(tb), t.value);
        }
        set_text(objects.air_temp, t.valid ? tb : "--");
    }
}

/*
 * Levelling: the bubble, the status word and the Side/Front line.
 *
 * DIRECTION follows Headwaters' own level indicator
 * (containers/frontend/public/js/components/level-indicator.js): a positive
 * side_to_side moves the bubble right, and front_back moves it along the
 * other axis -- positive towards the top of the well (the front).
 *
 * SCALE: LEVEL_FULL_DEG of tilt reaches the rim. 5 deg is where Headwaters
 * turns the reading red, so "bubble at the rim" and "red" mean the same thing,
 * and at that scale LEVEL_OK_DEG (0.5 deg) is roughly the centre ring: a
 * bubble inside the ring reads "Level". The offset is clamped to a circle,
 * not a square, so a diagonal tilt stops at the rim too.
 *
 * The bubble is authored at dead centre in the EEZ project and moved here by
 * TRANSLATION ONLY -- never lv_obj_set_pos -- so the authored position stays
 * the canvas-correct rest state (level), and this is purely the live value,
 * like a clock hand. It is the one runtime geometry write on this screen.
 */
static void refresh_level(void)
{
    const capstan_value_t fb = capstan_model_tilt_front_back();
    const capstan_value_t ss = capstan_model_tilt_side_to_side();
    lv_obj_t *const bubble = objects.level_bubble;
    lv_obj_t *const well   = objects.level_well;

    const lv_state_t status_mask = LV_STATE_CHECKED | LV_STATE_DISABLED |
                                   LV_STATE_PRESSED;

    if (!fb.valid || !ss.valid) {
        ui_lv_set_state_in(objects.level_status, status_mask,
                           LV_STATE_PRESSED);                   /* muted */
        set_text(objects.level_status, "--");
        set_text(objects.level_detail, "No level data");
        ui_lv_set_translate(bubble, 0, 0);
        return;
    }

    /* Room the bubble can travel: from centre to where its edge meets the
     * well's inner edge. Read from the laid-out widgets so it is right on
     * all three panels without a per-board constant. */
    if (bubble && well) {
        /* LVGL lays out a screen only when it is shown, so on a hidden page
         * these sizes read as zero until the first refresh after it loads --
         * which made travel negative and pinned the bubble near the centre.
         * update_layout computes this subtree's sizes now; it writes no
         * geometry of its own. */
        lv_obj_update_layout(well);
        int32_t travel = (lv_obj_get_content_width(well) -
                          lv_obj_get_width(bubble)) / 2;
        if (travel < 0) {
            travel = 0;
        }
        float dx = ss.value / LEVEL_FULL_DEG;
        float dy = -fb.value / LEVEL_FULL_DEG;     /* front = up */
        const float mag = sqrtf(dx * dx + dy * dy);
        if (mag > 1.0f) {
            dx /= mag;
            dy /= mag;
        }
        ui_lv_set_translate(bubble, (int32_t)lroundf(dx * travel),
                            (int32_t)lroundf(dy * travel));
        /* Trace, at most once a second and only when something moved by a
         * tenth of a degree: what arrived and where it put the bubble. */
        static int64_t s_last_us;
        static float   s_last_fb = 1e9f, s_last_ss = 1e9f;
        const int64_t now = esp_timer_get_time();
        if (now - s_last_us > 1000000 &&
            (fabsf(fb.value - s_last_fb) > 0.1f ||
             fabsf(ss.value - s_last_ss) > 0.1f)) {
            s_last_us = now; s_last_fb = fb.value; s_last_ss = ss.value;
            ESP_LOGI(TAG, "level: front_back=%.2f side_to_side=%.2f -> "
                          "bubble dx=%ld dy=%ld (travel %ld px)",
                     (double)fb.value, (double)ss.value,
                     (long)lroundf(dx * travel), (long)lroundf(dy * travel),
                     (long)travel);
        }
    }

    /* Status word: the dominant axis, as the prototype's "Tilted right". */
    float worst;
    const char *word = level_word(fb.value, ss.value, &worst);
    ui_lv_set_state_in(objects.level_status, status_mask,
                       worst < LEVEL_OK_DEG   ? 0                  /* green */
                       : worst > LEVEL_FULL_DEG ? LV_STATE_DISABLED  /* red */
                                                : LV_STATE_CHECKED); /* amber */
    set_text(objects.level_status, word);

    /*
     * The detail is how much higher or lower each side is -- Plateau's
     * height differences, in inches or mm per Settings > Locale, signed as
     * Headwaters and Milepost show them. Never degrees: nobody levelling a
     * trailer can turn 2 degrees into "raise the driver's side 2 inches"
     * without knowing its track and wheelbase, which Plateau does.
     */
    const capstan_value_t dlr = capstan_model_tilt_diff_left_right();
    const capstan_value_t dfb = capstan_model_tilt_diff_front_back();
    char side[16], front[16], detail[48];
    fmt_height(side, sizeof(side), dlr);
    fmt_height(front, sizeof(front), dfb);
    snprintf(detail, sizeof(detail), "Side %s \xC2\xB7 Front %s", side, front);
    set_text(objects.level_detail, detail);
}

/*
 * Connection status, on the Settings rows.
 *
 * Until this existed there was no way to tell from the panel whether
 * the device was on the network: the Wi-Fi screen's title only says so
 * while that screen is open, and a failed rejoin after a reboot looked
 * exactly like a successful one. On a wall-mounted display with no
 * console, "am I connected?" has to be answerable by looking at it.
 *
 * The SSID is shown rather than the word "Connected" -- in a vehicle
 * that may see a home network, a phone hotspot and a campground AP,
 * WHICH network is the useful half of the answer.
 */
static void refresh_settings(void)
{
    /* Wi-Fi and MQTT status change without input; the carousel is
     * change-only, so repainting it on every refresh costs nothing. */
    ui_settings_refresh();
}

void ui_data_refresh(void)
{
    /*
     * Everything is refreshed, not just the visible screen.
     *
     * EEZ builds all screens up front, so every widget exists and
     * writing to a hidden one is cheap -- LVGL invalidates nothing that
     * is not on screen, and the setters in ui_lv.h make an unchanged
     * value a no-op on the one that is. The alternative, refreshing only
     * the current screen, means every screen needs a "fill me in" path on
     * entry as well, and the two drift.
     */
    refresh_settings();
    ui_setup_tick();         /* portal progress while provisioning */
    refresh_menu();
    ui_devices_refresh();    /* controls from Headwaters -> the carousel */
    refresh_energy();
    refresh_water();
    refresh_air();
    refresh_level();
    ui_climate_refresh();
    ui_alerts_tick();        /* last: may switch to the alert overlay */
}

static void refresh_timer_cb(lv_timer_t *t)
{
    (void)t;
    ui_data_refresh();
}

/*
 * NOTHING RUNS ON THE SYSTEM EVENT TASK.
 *
 * This used to subscribe to capstan_wifi's state callback and call
 * capstan_mqtt_connect() from it. That callback runs on `sys_evt`,
 * whose stack is CONFIG_ESP_SYSTEM_EVENT_TASK_STACK_SIZE -- 2304 bytes
 * -- and building an MQTT client with a TLS context does not fit in it.
 * All three panels rebooted in a loop:
 *
 *   ***ERROR*** A stack overflow in task sys_evt has been detected.
 *
 * It was invisible on the board that never associated, because the
 * callback never fired there.
 *
 * Raising that stack would be the wrong fix twice over: it is a global
 * shared by every event handler in the system, and the real problem is
 * doing slow, allocating work on the task that delivers events at all.
 *
 * So state is POLLED instead, from two places that own generous stacks:
 * the service task connects the broker, and the LVGL refresh timer
 * updates the widgets. Polling a connection state four times a second
 * costs nothing and removes a whole class of context bug.
 */
static bool    s_mqtt_wanted;
static int64_t s_last_attempt_us;

/*
 * How often to retry the broker.
 *
 * The service task runs at 50 ms, and calling connect() on every pass
 * produced twenty "broker not configured" errors a second -- a log so
 * noisy it hid everything else, for a condition that is not an error at
 * all on a device nobody has configured yet.
 */
#define MQTT_RETRY_US 5000000   /* 5 s */

void ui_data_service_tick(void)
{
    /* Runs on the service task -- 6 KB, and nothing latency-sensitive
     * is waiting on it. */
    if (!capstan_wifi_is_connected()) {
        s_mqtt_wanted = false;
        return;
    }
    if (capstan_mqtt_is_connected()) {
        return;
    }

    /* Not configured is a normal state, not a failure: the panel ships
     * blank and the user has not been to the MQTT screen yet. Saying so
     * once is useful; saying it twenty times a second is not. */
    capstan_mqtt_cfg_t m;
    capstan_config_get_mqtt(&m);
    if (!m.configured || !m.host[0]) {
        if (!s_mqtt_wanted) {
            ESP_LOGI(TAG, "online, but no broker configured yet");
            s_mqtt_wanted = true;
        }
        return;
    }

    const int64_t now = esp_timer_get_time();
    if (s_last_attempt_us && (now - s_last_attempt_us) < MQTT_RETRY_US) {
        return;
    }
    s_last_attempt_us = now;

    ESP_LOGI(TAG, "connecting to broker %s:%u", m.host, (unsigned)m.port);

    /* connect() is idempotent while connected or just started, and
     * rebuilds a client that has been down -- which is what is needed
     * after a reassociation leaves a dead socket behind. */
    capstan_mqtt_connect();
}

void ui_data_init(void)
{
    lv_timer_create(refresh_timer_cb, REFRESH_MS, NULL);
}

#else

void ui_data_init(void) { }
void ui_data_refresh(void) { }
void ui_data_service_tick(void) { }
bool ui_data_energy_led(uint8_t *r, uint8_t *g, uint8_t *b)
{
    (void)r; (void)g; (void)b;
    return false;
}

#endif
