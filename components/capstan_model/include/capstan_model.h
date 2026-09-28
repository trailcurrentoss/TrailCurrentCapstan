/*
 * The data layer the UI observes.
 *
 * Every live value from the rig lands here, and the UI reads only from
 * here -- no screen touches MQTT. That boundary is what lets the whole UI
 * run against the mock source with no rig and no broker.
 *
 * WHY EVERY VALUE HAS A TIMESTAMP
 *
 * Nothing on this platform is retained. can-bridge.js publishes every
 * sensor topic WITHOUT the retain flag, so a freshly-booted display knows
 * nothing until the next periodic frame, and a module that dies leaves its
 * last reading sitting on screen looking perfectly current.
 *
 * So a value is never just a number: it is a number plus when it arrived.
 * Ask for it after its module's timeout and you get "no value", which the
 * UI renders as `--`. A stale number that looks live is worse than no
 * number -- someone acts on it.
 *
 * THREADING
 *
 * Setters are called from the MQTT task, getters from the LVGL task. Both
 * take an internal mutex; getters return by value so nothing holds a
 * pointer into the store.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Publish cadences and the timeouts that follow from them. Cadences are
 * what the modules actually do; the timeouts are Fireside's, which are
 * roughly 5-10x the cadence -- long enough not to flicker on a dropped
 * frame, short enough that a dead module is obvious.
 */
typedef enum {
    CAPSTAN_MOD_ENERGY = 0,   /* Ampline,  1 s   -> 10 s */
    CAPSTAN_MOD_AIR,          /* Borealis, 2 s   -> 20 s */
    CAPSTAN_MOD_WATER,        /* Reservoir,1 s   -> 10 s */
    CAPSTAN_MOD_LEVEL,        /* Plateau,  2 Hz  ->  5 s */
    CAPSTAN_MOD_LIGHTS,       /* on change only  -> never expires */
    CAPSTAN_MOD_DOORS,        /* on change only  -> never expires */
    CAPSTAN_MOD_COUNT
} capstan_module_t;

typedef enum {
    CAPSTAN_TANK_FRESH = 0,
    CAPSTAN_TANK_GREY,
    CAPSTAN_TANK_BLACK,
    CAPSTAN_TANK_COUNT
} capstan_tank_t;

#define CAPSTAN_MAX_LIGHTS  24
#define CAPSTAN_PICKET_ADDRS 8

/** A reading plus whether it is still trustworthy. */
typedef struct {
    float value;
    bool  valid;      /**< false = never received, or expired -> render `--` */
} capstan_value_t;

void capstan_model_init(void);

/* ---- setters, called from the MQTT task ---------------------------- */
void capstan_model_set_battery_volts(double v);
void capstan_model_set_battery_pct(double v);
void capstan_model_set_load_watts(double v);
void capstan_model_set_solar_watts(double v);
void capstan_model_set_runtime_min(double v);
void capstan_model_set_charge_type(const char *s);

void capstan_model_set_temp_f(double v);
void capstan_model_set_temp_c(double v);
void capstan_model_set_humidity(double v);
void capstan_model_set_tvoc(double v);
void capstan_model_set_eco2(double v);
void capstan_model_set_co(double v);
void capstan_model_set_safety_flags(bool co_alarm, bool co_warn,
                                    bool lpg_alarm, bool lpg_warn);

void capstan_model_set_tank(capstan_tank_t t, double pct);
void capstan_model_set_tilt(double front_back, double side_to_side);
void capstan_model_set_light(int id, bool on, int brightness);
void capstan_model_set_picket_inputs(int addr, uint16_t mask);
void capstan_model_note_trigger(const char *topic, const char *payload);

/* ---- clock --------------------------------------------------------- */
/*
 * The rig has no RTC and no SNTP. Time arrives on `local/gps/time` as a
 * UTC calendar date from Milepost's GNSS fix, and the zone to render it in
 * arrives separately on the retained `os/timezone/current`. Both land here
 * because the idle clock reads the model like every other screen does.
 */

/**
 * Set the system clock from a GNSS fix. Fields are UTC, as published.
 *
 * Years before 2020 are rejected: a GNSS module that has not achieved a
 * fix publishes 1980 or 2000 epochs, and accepting one would jump the
 * clock backwards by decades every time the receiver loses the sky.
 */
void capstan_model_set_gps_time(int year, int month, int day,
                                int hour, int minute, int second);

/**
 * Install a timezone from its IANA name, e.g. "America/Denver".
 *
 * Returns false for a zone this build does not know, in which case the
 * previously installed zone is kept -- rendering UTC silently would be
 * worse than rendering a slightly stale offset.
 */
bool capstan_model_set_timezone(const char *iana);

/** True once the clock has been set from a fix. The idle face parks its
 *  hands and shows `--` until then, rather than confidently drawing
 *  whatever the un-set system clock happens to say. */
bool capstan_model_time_valid(void);

/* ---- getters, called from the LVGL task ---------------------------- */
capstan_value_t capstan_model_battery_volts(void);
capstan_value_t capstan_model_battery_pct(void);
capstan_value_t capstan_model_load_watts(void);
capstan_value_t capstan_model_solar_watts(void);
capstan_value_t capstan_model_runtime_min(void);
const char     *capstan_model_charge_type(void);   /**< "--" when unknown */

capstan_value_t capstan_model_temp_f(void);
capstan_value_t capstan_model_temp_c(void);
capstan_value_t capstan_model_humidity(void);
capstan_value_t capstan_model_tvoc(void);
capstan_value_t capstan_model_eco2(void);
capstan_value_t capstan_model_co(void);
bool capstan_model_any_alarm(void);

capstan_value_t capstan_model_tank(capstan_tank_t t);
capstan_value_t capstan_model_tilt_front_back(void);
capstan_value_t capstan_model_tilt_side_to_side(void);

bool capstan_model_light_on(int id);
int  capstan_model_light_brightness(int id);
int  capstan_model_lights_on_count(void);

uint16_t capstan_model_picket_inputs(int addr);

/** True if the module has produced anything within its timeout. Drives the
 *  "module offline" treatment on a screen, as distinct from a single
 *  missing value. */
bool capstan_model_module_alive(capstan_module_t m);

/**
 * Expire anything past its module's timeout. Call every main-loop pass;
 * it is a handful of comparisons until something actually times out.
 */
void capstan_model_expire_stale(void);

#ifdef __cplusplus
}
#endif
