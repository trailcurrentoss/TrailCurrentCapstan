/*
 * Display bring-up. Three panels, one entry point.
 *
 *   MaTouch 2.1"    480x480  ST7701S  16-bit RGB565 parallel + 3-wire SPI init
 *   CrowPanel 1.46" 360x360  JD9855   4-wire SPI  (hand-written driver)
 *   CrowPanel 1.28" 240x240  GC9A01   4-wire SPI
 *
 * board_display_init() returns an LVGL display that esp_lvgl_port owns. The
 * backlight is deliberately NOT switched on here -- capstan_board_init()
 * leaves it dark until the first screen has been drawn, so the panel never
 * shows an unpainted frame.
 */

#include <string.h>

#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_lvgl_port.h"

#include "board_internal.h"
#include "capstan_board.h"
#include "board_pins.h"

#if CONFIG_CAPSTAN_BOARD_MATOUCH_21
#  include "esp_lcd_panel_rgb.h"
#  include "esp_lcd_panel_io_additions.h"
#  include "esp_lcd_st7701.h"
#  include "st7701_matouch_init.h"
#else
#  include "driver/spi_master.h"
#  if CONFIG_CAPSTAN_BOARD_CROWPANEL_128
#    include "esp_lcd_gc9a01.h"
#  else
#    include "panel_jd9855.h"
#  endif
#endif

static const char *TAG = "board.disp";

/* Kconfig bools that are 'n' are UNDEFINED, not 0, so they cannot appear in
 * a runtime expression. Normalise once. */
#ifdef CONFIG_CAPSTAN_LCD_MIRROR_X
#  define CAPSTAN_MIRROR_X 1
#else
#  define CAPSTAN_MIRROR_X 0
#endif
#ifdef CONFIG_CAPSTAN_LCD_MIRROR_Y
#  define CAPSTAN_MIRROR_Y 1
#else
#  define CAPSTAN_MIRROR_Y 0
#endif
#ifdef CONFIG_CAPSTAN_LCD_SWAP_XY
#  define CAPSTAN_SWAP_XY 1
#else
#  define CAPSTAN_SWAP_XY 0
#endif
#ifdef CONFIG_CAPSTAN_LCD_BGR
#  define CAPSTAN_BGR 1
#else
#  define CAPSTAN_BGR 0
#endif

#define LCD_H_RES CONFIG_CAPSTAN_LCD_H_RES
#define LCD_V_RES CONFIG_CAPSTAN_LCD_V_RES

static esp_lcd_panel_handle_t s_panel;
static esp_lcd_panel_io_handle_t s_io;

#ifdef CONFIG_CAPSTAN_PANEL_SMOKE_TEST
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*
 * Paint the panel directly through esp_lcd, with no LVGL involved.
 *
 * The point is to split a black screen into its two possible causes. If
 * these bars appear, everything below LVGL is correct -- pins, timings,
 * init sequence, framebuffer -- and the fault is in the LVGL flush path.
 * If they do not, LVGL is irrelevant and the fault is in the panel layer.
 *
 * Drawn in horizontal bands so the scratch buffer stays small rather than
 * allocating a whole 450 KB frame.
 */
static void board_display_smoke_test(void)
{
    /*
     * Three STATIC bands, drawn once and held: top red, middle green,
     * bottom blue.
     *
     * The first version cycled full-screen fills in sequence, which turned
     * out to be a bad test -- reporting it means remembering an order, and
     * "green, red, blue" is ambiguous between a channel swap and having
     * started watching mid-cycle. Bands are read top-to-bottom in one
     * glance, with no timing and no memory involved, and they name the
     * channel that is wrong rather than just showing that one is.
     */
    const int band_rows = 40;
    const size_t px = (size_t)LCD_H_RES * band_rows;
    uint16_t *buf = heap_caps_malloc(px * sizeof(uint16_t), MALLOC_CAP_DMA);
    if (!buf) {
        ESP_LOGE(TAG, "smoke test: no memory for scratch band");
        return;
    }

    const int third = LCD_V_RES / 3;

    /*
     * Backlight blink first, so "is the backlight even on?" stops being a
     * guess. Three slow pulses: if the panel visibly brightens and dims,
     * LEDC control works and any blackness afterwards is the panel not
     * receiving valid pixels -- a completely different fault from a dead
     * backlight, and worth separating before looking at anything else.
     */
    ESP_LOGW(TAG, "backlight blink x3 -- watch for the panel brightening");
    for (int i = 0; i < 3; i++) {
        capstan_board_backlight_set(0);
        vTaskDelay(pdMS_TO_TICKS(400));
        capstan_board_backlight_set(100);
        vTaskDelay(pdMS_TO_TICKS(400));
    }

    ESP_LOGW(TAG, "smoke test: expect three bands, top to bottom: "
                  "RED / GREEN / BLUE. Holding for %d s.",
             CONFIG_CAPSTAN_PANEL_SMOKE_TEST_DWELL_MS *
             CONFIG_CAPSTAN_PANEL_SMOKE_TEST_CYCLES / 1000);

    for (int y = 0; y < LCD_V_RES; y += band_rows) {
        const int h = (y + band_rows > LCD_V_RES) ? (LCD_V_RES - y) : band_rows;

        /*
         * Native RGB565, NOT byte-swapped. The 16-bit RGB parallel bus takes
         * the value as-is; only the SPI panels need the swap. An earlier
         * version swapped here and produced a magenta band, which briefly
         * looked like a panel fault and was purely this.
         */
        uint16_t c;
        if (y < third)          { c = 0xF800; }   /* red   */
        else if (y < 2 * third) { c = 0x07E0; }   /* green */
        else                    { c = 0x001F; }   /* blue  */

        for (size_t k = 0; k < px; k++) {
            buf[k] = c;
        }
        esp_lcd_panel_draw_bitmap(s_panel, 0, y, LCD_H_RES, y + h, buf);
    }

    vTaskDelay(pdMS_TO_TICKS(CONFIG_CAPSTAN_PANEL_SMOKE_TEST_DWELL_MS *
                             CONFIG_CAPSTAN_PANEL_SMOKE_TEST_CYCLES));

    free(buf);
    ESP_LOGW(TAG, "smoke test done. Bands NOT in R/G/B order top-to-bottom "
                  "means the channel mapping is wrong, and which band is "
                  "which colour says exactly how.");
}
#endif /* CONFIG_CAPSTAN_PANEL_SMOKE_TEST */

/* ------------------------------------------------------------------------ *
 * Rails that must be high before the panel will accept anything.
 *
 * Both CrowPanels need GPIO1 and GPIO2 driven high. The vendor firmware does
 * this in setup() with no comment; without it the panel ignores every byte
 * and the board looks dead -- black screen, no error, no clue. This is the
 * single most likely reason a CrowPanel bring-up "does nothing".
 * ------------------------------------------------------------------------ */
static void enable_panel_rails(void)
{
#if BOARD_LCD_RAIL_A >= 0 || BOARD_LCD_RAIL_B >= 0
    uint64_t mask = 0;
#  if BOARD_LCD_RAIL_A >= 0
    mask |= 1ULL << BOARD_LCD_RAIL_A;
#  endif
#  if BOARD_LCD_RAIL_B >= 0
    mask |= 1ULL << BOARD_LCD_RAIL_B;
#  endif
    gpio_config_t cfg = { .mode = GPIO_MODE_OUTPUT, .pin_bit_mask = mask };
    ESP_ERROR_CHECK(gpio_config(&cfg));
#  if BOARD_LCD_RAIL_A >= 0
    gpio_set_level(BOARD_LCD_RAIL_A, 1);
#  endif
#  if BOARD_LCD_RAIL_B >= 0
    gpio_set_level(BOARD_LCD_RAIL_B, 1);
#  endif
    ESP_LOGI(TAG, "panel rails enabled (GPIO%d, GPIO%d)",
             BOARD_LCD_RAIL_A, BOARD_LCD_RAIL_B);
#endif
}

/* ======================================================================== *
 * MaTouch 2.1" — ST7701S on a 16-bit RGB bus
 * ======================================================================== */
#if CONFIG_CAPSTAN_BOARD_MATOUCH_21

esp_err_t board_display_init(lv_display_t **out_disp)
{
    enable_panel_rails();

    /*
     * The ST7701S init sequence goes over a 3-wire (9-bit) SPI sideband that
     * is separate from the RGB data bus. It is used only at startup.
     */
    const spi_line_config_t line_cfg = {
        .cs_io_type  = IO_TYPE_GPIO,
        .cs_gpio_num = BOARD_LCD_SPI_CS,
        .scl_io_type = IO_TYPE_GPIO,
        .scl_gpio_num = BOARD_LCD_SPI_SCK,
        .sda_io_type = IO_TYPE_GPIO,
        .sda_gpio_num = BOARD_LCD_SPI_SDA,
        .io_expander = NULL,          /* no expander on this board */
    };
    esp_lcd_panel_io_3wire_spi_config_t io_cfg =
        ST7701_PANEL_IO_3WIRE_SPI_CONFIG(line_cfg, 0);
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_3wire_spi(&io_cfg, &s_io),
                        TAG, "3-wire SPI io failed");

    /*
     * Data pin order. esp_lcd's data_gpio_nums[] is blue-LSB first for
     * RGB565: [0..4] blue, [5..10] green, [11..15] red.
     *
     * The grouping here follows the vendor WIKI convention. The vendor
     * FIRMWARE labels the same pins the opposite way and compensates with a
     * BGR flag, so one of the two is wrong and only the panel can say which.
     * If the boot test screen shows red and blue swapped, set
     * CONFIG_CAPSTAN_LCD_SWAP_RB rather than editing the pin map.
     */
    const int blue[5]  = { BOARD_LCD_B0, BOARD_LCD_B1, BOARD_LCD_B2,
                           BOARD_LCD_B3, BOARD_LCD_B4 };
    const int red[5]   = { BOARD_LCD_R0, BOARD_LCD_R1, BOARD_LCD_R2,
                           BOARD_LCD_R3, BOARD_LCD_R4 };
/* A Kconfig bool that is 'n' is UNDEFINED, not 0, so it cannot appear in a
 * runtime expression. Normalise it to a plain 0/1 macro once. */
#ifdef CONFIG_CAPSTAN_LCD_SWAP_RB
#  define CAPSTAN_SWAP_RB 1
#else
#  define CAPSTAN_SWAP_RB 0
#endif

#if CAPSTAN_SWAP_RB
    const int *lsb = red,  *msb = blue;
#else
    const int *lsb = blue, *msb = red;
#endif

    esp_lcd_rgb_panel_config_t rgb_cfg = {
        .clk_src          = LCD_CLK_SRC_DEFAULT,
        .psram_trans_align = 64,
        .data_width       = 16,
        .bits_per_pixel   = 16,
        .de_gpio_num      = BOARD_LCD_DE,
        .pclk_gpio_num    = BOARD_LCD_PCLK,
        .vsync_gpio_num   = BOARD_LCD_VSYNC,
        .hsync_gpio_num   = BOARD_LCD_HSYNC,
        .disp_gpio_num    = -1,
        .data_gpio_nums = {
            lsb[0], lsb[1], lsb[2], lsb[3], lsb[4],
            BOARD_LCD_G0, BOARD_LCD_G1, BOARD_LCD_G2,
            BOARD_LCD_G3, BOARD_LCD_G4, BOARD_LCD_G5,
            msb[0], msb[1], msb[2], msb[3], msb[4],
        },
        .timings = {
            .pclk_hz           = BOARD_LCD_PCLK_HZ,
            .h_res             = LCD_H_RES,
            .v_res             = LCD_V_RES,
            .hsync_front_porch = BOARD_LCD_HSYNC_FRONT,
            .hsync_pulse_width = BOARD_LCD_HSYNC_PULSE,
            .hsync_back_porch  = BOARD_LCD_HSYNC_BACK,
            .vsync_front_porch = BOARD_LCD_VSYNC_FRONT,
            .vsync_pulse_width = BOARD_LCD_VSYNC_PULSE,
            .vsync_back_porch  = BOARD_LCD_VSYNC_BACK,
            .flags = {
                .hsync_idle_low = !BOARD_LCD_HSYNC_POL,
                .vsync_idle_low = !BOARD_LCD_VSYNC_POL,
                /*
                 * Latch data on the RISING pclk edge.
                 *
                 * This was 1 on the first hardware attempt and the panel
                 * showed pure black with the backlight on and no driver
                 * error -- the controller was initialised and lit, but
                 * sampling the bus on the wrong edge so it never saw valid
                 * video. esp_lcd_st7701's own reference timing macro
                 * (ST7701_480_480_PANEL_60HZ_RGB_TIMING) uses false, and
                 * the vendor Arduino sketch passes no override, so it gets
                 * Arduino_GFX's default of false too.
                 */
                .pclk_active_neg = 0,
            },
        },
        /*
         * Two framebuffers in PSRAM (450 KB each) give tear-free double
         * buffering. The bounce buffer lives in internal RAM and is what
         * actually feeds the LCD peripheral -- without it, PSRAM latency
         * spikes show up as horizontal tearing when Wi-Fi is busy.
         */
        .num_fbs            = 2,
        .bounce_buffer_size_px = LCD_H_RES * 10,
        .flags = {
            .fb_in_psram = 1,
        },
    };

    /*
     * auto_del_panel_io and enable_io_multiplex are a UNION in this driver --
     * the same bit. Setting either one enables both behaviours: the init
     * sequence is sent at create time and the 3-wire SPI IO is then deleted.
     *
     * That is meant for boards where the SPI sideband pins are shared with
     * the RGB bus and have to be released. This board does not do that:
     * SCK 46, SDA 0 and CS 1 appear nowhere in the RGB data set (verified
     * against the Makerfabs schematic netlist, LCD FPC P2). So there is
     * nothing to release, and keeping the IO means the panel can still be
     * sent commands after init -- which matters for debugging and for
     * disp_on_off.
     */
    st7701_vendor_config_t vendor_cfg = {
        /*
         * Panel-specific init. esp_lcd_st7701's DEFAULT table blanks this
         * panel -- see st7701_matouch_init.h for why (wrong GIP/gate-driver
         * registers for this glass). Verified on hardware before this was
         * added.
         */
        .init_cmds      = matouch21_round_init,
        .init_cmds_size = MATOUCH21_ROUND_INIT_COUNT,
        .rgb_config     = &rgb_cfg,
        .flags = { .auto_del_panel_io = 0 },
    };
    esp_lcd_panel_dev_config_t dev_cfg = {
        .reset_gpio_num = BOARD_LCD_RST,
        .rgb_ele_order  = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .vendor_config  = &vendor_cfg,
    };
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_st7701(s_io, &dev_cfg, &s_panel),
                        TAG, "st7701 panel failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_reset(s_panel), TAG, "reset failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_init(s_panel),  TAG, "init failed");

    /*
     * Safe now that the panel IO is retained (auto_del_panel_io = 0). With
     * that flag set, this call fails with "Panel IO is deleted, cannot send
     * command" and aborts board init -- which boot-looped the board 21
     * times in 20 seconds on the first hardware flash.
     */
    ESP_RETURN_ON_ERROR(esp_lcd_panel_disp_on_off(s_panel, true), TAG,
                        "display on failed");


#ifdef CONFIG_CAPSTAN_PANEL_SMOKE_TEST
    /* The backlight is normally held off until the first frame; force it on
     * here or the smoke test is invisible. */
    capstan_board_backlight_set(100);
    board_display_smoke_test();
#endif

    lvgl_port_display_rgb_cfg_t rgb_lv = {
        .flags = { .bb_mode = true, .avoid_tearing = true },
    };
    /*
     * full_refresh is REQUIRED with avoid_tearing, and omitting it is a
     * subtle, ugly bug.
     *
     * avoid_tearing hands LVGL the panel's own two framebuffers and swaps
     * between them. In LVGL's default PARTIAL render mode only the changed
     * region is drawn, into whichever buffer is next -- so the two buffers
     * steadily diverge and every swap shows a mix of old and new. On
     * hardware that presented as occasional flicker while the ring turned,
     * stale green banding in areas nothing had redrawn, and a label whose
     * text never appeared to change because each update landed in the
     * buffer that was not currently being scanned out.
     *
     * full_refresh redraws the whole screen each time, so both buffers stay
     * complete and consistent. 450 KB per frame out of octal PSRAM, which
     * this panel's 12 MHz pixel clock absorbs comfortably.
     *
     * buff_dma / buff_spiram are false on purpose: with avoid_tearing,
     * esp_lvgl_port ignores them and uses the RGB panel's own framebuffers,
     * which already live in PSRAM per rgb_cfg.flags.fb_in_psram.
     */
    lvgl_port_display_cfg_t disp_cfg = {
        .io_handle     = s_io,
        .panel_handle  = s_panel,
        .buffer_size   = LCD_H_RES * LCD_V_RES,
        .double_buffer = true,
        .hres          = LCD_H_RES,
        .vres          = LCD_V_RES,
        .color_format  = LV_COLOR_FORMAT_RGB565,
        .rotation = {
            .swap_xy  = CAPSTAN_SWAP_XY,
            .mirror_x = CAPSTAN_MIRROR_X,
            .mirror_y = CAPSTAN_MIRROR_Y,
        },
        .flags = {
            .buff_dma     = false,
            .buff_spiram  = false,
            .full_refresh = true,
        },
    };
    *out_disp = lvgl_port_add_disp_rgb(&disp_cfg, &rgb_lv);
    ESP_RETURN_ON_FALSE(*out_disp, ESP_FAIL, TAG, "lvgl_port_add_disp_rgb failed");

    ESP_LOGI(TAG, "ST7701S %dx%d RGB up, pclk %d MHz, R/B %sswapped",
             LCD_H_RES, LCD_V_RES, BOARD_LCD_PCLK_HZ / 1000000,
             CAPSTAN_SWAP_RB ? "" : "not ");
    return ESP_OK;
}

/* ======================================================================== *
 * CrowPanel 1.28" and 1.46" — 4-wire SPI
 * ======================================================================== */
#else

esp_err_t board_display_init(lv_display_t **out_disp)
{
    enable_panel_rails();

    /*
     * Transfer size caps one esp_lcd colour transaction. A full-width band is
     * the natural unit; 80 rows keeps the DMA descriptor list small while
     * still amortising the per-transaction overhead.
     */
    const size_t max_transfer = LCD_H_RES * 80 * sizeof(uint16_t);

    spi_bus_config_t bus = {
        .sclk_io_num     = BOARD_LCD_SPI_SCK,
        .mosi_io_num     = BOARD_LCD_SPI_MOSI,
        .miso_io_num     = BOARD_LCD_SPI_MISO,
        .quadwp_io_num   = -1,
        .quadhd_io_num   = -1,
        .max_transfer_sz = max_transfer,
    };
    ESP_RETURN_ON_ERROR(
        spi_bus_initialize(BOARD_LCD_SPI_HOST, &bus, SPI_DMA_CH_AUTO),
        TAG, "spi bus init failed");

    esp_lcd_panel_io_spi_config_t io_cfg = {
        .cs_gpio_num       = BOARD_LCD_SPI_CS,
        .dc_gpio_num       = BOARD_LCD_SPI_DC,
        .spi_mode          = 0,
        .pclk_hz           = BOARD_LCD_SPI_HZ,
        .trans_queue_depth = 10,
        .lcd_cmd_bits      = 8,
        .lcd_param_bits    = 8,
    };
    ESP_RETURN_ON_ERROR(
        esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)BOARD_LCD_SPI_HOST,
                                 &io_cfg, &s_io),
        TAG, "spi panel io failed");

    esp_lcd_panel_dev_config_t dev_cfg = {
        .reset_gpio_num = BOARD_LCD_RST,
        /*
         * From Kconfig, not from the pin map. The vendor's LovyanGFX
         * cfg.rgb_order does not map onto esp_lcd's element order the way
         * the names imply -- see CAPSTAN_LCD_BGR's help. The two
         * CrowPanels are verified on hardware as needing OPPOSITE values:
         * 1.28in wants BGR, 1.46in wants RGB.
         */
        .rgb_ele_order  = CAPSTAN_BGR ? LCD_RGB_ELEMENT_ORDER_BGR
                                      : LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };

#if CONFIG_CAPSTAN_BOARD_CROWPANEL_128
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_gc9a01(s_io, &dev_cfg, &s_panel),
                        TAG, "gc9a01 panel failed");
#else
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_jd9855(s_io, &dev_cfg, &s_panel),
                        TAG, "jd9855 panel failed");
#endif

    ESP_RETURN_ON_ERROR(esp_lcd_panel_reset(s_panel), TAG, "reset failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_init(s_panel),  TAG, "init failed");

    /*
     * Orientation is NOT set here.
     *
     * esp_lvgl_port owns it: lvgl_port_add_disp() applies its own
     * cfg.rotation by calling esp_lcd_panel_swap_xy() and
     * esp_lcd_panel_mirror(), which overwrites anything set beforehand
     * with its defaults of zero.
     *
     * An earlier version called those functions here, right after
     * panel_init(). They took effect and were then silently undone a few
     * lines later -- the log said mirror_x=1, the panel did mirror_x=0,
     * and the display stayed mirrored through two attempts. Set it in
     * disp_cfg.rotation below instead.
     */
#if BOARD_LCD_INVERT
    ESP_RETURN_ON_ERROR(esp_lcd_panel_invert_color(s_panel, true), TAG,
                        "invert failed");
#endif
    ESP_RETURN_ON_ERROR(esp_lcd_panel_disp_on_off(s_panel, true), TAG,
                        "display on failed");

    /*
     * Partial-buffer rendering: an eighth of the screen, twice, in PSRAM.
     * These panels are small enough that a full framebuffer would fit, but
     * SPI is the bottleneck here, not memory -- smaller buffers let LVGL
     * start flushing sooner.
     */
    lvgl_port_display_cfg_t disp_cfg = {
        .io_handle     = s_io,
        .panel_handle  = s_panel,
        .buffer_size   = LCD_H_RES * LCD_V_RES / 8,
        .double_buffer = true,
        .hres          = LCD_H_RES,
        .vres          = LCD_V_RES,
        .color_format  = LV_COLOR_FORMAT_RGB565,
        /* The port applies this to the panel itself; see the note above. */
        .rotation = {
            .swap_xy  = CAPSTAN_SWAP_XY,
            .mirror_x = CAPSTAN_MIRROR_X,
            .mirror_y = CAPSTAN_MIRROR_Y,
        },
        .flags = {
            .buff_dma    = true,
            .buff_spiram = false,
            .swap_bytes  = true,   /* SPI panels take big-endian RGB565. */
        },
    };
    *out_disp = lvgl_port_add_disp(&disp_cfg);
    ESP_RETURN_ON_FALSE(*out_disp, ESP_FAIL, TAG, "lvgl_port_add_disp failed");

    ESP_LOGI(TAG, "%s %dx%d SPI up @ %d MHz",
             CONFIG_CAPSTAN_BOARD_NAME, LCD_H_RES, LCD_V_RES,
             BOARD_LCD_SPI_HZ / 1000000);
    ESP_LOGI(TAG, "orientation: mirror_x=%d mirror_y=%d swap_xy=%d, element order %s",
             CAPSTAN_MIRROR_X, CAPSTAN_MIRROR_Y, CAPSTAN_SWAP_XY,
             CAPSTAN_BGR ? "BGR" : "RGB");
    return ESP_OK;
}

#endif

esp_lcd_panel_handle_t board_display_panel(void) { return s_panel; }
