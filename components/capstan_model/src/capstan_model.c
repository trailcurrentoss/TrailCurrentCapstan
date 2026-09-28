/*
 * The data layer the UI observes. See capstan_model.h.
 */

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "capstan_model.h"

static const char *TAG = "model";

/* Timeouts per module, in microseconds. See the header for the cadences
 * these derive from. Lights and doors are change-driven, so they never
 * expire -- a light that has not been touched in an hour is not stale. */
static const int64_t TIMEOUT_US[CAPSTAN_MOD_COUNT] = {
    [CAPSTAN_MOD_ENERGY] = 10LL * 1000000,
    [CAPSTAN_MOD_AIR]    = 20LL * 1000000,
    [CAPSTAN_MOD_WATER]  = 10LL * 1000000,
    [CAPSTAN_MOD_LEVEL]  =  5LL * 1000000,
    [CAPSTAN_MOD_LIGHTS] = 0,
    [CAPSTAN_MOD_DOORS]  = 0,
};

typedef struct {
    float            value;
    bool             seen;
    int64_t          at_us;
    capstan_module_t mod;
} metric_t;

static SemaphoreHandle_t s_lock;

static metric_t m_batt_v, m_batt_pct, m_load_w, m_solar_w, m_runtime;
static metric_t m_temp_f, m_temp_c, m_humid, m_tvoc, m_eco2, m_co;
static metric_t m_tank[CAPSTAN_TANK_COUNT];
static metric_t m_tilt_fb, m_tilt_ss;

static char     s_charge_type[16] = "--";
static bool     s_co_alarm, s_co_warn, s_lpg_alarm, s_lpg_warn;
static bool     s_light_on[CAPSTAN_MAX_LIGHTS + 1];
static uint8_t  s_light_bri[CAPSTAN_MAX_LIGHTS + 1];
static uint16_t s_picket[CAPSTAN_PICKET_ADDRS];
static int64_t  s_module_seen[CAPSTAN_MOD_COUNT];

#define LOCK()   xSemaphoreTake(s_lock, portMAX_DELAY)
#define UNLOCK() xSemaphoreGive(s_lock)

void capstan_model_init(void)
{
    if (!s_lock) {
        s_lock = xSemaphoreCreateMutex();
    }
    /* Everything starts unseen, which renders as `--`. That is the correct
     * boot state: nothing is retained, so we genuinely do not know. */
    m_batt_v.mod = m_batt_pct.mod = m_load_w.mod = m_solar_w.mod =
        m_runtime.mod = CAPSTAN_MOD_ENERGY;
    m_temp_f.mod = m_temp_c.mod = m_humid.mod = m_tvoc.mod =
        m_eco2.mod = m_co.mod = CAPSTAN_MOD_AIR;
    for (int i = 0; i < CAPSTAN_TANK_COUNT; i++) {
        m_tank[i].mod = CAPSTAN_MOD_WATER;
    }
    m_tilt_fb.mod = m_tilt_ss.mod = CAPSTAN_MOD_LEVEL;
    ESP_LOGI(TAG, "model ready; all values unset");
}

static void put(metric_t *m, double v)
{
    LOCK();
    m->value = (float)v;
    m->seen  = true;
    m->at_us = esp_timer_get_time();
    s_module_seen[m->mod] = m->at_us;
    UNLOCK();
}

static capstan_value_t get(const metric_t *m)
{
    capstan_value_t out = { 0.0f, false };
    LOCK();
    if (m->seen) {
        const int64_t t = TIMEOUT_US[m->mod];
        if (t == 0 || (esp_timer_get_time() - m->at_us) < t) {
            out.value = m->value;
            out.valid = true;
        }
    }
    UNLOCK();
    return out;
}

/* ---- setters -------------------------------------------------------- */
void capstan_model_set_battery_volts(double v) { put(&m_batt_v, v); }
void capstan_model_set_battery_pct(double v)   { put(&m_batt_pct, v); }
void capstan_model_set_load_watts(double v)    { put(&m_load_w, v); }
void capstan_model_set_solar_watts(double v)   { put(&m_solar_w, v); }
void capstan_model_set_runtime_min(double v)   { put(&m_runtime, v); }

void capstan_model_set_charge_type(const char *s)
{
    if (!s) { return; }
    LOCK();
    strncpy(s_charge_type, s, sizeof(s_charge_type) - 1);
    s_charge_type[sizeof(s_charge_type) - 1] = '\0';
    s_module_seen[CAPSTAN_MOD_ENERGY] = esp_timer_get_time();
    UNLOCK();
}

void capstan_model_set_temp_f(double v)  { put(&m_temp_f, v); }
void capstan_model_set_temp_c(double v)  { put(&m_temp_c, v); }
void capstan_model_set_humidity(double v){ put(&m_humid, v); }
void capstan_model_set_tvoc(double v)    { put(&m_tvoc, v); }
void capstan_model_set_eco2(double v)    { put(&m_eco2, v); }
void capstan_model_set_co(double v)      { put(&m_co, v); }

void capstan_model_set_safety_flags(bool co_a, bool co_w, bool lpg_a, bool lpg_w)
{
    LOCK();
    /* Thresholds are evaluated on-board Borealis; these booleans are the
     * source of truth and must not be re-derived from the ppm values. */
    s_co_alarm = co_a; s_co_warn = co_w;
    s_lpg_alarm = lpg_a; s_lpg_warn = lpg_w;
    s_module_seen[CAPSTAN_MOD_AIR] = esp_timer_get_time();
    UNLOCK();
}

void capstan_model_set_tank(capstan_tank_t t, double pct)
{
    if (t >= 0 && t < CAPSTAN_TANK_COUNT) { put(&m_tank[t], pct); }
}

void capstan_model_set_tilt(double fb, double ss)
{
    put(&m_tilt_fb, fb);
    put(&m_tilt_ss, ss);
}

void capstan_model_set_light(int id, bool on, int brightness)
{
    if (id < 1 || id > CAPSTAN_MAX_LIGHTS) { return; }
    LOCK();
    s_light_on[id]  = on;
    s_light_bri[id] = (uint8_t)(brightness < 0 ? 0 :
                                brightness > 255 ? 255 : brightness);
    s_module_seen[CAPSTAN_MOD_LIGHTS] = esp_timer_get_time();
    UNLOCK();
}

void capstan_model_set_picket_inputs(int addr, uint16_t mask)
{
    if (addr < 0 || addr >= CAPSTAN_PICKET_ADDRS) { return; }
    LOCK();
    s_picket[addr] = mask;
    s_module_seen[CAPSTAN_MOD_DOORS] = esp_timer_get_time();
    UNLOCK();
}

void capstan_model_note_trigger(const char *topic, const char *payload)
{
    /* Discovery and OTA triggers are bare strings, not JSON. Handled by
     * the discovery/OTA layers; recorded here only so the parse path has
     * somewhere to hand them. */
    ESP_LOGI(TAG, "trigger %s = %s", topic ? topic : "?", payload ? payload : "");
}

/* ---- clock ---------------------------------------------------------- */
/*
 * WHY THE ZONE IS A TABLE AND NOT A ZONEINFO LOOKUP
 *
 * `os/timezone/current` carries an IANA name because that is what the
 * Headwaters OS daemon has -- it reads /etc/timezone. Newlib on the ESP32
 * has no zoneinfo database, so an IANA name means nothing to it; it wants a
 * POSIX TZ string with the DST rule spelled out. The translation has to
 * happen somewhere, and a table of the zones the product actually ships to
 * is smaller than the alternative by several hundred kilobytes.
 *
 * Same list, same rules as Fireside's apply_timezone(). Post-2007 US
 * convention: DST from the 2nd Sunday of March to the 1st Sunday of
 * November; Phoenix never observes it.
 */
static const struct { const char *iana; const char *posix; } ZONES[] = {
    { "America/New_York",    "EST5EDT,M3.2.0,M11.1.0" },
    { "America/Chicago",     "CST6CDT,M3.2.0,M11.1.0" },
    { "America/Denver",      "MST7MDT,M3.2.0,M11.1.0" },
    { "America/Los_Angeles", "PST8PDT,M3.2.0,M11.1.0" },
    { "America/Phoenix",     "MST7"                   },
    { "America/Anchorage",   "AKST9AKDT,M3.2.0,M11.1.0" },
    { "Pacific/Honolulu",    "HST10"                  },
    { "UTC",                 "UTC0"                   },
};

static bool s_time_valid;
static bool s_tz_known;

bool capstan_model_set_timezone(const char *iana)
{
    if (!iana || !*iana) {
        return false;
    }
    for (size_t i = 0; i < sizeof(ZONES) / sizeof(*ZONES); i++) {
        if (strcmp(iana, ZONES[i].iana) == 0) {
            setenv("TZ", ZONES[i].posix, 1);
            tzset();
            s_tz_known = true;
            ESP_LOGI(TAG, "timezone %s -> %s", iana, ZONES[i].posix);
            return true;
        }
    }
    ESP_LOGW(TAG, "unknown timezone '%s'; keeping the current one -- add it "
                  "to ZONES above. The clock will read UTC until then", iana);
    return false;
}

bool capstan_model_timezone_known(void) { return s_tz_known; }

void capstan_model_set_gps_time(int year, int month, int day,
                                int hour, int minute, int second)
{
    /*
     * No fix yet. The GNSS module keeps publishing CAN 0x006 either way, with
     * a placeholder date, so a stream of these is NOT evidence that the clock
     * should be working.
     *
     * Rate-limited and logged at WARNING, because this is the answer to the
     * only question anyone asks about the idle face: the clock shows `--` and
     * the panel is plainly on the broker with every other reading live. When
     * that happens, either this line is in the log -- the fix is the antenna,
     * not the firmware -- or `local/gps/time` is not arriving at all, and its
     * absence from the log is what says so.
     *
     * Roughly once a minute at the 1 Hz this topic publishes at.
     */
    if (year < 2020) {
        static int64_t s_last_nofix_us;
        const int64_t now = esp_timer_get_time();
        if (s_last_nofix_us == 0 || (now - s_last_nofix_us) > 60LL * 1000000) {
            s_last_nofix_us = now;
            ESP_LOGW(TAG, "GNSS time has no fix yet (year=%d) -- the clock "
                          "stays blank until it does", year);
        }
        return;
    }

    struct tm t = {
        .tm_year = year - 1900, .tm_mon = month - 1, .tm_mday = day,
        .tm_hour = hour, .tm_min = minute, .tm_sec = second,
        .tm_isdst = 0,
    };

    /*
     * The fix is UTC, but mktime() reads its argument as LOCAL time under
     * whatever TZ is currently installed. Calling it directly would fold
     * the local offset into the epoch -- the clock would be wrong by the
     * offset, and localtime_r() would then render UTC hours no matter
     * which zone the user is in, which looks exactly like "the timezone
     * setting does nothing". Pin TZ to UTC across the conversion only.
     */
    const char *saved = getenv("TZ");
    char saved_buf[48] = { 0 };
    if (saved) {
        strncpy(saved_buf, saved, sizeof(saved_buf) - 1);
    }
    setenv("TZ", "UTC0", 1);
    tzset();
    const time_t epoch = mktime(&t);
    if (saved_buf[0]) setenv("TZ", saved_buf, 1);
    else              unsetenv("TZ");
    tzset();

    if (epoch <= 0) {
        return;
    }

    /*
     * Only step the clock when it is actually wrong. Milepost publishes at
     * ~1 Hz, and settimeofday() on every message would drag the second
     * hand backwards and forwards by the message latency, visibly, on a
     * face whose whole job is to look calm.
     */
    struct timeval now;
    if (s_time_valid && gettimeofday(&now, NULL) == 0) {
        const long drift = (long)(epoch - now.tv_sec);
        if (drift > -2 && drift < 2) {
            return;
        }
    }

    struct timeval tv = { .tv_sec = epoch, .tv_usec = 0 };
    settimeofday(&tv, NULL);

    if (!s_time_valid) {
        ESP_LOGI(TAG, "clock set from GNSS: %04d-%02d-%02d %02d:%02d:%02dZ",
                 year, month, day, hour, minute, second);
    }
    s_time_valid = true;
}

bool capstan_model_time_valid(void) { return s_time_valid; }

/* ---- getters -------------------------------------------------------- */
capstan_value_t capstan_model_battery_volts(void) { return get(&m_batt_v); }
capstan_value_t capstan_model_battery_pct(void)   { return get(&m_batt_pct); }
capstan_value_t capstan_model_load_watts(void)    { return get(&m_load_w); }
capstan_value_t capstan_model_solar_watts(void)   { return get(&m_solar_w); }
capstan_value_t capstan_model_runtime_min(void)   { return get(&m_runtime); }
capstan_value_t capstan_model_temp_f(void)        { return get(&m_temp_f); }
capstan_value_t capstan_model_temp_c(void)        { return get(&m_temp_c); }
capstan_value_t capstan_model_humidity(void)      { return get(&m_humid); }
capstan_value_t capstan_model_tvoc(void)          { return get(&m_tvoc); }
capstan_value_t capstan_model_eco2(void)          { return get(&m_eco2); }
capstan_value_t capstan_model_co(void)            { return get(&m_co); }
capstan_value_t capstan_model_tilt_front_back(void) { return get(&m_tilt_fb); }
capstan_value_t capstan_model_tilt_side_to_side(void) { return get(&m_tilt_ss); }

capstan_value_t capstan_model_tank(capstan_tank_t t)
{
    if (t < 0 || t >= CAPSTAN_TANK_COUNT) {
        return (capstan_value_t){ 0.0f, false };
    }
    return get(&m_tank[t]);
}

const char *capstan_model_charge_type(void) { return s_charge_type; }

bool capstan_model_any_alarm(void)
{
    bool a;
    LOCK();
    a = s_co_alarm || s_lpg_alarm;
    UNLOCK();
    return a;
}

bool capstan_model_light_on(int id)
{
    if (id < 1 || id > CAPSTAN_MAX_LIGHTS) { return false; }
    bool v; LOCK(); v = s_light_on[id]; UNLOCK(); return v;
}

int capstan_model_light_brightness(int id)
{
    if (id < 1 || id > CAPSTAN_MAX_LIGHTS) { return 0; }
    int v; LOCK(); v = s_light_bri[id]; UNLOCK(); return v;
}

int capstan_model_lights_on_count(void)
{
    int n = 0;
    LOCK();
    for (int i = 1; i <= CAPSTAN_MAX_LIGHTS; i++) {
        if (s_light_on[i]) { n++; }
    }
    UNLOCK();
    return n;
}

uint16_t capstan_model_picket_inputs(int addr)
{
    if (addr < 0 || addr >= CAPSTAN_PICKET_ADDRS) { return 0; }
    uint16_t v; LOCK(); v = s_picket[addr]; UNLOCK(); return v;
}

bool capstan_model_module_alive(capstan_module_t m)
{
    if (m < 0 || m >= CAPSTAN_MOD_COUNT) { return false; }
    const int64_t t = TIMEOUT_US[m];
    bool alive;
    LOCK();
    alive = s_module_seen[m] != 0 &&
            (t == 0 || (esp_timer_get_time() - s_module_seen[m]) < t);
    UNLOCK();
    return alive;
}

void capstan_model_expire_stale(void)
{
    /*
     * Deliberately does nothing.
     *
     * Expiry is evaluated lazily in get(), by comparing the timestamp at
     * read time. There is no state to sweep, so a periodic pass would only
     * be able to log -- and a value cannot be read as fresh after its
     * timeout regardless of whether this was called.
     *
     * The function stays because the call site is the natural place to add
     * "module went offline" logging or a UI transition later, and because
     * a data layer with no visible expiry step invites someone to assume
     * there isn't one.
     */
}
