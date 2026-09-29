/*
 * Board bring-up and the public capstan_board API.
 */

#include "esp_check.h"
#include "esp_log.h"
#include "driver/ledc.h"
#include "esp_lvgl_port.h"

#include "board_internal.h"
#include "board_pins.h"
#include "capstan_board.h"

static const char *TAG = "board";

/*
 * The LED-ring capability is declared in two places -- Kconfig (so it can be
 * reasoned about at configure time and shown in menuconfig) and board_pins.h
 * (the hardware truth). They must agree, so assert it rather than trusting
 * whoever edits one of them next.
 */
#ifdef CONFIG_CAPSTAN_HAS_RGB_LEDS
#  define CAPSTAN_HAS_RGB 1
#else
#  define CAPSTAN_HAS_RGB 0
#endif

_Static_assert(CAPSTAN_HAS_RGB == BOARD_HAS_WS2812,
               "CONFIG_CAPSTAN_HAS_RGB_LEDS disagrees with BOARD_HAS_WS2812 "
               "in board_pins.h -- one of them is wrong for this board");
_Static_assert(CONFIG_CAPSTAN_RGB_LED_COUNT == BOARD_WS2812_COUNT,
               "CONFIG_CAPSTAN_RGB_LED_COUNT disagrees with "
               "BOARD_WS2812_COUNT in board_pins.h");

/* The runtime flag folds together "the board has a ring" and "this build
 * uses it", so callers need only one test. */
#if CAPSTAN_HAS_RGB && defined(CONFIG_CAPSTAN_RGB_LEDS_ENABLED)
#  define CAPSTAN_RGB_ACTIVE 1
#else
#  define CAPSTAN_RGB_ACTIVE 0
#endif

static const capstan_board_info_t s_info = {
    .name  = CONFIG_CAPSTAN_BOARD_NAME,
    .h_res = CONFIG_CAPSTAN_LCD_H_RES,
    .v_res = CONFIG_CAPSTAN_LCD_V_RES,
    .round = true,
    .has_rgb_leds  = CAPSTAN_RGB_ACTIVE ? true : false,
    .rgb_led_count = CAPSTAN_RGB_ACTIVE ? BOARD_WS2812_COUNT : 0,
};

const capstan_board_info_t *capstan_board_info(void) { return &s_info; }

/* ------------------------------------------------------------------ */
/* Backlight                                                          */
/* ------------------------------------------------------------------ */
#define BL_TIMER   LEDC_TIMER_0
#define BL_CHANNEL LEDC_CHANNEL_0
#define BL_MODE    LEDC_LOW_SPEED_MODE

#ifndef BOARD_BL_PWM_HZ
#  define BOARD_BL_PWM_HZ   5000
#endif
#ifndef BOARD_BL_PWM_BITS
#  define BOARD_BL_PWM_BITS 8
#endif

static esp_err_t backlight_init(void)
{
    ledc_timer_config_t timer = {
        .speed_mode      = BL_MODE,
        .timer_num       = BL_TIMER,
        .duty_resolution = BOARD_BL_PWM_BITS,
        .freq_hz         = BOARD_BL_PWM_HZ,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    ESP_RETURN_ON_ERROR(ledc_timer_config(&timer), TAG, "bl timer failed");

    ledc_channel_config_t ch = {
        .gpio_num   = BOARD_BL_GPIO,
        .speed_mode = BL_MODE,
        .channel    = BL_CHANNEL,
        .timer_sel  = BL_TIMER,
        .duty       = 0,          /* dark until the first frame is drawn */
        .hpoint     = 0,
    };
    ESP_RETURN_ON_ERROR(ledc_channel_config(&ch), TAG, "bl channel failed");
    return ESP_OK;
}

esp_err_t capstan_board_backlight_set(uint8_t percent)
{
    if (percent > 100) {
        percent = 100;
    }
    const uint32_t max  = (1u << BOARD_BL_PWM_BITS) - 1u;
    const uint32_t duty = (max * percent) / 100u;
    ESP_RETURN_ON_ERROR(ledc_set_duty(BL_MODE, BL_CHANNEL, duty),
                        TAG, "set duty failed");
    return ledc_update_duty(BL_MODE, BL_CHANNEL);
}

/* ------------------------------------------------------------------ */
/* LVGL lock — thin wrappers so no caller needs esp_lvgl_port directly */
/* ------------------------------------------------------------------ */
bool capstan_board_lock(uint32_t timeout_ms)
{
    return lvgl_port_lock(timeout_ms);
}

void capstan_board_unlock(void)
{
    lvgl_port_unlock();
}

/* ------------------------------------------------------------------ */
esp_err_t capstan_board_init(void)
{
    ESP_LOGI(TAG, "Capstan board '%s' %ux%u, LED ring: %s",
             s_info.name, s_info.h_res, s_info.v_res,
             s_info.has_rgb_leds ? "yes" : "no");

    /* The LED ring (board_leds.c). Not fatal: a dial whose ring fails to
     * start is still a working dial. */
    if (board_leds_init() != ESP_OK) {
        ESP_LOGW(TAG, "LED ring init failed -- continuing without it");
    }

    ESP_RETURN_ON_ERROR(backlight_init(), TAG, "backlight init failed");

    /*
     * LVGL's task runs at priority 4 with a 6 KB stack. It must sit below the
     * MQTT dispatch task so a burst of broker traffic cannot starve the UI,
     * and above the idle-priority housekeeping. Pin it to core 1 to keep the
     * Wi-Fi/LWIP work on core 0 from colliding with redraws.
     */
    const lvgl_port_cfg_t lv_cfg = {
        .task_priority   = 4,
        .task_stack      = 6144,
        .task_affinity   = 1,
        .task_max_sleep_ms = 500,
        .timer_period_ms   = 5,
    };
    ESP_RETURN_ON_ERROR(lvgl_port_init(&lv_cfg), TAG, "lvgl_port_init failed");

    lv_display_t *disp = NULL;
    ESP_RETURN_ON_ERROR(board_display_init(&disp), TAG, "display init failed");

#if CONFIG_CAPSTAN_TOUCH_ENABLED
    lv_indev_t *touch = NULL;
    esp_err_t err = board_touch_init(disp, &touch);
    if (err != ESP_OK) {
        /* A dead touch controller should not stop the device booting -- the
         * ring alone can drive the whole UI, and a unit with a failed touch
         * panel is still useful. Log loudly and continue. */
        ESP_LOGE(TAG, "touch init failed (%s) -- continuing with ring only",
                 esp_err_to_name(err));
    }
#else
    /* Not started: nothing in the UI is touchable (see
     * CONFIG_CAPSTAN_TOUCH_ENABLED). capstan_board_touch_set_enabled() and
     * the other touch calls already cope with a touch that never came up. */
    ESP_LOGI(TAG, "touch disabled in config -- ring only");
#endif

    lv_indev_t *enc = NULL;
    ESP_RETURN_ON_ERROR(board_encoder_init(&enc), TAG, "encoder init failed");

    ESP_LOGI(TAG, "board ready");
    return ESP_OK;
}
