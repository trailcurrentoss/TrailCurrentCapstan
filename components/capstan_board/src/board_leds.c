/*
 * The addressable LED ring (WS2812) on the two CrowPanels.
 *
 * Driven by the RMT peripheral with ESP-IDF's own bytes encoder: WS2812 bits
 * are one high/low pulse pair each, which is exactly what a bytes encoder
 * emits, so no extra component is needed. Colour order on the wire is GRB.
 *
 * Every write is scaled by CONFIG_CAPSTAN_RGB_LEDS_MAX_BRIGHTNESS -- these
 * rings are uncomfortably bright on a wall at night at full scale.
 *
 * On a board without a ring (MaTouch), or with CONFIG_CAPSTAN_RGB_LEDS_ENABLED
 * off, every call here is a quiet no-op, so callers never need to check.
 *
 * TURN FEEDBACK
 *
 * Every detent of the ring darkens one LED for a moment: the same colour the
 * UI set, at a fraction of its brightness, then faded back. Nothing changes
 * hue, so the ring keeps meaning what the screen made it mean -- on Climate
 * a turn toward warmer dips a red LED and a turn toward cooler dips a blue
 * one. A dark LED has nothing to darken, so a dark ring stays dark.
 *
 * Which LED is chosen by a marker that starts at 12 o'clock and travels with
 * the turn, at the ring's own rate: clockwise goes down the right side,
 * counter-clockwise down the left. It parks a third of a lap down rather
 * than carrying on round the bottom and up the far side, and it starts over
 * from the top on a reversal or after a pause -- so a clockwise turn only
 * ever touches the right half and a counter-clockwise turn the left.
 *
 * The marker only picks an LED and is clamped as it moves, so it cannot
 * store overshoot -- see "NO ABSOLUTE POSITION" in board_encoder.c.
 */

#include <stdlib.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/rmt_encoder.h"
#include "driver/rmt_tx.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "board_internal.h"
#include "board_pins.h"
#include "capstan_board.h"

#if BOARD_HAS_WS2812 && defined(CONFIG_CAPSTAN_RGB_LEDS_ENABLED)

static const char *TAG = "board.leds";

/* 10 MHz: one tick is 100 ns. WS2812: a 0 is ~0.3 us high then ~0.9 us low,
 * a 1 is ~0.9 us high then ~0.3 us low; >50 us low latches the frame. */
#define LED_RMT_HZ   10000000

/* The turn dip: held dark, then faded back up to the colour as set. Stepped
 * by a timer that only runs while a dip is showing. */
#define FLASH_HOLD_US    120000
#define FLASH_FADE_US    120000
#define FLASH_STEP_US    20000
/* A gap this long between detents is a new turn; the marker restarts. */
#define TURN_IDLE_US     1500000
/* Brightness left at the bottom of the dip, percent of the colour as set. */
#define FLASH_DIM_PCT    20

static rmt_channel_handle_t s_chan;
static rmt_encoder_handle_t s_enc;
static uint8_t              s_base[BOARD_WS2812_COUNT][3];  /* RGB, as set */
static uint8_t              s_grb[BOARD_WS2812_COUNT * 3];
static uint8_t              s_sent[BOARD_WS2812_COUNT * 3];
static bool                 s_ready;
static bool                 s_sent_valid;               /* force first write */

/* Colours are set from the UI and the flash is faded from the timer task. */
static SemaphoreHandle_t    s_lock;
static esp_timer_handle_t   s_flash_timer;
static int                  s_flash_led = -1;           /* -1: no flash */
static int64_t              s_flash_us;                 /* when it started */
static int                  s_marker;                   /* detents from top */
static int                  s_turn_dir;                 /* -1, 0, +1 */
static int64_t              s_turn_us;                  /* last detent */

/* Scale one channel by the brightness ceiling. */
static uint8_t scale(uint8_t v)
{
    return (uint8_t)(v * (unsigned)CONFIG_CAPSTAN_RGB_LEDS_MAX_BRIGHTNESS / 100u);
}

static void put(int i, uint8_t r, uint8_t g, uint8_t b)
{
    s_grb[i * 3 + 0] = scale(g);   /* GRB on the wire */
    s_grb[i * 3 + 1] = scale(r);
    s_grb[i * 3 + 2] = scale(b);
}

/* One channel of the base colour, darkened by `level` (0-255): untouched at
 * 0, down to FLASH_DIM_PCT of itself at 255. Every channel takes the same
 * factor, so the hue does not move. */
static uint8_t dim(uint8_t v, int level)
{
    return (uint8_t)(v - v * level * (100 - FLASH_DIM_PCT) / (255 * 100));
}

/* Send s_grb unless it is what the ring already shows -- callers repeat
 * themselves every refresh. */
static void send(void)
{
    if (s_sent_valid && memcmp(s_grb, s_sent, sizeof(s_grb)) == 0) {
        return;
    }
    const rmt_transmit_config_t tx = { .loop_count = 0 };
    if (rmt_transmit(s_chan, s_enc, s_grb, sizeof(s_grb), &tx) != ESP_OK) {
        ESP_LOGW(TAG, "LED write failed");
        s_sent_valid = false;              /* retry on the next call */
        return;
    }
    memcpy(s_sent, s_grb, sizeof(s_grb));
    s_sent_valid = true;
}

/* How deep the dip is now, 0-255; 0 once it has run its course. */
static int flash_level(void)
{
    if (s_flash_led < 0) {
        return 0;
    }
    const int64_t age = esp_timer_get_time() - s_flash_us;
    if (age < FLASH_HOLD_US) {
        return 255;
    }
    if (age >= FLASH_HOLD_US + FLASH_FADE_US) {
        return 0;
    }
    return (int)(255 - (age - FLASH_HOLD_US) * 255 / FLASH_FADE_US);
}

/* Base colours with the flash on top, sent. Call with s_lock held.
 * Returns false once there is no flash left to animate. */
static bool render(void)
{
    const int level = flash_level();
    if (level == 0) {
        s_flash_led = -1;
    }
    for (int i = 0; i < BOARD_WS2812_COUNT; i++) {
        const int l = (i == s_flash_led) ? level : 0;
        put(i, dim(s_base[i][0], l), dim(s_base[i][1], l),
            dim(s_base[i][2], l));
    }
    send();
    return level != 0;
}

static void flash_timer_cb(void *arg)
{
    (void)arg;
    xSemaphoreTake(s_lock, portMAX_DELAY);
    if (!render()) {
        esp_timer_stop(s_flash_timer);
    }
    xSemaphoreGive(s_lock);
}

static void set_base(int i, uint8_t r, uint8_t g, uint8_t b)
{
    s_base[i][0] = r;
    s_base[i][1] = g;
    s_base[i][2] = b;
}

/* The LED nearest the marker. A marker exactly between two LEDs goes to the
 * one in the direction of travel, so the tie never reads as a step back. */
static int marker_led(int dir)
{
    static const int16_t angle[BOARD_WS2812_COUNT] = BOARD_WS2812_ANGLE;
    const int at = (s_marker * 360 / BOARD_ENC_DETENTS_PER_REV + dir + 360) % 360;
    int best = 0, best_d = 360;
    for (int i = 0; i < BOARD_WS2812_COUNT; i++) {
        int d = abs(angle[i] - at);
        if (d > 180) {
            d = 360 - d;
        }
        if (d < best_d) {
            best_d = d;
            best = i;
        }
    }
    return best;
}

esp_err_t board_leds_init(void)
{
#if BOARD_WS2812_EN >= 0
    /* The 1.46" gates the ring's supply; it stays dark without this. */
    const gpio_config_t en = {
        .pin_bit_mask = 1ULL << BOARD_WS2812_EN,
        .mode = GPIO_MODE_OUTPUT,
    };
    ESP_RETURN_ON_ERROR(gpio_config(&en), TAG, "enable pin");
    gpio_set_level(BOARD_WS2812_EN, 1);
#endif

    const rmt_tx_channel_config_t ch = {
        .gpio_num = BOARD_WS2812_GPIO,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = LED_RMT_HZ,
        .mem_block_symbols = 48,
        .trans_queue_depth = 2,
    };
    ESP_RETURN_ON_ERROR(rmt_new_tx_channel(&ch, &s_chan), TAG, "rmt channel");

    const rmt_bytes_encoder_config_t enc = {
        .bit0 = { .level0 = 1, .duration0 = 3, .level1 = 0, .duration1 = 9 },
        .bit1 = { .level0 = 1, .duration0 = 9, .level1 = 0, .duration1 = 3 },
        .flags.msb_first = 1,
    };
    ESP_RETURN_ON_ERROR(rmt_new_bytes_encoder(&enc, &s_enc), TAG, "encoder");
    ESP_RETURN_ON_ERROR(rmt_enable(s_chan), TAG, "rmt enable");

    s_lock = xSemaphoreCreateMutex();
    ESP_RETURN_ON_FALSE(s_lock, ESP_ERR_NO_MEM, TAG, "lock");
    const esp_timer_create_args_t flash = {
        .callback = flash_timer_cb,
        .name = "led_flash",
    };
    ESP_RETURN_ON_ERROR(esp_timer_create(&flash, &s_flash_timer), TAG,
                        "flash timer");

    s_ready = true;
    ESP_LOGI(TAG, "%d LEDs on GPIO%d, max %d%%", BOARD_WS2812_COUNT,
             BOARD_WS2812_GPIO, CONFIG_CAPSTAN_RGB_LEDS_MAX_BRIGHTNESS);
    capstan_board_leds_set_all(0, 0, 0);   /* known state, not power-on noise */
    return ESP_OK;
}

void capstan_board_leds_set_all(uint8_t r, uint8_t g, uint8_t b)
{
    if (!s_ready) {
        return;
    }
    xSemaphoreTake(s_lock, portMAX_DELAY);
    for (int i = 0; i < BOARD_WS2812_COUNT; i++) {
        set_base(i, r, g, b);
    }
    render();
    xSemaphoreGive(s_lock);
}

/*
 * Left half one colour, right half another, as seen from the front. Which
 * LED is on which side is board geometry, so it lives in board_pins.h
 * (BOARD_WS2812_SIDE); an LED on the vertical centre line belongs to
 * neither half and stays dark.
 */
void capstan_board_leds_set_sides(uint8_t lr, uint8_t lg, uint8_t lb,
                                  uint8_t rr, uint8_t rg, uint8_t rb)
{
    if (!s_ready) {
        return;
    }
    static const int8_t side[BOARD_WS2812_COUNT] = BOARD_WS2812_SIDE;
    xSemaphoreTake(s_lock, portMAX_DELAY);
    for (int i = 0; i < BOARD_WS2812_COUNT; i++) {
        if (side[i] < 0) {
            set_base(i, lr, lg, lb);
        } else if (side[i] > 0) {
            set_base(i, rr, rg, rb);
        } else {
            set_base(i, 0, 0, 0);
        }
    }
    render();
    xSemaphoreGive(s_lock);
}

void board_leds_turn(int detents)
{
    if (!s_ready || detents == 0) {
        return;
    }
    const int64_t now = esp_timer_get_time();
    xSemaphoreTake(s_lock, portMAX_DELAY);
    const int dir = detents > 0 ? 1 : -1;
    if (dir != s_turn_dir || now - s_turn_us > TURN_IDLE_US) {
        s_marker = 0;                      /* a new turn starts at the top */
    }
    s_turn_dir = dir;
    s_turn_us = now;
    /* Applied and clamped, never wrapped: it parks on its own side. */
    const int park = BOARD_ENC_DETENTS_PER_REV / 3;
    s_marker += detents;
    if (s_marker > park) {
        s_marker = park;
    } else if (s_marker < -park) {
        s_marker = -park;
    }
    s_flash_led = marker_led(dir);
    s_flash_us = now;
    render();
    esp_timer_stop(s_flash_timer);         /* not running is fine */
    esp_timer_start_periodic(s_flash_timer, FLASH_STEP_US);
    xSemaphoreGive(s_lock);
}

#else

esp_err_t board_leds_init(void) { return ESP_OK; }
void board_leds_turn(int detents) { (void)detents; }
void capstan_board_leds_set_all(uint8_t r, uint8_t g, uint8_t b)
{
    (void)r; (void)g; (void)b;
}

void capstan_board_leds_set_sides(uint8_t lr, uint8_t lg, uint8_t lb,
                                  uint8_t rr, uint8_t rg, uint8_t rb)
{
    (void)lr; (void)lg; (void)lb; (void)rr; (void)rg; (void)rb;
}

#endif
