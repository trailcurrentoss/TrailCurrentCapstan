/*
 * A rig for the EEZ Studio simulator to display: a travel trailer parked at a
 * campsite, lived in, on solar.
 *
 * Every value goes through the same capstan_model_set_*() calls that
 * capstan_mqtt.c's apply() makes for a real broker message, at the cadence
 * each module really publishes at. The UI cannot tell the difference, so the
 * code under test is the code that ships -- formatting, expiry, thresholds,
 * the LED ring and the alert overlay included.
 *
 * The scenario runs on the local wall clock rather than on a loop, so the
 * dial reads like the rig would at this hour: solar follows the sun, the
 * cabin cools overnight and CO2 builds while people sleep. On top of that a
 * few household events repeat on their own periods -- the fridge compressor,
 * the water pump, someone cooking, the door -- chosen so something on every
 * screen changes within a few minutes and no two values move in lockstep.
 *
 * Numbers are from the payload examples in docs/mqtt.md and the hardware a
 * rig like this carries:
 *
 *   battery    200 Ah LiFePO4, 12.8 V nominal -> 2560 Wh, 10 % floor
 *   solar      400 W of panels; ~340 W is a good clear noon
 *   loads      ~9 W always on (Headwaters, router, this panel), a 12 V
 *              compressor fridge, LED lighting, a water pump, a furnace fan
 *   tanks      percent, integers, as Reservoir sends them
 *   level      parked a little nose-down and to the right
 *
 * Nothing alarms by default. The mode is Camping, so the configured door
 * alarms are disarmed while the door opens and closes, and Borealis's CO and
 * LPG verdicts stay false. CO2 does cross Borealis's warning threshold when
 * dinner is cooked with everyone inside, which is what a small cabin with a
 * gas stove really does.
 *
 * Simulator only -- see docs/simulator.md.
 */
#ifdef EEZ_LVGL_SIMULATOR

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <emscripten.h>

#include "lvgl.h"

#include "capstan_config.h"
#include "capstan_model.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "sim.h"

static const char *TAG = "sim_feed";

#define TWO_PI 6.28318530718

/* ---- the rig --------------------------------------------------------- */

#define BATT_WH          2560.0     /* 200 Ah x 12.8 V */
#define BATT_FLOOR_PCT   10.0       /* where time-to-go counts down to */
#define SOLAR_CLEAR_W    340.0      /* a clear noon on a 400 W array */
#define BASE_LOAD_W      9.0
#define FRIDGE_W         46.0
#define PUMP_W           55.0
#define FURNACE_FAN_W    36.0

#define WHEELBASE_MM     5200.0     /* front_back_diff_mm spans this */
#define TRACK_MM         2350.0     /* left_right_diff_mm spans this */

/* Light ids: PDM channels are 1..N, Switchback relays from 100 up
 * (CAPSTAN_SWITCHBACK_ID_BASE). The watts are what each adds to the load. */
typedef struct {
    uint16_t    id;
    const char *name;
    const char *icon;
    float       watts;
} sim_control_t;

enum { L_CEILING, L_KITCHEN, L_PORCH, L_AWNING, L_PUMP, L_FURNACE, L_INVERTER,
       L_COUNT };

static const sim_control_t CONTROLS[L_COUNT] = {
    [L_CEILING]  = { 1,   "Ceiling Lights", "ceiling-light",  12.0f },
    [L_KITCHEN]  = { 2,   "Kitchen",        "strip-light",     8.0f },
    [L_PORCH]    = { 3,   "Porch",          "exterior-light",  6.0f },
    [L_AWNING]   = { 4,   "Awning",         "awning",         18.0f },
    [L_PUMP]     = { 101, "Water Pump",     "water-pump",      0.0f },  /* draws only when running */
    [L_FURNACE]  = { 102, "Furnace",        "heater",          0.0f },  /* fan draw is modelled */
    [L_INVERTER] = { 103, "Inverter",       "power-outlet",    6.0f },  /* idle draw */
};

/* ---- simulation state ------------------------------------------------ */

static double s_soc      = 78.0;    /* % */
static double s_fresh    = 64.0;    /* % -- integers go out, fractions accumulate */
static double s_grey     = 37.0;
static double s_black    = 21.0;
static double s_cabin_f  = 0.0;     /* set from the time of day at start */
static double s_furnace_heat;       /* degrees F the furnace has added */
static double s_level_wobble;       /* degrees, decays after a footstep */
static double s_t0;                 /* scenario seconds at start */
static int64_t s_last_us;
static uint32_t s_rng = 0x9E3779B9u;

/* ---- helpers --------------------------------------------------------- */

/* xorshift32 -> [-1, 1). Deterministic, so a run is repeatable. */
static double noise(void)
{
    s_rng ^= s_rng << 13;
    s_rng ^= s_rng >> 17;
    s_rng ^= s_rng << 5;
    return (double)s_rng / 2147483648.0 - 1.0;
}

static double clamp(double v, double lo, double hi)
{
    return v < lo ? lo : v > hi ? hi : v;
}

/* Seconds since the scenario started. Drives the household events. */
static double run_s(void) { return esp_timer_get_time() / 1e6 - s_t0; }

/* True for `on_s` seconds out of every `period_s`, offset by `phase_s`. */
static bool duty(double period_s, double on_s, double phase_s)
{
    return fmod(run_s() + phase_s, period_s) < on_s;
}

/* Local hour of day as a fraction, 0..24. */
static double hour_of_day(void)
{
    const time_t now = time(NULL);
    struct tm lt;
    localtime_r(&now, &lt);
    return lt.tm_hour + lt.tm_min / 60.0 + lt.tm_sec / 3600.0;
}

static bool light_on(int which)
{
    return capstan_model_light_on(CONTROLS[which].id);
}

/* ---- household events ------------------------------------------------ */

/* Compressor fridge: 7 minutes on, 11 off. */
static bool fridge_running(void)  { return duty(18 * 60, 7 * 60, 0); }

/* Someone at a tap: 25 s every 9 minutes, only if the pump is switched on. */
static bool water_running(void)
{
    return light_on(L_PUMP) && duty(9 * 60, 25, 240);
}

/* A meal on the stove: 6 minutes every 26. */
static bool cooking(void)          { return duty(26 * 60, 6 * 60, 900); }

/* The entry door opens for 20 s every 11 minutes. */
static bool door_open(void)        { return duty(11 * 60, 20, 120); }

/* ---- models ---------------------------------------------------------- */

/* Clear-sky solar for the hour, with sunrise ~06:30 and sunset ~19:30. */
static double clear_sky_w(double h)
{
    if (h <= 6.5 || h >= 19.5) {
        return 0.0;
    }
    const double s = sin(M_PI * (h - 6.5) / 13.0);
    return SOLAR_CLEAR_W * pow(s, 1.3);
}

/* Cloud shadow: two incommensurate periods so it never repeats neatly, and
 * never darker than a thick cloud (35 %). */
static double cloud_factor(void)
{
    const double t = run_s();
    const double c = sin(TWO_PI * t / 97.0) + 0.5 * sin(TWO_PI * t / 41.0);
    return clamp(1.0 - 0.3 * fmax(0.0, c), 0.35, 1.0);
}

/* LiFePO4 resting voltage against state of charge -- flat through the
 * middle, steep at both ends. Linear between the points. */
static double lfp_ocv(double soc)
{
    static const double pts[][2] = {
        {   0, 11.8 }, {  10, 12.9 }, {  20, 13.05 }, {  40, 13.15 },
        {  70, 13.25 }, {  90, 13.35 }, {  99, 13.45 }, { 100, 13.6 },
    };
    const int n = sizeof(pts) / sizeof(*pts);
    if (soc <= pts[0][0]) {
        return pts[0][1];
    }
    for (int i = 1; i < n; i++) {
        if (soc <= pts[i][0]) {
            const double f = (soc - pts[i - 1][0]) / (pts[i][0] - pts[i - 1][0]);
            return pts[i - 1][1] + f * (pts[i][1] - pts[i - 1][1]);
        }
    }
    return pts[n - 1][1];
}

/* Cabin temperature: ~61 F before dawn to ~73 F mid-afternoon, plus the
 * furnace, plus people moving about. */
static double cabin_target_f(double h)
{
    return 67.0 + 6.0 * sin(TWO_PI * (h - 9.0) / 24.0);
}

/* ---- publishing, one function per topic ------------------------------ */

typedef struct {
    double solar_w, load_w, batt_w;
} energy_t;

static energy_t s_energy;

static void step_energy(double dt, double h)
{
    double load = BASE_LOAD_W;
    if (fridge_running())    load += FRIDGE_W;
    if (water_running())     load += PUMP_W;
    if (light_on(L_FURNACE)) load += FURNACE_FAN_W;
    for (int i = 0; i < L_COUNT; i++) {
        if (light_on(i)) {
            load += CONTROLS[i].watts;
        }
    }
    load += 0.8 * noise();

    double solar = clear_sky_w(h) * cloud_factor();
    if (solar > 0.0) {
        solar = fmax(0.0, solar + 3.0 * noise());
    }

    /* A full battery takes only what the loads use: the MPPT backs off. */
    if (s_soc >= 99.5 && solar > load) {
        solar = load + 4.0;
    }

    const double batt = solar - load;
    s_soc = clamp(s_soc + batt * dt / 3600.0 / BATT_WH * 100.0, 0.0, 100.0);

    s_energy = (energy_t){ solar, load, batt };
}

static void publish_energy(void)
{
    const double batt = s_energy.batt_w;
    const double amps = batt / 13.2;

    double volts = lfp_ocv(s_soc) + 0.012 * amps;
    if (batt > 0.0) {
        volts = fmin(volts, s_soc >= 99.5 ? 13.5 : 14.2);  /* float, absorb */
    }

    const char *charge = "off";
    if (s_energy.solar_w >= 5.0) {
        charge = s_soc >= 99.5 ? "float" : s_soc >= 90.0 ? "absorption" : "bulk";
    }

    /* The shapes can-bridge.js sends: battery_watts signed, consumption_watts
     * the draw while discharging and 0 otherwise, time-to-go only while
     * discharging (the SmartShunt reports 0xFFFF, which is omitted, while
     * charging -- and the model keeps the last figure, as on the rig). */
    capstan_model_set_battery_volts(round(volts * 100.0) / 100.0);
    capstan_model_set_battery_pct(round(s_soc * 10.0) / 10.0);
    capstan_model_set_battery_watts(round(batt));
    capstan_model_set_load_watts(batt < 0.0 ? round(-batt) : 0.0);
    capstan_model_set_solar_watts(round(s_energy.solar_w));
    capstan_model_set_charge_type(charge);
    if (batt < -1.0) {
        const double wh_left = (s_soc - BATT_FLOOR_PCT) / 100.0 * BATT_WH;
        capstan_model_set_runtime_min(round(fmax(0.0, wh_left) / -batt * 60.0));
    }
}

static void step_air(double dt, double h)
{
    /* The furnace heats while it runs and the heat bleeds away after. */
    if (light_on(L_FURNACE)) {
        s_furnace_heat = fmin(8.0, s_furnace_heat + 0.02 * dt);
    } else {
        s_furnace_heat = fmax(0.0, s_furnace_heat - 0.004 * dt);
    }
    const double target = cabin_target_f(h) + s_furnace_heat +
                          (cooking() ? 1.5 : 0.0);
    /* First-order lag: a cabin takes minutes to follow, not a tick. */
    s_cabin_f += (target - s_cabin_f) * fmin(1.0, dt / 180.0);
}

static void publish_air(double h)
{
    const double t = run_s();
    const double f = s_cabin_f + 0.25 * sin(TWO_PI * t / 420.0) + 0.05 * noise();
    const double c = (f - 32.0) * 5.0 / 9.0;

    /* Warmer air holds more, so relative humidity runs opposite to
     * temperature; cooking and the tap both add some. */
    double rh = 46.0 - 1.1 * (f - 67.0) + 1.5 * sin(TWO_PI * t / 610.0);
    if (cooking())       rh += 7.0;
    if (water_running()) rh += 2.0;
    rh = clamp(rh + 0.3 * noise(), 20.0, 85.0);

    /* CO2 climbs overnight with the windows shut and two people asleep,
     * sits higher in the evening with everyone inside, and builds while a
     * gas burner runs -- over the 6 minutes, not in one step. Dinner is
     * the meal that tips it past Borealis's 1500 ppm warning. */
    const double night = (h >= 22.0 || h < 7.0) ? 1.0
                       : (h >= 7.0 && h < 9.0) ? (9.0 - h) / 2.0 : 0.0;
    const double evening = (h >= 18.0 && h < 22.0) ? 1.0 : 0.0;
    double eco2 = 520.0 + 450.0 * night + 250.0 * evening +
                  60.0 * sin(TWO_PI * t / 530.0);
    double tvoc = 90.0 + 25.0 * sin(TWO_PI * t / 350.0);
    if (cooking()) {
        const double ramp = fmin(1.0, fmod(run_s() + 900.0, 26 * 60) / 240.0);
        eco2 += 800.0 * ramp;
        tvoc += 480.0 * ramp;
    }
    eco2 += 8.0 * noise();
    tvoc += 4.0 * noise();

    /* local/airquality/temphumid -- whole degrees, as Borealis sends them. */
    capstan_model_set_temp_f(round(f));
    capstan_model_set_temp_c(round(c));
    capstan_model_set_humidity(round(rh * 100.0) / 100.0);

    /* local/airquality/status */
    capstan_model_set_tvoc(round(tvoc));
    capstan_model_set_eco2(round(eco2));

    /* local/airquality/safety. The verdicts are Borealis's to make; the
     * simulator is standing in for Borealis, so it makes them from its own
     * numbers with compute_alarm_flags() from TrailCurrentBorealis
     * main/main.c: CO2 warn at 1500 ppm and alarm at 2500, exclusive. CO
     * (70/200 ppm) and LPG never get near theirs here. The VOC alarm is on
     * the SGP40 index (>= 400, a severe pollution event), which cooking in
     * this scenario does not reach. */
    const double co = fmax(0.0, 1.0 + noise());
    capstan_model_set_co(round(co));
    capstan_model_set_safety_flags(co >= 200.0, co >= 70.0 && co < 200.0,
                                   false, false,                 /* LPG */
                                   eco2 >= 2500.0,
                                   eco2 >= 1500.0 && eco2 < 2500.0,
                                   false);                       /* VOC */
}

static void step_water(double dt)
{
    /* ~1 % of fresh per 25 s at the tap goes down the sink to grey. */
    if (water_running()) {
        const double d = 0.04 * dt;
        s_fresh = fmax(0.0, s_fresh - d);
        s_grey  = fmin(100.0, s_grey + d * 0.9);
    }
    /* A flush every 40 minutes. */
    static bool s_flushed;
    const bool flush = duty(40 * 60, 5, 1500);
    if (flush && !s_flushed && light_on(L_PUMP)) {
        s_fresh = fmax(0.0, s_fresh - 0.3);
        s_black = fmin(100.0, s_black + 0.6);
    }
    s_flushed = flush;
}

static void publish_water(void)
{
    capstan_model_set_tank(CAPSTAN_TANK_FRESH, floor(s_fresh + 0.5));
    capstan_model_set_tank(CAPSTAN_TANK_GREY,  floor(s_grey + 0.5));
    capstan_model_set_tank(CAPSTAN_TANK_BLACK, floor(s_black + 0.5));
}

static void publish_level(double dt)
{
    /* Someone walking about rocks the trailer a few hundredths of a degree;
     * it settles in a couple of seconds. */
    s_level_wobble *= exp(-dt / 0.8);
    if (noise() > 0.93) {
        s_level_wobble += 0.06 * noise();
    }
    const double fb = -0.82 + s_level_wobble + 0.004 * noise();
    const double ss =  0.37 + 0.5 * s_level_wobble + 0.004 * noise();

    /* 0.01 degree resolution, as Plateau sends it. */
    capstan_model_set_tilt(round(fb * 100.0) / 100.0, round(ss * 100.0) / 100.0);
    capstan_model_set_tilt_diff(round(tan(fb * M_PI / 180.0) * WHEELBASE_MM),
                                round(tan(ss * M_PI / 180.0) * TRACK_MM));
}

static void publish_inputs(void)
{
    /* Picket 0: bit 0 entry door, bit 1 baggage bay (always shut here). */
    capstan_model_set_picket_inputs(0, door_open() ? 0x001 : 0x000);
    /* Switchback 0: bit 0 propane locker lid, shut. */
    capstan_model_set_spoor_inputs(0, 0x00);
}

/* GNSS time from Milepost: the UTC calendar fields, once a second. */
static void publish_gps_time(void)
{
    const time_t now = time(NULL);
    struct tm utc;
    gmtime_r(&now, &utc);
    capstan_model_set_gps_time(utc.tm_year + 1900, utc.tm_mon + 1, utc.tm_mday,
                               utc.tm_hour, utc.tm_min, utc.tm_sec);
}

/*
 * os/timezone/current, retained, so it arrives once on connect.
 *
 * The browser's own zone, so the dial agrees with the clock on the wall of
 * whoever is running the simulator. Emscripten's localtime() renders in that
 * zone regardless of TZ, so what this exercises is the model's lookup: a zone
 * missing from its table is logged and left UTC-flagged, as on the device.
 */
EM_JS(int, sim_browser_tz, (char *out, int len), {
    var tz = "UTC";
    try { tz = Intl.DateTimeFormat().resolvedOptions().timeZone || "UTC"; } catch (e) {}
    stringToUTF8(tz, out, len);
    return 0;
});

static void publish_timezone(void)
{
    char tz[64];
    sim_browser_tz(tz, sizeof(tz));
    capstan_model_set_timezone(tz);
}

/* ---- the lights' starting states ------------------------------------- */
/*
 * Statuses the rig would deliver on connect. Evening lights are on after
 * sunset, the pump is on (it usually is while camping), and the furnace runs
 * in the small hours when the cabin is at its coldest.
 */
static void publish_initial_lights(double h)
{
    const bool evening = h >= 19.5 && h < 23.5;
    const bool small_hours = h < 6.5;
    const bool on[L_COUNT] = {
        [L_CEILING]  = evening,
        [L_KITCHEN]  = h >= 17.5 && h < 21.0,
        [L_PORCH]    = evening,
        [L_AWNING]   = false,
        [L_PUMP]     = true,
        [L_FURNACE]  = small_hours,
        [L_INVERTER] = false,
    };
    for (int i = 0; i < L_COUNT; i++) {
        capstan_model_set_light(CONTROLS[i].id, on[i], on[i] ? 255 : 0);
    }
}

/* ---- scheduler ------------------------------------------------------- */

static void feed_tick(lv_timer_t *t)
{
    (void)t;
    const int64_t now_us = esp_timer_get_time();
    const double  dt = (now_us - s_last_us) / 1e6;
    s_last_us = now_us;

    const double h = hour_of_day();
    step_energy(dt, h);
    step_air(dt, h);
    step_water(dt);

    /* Each at its module's real cadence (capstan_model.h), so expiry is
     * exercised the way the rig exercises it. */
    static int64_t next_1s, next_2s;
    publish_level(dt);                          /* Plateau, 2 Hz */
    if (now_us >= next_1s) {                    /* Ampline, Reservoir, Milepost */
        next_1s = now_us + 1000000;
        publish_energy();
        publish_water();
        publish_gps_time();
        publish_inputs();
    }
    if (now_us >= next_2s) {                    /* Borealis */
        next_2s = now_us + 2000000;
        publish_air(h);
    }
}

/* ---- entry points ---------------------------------------------------- */

void sim_feed_seed_config(void)
{
    /*
     * The retained config Headwaters would deliver, stored through the same
     * capstan_config setters the MQTT handlers call. Credentials are empty:
     * nothing reads them in the simulator, and a plausible-looking one is
     * exactly what should never be written into source.
     */
    capstan_wifi_cfg_t wifi = { .security = CAPSTAN_WIFI_SEC_WPA2_PSK,
                                .configured = true };
    snprintf(wifi.ssid, sizeof(wifi.ssid), "%s", "TrailCurrent-Rig");
    capstan_config_set_wifi(&wifi);

    capstan_mqtt_cfg_t mqtt = { .port = 8883, .configured = true };
    snprintf(mqtt.host, sizeof(mqtt.host), "%s", "headwaters.local");
    capstan_config_set_mqtt(&mqtt);

    capstan_controls_t controls = { .count = L_COUNT };
    for (int i = 0; i < L_COUNT; i++) {
        controls.items[i].id = CONTROLS[i].id;
        snprintf(controls.items[i].name, sizeof(controls.items[i].name),
                 "%s", CONTROLS[i].name);
        snprintf(controls.items[i].icon, sizeof(controls.items[i].icon),
                 "%s", CONTROLS[i].icon);
    }
    capstan_config_set_controls(&controls);

    /* Doors that matter on the road and in storage, not while camping. */
    capstan_alarms_t alarms = {
        .count = 3,
        .items = {
            { .src = CAPSTAN_ALARM_SRC_PICKET, .addr = 0, .sensor = 1,
              .name = "Entry Door", .icon = "door-closed",
              .modes = { CAPSTAN_VERDICT_NONE, CAPSTAN_VERDICT_HIGH,
                         CAPSTAN_VERDICT_HIGH } },
            { .src = CAPSTAN_ALARM_SRC_PICKET, .addr = 0, .sensor = 2,
              .name = "Baggage Bay", .icon = "warehouse",
              .modes = { CAPSTAN_VERDICT_NONE, CAPSTAN_VERDICT_HIGH,
                         CAPSTAN_VERDICT_HIGH } },
            { .src = CAPSTAN_ALARM_SRC_SWITCHBACK, .addr = 0, .sensor = 1,
              .name = "Propane Locker", .icon = "lock",
              .modes = { CAPSTAN_VERDICT_HIGH, CAPSTAN_VERDICT_HIGH,
                         CAPSTAN_VERDICT_HIGH } },
        },
    };
    capstan_config_set_alarms(&alarms);
    capstan_config_set_mode(CAPSTAN_MODE_CAMPING);
}

void sim_feed_start(void)
{
    s_t0      = esp_timer_get_time() / 1e6;
    s_last_us = esp_timer_get_time();

    const double h = hour_of_day();
    s_cabin_f = cabin_target_f(h);

    /* A believable charge for the hour: lowest at dawn after a night on the
     * battery, highest late afternoon. */
    s_soc = clamp(72.0 + 18.0 * sin(TWO_PI * (h - 11.0) / 24.0), 25.0, 97.0);

    publish_timezone();
    publish_initial_lights(h);
    feed_tick(NULL);

    lv_timer_create(feed_tick, 500, NULL);
    ESP_LOGI(TAG, "rig online: SOC %.0f%%, cabin %.0f F, local hour %.1f",
             s_soc, s_cabin_f, h);
}

#endif /* EEZ_LVGL_SIMULATOR */
