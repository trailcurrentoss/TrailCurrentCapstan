/*
 * capstan_board — the only board-aware code in the firmware.
 *
 * Three panels sit behind this header: a 480x480 ST7701S on a 16-bit RGB
 * parallel bus, a 360x360 JD9855, and a 240x240 GC9A01 on SPI. Each has its
 * own implementation file and its own pin map, cited to the vendor schematic.
 * Everything above this header -- the UI, the data model, MQTT, Wi-Fi -- is
 * board-agnostic and must stay that way.
 *
 * If you find yourself adding an #ifdef CONFIG_CAPSTAN_BOARD_* outside
 * components/capstan_board, the abstraction is missing something. Add it here
 * instead.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Static description of the panel this firmware was built for. */
typedef struct {
    const char *name;      /**< Short board id, e.g. "matouch21". */
    uint16_t    h_res;     /**< Horizontal resolution in pixels. */
    uint16_t    v_res;     /**< Vertical resolution in pixels. */
    bool        round;     /**< True for every board so far; kept explicit
                                because the layouts depend on it. */

    /**
     * Addressable RGB LED ring around the display.
     *
     * Fitted on both Elecrow CrowPanels (5 LEDs on the 1.28", 8 on the
     * 1.46") and absent on the Makerfabs MaTouch. A screen that wants to
     * use the ring must check has_rgb_leds and degrade gracefully when it
     * is false -- the same UI has to work on a board with no LEDs at all.
     *
     * has_rgb_leds reports the HARDWARE. It is false when the board has no
     * ring, and also false when the build has switched the feature off, so
     * callers only need the one test.
     */
    bool        has_rgb_leds;
    uint16_t    rgb_led_count;   /**< 0 when has_rgb_leds is false. */
} capstan_board_info_t;

const capstan_board_info_t *capstan_board_info(void);

/**
 * Bring up the panel, the touch controller, the rotary encoder and LVGL.
 *
 * On return LVGL is running on its own task and the display is cleared but
 * still dark -- call capstan_board_backlight_set() once the first screen has
 * been drawn, so the user never sees an unpainted panel.
 *
 * Must be called after nvs_flash_init().
 */
esp_err_t capstan_board_init(void);

/**
 * Take the LVGL mutex. Every call into an lv_* function from a task other
 * than LVGL's own must be wrapped in this.
 *
 * Callbacks that LVGL itself invokes -- lv_timer callbacks, event handlers --
 * already hold the lock. Taking it again there deadlocks.
 *
 * @param timeout_ms  0 waits forever.
 * @return true if the lock was taken.
 */
bool capstan_board_lock(uint32_t timeout_ms);
void capstan_board_unlock(void);

/** Backlight duty, 0-100. Panels differ in whether this is PWM or on/off;
 *  the implementation hides that, and a board without PWM rounds to on/off. */
esp_err_t capstan_board_backlight_set(uint8_t percent);

/**
 * The encoder input device, for attaching lv_group_t objects to.
 *
 * The ring is presented to LVGL as a single LV_INDEV_TYPE_ENCODER: rotation
 * becomes enc_diff, a short press becomes LV_KEY_ENTER. A long press does NOT
 * reach LVGL -- it is consumed by the driver and delivered to the navigation
 * layer as a "back" request, which matches the prototype.
 */
lv_indev_t *capstan_board_encoder_indev(void);

/** The capacitive touch input device (LV_INDEV_TYPE_POINTER). */
lv_indev_t *capstan_board_touch_indev(void);

/**
 * Enable or disable touch input.
 *
 * TOUCH IS OFF BY DEFAULT, AND THAT IS DELIBERATE.
 *
 * The ring is the primary input: rotate to select, press to confirm, long
 * press to go back. That covers all navigation and nearly all control, and
 * on these units the whole panel is the button -- pressing the ring means
 * pressing the screen. With touch live, one physical press produces both an
 * encoder ENTER and a touch event, and the two fight: a press lands on
 * whatever happens to be under the user's finger rather than on what the
 * ring had focused.
 *
 * So touch stays off unless a screen genuinely needs pointer input -- in
 * practice the on-screen keyboard, and any list where hitting a target
 * directly beats rotating to it.
 *
 * A screen that enables touch is responsible for disabling it again on the
 * way out. Prefer capstan_board_touch_scope() over bare calls.
 *
 * Safe to call before touch has been initialised, and a no-op if the touch
 * controller failed to start.
 */
void capstan_board_touch_set_enabled(bool enabled);
bool capstan_board_touch_is_enabled(void);

/**
 * Set every LED in the ring to one colour (0-255 per channel, scaled by
 * CONFIG_CAPSTAN_RGB_LEDS_MAX_BRIGHTNESS). A no-op on a board without a ring
 * or with the ring disabled, so callers need not check has_rgb_leds.
 * Repeating the current colour costs nothing -- it is not re-sent.
 */
void capstan_board_leds_set_all(uint8_t r, uint8_t g, uint8_t b);

/**
 * RAW, uncalibrated coordinates from the most recent press, plus a
 * sequence number that increments once per PHYSICAL contact.
 *
 * Only the calibration screen should use this -- calibrating through the
 * transform being calibrated is circular. Everything else receives
 * corrected coordinates through LVGL automatically.
 *
 * The value is captured inside the same read LVGL consumed; this does NOT
 * issue its own I2C transaction. An earlier version did, and racing
 * esp_lvgl_port's touch task over one device paired stale samples with the
 * wrong targets and produced a fit that made accuracy worse.
 *
 * Use `seq` to count distinct presses rather than event callbacks -- LVGL
 * may report a single finger-down many times.
 *
 * Returns false if nothing has been touched yet.
 */
bool capstan_board_touch_raw(int *x, int *y, uint32_t *seq);

/** Registered by the navigation layer to receive ring long-presses. */
typedef void (*capstan_board_back_cb_t)(void *ctx);
void capstan_board_set_back_callback(capstan_board_back_cb_t cb, void *ctx);

/**
 * Receive ring rotation directly, as a signed number of detents.
 *
 * WHY NOT JUST USE LVGL'S ENCODER HANDLING
 *
 * LVGL only delivers LV_EVENT_ROTARY to a focused widget whose CLASS is
 * declared editable (lv_indev.c: `if (lv_obj_is_editable(obj))`). A plain
 * lv_obj is LV_OBJ_CLASS_EDITABLE_FALSE, so rotation over one silently
 * falls through to LVGL's scroll branch and does nothing at all -- which is
 * exactly what happened on first hardware bring-up, with the driver
 * correctly delivering 190 detents that nothing consumed.
 *
 * Working within that model would mean every screen builds its interaction
 * out of editable widgets and manages group editing state. That is a poor
 * fit here: the ring is the primary input for screens that are mostly
 * read-only, and LVGL's editable widgets clamp internally, which is the
 * accumulated-overshoot behaviour we specifically do not want.
 *
 * So rotation is delivered here instead, as a plain relative delta, and the
 * screen decides what it means. See the "NO ABSOLUTE POSITION" note in
 * board_encoder.c.
 *
 * The callback runs in the LVGL task with the display lock ALREADY HELD --
 * call lv_* freely, but do NOT take capstan_board_lock() inside it.
 *
 * While a callback is registered, detents are NOT also written to
 * lv_indev_data_t::enc_diff, so nothing is counted twice. Pass NULL to
 * clear and hand rotation back to LVGL's own focus handling.
 */
typedef void (*capstan_board_rotate_cb_t)(int diff, void *ctx);
void capstan_board_set_rotate_callback(capstan_board_rotate_cb_t cb, void *ctx);

/**
 * Receive a short ring press (confirm / activate).
 *
 * Same contract as the rotate callback: runs in the LVGL task with the
 * display lock already held, and while registered the press is NOT also
 * delivered to LVGL as LV_KEY_ENTER, so nothing acts on it twice. Pass NULL
 * to clear and hand presses back to LVGL's focus handling.
 *
 * A LONG press never arrives here -- it is consumed by the driver and sent
 * to the back callback instead, so a screen cannot mistake one for the
 * other.
 */
typedef void (*capstan_board_press_cb_t)(void *ctx);
void capstan_board_set_press_callback(capstan_board_press_cb_t cb, void *ctx);

/**
 * Milliseconds since the last touch, rotation or press. The idle-clock
 * timeout is driven from this rather than from LVGL's own inactivity timer,
 * because LVGL's resets on any redraw -- including the clock's own second
 * hand, which would keep the display awake forever.
 */
uint32_t capstan_board_ms_since_input(void);

#ifdef __cplusplus
}
#endif
