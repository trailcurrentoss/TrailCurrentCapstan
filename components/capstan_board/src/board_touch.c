/*
 * Capacitive touch.
 *
 * All three boards use a CST816-family controller at I2C address 0x15, so
 * one driver covers them. The MaTouch carries a CST826 (NOT the CST8266 the
 * product brief names) and the CrowPanels a CST816D/T -- same register map.
 *
 * The MaTouch difference that matters: its INT and RST lines are not
 * connected. TP_INT reaches GPIO0 only through an unpopulated resistor, and
 * TP_RST has a pull-up with no GPIO drive at all. So that board is polled,
 * while the CrowPanels can use their interrupt. esp_lcd_touch handles both
 * from the same config -- passing -1 simply disables the feature.
 */

#include "driver/i2c_master.h"
#include "esp_check.h"
#include "esp_lcd_touch_cst816s.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"

#include "board_internal.h"
#include "board_pins.h"
#include "capstan_board.h"
#include "capstan_touch_cal.h"

static const char *TAG = "board.touch";

/*
 * Touch orientation is INDEPENDENT of the display's, and on this hardware
 * it is not the same value. See the note on .flags below.
 */
#ifdef CONFIG_CAPSTAN_TOUCH_MIRROR_X
#  define CAPSTAN_TOUCH_MIRROR_X 1
#else
#  define CAPSTAN_TOUCH_MIRROR_X 0
#endif
#ifdef CONFIG_CAPSTAN_TOUCH_MIRROR_Y
#  define CAPSTAN_TOUCH_MIRROR_Y 1
#else
#  define CAPSTAN_TOUCH_MIRROR_Y 0
#endif
#ifdef CONFIG_CAPSTAN_TOUCH_SWAP_XY
#  define CAPSTAN_TOUCH_SWAP_XY 1
#else
#  define CAPSTAN_TOUCH_SWAP_XY 0
#endif

static esp_lcd_touch_handle_t s_touch;
static lv_indev_t            *s_indev;

/*
 * Touch gating state. See capstan_board_touch_set_enabled() in
 * capstan_board.h for why touch is off by default.
 */
static bool s_touch_enabled;

/*
 * Chain in front of esp_lvgl_port's own read callback so any contact also
 * counts as user activity for the idle timeout.
 *
 * The port's read function is not exported, so we capture whatever it
 * installed and call through to it rather than reimplementing it -- that
 * keeps this correct across esp_lvgl_port versions.
 */
static lv_indev_read_cb_t s_port_read_cb;

/*
 * Last RAW coordinate, captured in the same read LVGL just consumed.
 *
 * An earlier version let the calibration screen issue its OWN
 * esp_lcd_touch_read_data() call. That races the esp_lvgl_port touch task
 * over one device: the sample paired with a crosshair could be a stale or
 * duplicated reading from a different press, and pairing garbage samples
 * with known targets produces a confidently wrong fit. It measurably made
 * touch accuracy worse. Capture once, here, and hand that value out.
 */
static int  s_last_raw_x, s_last_raw_y;
static bool s_last_raw_valid;
static uint32_t s_raw_seq;   /* increments per press edge */
static bool s_was_pressed;

static void touch_activity_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    if (s_port_read_cb) {
        s_port_read_cb(indev, data);
    }
    if (data->state == LV_INDEV_STATE_PRESSED) {
        /* Stash the uncorrected value before the transform is applied. */
        s_last_raw_x = data->point.x;
        s_last_raw_y = data->point.y;
        s_last_raw_valid = true;
        if (!s_was_pressed) {
            s_raw_seq++;          /* new physical contact */
            s_was_pressed = true;
        }
    } else {
        s_was_pressed = false;
    }

    if (data->state == LV_INDEV_STATE_PRESSED) {
        /*
         * Correct here, between the driver and LVGL, so every consumer sees
         * calibrated coordinates and no screen has to know this exists.
         * A no-op until a calibration has been stored.
         */
        int x = data->point.x, y = data->point.y;
        capstan_touch_cal_apply(&x, &y);
        data->point.x = (int32_t)x;
        data->point.y = (int32_t)y;
        board_note_input();
    }
}

/*
 * Raw, uncalibrated coordinates from the most recent press, plus a sequence
 * number that increments once per physical contact.
 *
 * The sequence number is what lets the calibration screen count DISTINCT
 * presses instead of however many times LVGL happened to fire an event for
 * one finger-down. Without it, four "samples" can all come from a single
 * contact, which averages nothing and defeats the point of averaging.
 *
 * Only the calibration screen wants this -- calibrating through the
 * transform being calibrated is circular.
 */
bool capstan_board_touch_raw(int *x, int *y, uint32_t *seq)
{
    if (!s_last_raw_valid || !x || !y) {
        return false;
    }
    *x = s_last_raw_x;
    *y = s_last_raw_y;
    if (seq) {
        *seq = s_raw_seq;
    }
    return true;
}

esp_err_t board_touch_init(lv_display_t *disp, lv_indev_t **out_indev)
{
    i2c_master_bus_handle_t bus = NULL;
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port   = I2C_NUM_0,
        .sda_io_num = BOARD_TOUCH_I2C_SDA,
        .scl_io_num = BOARD_TOUCH_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_RETURN_ON_ERROR(i2c_new_master_bus(&bus_cfg, &bus), TAG,
                        "i2c bus (SDA %d SCL %d) failed",
                        BOARD_TOUCH_I2C_SDA, BOARD_TOUCH_I2C_SCL);

    esp_lcd_panel_io_handle_t tp_io = NULL;
    esp_lcd_panel_io_i2c_config_t tp_io_cfg = ESP_LCD_TOUCH_IO_I2C_CST816S_CONFIG();
    tp_io_cfg.dev_addr = BOARD_TOUCH_I2C_ADDR;
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i2c(bus, &tp_io_cfg, &tp_io),
                        TAG, "touch panel io failed");

    esp_lcd_touch_config_t tp_cfg = {
        .x_max = CONFIG_CAPSTAN_LCD_H_RES,
        .y_max = CONFIG_CAPSTAN_LCD_V_RES,
        .rst_gpio_num = BOARD_TOUCH_RST,   /* -1 on MaTouch: no reset line. */
        .int_gpio_num = BOARD_TOUCH_INT,   /* -1 on MaTouch: polled instead. */
        .levels = { .reset = 0, .interrupt = 0 },
        /*
         * NOT derived from the display transform.
         *
         * The obvious assumption is that touch must carry the same mirror
         * as the display. It is wrong. The display mirror changes how the
         * framebuffer maps onto the glass; the touch controller is a
         * separate device whose coordinates are fixed by how its own layer
         * is bonded. On the CrowPanel 1.28in the touch layer already
         * reports in the corrected orientation, so display needs mirror_x
         * and touch does not.
         *
         * An earlier version tied both to CONFIG_CAPSTAN_LCD_MIRROR_X and
         * produced a correct display with back-to-front touch.
         */
        .flags  = {
            .swap_xy  = CAPSTAN_TOUCH_SWAP_XY,
            .mirror_x = CAPSTAN_TOUCH_MIRROR_X,
            .mirror_y = CAPSTAN_TOUCH_MIRROR_Y,
        },
    };
    ESP_RETURN_ON_ERROR(esp_lcd_touch_new_i2c_cst816s(tp_io, &tp_cfg, &s_touch),
                        TAG, "cst816 init failed");

    const lvgl_port_touch_cfg_t lv_cfg = {
        .disp   = disp,
        .handle = s_touch,
    };
    s_indev = lvgl_port_add_touch(&lv_cfg);
    ESP_RETURN_ON_FALSE(s_indev, ESP_FAIL, TAG, "lvgl_port_add_touch failed");

    /* Chain our activity hook in front of the port's own read callback. */
    s_port_read_cb = lv_indev_get_read_cb(s_indev);
    lv_indev_set_read_cb(s_indev, touch_activity_cb);

    /*
     * Start disabled unless the build says otherwise. Screens that need
     * pointer input turn it on for as long as they are showing.
     */
#ifdef CONFIG_CAPSTAN_TOUCH_DEFAULT_ENABLED
    s_touch_enabled = true;
#else
    s_touch_enabled = false;
#endif
    lv_indev_enable(s_indev, s_touch_enabled);

    capstan_touch_cal_reload();

    ESP_LOGI(TAG, "CST816-family @0x%02X on SDA %d / SCL %d, INT %d RST %d%s -- %s",
             BOARD_TOUCH_I2C_ADDR, BOARD_TOUCH_I2C_SDA, BOARD_TOUCH_I2C_SCL,
             BOARD_TOUCH_INT, BOARD_TOUCH_RST,
             (BOARD_TOUCH_INT < 0) ? " (polled)" : "",
             s_touch_enabled ? "ENABLED" : "disabled (ring-only)");
    ESP_LOGI(TAG, "touch transform (independent of display): mirror_x=%d mirror_y=%d swap_xy=%d",
             CAPSTAN_TOUCH_MIRROR_X, CAPSTAN_TOUCH_MIRROR_Y, CAPSTAN_TOUCH_SWAP_XY);

    *out_indev = s_indev;
    return ESP_OK;
}

lv_indev_t *capstan_board_touch_indev(void) { return s_indev; }

/*
 * The ring press and a screen touch are the same physical action on this
 * hardware, so they must not both be live at once.
 */
void capstan_board_touch_set_enabled(bool enabled)
{
    if (!s_indev) {
        /* Touch never came up. Record the intent so a later query is honest,
         * but there is nothing to toggle. */
        s_touch_enabled = false;
        return;
    }
    if (enabled == s_touch_enabled) {
        return;
    }
    s_touch_enabled = enabled;
    lv_indev_enable(s_indev, enabled);
    ESP_LOGD(TAG, "touch %s", enabled ? "enabled" : "disabled");
}

bool capstan_board_touch_is_enabled(void) { return s_touch_enabled; }
