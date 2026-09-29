/*
 * Alarm evaluation and the alert overlay. See ui_alerts.h.
 *
 * WHY THIS EXISTS
 *
 * The alarm contract was built end to end except for its last step. The PWA
 * saved each dial's list, Headwaters published it retained, the dial parsed it
 * and wrote it to NVS -- and then nothing ever read it back.
 * capstan_alarm_is_active() had no callers and nothing navigated to
 * PageAlert, so a configured alarm could fire all night on a dial that showed
 * the clock. The CO and LPG flags were in the same position: the model
 * header says they raise this overlay, and nothing did.
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "esp_log.h"
#include "esp_timer.h"

#include "capstan_board.h"
#include "capstan_config.h"
#include "capstan_model.h"
#include "ui_climate.h"
#include "ui_data.h"
#include "ui_devices.h"
#include "ui_alerts.h"
#include "ui_light_icons.h"
#include "ui_nav.h"

#if CAPSTAN_HAVE_UI

#include "screens.h"
#include "ui_lv.h"

static const char *TAG = "ui.alerts";

/*
 * One bit per source. The gas alarm takes bit 0 so it wins when several rise
 * on the same tick -- a CO or propane alarm outranks a fridge sense line.
 * Configured alarms follow in the order Headwaters sent them.
 */
#define SRC_GAS        0
#define SRC_CFG_FIRST  1
#define SRC_COUNT      (SRC_CFG_FIRST + CAPSTAN_MAX_ALARMS)

_Static_assert(SRC_COUNT <= 16, "active mask is a uint16_t");

#define GLYPH_BELL     "\xEF\x83\xB3"   /* 0xF0F3 */
#define GLYPH_WARNING  "\xEF\x81\xB1"   /* 0xF071 triangle-exclamation */

static uint16_t         s_prev_active;
static uint16_t         s_prev_attention; /* active and not snoozed, last tick */
static int              s_shown = -1;     /* source on the overlay, or -1 */
static capstan_screen_t s_return = CAPSTAN_SCREEN_IDLE; /* where the overlay was raised from */
static capstan_alarms_t s_last_cfg;       /* to notice a new list arriving */

/*
 * Per-source snooze deadline (esp_timer microseconds), 0 = not snoozed.
 *
 * Set for every alarm on the overlay when the user acknowledges it. While a
 * deadline is in the future that alarm cannot raise the overlay; when it
 * passes and the alarm is STILL active, the alarm is raised again.
 *
 * A deadline is thrown away the moment its alarm stops being active. That is
 * the whole difference between a snooze and a mute: without it, an alarm
 * acknowledged at 10:00, cleared at 10:02 and firing again at 10:05 would sit
 * silent until 10:10 on a stale timer -- and conversely a timer left armed
 * past its alarm's clearing would have nothing real to re-raise.
 */
static int64_t          s_snooze_until[SRC_COUNT];

/* Wall-clock time each source's input last went into alarm, for the
 * prototype's "Opened 7:42 PM" line. 0 when the clock was not set yet. */
static time_t           s_raised_at[SRC_COUNT];

static void clear_snooze(uint16_t mask)
{
    for (int i = 0; i < SRC_COUNT; i++) {
        if ((mask >> i) & 1u) {
            s_snooze_until[i] = 0;
        }
    }
}

/*
 * An alarm's icon key through the same table the device controls use, since
 * both come from the PWA's icon picker. The table falls back to the bulb, which
 * is right for a light and wrong for an alarm, so an unknown key resolves to
 * the bell here instead.
 */
static const char *alarm_glyph(const char *key)
{
    if (key) {
        for (size_t i = 0; i < sizeof(UI_LIGHT_ICONS) / sizeof(*UI_LIGHT_ICONS); i++) {
            if (strcmp(UI_LIGHT_ICONS[i].key, key) == 0) {
                return UI_LIGHT_ICONS[i].glyph;
            }
        }
    }
    return GLYPH_BELL;
}

/*
 * One line per alarm whenever what it sees changes: its board's input word
 * (or that the board has not reported), the mode, and the verdict. Quiet in
 * steady state, and exactly what is needed to answer "why did this not
 * fire?" from a serial log.
 */
static void trace_inputs(const capstan_alarms_t *cfg, capstan_mode_t mode)
{
    static int32_t s_last_word[CAPSTAN_MAX_ALARMS] = { -2, -2, -2, -2, -2, -2 };
    static int     s_last_mode = -1;
    const bool mode_changed = (int)mode != s_last_mode;
    s_last_mode = (int)mode;

    for (uint8_t i = 0; i < cfg->count; i++) {
        const capstan_alarm_t *a = &cfg->items[i];
        uint16_t w;
        const bool seen = capstan_model_input_word(
            a->src == CAPSTAN_ALARM_SRC_SWITCHBACK, a->addr, &w);
        const int32_t key = seen ? (int32_t)w : -1;
        if (key == s_last_word[i] && !mode_changed) {
            continue;
        }
        s_last_word[i] = key;
        if (!seen) {
            ESP_LOGI(TAG, "%s: %s board %u has not reported -- not evaluated",
                     a->name,
                     a->src == CAPSTAN_ALARM_SRC_SWITCHBACK ? "spoor" : "picket",
                     (unsigned)a->addr);
            continue;
        }
        ESP_LOGI(TAG, "%s: inputs=0x%03X sensor %u is %s, mode %s -> %s",
                 a->name, (unsigned)w, (unsigned)a->sensor,
                 ((w >> (a->sensor - 1)) & 1u) ? "HIGH" : "low",
                 capstan_mode_name(mode),
                 capstan_alarm_is_active(a, w, mode) ? "ALARM" : "quiet");
    }
}

static uint16_t evaluate(const capstan_alarms_t *cfg, capstan_mode_t mode)
{
    uint16_t active = 0;
    trace_inputs(cfg, mode);

    if (capstan_model_any_alarm()) {
        active |= 1u << SRC_GAS;
    }

    for (uint8_t i = 0; i < cfg->count; i++) {
        const capstan_alarm_t *a = &cfg->items[i];
        uint16_t word;
        /* A board that has never reported is not evaluated at all: its word
         * reads zero, and every `low` alarm on it would fire. */
        if (!capstan_model_input_word(a->src == CAPSTAN_ALARM_SRC_SWITCHBACK,
                                      a->addr, &word)) {
            continue;
        }
        if (capstan_alarm_is_active(a, word, mode)) {
            active |= 1u << (SRC_CFG_FIRST + i);
        }
    }
    return active;
}

static void set_text(lv_obj_t *label, const char *text)
{
    ui_lv_set_text(label, text);
}

static int popcount16(uint16_t v)
{
    int n = 0;
    for (; v; v &= (uint16_t)(v - 1)) { n++; }
    return n;
}

static void paint(const capstan_alarms_t *cfg, capstan_mode_t mode,
                  uint16_t attention)
{
    if (s_shown < 0) {
        return;
    }

    /* The prototype's second line is WHEN: "Opened 7:42 PM". The verb
     * follows the verdict -- a door opens, a sense line goes off -- and the
     * time is left out, not faked, until GNSS has set the clock. */
    const char *verb;
    if (s_shown == SRC_GAS) {
        set_text(objects.alert_icon, GLYPH_WARNING);
        set_text(objects.alert_title, "Gas detected");
        verb = "Detected";
    } else {
        const capstan_alarm_t *a = &cfg->items[s_shown - SRC_CFG_FIRST];
        set_text(objects.alert_icon, alarm_glyph(a->icon));
        set_text(objects.alert_title, a->name);
        verb = a->modes[mode] == CAPSTAN_VERDICT_LOW ? "Off since" : "Opened";
    }

    char msg[64];
    const time_t at = s_raised_at[s_shown];
    if (at) {
        struct tm tm;
        localtime_r(&at, &tm);
        const int h12 = tm.tm_hour % 12 ? tm.tm_hour % 12 : 12;
        snprintf(msg, sizeof(msg), "%s %d:%02d %s", verb, h12, tm.tm_min,
                 tm.tm_hour < 12 ? "AM" : "PM");
    } else {
        snprintf(msg, sizeof(msg), "%s", verb);
    }

    const int others = popcount16(attention & (uint16_t)~(1u << s_shown));
    if (others > 0) {
        const size_t len = strlen(msg);
        snprintf(msg + len, sizeof(msg) - len, "\n+%d more active", others);
    }
    set_text(objects.alert_message, msg);

    capstan_display_cfg_t d;
    capstan_config_get_display(&d);
    char hint[32];
    snprintf(hint, sizeof(hint), "Press to snooze %u min",
             (unsigned)d.alarm_snooze_min);
    set_text(objects.alert_hint, hint);
}

/*
 * The LED ring, decided in one place. See the numbered rules inside.
 * Called every tick, and again on every screen change (ui_alerts_leds_now)
 * so leaving an app darkens the ring at once rather than on the next tick.
 */
static bool s_alarm_active;

static void apply_leds(bool alarm)
{
    s_alarm_active = alarm;
    /*
     * The LED ring (on the boards that have one) is decided HERE and only
     * here, so the features sharing it cannot fight over it:
     *
     *   1. Any alarm active -- snoozed or not -- solid red. Snoozing clears
     *      the screen so the dial is usable; it does not pretend the door
     *      is shut.
     *   2. Otherwise, on the Climate screen: orange heating, blue cooling.
     *   3. Otherwise, on the Devices screen: green while the selected
     *      device is REPORTED on; dark again the moment it reports off.
     *   3b. Otherwise, on Energy's Battery page: the state of charge --
     *      green > 75 %, yellow 40-75 %, red < 40 %, brighter when fuller.
     *   3c. Otherwise, on Water: orange when fresh < 40 % or grey/black
     *      > 60 %, green when fresh > 40 % and grey/black < 50 %.
     *   4. Otherwise dark -- including the app carousel, deliberately: a
     *      colour left over from the last app would read as live status,
     *      and a dark ring is what makes an incoming alarm's red stand out.
     *
     * Unchanged colours are not re-sent.
     */
    uint8_t lr = 0, lg = 0, lb = 0;
    if (alarm) {
        lr = 255;
    } else if (ui_nav_current() == CAPSTAN_SCREEN_CLIMATE) {
        ui_climate_led(&lr, &lg, &lb);
    } else if (ui_nav_current() == CAPSTAN_SCREEN_DEVICES &&
               ui_devices_selected_on()) {
        lr = 82; lg = 164; lb = 65;     /* AccentPrimary #52a441, brand green */
    } else if (ui_nav_current() == CAPSTAN_SCREEN_ENERGY) {
        ui_data_energy_led(&lr, &lg, &lb);
    } else if (ui_nav_current() == CAPSTAN_SCREEN_WATER) {
        ui_data_water_led(&lr, &lg, &lb);
    }
    capstan_board_leds_set_all(lr, lg, lb);
}

void ui_alerts_leds_now(void)
{
    apply_leds(s_alarm_active);
}

void ui_alerts_dismiss(void)
{
    s_shown = -1;
    ui_nav_goto(s_return);
}

void ui_alerts_acknowledge(void)
{
    /* Everything the user was being shown, not just the alarm on the glass:
     * "+2 more active" was on screen too, and acknowledging one of three
     * would bring the overlay straight back for the next. */
    capstan_display_cfg_t d;
    capstan_config_get_display(&d);
    const int64_t until = esp_timer_get_time() +
                          (int64_t)d.alarm_snooze_min * 60 * 1000000;
    for (int i = 0; i < SRC_COUNT; i++) {
        if ((s_prev_attention >> i) & 1u) {
            s_snooze_until[i] = until;
        }
    }
    ESP_LOGI(TAG, "acknowledged %d alarm(s); snoozed %u min",
             popcount16(s_prev_attention), (unsigned)d.alarm_snooze_min);
    ui_alerts_dismiss();
}

void ui_alerts_tick(void)
{
    capstan_alarms_t cfg;
    capstan_config_get_alarms(&cfg);
    const capstan_mode_t mode = capstan_config_get_mode();

    /*
     * A new list re-indexes the bits, so the previous masks and any snooze
     * deadlines mean nothing against it. Starting from zero re-raises
     * whatever is firing under the new list, which is what someone who just
     * pressed Save in the PWA wants to see.
     */
    if (memcmp(&cfg, &s_last_cfg, sizeof(cfg)) != 0) {
        const uint16_t cfg_bits = (uint16_t)~(1u << SRC_GAS);
        s_last_cfg = cfg;
        s_prev_active    &= (uint16_t)~cfg_bits;
        s_prev_attention &= (uint16_t)~cfg_bits;
        clear_snooze(cfg_bits);
        if (s_shown >= SRC_CFG_FIRST) {
            s_shown = -1;   /* re-picked below from whatever needs attention */
        }
        ESP_LOGI(TAG, "alarm list changed: %u configured", (unsigned)cfg.count);
    }

    const uint16_t active = evaluate(&cfg, mode);

    /* Cleared by a state change: drop the snooze with it, so the next time
     * this alarm fires it is raised at once, not on an old timer. */
    const uint16_t falling = s_prev_active & (uint16_t)~active;
    for (int i = 0; i < SRC_COUNT; i++) {
        if (((falling >> i) & 1u) && s_snooze_until[i]) {
            ESP_LOGI(TAG, "source %d cleared while snoozed -- timer reset", i);
        }
    }
    clear_snooze(falling);

    /* Stamped on the input's own edge, not on (re-)raising: an alarm raised
     * again after its snooze still opened when it opened. */
    const uint16_t rising_active = active & (uint16_t)~s_prev_active;
    for (int i = 0; i < SRC_COUNT; i++) {
        if ((rising_active >> i) & 1u) {
            s_raised_at[i] = capstan_model_time_valid() ? time(NULL) : 0;
        }
    }
    s_prev_active = active;

    /* Snooze deadlines that have passed are retired here; the alarm, if it
     * is still active, then needs attention again and re-raises below. */
    const int64_t now = esp_timer_get_time();
    uint16_t snoozed = 0;
    for (int i = 0; i < SRC_COUNT; i++) {
        if (!s_snooze_until[i]) {
            continue;
        }
        if (now >= s_snooze_until[i]) {
            s_snooze_until[i] = 0;
            ESP_LOGW(TAG, "snooze elapsed for source %d -- still active", i);
        } else {
            snoozed |= (uint16_t)(1u << i);
        }
    }

    apply_leds(active != 0);

    const uint16_t attention = active & (uint16_t)~snoozed;
    const uint16_t raised = attention & (uint16_t)~s_prev_attention;
    s_prev_attention = attention;


    if (raised) {
        int src = 0;
        while (!((raised >> src) & 1u)) { src++; }
        s_shown = src;
        ESP_LOGW(TAG, "alarm raised: %s",
                 src == SRC_GAS ? "gas" : cfg.items[src - SRC_CFG_FIRST].name);

        /* SETUP is left alone: the user is typing credentials off the glass,
         * and a dial without a network has no inputs to alarm on anyway. */
        const capstan_screen_t cur = ui_nav_current();
        if (cur != CAPSTAN_SCREEN_ALERT && cur != CAPSTAN_SCREEN_SETUP) {
            s_return = cur;
            paint(&cfg, mode, attention);
            ui_nav_goto(CAPSTAN_SCREEN_ALERT);
            return;
        }
    }

    if (ui_nav_current() != CAPSTAN_SCREEN_ALERT) {
        return;
    }

    /*
     * An alarm holds the overlay until it is acknowledged OR it clears. When
     * the one on the glass clears, move to the next alarm still needing
     * attention; when none is left, take the overlay down and put back
     * whatever was showing.
     */
    if (s_shown < 0 || !((attention >> s_shown) & 1u)) {
        if (!attention) {
            ESP_LOGI(TAG, "all alarms cleared -- closing overlay");
            ui_alerts_dismiss();
            return;
        }
        int src = 0;
        while (!((attention >> src) & 1u)) { src++; }
        s_shown = src;
    }
    paint(&cfg, mode, attention);
}

#else

void ui_alerts_tick(void) { }
void ui_alerts_dismiss(void) { }
void ui_alerts_acknowledge(void) { }
void ui_alerts_leds_now(void) { }

#endif
