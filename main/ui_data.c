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

#include <stdio.h>

#include "esp_log.h"
#include "esp_timer.h"

#include "capstan_config.h"
#include "capstan_model.h"
#include "capstan_mqtt.h"
#include "capstan_wifi.h"
#include "ui_data.h"
#include "ui_lights.h"
#include "ui_nav.h"
#include "ui_setup.h"

#ifndef CAPSTAN_HAVE_UI
#  error "CAPSTAN_HAVE_UI is not defined -- main/CMakeLists.txt must set it"
#endif

#if CAPSTAN_HAVE_UI

#include "screens.h"
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
        lv_label_set_text(label, "--");
        return;
    }
    char buf[24];
    snprintf(buf, sizeof(buf), fmt, v.value);
    lv_label_set_text(label, buf);
}

static void set_text(lv_obj_t *label, const char *text)
{
    if (label) {
        lv_label_set_text(label, text ? text : "--");
    }
}

/*
 * Energy: one page at a time, chosen by the ring.
 *
 * The page list must match ENERGY_PAGES in GUI/tmp/screens_layout.py,
 * which is what the dots were authored from, and ENERGY_PAGE_COUNT in
 * ui_nav.c, which is how far the ring will turn.
 */
static void refresh_energy(void)
{
    const int page = ui_nav_selection_of(CAPSTAN_SCREEN_ENERGY);

    const char *title = "Battery";
    const char *unit  = "V";
    capstan_value_t v = { 0.0f, false };
    const char *fmt = "%.1f";
    char sub[32] = "";

    switch (page) {
    case 0:
        v = capstan_model_battery_volts();
        title = "Battery"; unit = "V"; fmt = "%.1f";
        {
            const capstan_value_t pct = capstan_model_battery_pct();
            if (pct.valid) {
                snprintf(sub, sizeof(sub), "%.0f%%", pct.value);
            }
        }
        break;

    case 1:
        v = capstan_model_battery_pct();
        title = "Charge"; unit = "%"; fmt = "%.0f";
        /* The charger's own view of what it is doing -- bulk, float,
         * absorption. "--" when the MPPT has not reported. */
        snprintf(sub, sizeof(sub), "%s", capstan_model_charge_type());
        break;

    case 2:
        v = capstan_model_solar_watts();
        title = "Solar"; unit = "W"; fmt = "%.0f";
        break;

    case 3:
        v = capstan_model_load_watts();
        title = "Load"; unit = "W"; fmt = "%.0f";
        break;

    case 4: {
        /* Time-to-go arrives in minutes; hours are what a person wants
         * once it is past an hour or two. */
        const capstan_value_t mins = capstan_model_runtime_min();
        title = "Runtime";
        if (mins.valid && mins.value >= 120.0f) {
            v.value = mins.value / 60.0f; v.valid = true;
            unit = "hours"; fmt = "%.1f";
        } else {
            v = mins;
            unit = "min"; fmt = "%.0f";
        }
        break;
    }

    default:
        break;
    }

    set_text(objects.energy_title, title);
    set_value(objects.energy_value, v, fmt);
    set_text(objects.energy_unit, unit);
    set_text(objects.energy_sub, sub[0] ? sub : "--");
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
    case 0:     /* Climate -- no thermostat topic exists yet. See docs/mqtt.md. */
        break;

    case 1: {   /* Lights */
        if (capstan_model_module_alive(CAPSTAN_MOD_LIGHTS)) {
            snprintf(buf, sizeof(buf), "%d on",
                     capstan_model_lights_on_count());
        }
        break;
    }

    case 2:     /* Heater -- no topic yet. */
        break;

    case 3: {   /* Energy -- battery volts, the one number worth a glance. */
        const capstan_value_t v = capstan_model_battery_volts();
        if (v.valid) {
            snprintf(buf, sizeof(buf), "%.1f V", v.value);
        }
        break;
    }

    case 4: {   /* Water -- fresh is the tank people care about. */
        const capstan_value_t v = capstan_model_tank(CAPSTAN_TANK_FRESH);
        if (v.valid) {
            snprintf(buf, sizeof(buf), "Fresh %.0f%%", v.value);
        }
        break;
    }

    case 5: {   /* Air */
        const capstan_value_t t = capstan_model_temp_f();
        if (t.valid) {
            snprintf(buf, sizeof(buf), "%.0f F", t.value);
        }
        break;
    }

    case 6: {   /* Level -- the larger of the two tilts is the actionable one. */
        const capstan_value_t fb = capstan_model_tilt_front_back();
        const capstan_value_t ss = capstan_model_tilt_side_to_side();
        if (fb.valid && ss.valid) {
            const float afb = fb.value < 0 ? -fb.value : fb.value;
            const float ass = ss.value < 0 ? -ss.value : ss.value;
            snprintf(buf, sizeof(buf), "%.1f deg", afb > ass ? afb : ass);
        }
        break;
    }

    case 7:     /* Doors -- needs the Picket channel map to name a door. */
        break;

    case 8:     /* Settings */
        snprintf(buf, sizeof(buf), "%s",
                 capstan_mqtt_is_connected() ? "Connected" : "Offline");
        break;

    case 9:     /* Clock. Says what pressing does, because nothing else does. */
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
                lv_label_set_text(*rows[i].value, buf);
            } else {
                lv_label_set_text(*rows[i].value, "--");
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
    if (!obj) {
        return;
    }
    lv_obj_remove_state(obj, LV_STATE_CHECKED | LV_STATE_DISABLED |
                            LV_STATE_PRESSED);
    switch (level) {
    case CAPSTAN_AIR_MODERATE:  lv_obj_add_state(obj, LV_STATE_CHECKED);  break;
    case CAPSTAN_AIR_UNHEALTHY: lv_obj_add_state(obj, LV_STATE_DISABLED); break;
    case CAPSTAN_AIR_UNKNOWN:   lv_obj_add_state(obj, LV_STATE_PRESSED);  break;
    case CAPSTAN_AIR_GOOD:      break;   /* DEFAULT */
    }
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
    set_value(objects.air_temp, capstan_model_temp_f(), "%.0f F");
}

static void refresh_level(void)
{
    set_value(objects.level_pitch, capstan_model_tilt_front_back(), "%.1f");
    set_value(objects.level_roll, capstan_model_tilt_side_to_side(), "%.1f");
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
    const char *wifi;
    switch (capstan_wifi_state()) {
    case CAPSTAN_WIFI_CONNECTED: {
        capstan_wifi_cfg_t c;
        capstan_config_get_wifi(&c);
        wifi = c.ssid[0] ? c.ssid : "Connected";
        break;
    }
    case CAPSTAN_WIFI_CONNECTING: wifi = "Connecting..."; break;
    case CAPSTAN_WIFI_SCANNING:   wifi = "Scanning...";   break;
    case CAPSTAN_WIFI_FAILED:
        /* The reason, not just "failed": a wrong passphrase and an AP
         * that is switched off need different things from the user, and
         * the retry backoff means this state persists long enough to
         * read. */
        wifi = capstan_wifi_last_error();
        break;
    default: {
        capstan_wifi_cfg_t c;
        capstan_config_get_wifi(&c);
        wifi = c.configured ? "Offline" : "Not set";
        break;
    }
    }
    set_text(objects.settings_item0_value, wifi);

    const char *mq;
    if (capstan_mqtt_is_connected()) {
        mq = "Connected";
    } else {
        capstan_mqtt_cfg_t m;
        capstan_config_get_mqtt(&m);
        mq = m.configured ? "Offline" : "Not set";
    }
    set_text(objects.settings_item1_value, mq);
}

void ui_data_refresh(void)
{
    /*
     * Everything is refreshed, not just the visible screen.
     *
     * EEZ builds all screens up front, so every widget exists and
     * writing to a hidden one is cheap -- LVGL invalidates nothing that
     * is not on screen. The alternative, refreshing only the current
     * screen, means every screen needs a "fill me in" path on entry as
     * well, and the two drift.
     */
    refresh_settings();
    ui_setup_tick();         /* portal progress while provisioning */
    refresh_menu();
    ui_lights_refresh();     /* controls from Headwaters -> rows */
    refresh_energy();
    refresh_water();
    refresh_air();
    refresh_level();
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

#endif
