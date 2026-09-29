/*
 * capstan_board for the EEZ Studio simulator.
 *
 * The UI never talks to the ring, the panel or the LED ring directly -- it
 * goes through capstan_board.h -- so standing in for this one header is what
 * lets the real ui_*.c run unchanged. Input is mapped onto the callbacks the
 * way board_encoder.c delivers the physical ring:
 *
 *   mouse wheel                 ring rotation, one detent per notch
 *   middle click / Enter        ring press
 *   hold either for 700 ms      long press -> back
 *   Esc or Backspace            back
 *   arrow keys, + and -         one detent
 *   left click                  the touchscreen, on screens whose policy
 *                               enables touch; otherwise the press, like
 *                               pushing the glass on a CrowPanel
 *
 * The LED ring has no pixels in the simulator, so colour changes are logged
 * to the console panel instead.
 *
 * Simulator only -- see docs/simulator.md.
 */
#ifdef EEZ_LVGL_SIMULATOR

#include <stdbool.h>
#include <stdint.h>

#include "lvgl.h"

#include "capstan_board.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "sdkconfig.h"
#include "sim.h"

static const char *TAG = "sim_board";

static const capstan_board_info_t s_info = {
    .name          = CONFIG_CAPSTAN_BOARD_NAME,
    .h_res         = CONFIG_CAPSTAN_LCD_H_RES,
    .v_res         = CONFIG_CAPSTAN_LCD_V_RES,
    .round         = true,
    .has_rgb_leds  = CONFIG_CAPSTAN_RGB_LED_COUNT > 0,
    .rgb_led_count = CONFIG_CAPSTAN_RGB_LED_COUNT,
};

static capstan_board_back_cb_t   s_back_cb;
static void                     *s_back_ctx;
static capstan_board_rotate_cb_t s_rotate_cb;
static void                     *s_rotate_ctx;
static capstan_board_press_cb_t  s_press_cb;
static void                     *s_press_ctx;

static int64_t s_last_input_us;
static bool    s_touch_enabled;

static lv_indev_t        *s_pointer, *s_wheel, *s_keys;
static lv_indev_read_cb_t s_pointer_read, s_wheel_read, s_keys_read;

static void note_input(void) { s_last_input_us = esp_timer_get_time(); }

/* ---- the capstan_board.h surface ----------------------------------- */

const capstan_board_info_t *capstan_board_info(void) { return &s_info; }
esp_err_t capstan_board_init(void) { note_input(); return ESP_OK; }

/* One thread: the lock is always free. */
bool capstan_board_lock(uint32_t timeout_ms) { (void)timeout_ms; return true; }
void capstan_board_unlock(void) { }

esp_err_t capstan_board_backlight_set(uint8_t percent)
{
    ESP_LOGI(TAG, "backlight %u%%", (unsigned)percent);
    return ESP_OK;
}

lv_indev_t *capstan_board_encoder_indev(void) { return s_wheel; }
lv_indev_t *capstan_board_touch_indev(void)   { return s_pointer; }

void capstan_board_touch_set_enabled(bool enabled) { s_touch_enabled = enabled; }
bool capstan_board_touch_is_enabled(void)          { return s_touch_enabled; }

bool capstan_board_touch_raw(int *x, int *y, uint32_t *seq)
{
    (void)x; (void)y; (void)seq;
    return false;   /* no raw controller to calibrate */
}

void capstan_board_set_back_callback(capstan_board_back_cb_t cb, void *ctx)
{ s_back_cb = cb; s_back_ctx = ctx; }
void capstan_board_set_rotate_callback(capstan_board_rotate_cb_t cb, void *ctx)
{ s_rotate_cb = cb; s_rotate_ctx = ctx; }
void capstan_board_set_press_callback(capstan_board_press_cb_t cb, void *ctx)
{ s_press_cb = cb; s_press_ctx = ctx; }

uint32_t capstan_board_ms_since_input(void)
{
    return (uint32_t)((esp_timer_get_time() - s_last_input_us) / 1000);
}

/* Called on every UI refresh with the same colour; log only a change. */
static uint32_t s_led_last = 0xFFFFFFFFu, s_led_last_r = 0xFFFFFFFFu;

void capstan_board_leds_set_all(uint8_t r, uint8_t g, uint8_t b)
{
    if (!s_info.has_rgb_leds) {
        return;
    }
    const uint32_t c = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
    if (c != s_led_last || s_led_last_r != c) {
        s_led_last = s_led_last_r = c;
        ESP_LOGI(TAG, "LED ring  #%06x", (unsigned)c);
    }
}

void capstan_board_leds_set_sides(uint8_t lr, uint8_t lg, uint8_t lb,
                                  uint8_t rr, uint8_t rg, uint8_t rb)
{
    if (!s_info.has_rgb_leds) {
        return;
    }
    const uint32_t l = ((uint32_t)lr << 16) | ((uint32_t)lg << 8) | lb;
    const uint32_t r = ((uint32_t)rr << 16) | ((uint32_t)rg << 8) | rb;
    if (l != s_led_last || r != s_led_last_r) {
        s_led_last = l;
        s_led_last_r = r;
        ESP_LOGI(TAG, "LED ring  left #%06x  right #%06x",
                 (unsigned)l, (unsigned)r);
    }
}

/* ---- the ring button ------------------------------------------------ */
/*
 * The same state machine as button_poll() in board_encoder.c: a release
 * under CONFIG_CAPSTAN_LONG_PRESS_MS is a press, and holding past it fires
 * back once, while still held, and swallows the release.
 */
typedef struct {
    bool    down;
    bool    long_fired;
    int64_t down_us;
} button_t;

static void button_poll(button_t *b, bool down)
{
    const int64_t now = esp_timer_get_time();

    if (down && !b->down) {
        b->down       = true;
        b->down_us    = now;
        b->long_fired = false;
        note_input();
    } else if (down && !b->long_fired &&
               (now - b->down_us) / 1000 >= CONFIG_CAPSTAN_LONG_PRESS_MS) {
        b->long_fired = true;
        note_input();
        if (s_back_cb) {
            s_back_cb(s_back_ctx);
        }
    } else if (!down && b->down) {
        b->down = false;
        note_input();
        if (!b->long_fired && s_press_cb) {
            s_press_cb(s_press_ctx);
        }
    }
}

static void rotate(int detents)
{
    note_input();
    if (s_rotate_cb) {
        s_rotate_cb(detents, s_rotate_ctx);
    }
}

/* ---- wrapped read callbacks ---------------------------------------- */

static void wheel_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    static button_t s_btn;

    s_wheel_read(indev, data);

    /* Delivered to the callback, never also to LVGL -- the rule
     * board_encoder.c follows so a detent is not counted twice. */
    if (data->enc_diff != 0) {
        rotate(data->enc_diff);
        data->enc_diff = 0;
    }
    button_poll(&s_btn, data->state == LV_INDEV_STATE_PRESSED);
    data->state = LV_INDEV_STATE_RELEASED;
}

static void pointer_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    static button_t s_btn;

    s_pointer_read(indev, data);
    const bool down = data->state == LV_INDEV_STATE_PRESSED;

    if (s_touch_enabled) {
        if (down) {
            note_input();
        }
        return;   /* a real touch, for the screens that take one */
    }

    /* Touch is off on this screen, so the glass is only a button. */
    button_poll(&s_btn, down);
    data->state = LV_INDEV_STATE_RELEASED;
}

static void keys_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    static button_t s_btn;
    static bool     s_was_down;

    s_keys_read(indev, data);
    const bool down = data->state == LV_INDEV_STATE_PRESSED;

    if (down && !s_was_down) {
        switch (data->key) {
        case LV_KEY_LEFT: case LV_KEY_UP:   case '-': rotate(-1); break;
        case LV_KEY_RIGHT: case LV_KEY_DOWN: case '+': case '=': rotate(+1); break;
        case LV_KEY_ESC: case LV_KEY_BACKSPACE:
            note_input();
            if (s_back_cb) {
                s_back_cb(s_back_ctx);
            }
            break;
        default: break;
        }
    }
    const bool enter = down && (data->key == LV_KEY_ENTER || data->key == ' ');
    button_poll(&s_btn, enter);

    s_was_down = down;
    data->state = LV_INDEV_STATE_RELEASED;
}

void sim_board_attach_inputs(void)
{
    /* Found by type rather than by the simulator main.c's globals, so a
     * change to that file cannot silently detach the ring. */
    for (lv_indev_t *i = lv_indev_get_next(NULL); i; i = lv_indev_get_next(i)) {
        switch (lv_indev_get_type(i)) {
        case LV_INDEV_TYPE_POINTER: if (!s_pointer) s_pointer = i; break;
        case LV_INDEV_TYPE_ENCODER: if (!s_wheel)   s_wheel   = i; break;
        case LV_INDEV_TYPE_KEYPAD:  if (!s_keys)    s_keys    = i; break;
        default: break;
        }
    }
    if (s_pointer) {
        s_pointer_read = lv_indev_get_read_cb(s_pointer);
        lv_indev_set_read_cb(s_pointer, pointer_read);
    }
    if (s_wheel) {
        s_wheel_read = lv_indev_get_read_cb(s_wheel);
        lv_indev_set_read_cb(s_wheel, wheel_read);
    }
    if (s_keys) {
        s_keys_read = lv_indev_get_read_cb(s_keys);
        lv_indev_set_read_cb(s_keys, keys_read);
    }
    note_input();
    ESP_LOGI(TAG, "%s %ux%u -- wheel turns the ring, middle-click/Enter "
                  "presses, hold or Esc for back",
             s_info.name, (unsigned)s_info.h_res, (unsigned)s_info.v_res);
}

#endif /* EEZ_LVGL_SIMULATOR */
