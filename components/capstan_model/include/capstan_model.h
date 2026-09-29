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

/*
 * How many DISTINCT lights this dial tracks state for -- not a ceiling on the
 * light id.
 *
 * Ids are sparse: PDM channels are 1..N and Switchback relays start at 100, so
 * an array indexed by id would have to be 100+ entries and would still break
 * the moment the id space moved. State is held in a table keyed by id
 * instead; this is its capacity.
 */
#define CAPSTAN_MAX_LIGHTS  56
#define CAPSTAN_PICKET_ADDRS 8
#define CAPSTAN_SPOOR_ADDRS  8   /**< Switchback DI boards, local/spoor/<addr>/inputs */

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
/**
 * Battery power, SIGNED, from the SmartShunt via Solstice: positive while the
 * battery is charging, negative while it is discharging (Victron's `P`).
 * `battery_watts` on local/energy/status; a Headwaters that predates the
 * field never sets it, and it then reads invalid.
 */
void capstan_model_set_battery_watts(double v);
void capstan_model_set_solar_watts(double v);
void capstan_model_set_runtime_min(double v);
void capstan_model_set_charge_type(const char *s);

void capstan_model_set_temp_f(double v);
void capstan_model_set_temp_c(double v);
void capstan_model_set_humidity(double v);
void capstan_model_set_tvoc(double v);
void capstan_model_set_eco2(double v);
void capstan_model_set_co(double v);
/**
 * Borealis's own threshold verdicts, from `local/airquality/safety`.
 *
 * All seven are evaluated ON BOARD Borealis and must be taken as given --
 * docs/mqtt.md says so explicitly, and re-deriving any of them from the ppm
 * values here would mean this display disagreeing with the module that owns
 * the sensor.
 */
void capstan_model_set_safety_flags(bool co_alarm, bool co_warn,
                                    bool lpg_alarm, bool lpg_warn,
                                    bool co2_alarm, bool co2_warn,
                                    bool voc_alarm);

void capstan_model_set_tank(capstan_tank_t t, double pct);
void capstan_model_set_tilt(double front_back, double side_to_side);
/** Plateau's height differences across the vehicle, mm, from the same
 *  `local/level/tilt` frame (front_back_diff_mm, left_right_diff_mm). */
void capstan_model_set_tilt_diff(double front_back_mm, double left_right_mm);
void capstan_model_set_light(int id, bool on, int brightness);
void capstan_model_set_picket_inputs(int addr, uint16_t mask);
/** Switchback digital inputs, from `local/spoor/<addr>/inputs` (8 bits). */
void capstan_model_set_spoor_inputs(int addr, uint16_t mask);
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

/**
 * True once `os/timezone/current` has been received AND its zone was one this
 * build knows.
 *
 * False therefore covers two different situations that look identical on the
 * glass -- the topic never arrived, or it named a zone missing from ZONES --
 * and in both the clock is rendering UTC. That is only a few hours wrong,
 * which is exactly the kind of wrong nobody notices from the panel, so the
 * distinction is worth being able to log.
 */
bool capstan_model_timezone_known(void);

/* ---- getters, called from the LVGL task ---------------------------- */
capstan_value_t capstan_model_battery_volts(void);
capstan_value_t capstan_model_battery_pct(void);
capstan_value_t capstan_model_load_watts(void);
capstan_value_t capstan_model_battery_watts(void);
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

/**
 * How the air screen's status word and ring should read.
 *
 * This is a VERDICT, not a measurement: it is folded from Borealis's
 * threshold booleans and from nothing else. There is no AQI on this bus and
 * this is deliberately not one -- see the note on page_air in
 * GUI/tmp/screens_layout.py.
 *
 * CAPSTAN_AIR_UNKNOWN covers both "Borealis has never reported" and
 * "Borealis has gone quiet", which the screen renders as `--` with the ring
 * empty, for the same reason every other reading does.
 *
 * LPG is NOT folded in. A propane leak is a leak, not air quality, and it
 * already raises the alert overlay through capstan_model_any_alarm(). Having
 * it also turn this ring amber would mean two controls reporting one event
 * with different words.
 */
typedef enum {
    CAPSTAN_AIR_UNKNOWN = 0,  /**< no data -- render `--` */
    CAPSTAN_AIR_GOOD,         /**< nothing flagged */
    CAPSTAN_AIR_MODERATE,     /**< co2_warn or co_warn */
    CAPSTAN_AIR_UNHEALTHY,    /**< co2_alarm, voc_alarm or co_alarm */
} capstan_air_level_t;

capstan_air_level_t capstan_model_air_level(void);

capstan_value_t capstan_model_tank(capstan_tank_t t);
capstan_value_t capstan_model_tilt_front_back(void);
capstan_value_t capstan_model_tilt_side_to_side(void);
capstan_value_t capstan_model_tilt_diff_front_back(void);   /* mm */
capstan_value_t capstan_model_tilt_diff_left_right(void);   /* mm */

bool capstan_model_light_on(int id);

/** True once a status has been seen for this id. Distinguishes "off" from
 *  "never reported", which look identical through light_on() alone. */
bool capstan_model_light_known(int id);
int  capstan_model_light_brightness(int id);
int  capstan_model_lights_on_count(void);

uint16_t capstan_model_picket_inputs(int addr);

/**
 * A board's raw input word, and whether it has ever reported.
 *
 * The flag is the point. An alarm with a `low` verdict fires while its input
 * is NOT asserted, and a board that has never published reads as all-zero --
 * so without it every `low` alarm would fire at boot, before the first input
 * broadcast, and keep firing on a rig whose board is not fitted at all.
 * Returns false (and *out = 0) for a board never heard from.
 */
bool capstan_model_input_word(bool switchback, int addr, uint16_t *out);

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
