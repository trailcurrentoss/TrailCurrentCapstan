/*
 * Rotary ring: quadrature rotation, short press, long press.
 *
 * Rotation is decoded in hardware by PCNT rather than by GPIO interrupts.
 * The vendor demos all use an ISR per edge, which is fine when the CPU is
 * idle and drops counts when it is not -- and on this device the CPU is busy
 * exactly when the user is turning the ring, because LVGL is redrawing. PCNT
 * counts in hardware and cannot miss a step.
 *
 * The ring is presented to LVGL as a single LV_INDEV_TYPE_ENCODER. A LONG
 * press is deliberately NOT forwarded to LVGL -- it is consumed here and
 * delivered to the navigation layer as "back", matching the prototype, where
 * a long press returns to the menu from anywhere.
 *
 * ===================================================================
 * NO ABSOLUTE POSITION. EVER.
 * ===================================================================
 *
 * This driver emits DIRECTION ONLY -- "the user turned one detent left" or
 * "one detent right". It never exposes, stores, or reasons about an absolute
 * encoder count, and neither may anything above it.
 *
 * A rotary ring has no end stops. It spins forever in both directions, so
 * there is no such thing as "the encoder's value" -- only what the user just
 * did with it. Any code that keeps a running total and derives UI state from
 * it will produce dead travel: the vendor firmware on these boards keeps a
 * `counter`, so scrolling past the end of a range and back again does
 * nothing for as many detents as you overshot. The ring feels broken, and it
 * is a very annoying bug to live with.
 *
 * Concretely:
 *   - PCNT's hardware count is read and IMMEDIATELY ZEROED on every read.
 *     It is a delta accumulator between reads, not a position.
 *   - The sub-detent remainder is reset the instant direction reverses (see
 *     apply_direction_change), so a reversal takes effect on the very next
 *     detent rather than first cancelling out leftover travel.
 *   - Consumers receive lv_indev_data_t::enc_diff, which is relative by
 *     construction.
 *
 * CLAMPING IS FINE. ACCUMULATED OVERSHOOT IS NOT.
 *
 * Turning past the end of a bounded range should simply stop doing
 * anything -- that is correct and expected behaviour. What must never happen
 * is the overshoot being STORED, so that reversing requires winding back
 * through all of it before the UI responds. Two full turns past the end of a
 * list, then one detent back, must move the selection by exactly one.
 *
 * The rule that guarantees it, for every consumer: apply the delta to the
 * DISPLAYED value and clamp the result. Never keep a private counter and
 * derive the displayed value from it -- that is exactly how the overshoot
 * gets stored, and it is what the stock vendor firmware does.
 *
 *     sel = clamp(sel + diff, 0, n - 1);     // correct
 *     raw += diff;  sel = clamp(raw, 0, n-1) // WRONG: raw keeps the overshoot
 */

#include "driver/gpio.h"
#include "driver/pulse_cnt.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "board_internal.h"
#include "board_pins.h"
#include "capstan_board.h"

static const char *TAG = "board.enc";

/* Kconfig bools that are 'n' are UNDEFINED rather than 0. That is harmless
 * inside #if (an undefined symbol evaluates to 0 there) but breaks any
 * runtime expression, so normalise the ones used at runtime. */
#ifdef CONFIG_CAPSTAN_ENCODER_INVERT
#  define CAPSTAN_ENC_INVERT 1
#else
#  define CAPSTAN_ENC_INVERT 0
#endif

/* PCNT hardware limits. The counter is drained and zeroed on every LVGL read
 * (every few milliseconds), so these only need to cover one read interval --
 * they are set wide enough that a fast spin cannot saturate them. */
#define PCNT_HIGH_LIMIT  1000
#define PCNT_LOW_LIMIT  -1000

static pcnt_unit_handle_t s_pcnt;
static lv_indev_t      *s_indev;

/*
 * Sub-detent remainder. This is NOT a position -- it only carries the counts
 * left over when a partial detent has been turned, so that slow movement is
 * not silently discarded. It is bounded by one detent and is reset on any
 * direction change.
 */
static int              s_residue;
static int              s_last_dir;     /* -1, 0, +1 -- last emitted direction */

static bool             s_btn_down;
static int64_t          s_btn_down_us;
static bool             s_long_fired;
static bool             s_short_pending;

static capstan_board_back_cb_t s_back_cb;
static void                   *s_back_ctx;

static capstan_board_rotate_cb_t s_rotate_cb;
static void                     *s_rotate_ctx;

static capstan_board_press_cb_t s_press_cb;
static void                    *s_press_ctx;

static volatile int64_t s_last_input_us;

void board_note_input(void)
{
    s_last_input_us = esp_timer_get_time();
}

uint32_t capstan_board_ms_since_input(void)
{
    return (uint32_t)((esp_timer_get_time() - s_last_input_us) / 1000);
}

void capstan_board_set_back_callback(capstan_board_back_cb_t cb, void *ctx)
{
    s_back_cb  = cb;
    s_back_ctx = ctx;
}

void capstan_board_set_rotate_callback(capstan_board_rotate_cb_t cb, void *ctx)
{
    s_rotate_cb  = cb;
    s_rotate_ctx = ctx;
}

void capstan_board_set_press_callback(capstan_board_press_cb_t cb, void *ctx)
{
    s_press_cb  = cb;
    s_press_ctx = ctx;
}

static bool button_is_pressed(void)
{
    /* Active low on all three boards. */
    return gpio_get_level(BOARD_ENC_BTN) == 0;
}

/*
 * Poll the button. Called from the LVGL read callback, so it runs at LVGL's
 * input period (a few ms) -- fast enough for a human finger and slow enough
 * to act as its own debounce for the <=5 ms contact chatter the encoder
 * datasheet specifies.
 */
static void button_poll(void)
{
    const bool down = button_is_pressed();
    const int64_t now = esp_timer_get_time();

    if (down && !s_btn_down) {
        s_btn_down    = true;
        s_btn_down_us = now;
        s_long_fired  = false;
        board_note_input();
    } else if (down && s_btn_down && !s_long_fired) {
        if ((now - s_btn_down_us) / 1000 >= CONFIG_CAPSTAN_LONG_PRESS_MS) {
            s_long_fired = true;
            board_note_input();
            ESP_LOGD(TAG, "long press -> back");
            if (s_back_cb) {
                s_back_cb(s_back_ctx);
            }
        }
    } else if (!down && s_btn_down) {
        s_btn_down = false;
        board_note_input();
        /* A release that already fired the long press is consumed -- LVGL
         * must not also see it as a confirm. */
        if (!s_long_fired) {
            s_short_pending = true;
        }
    }
}

static void encoder_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    (void)indev;

#if CONFIG_CAPSTAN_ENCODER_DEBUG
    /* Heartbeat: confirms LVGL is actually polling this indev. If this never
     * appears, nothing below it can possibly run. */
    static uint32_t s_reads;
    if ((s_reads++ % 200) == 0) {
        ESP_LOGI(TAG, "read_cb alive (%lu polls)", (unsigned long)s_reads);
    }
#endif

    int raw = 0;
    if (s_pcnt && pcnt_unit_get_count(s_pcnt, &raw) == ESP_OK && raw != 0) {
        /* Zero it immediately: the hardware counter is a delta between
         * reads, never a position. */
        pcnt_unit_clear_count(s_pcnt);

        /*
         * Direction reversal discards the remainder. Without this, a user
         * who turns half a detent one way and then back the other way has to
         * "pay back" that half detent before anything moves -- the same
         * dead-travel feel we are avoiding, just at a smaller scale.
         */
        const int dir = (raw > 0) ? 1 : -1;
        if (s_last_dir != 0 && dir != s_last_dir) {
            s_residue = 0;
        }
        s_last_dir = dir;

        s_residue += raw;

        /*
         * Truncate toward zero, which for a same-signed residue is what we
         * want: whole detents are emitted, the partial remainder is carried.
         */
        const int per = CONFIG_CAPSTAN_ENCODER_STEPS_PER_DETENT;
        int detents = s_residue / per;
        s_residue  -= detents * per;

#if CONFIG_CAPSTAN_ENCODER_DEBUG
        /* Log the RAW count, not just completed detents. If this line never
         * appears while the ring turns, PCNT is not counting and the fault
         * is the pin map or the channel config -- not LVGL. */
        ESP_LOGI(TAG, "raw=%+d residue=%+d", raw, s_residue);
#endif

        if (detents != 0) {
#if CAPSTAN_ENC_INVERT
            detents = -detents;
#endif
            /*
             * Deliver to the UI layer if it asked for rotation, otherwise
             * fall back to LVGL's own encoder handling. Never both -- that
             * would count every detent twice.
             */
            if (s_rotate_cb) {
                s_rotate_cb(detents, s_rotate_ctx);
            } else {
                data->enc_diff = (int16_t)detents;
            }
            board_note_input();
#if CONFIG_CAPSTAN_ENCODER_DEBUG
            ESP_LOGI(TAG, "  -> detents=%+d delivered to LVGL", detents);
#endif
        }
    }

    button_poll();

    /*
     * LVGL samples state each read. A short press is reported as PRESSED for
     * exactly one read and then released, which LVGL turns into a clean
     * LV_KEY_ENTER on the focused widget.
     */
    if (s_short_pending) {
        s_short_pending = false;
        if (s_press_cb) {
            /* Consumed here -- do not also hand LVGL an ENTER. */
            s_press_cb(s_press_ctx);
            data->state = LV_INDEV_STATE_RELEASED;
        } else {
            data->state = LV_INDEV_STATE_PRESSED;
            data->continue_reading = true;  /* release on the next read */
        }
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

esp_err_t board_encoder_init(lv_indev_t **out_indev)
{
    board_note_input();

    /* ---- button ---- */
    gpio_config_t btn = {
        .mode         = GPIO_MODE_INPUT,
        .pin_bit_mask = 1ULL << BOARD_ENC_BTN,
        .pull_up_en   = BOARD_ENC_BTN_PULLUP ? GPIO_PULLUP_ENABLE
                                             : GPIO_PULLUP_DISABLE,
    };
    ESP_RETURN_ON_ERROR(gpio_config(&btn), TAG, "button gpio failed");

    /* ---- quadrature via PCNT ---- */
    pcnt_unit_config_t unit_cfg = {
        .high_limit = PCNT_HIGH_LIMIT,
        .low_limit  = PCNT_LOW_LIMIT,
    };
    ESP_RETURN_ON_ERROR(pcnt_new_unit(&unit_cfg, &s_pcnt), TAG, "pcnt unit failed");

    /*
     * Glitch filter. The EC3501 datasheet specifies contact chatter up to
     * 5 ms, but PCNT's filter is capped at 1023 APB cycles (~12.8 us at
     * 80 MHz). That is enough to kill electrical noise; the mechanical
     * bounce is absorbed by full quadrature decoding, which is immune to it
     * because a bouncing edge oscillates between two adjacent Gray-code
     * states and nets to zero.
     */
    pcnt_glitch_filter_config_t filt = { .max_glitch_ns = 1000 };
    ESP_RETURN_ON_ERROR(pcnt_unit_set_glitch_filter(s_pcnt, &filt),
                        TAG, "glitch filter failed");

    pcnt_chan_config_t ch_a_cfg = {
        .edge_gpio_num  = BOARD_ENC_A,
        .level_gpio_num = BOARD_ENC_B,
    };
    pcnt_chan_config_t ch_b_cfg = {
        .edge_gpio_num  = BOARD_ENC_B,
        .level_gpio_num = BOARD_ENC_A,
    };
    pcnt_channel_handle_t ch_a = NULL, ch_b = NULL;
    ESP_RETURN_ON_ERROR(pcnt_new_channel(s_pcnt, &ch_a_cfg, &ch_a), TAG, "ch a failed");
    ESP_RETURN_ON_ERROR(pcnt_new_channel(s_pcnt, &ch_b_cfg, &ch_b), TAG, "ch b failed");

    /* Both edges of both phases => 4 counts per quadrature cycle. */
    ESP_RETURN_ON_ERROR(pcnt_channel_set_edge_action(ch_a,
        PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE),
        TAG, "ch a edge failed");
    ESP_RETURN_ON_ERROR(pcnt_channel_set_level_action(ch_a,
        PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE),
        TAG, "ch a level failed");
    ESP_RETURN_ON_ERROR(pcnt_channel_set_edge_action(ch_b,
        PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE),
        TAG, "ch b edge failed");
    ESP_RETURN_ON_ERROR(pcnt_channel_set_level_action(ch_b,
        PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE),
        TAG, "ch b level failed");

#if BOARD_ENC_PULLUP
    /* MaTouch relies on internal pull-ups; the CrowPanels have external
     * ones and enabling these there would weaken the edges. */
    gpio_set_pull_mode(BOARD_ENC_A, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(BOARD_ENC_B, GPIO_PULLUP_ONLY);
#endif

    ESP_RETURN_ON_ERROR(pcnt_unit_enable(s_pcnt),       TAG, "pcnt enable failed");
    ESP_RETURN_ON_ERROR(pcnt_unit_clear_count(s_pcnt),  TAG, "pcnt clear failed");
    ESP_RETURN_ON_ERROR(pcnt_unit_start(s_pcnt),        TAG, "pcnt start failed");

    /* ---- LVGL input device ---- */
    s_indev = lv_indev_create();
    ESP_RETURN_ON_FALSE(s_indev, ESP_FAIL, TAG, "lv_indev_create failed");
    lv_indev_set_type(s_indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(s_indev, encoder_read_cb);

    ESP_LOGI(TAG, "encoder A=%d B=%d btn=%d, %d counts/detent%s",
             BOARD_ENC_A, BOARD_ENC_B, BOARD_ENC_BTN,
             CONFIG_CAPSTAN_ENCODER_STEPS_PER_DETENT,
             CAPSTAN_ENC_INVERT ? ", inverted" : "");

    *out_indev = s_indev;
    return ESP_OK;
}

lv_indev_t *capstan_board_encoder_indev(void) { return s_indev; }
