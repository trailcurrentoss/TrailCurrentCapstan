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
 */

#include <string.h>

#include "driver/gpio.h"
#include "driver/rmt_encoder.h"
#include "driver/rmt_tx.h"
#include "esp_check.h"
#include "esp_log.h"

#include "board_internal.h"
#include "board_pins.h"
#include "capstan_board.h"

#if BOARD_HAS_WS2812 && defined(CONFIG_CAPSTAN_RGB_LEDS_ENABLED)

static const char *TAG = "board.leds";

/* 10 MHz: one tick is 100 ns. WS2812: a 0 is ~0.3 us high then ~0.9 us low,
 * a 1 is ~0.9 us high then ~0.3 us low; >50 us low latches the frame. */
#define LED_RMT_HZ   10000000

static rmt_channel_handle_t s_chan;
static rmt_encoder_handle_t s_enc;
static uint8_t              s_grb[BOARD_WS2812_COUNT * 3];
static uint8_t              s_sent[BOARD_WS2812_COUNT * 3];
static bool                 s_ready;
static bool                 s_sent_valid;               /* force first write */

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
    for (int i = 0; i < BOARD_WS2812_COUNT; i++) {
        put(i, r, g, b);
    }
    send();
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
    for (int i = 0; i < BOARD_WS2812_COUNT; i++) {
        if (side[i] < 0) {
            put(i, lr, lg, lb);
        } else if (side[i] > 0) {
            put(i, rr, rg, rb);
        } else {
            put(i, 0, 0, 0);
        }
    }
    send();
}

#else

esp_err_t board_leds_init(void) { return ESP_OK; }
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
