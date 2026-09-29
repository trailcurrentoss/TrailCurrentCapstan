/*
 * Per-board pin maps.
 *
 * PROVENANCE. Every value here was read out of the vendor's own firmware and
 * cross-checked against the vendor wiki pin table. Where the two disagreed, or
 * where a value could not be confirmed, it is called out in a comment. Nothing
 * in this file is inferred from a photograph, a product page, or a sibling
 * board.
 *
 * If you change a number here, cite where it came from. A wrong GPIO in this
 * file presents as a dead panel or a dead encoder, and the debugging goes
 * looking at the driver.
 */
#pragma once

/* ===========================================================================
 * Makerfabs MaTouch ESP32-S3 Rotary IPS 2.1" — 480x480 ST7701S
 *
 * Source: github.com/Makerfabs/MaTouch-ESP32-S3-Rotary-IPS-Display-with-Touch-
 *         2.1-ST7701, example/fw_test/fw_test.ino (Arduino_ESP32RGBPanel
 *         constructor), plus wiki.makerfabs.com pin table.
 * ======================================================================== */
#if CONFIG_CAPSTAN_BOARD_MATOUCH_21

/* 3-wire SPI sideband, used only to push the ST7701S init sequence. */
#define BOARD_LCD_SPI_CS        1
#define BOARD_LCD_SPI_SCK       46
#define BOARD_LCD_SPI_SDA       0
#define BOARD_LCD_RST           -1   /* Not connected. The vendor passes
                                        GFX_NOT_DEFINED; there is no reset
                                        line to drive. */

/* 16-bit RGB565 parallel bus. */
#define BOARD_LCD_DE            2
#define BOARD_LCD_VSYNC         42
#define BOARD_LCD_HSYNC         3
#define BOARD_LCD_PCLK          45

/*
 * COLOUR CHANNEL ORDER — highest-risk item on this board.
 *
 * The vendor's firmware comments label 11/15/12/16/21 as R0..R4 and
 * 4/41/5/40/6 as B0..B4. The vendor's own wiki table labels them the exact
 * opposite way. They are the same physical wires; Arduino_GFX reconciles the
 * discrepancy by passing `true` for its BGR flag.
 *
 * esp_lcd's data_gpio_nums[] is ordered blue-LSB first: [0..4] blue,
 * [5..10] green, [11..15] red. The grouping below follows the WIKI
 * convention (11..21 = blue). If the first bring-up shows red and blue
 * swapped, do NOT rewire this table -- flip the panel's RGB element order
 * bit instead, which is the single-line fix and keeps this map matching the
 * schematic.
 */
#define BOARD_LCD_B0            11
#define BOARD_LCD_B1            15
#define BOARD_LCD_B2            12
#define BOARD_LCD_B3            16
#define BOARD_LCD_B4            21
#define BOARD_LCD_G0            39
#define BOARD_LCD_G1            7
#define BOARD_LCD_G2            47
#define BOARD_LCD_G3            8
#define BOARD_LCD_G4            48
#define BOARD_LCD_G5            9
#define BOARD_LCD_R0            4
#define BOARD_LCD_R1            41
#define BOARD_LCD_R2            5
#define BOARD_LCD_R3            40
#define BOARD_LCD_R4            6

/*
 * RGB timings from the vendor's Arduino_ST7701_RGBPanel constructor; the wiki
 * additionally specifies both sync polarities as 1 (active high).
 *
 * pclk_hz is NOT a vendor specification. No example passes `prefer_speed`, so
 * the demos inherit Arduino_GFX's default, which for octal PSRAM is 12 MHz
 * (src/databus/Arduino_ESP32RGBPanel.cpp). Treat 12 MHz as a known-good
 * starting point and raise it only with a frame-rate measurement to justify
 * it -- too high and the panel tears as PSRAM bandwidth runs out.
 */
#define BOARD_LCD_PCLK_HZ       (12 * 1000 * 1000)
#define BOARD_LCD_HSYNC_FRONT   10
#define BOARD_LCD_HSYNC_PULSE   8
#define BOARD_LCD_HSYNC_BACK    50
#define BOARD_LCD_VSYNC_FRONT   10
#define BOARD_LCD_VSYNC_PULSE   8
#define BOARD_LCD_VSYNC_BACK    20
#define BOARD_LCD_HSYNC_POL     1
#define BOARD_LCD_VSYNC_POL     1

/* Backlight is a plain GPIO in the vendor firmware, but the schematic shows
 * it gating an S8050 NPN, so LEDC dimming works. We drive it with LEDC. */
#define BOARD_BL_GPIO           38
#define BOARD_BL_HAS_PWM        1

/*
 * Touch: CST826 (NOT CST8266 -- the product brief is wrong; the schematic
 * refdes and the wiki both say CST826). Shares the LCD FPC.
 *
 * INT and RST are physically unusable, and this is a hardware fact rather
 * than a documentation gap: TP_INT reaches GPIO0 only through an unpopulated
 * resistor (and GPIO0 is the LCD SPI SDA and a boot strap anyway), and TP_RST
 * has a 10K pull-up with no GPIO drive at all. The `// 38` and `// 0`
 * comments in the vendor firmware are stale -- 38 is the backlight and 0 is
 * the LCD SDA. Poll the controller over I2C.
 */
#define BOARD_TOUCH_I2C_SDA     17
#define BOARD_TOUCH_I2C_SCL     18
#define BOARD_TOUCH_I2C_ADDR    0x15
#define BOARD_TOUCH_INT         -1
#define BOARD_TOUCH_RST         -1

/* Rotary encoder. The vendor demo uses a single-edge ISR on CLK only; we use
 * PCNT with full quadrature decode, hence 4 edges per cycle. */
#define BOARD_ENC_A             13
#define BOARD_ENC_B             10
#define BOARD_ENC_BTN           14
#define BOARD_ENC_PULLUP        1    /* Vendor sketch sets INPUT_PULLUP on both
                                        A and B -- unlike the CrowPanels, which
                                        rely on external pull-ups. */
#define BOARD_ENC_BTN_PULLUP    1    /* R13 pulls up externally; we enable the
                                        internal pull-up as well. */

/* No WS2812, no battery, no SD, no IO expander on this board. Defined as
 * absent rather than left undefined so the capability assertion in
 * capstan_board.c compiles for every board. */
#define BOARD_HAS_WS2812        0
#define BOARD_WS2812_GPIO       -1
#define BOARD_WS2812_COUNT      0
#define BOARD_WS2812_SIDE       { 0 }
#define BOARD_WS2812_EN         -1
#define BOARD_LCD_RAIL_A        -1
#define BOARD_LCD_RAIL_B        -1

/* ===========================================================================
 * Elecrow CrowPanel 1.28" Rotary — 240x240 GC9A01
 *
 * Source: github.com/Elecrow-RD/CrowPanel-1.28inch-HMI-ESP32-Rotary-Display-
 *         240-240-IPS-Round-Touch-Knob-Screen (branch `master`),
 *         example/Arduino/RotaryScreen_1_28/ — LovyanGFX LGFX class.
 *         Wiki pin table agrees on every value.
 *
 * Despite "ESP32" in the product name this is an ESP32-S3R8, confirmed by
 * reading the silicon over USB.
 * ======================================================================== */
#elif CONFIG_CAPSTAN_BOARD_CROWPANEL_128

#define BOARD_LCD_SPI_HOST      SPI2_HOST
#define BOARD_LCD_SPI_SCK       10
#define BOARD_LCD_SPI_MOSI      11
#define BOARD_LCD_SPI_MISO      -1
#define BOARD_LCD_SPI_DC        3
#define BOARD_LCD_SPI_CS        9
#define BOARD_LCD_RST           14
#define BOARD_LCD_SPI_HZ        (80 * 1000 * 1000)
#define BOARD_LCD_INVERT        1    /* cfg.invert = true */
/* Vendor's LovyanGFX value, recorded for provenance only. The element
 * order actually used comes from CONFIG_CAPSTAN_LCD_BGR -- the two do not
 * correspond the way the names suggest. */
#define BOARD_LCD_VENDOR_RGB_ORDER  0    /* cfg.rgb_order = false */

/*
 * TWO RAILS MUST BE HIGH BEFORE ANY PANEL TRAFFIC.
 *
 * The vendor firmware drives GPIO1 and GPIO2 high in setup() with no
 * explanation. Without them the panel accepts no data and the board looks
 * dead -- no error, just a black screen. Drive these before touching the bus.
 */
#define BOARD_LCD_RAIL_A        1
#define BOARD_LCD_RAIL_B        2

#define BOARD_BL_GPIO           46
#define BOARD_BL_HAS_PWM        1
#define BOARD_BL_PWM_HZ         5000
#define BOARD_BL_PWM_BITS       8

/* Touch: CST816D. */
#define BOARD_TOUCH_I2C_SDA     6
#define BOARD_TOUCH_I2C_SCL     7
#define BOARD_TOUCH_I2C_ADDR    0x15
#define BOARD_TOUCH_INT         5
#define BOARD_TOUCH_RST         13

/*
 * Encoder: EC3501 C15H30P3, per Datasheet/ in the vendor repo -- 30 detents,
 * "1 pulse per 2 detents", chatter <= 5 ms.
 *
 * With full 4x quadrature decode that is 4 counts per 2 detents, i.e. 2 counts
 * per detent. (The datasheet contradicts itself: section 4-2 claims
 * "30 pulses/360 deg for each phase", which would be 1 pulse per detent. The
 * part number C15H30P3 -- 15 pulses, 30 detents -- supports the output note,
 * so we follow it. Confirm on hardware with CONFIG_CAPSTAN_ENCODER_DEBUG.)
 */
#define BOARD_ENC_A             45
#define BOARD_ENC_B             42
#define BOARD_ENC_PULLUP        0    /* External pull-ups; vendor uses bare
                                        INPUT on both phases. */
#define BOARD_ENC_BTN           41
#define BOARD_ENC_BTN_PULLUP    1
#define BOARD_ENC_CHATTER_MS    5

#define BOARD_HAS_WS2812        1
#define BOARD_WS2812_GPIO       48
#define BOARD_WS2812_COUNT      5
/* Which half of the ring each LED is on, seen from the front, in chain
 * order: -1 left, +1 right, 0 left dark in a two-sided pattern (at or near
 * the bottom centre). Mapped on hardware 2026-09-29 by lighting each index
 * its own colour: 0 at 4 o'clock, 1 at 1, 2 at 11, 3 just shy of 9, 4 at 6. */
#define BOARD_WS2812_SIDE       { +1, +1, -1, -1, 0 }
#define BOARD_WS2812_EN         -1   /* No enable rail on this board. */
#define BOARD_PWR_LED           40   /* Active low. */

/* ===========================================================================
 * Elecrow CrowPanel 1.46" Rotary — 360x360 JD9855
 *
 * Source: github.com/Elecrow-RD/CrowPanel-1.46inch-HMI-ESP32-Rotary-Display
 *         (branch `master`), example/V1.0/Arduino/
 *         RotaryScreen_1_46_Code_Core3_LVGL9/RotaryScreen_1_46.h.
 *         Wiki pin table agrees.
 *
 * Pin-for-pin identical to the 1.28" for display, touch, encoder and
 * backlight. The differences are the controller, the resolution, the LED
 * count, the LED enable rail, and rgb_order.
 * ======================================================================== */
#elif CONFIG_CAPSTAN_BOARD_CROWPANEL_146

#define BOARD_LCD_SPI_HOST      SPI2_HOST
#define BOARD_LCD_SPI_SCK       10
#define BOARD_LCD_SPI_MOSI      11
#define BOARD_LCD_SPI_MISO      -1
#define BOARD_LCD_SPI_DC        3
#define BOARD_LCD_SPI_CS        9
#define BOARD_LCD_RST           14
#define BOARD_LCD_SPI_HZ        (80 * 1000 * 1000)
#define BOARD_LCD_INVERT        0    /* cfg.invert = false */
/* Vendor's LovyanGFX value, recorded for provenance only -- and note it is
 * the OPPOSITE of the 1.28"'s, yet BOTH panels need RGB in esp_lcd terms.
 * The element order actually used comes from CONFIG_CAPSTAN_LCD_BGR. */
#define BOARD_LCD_VENDOR_RGB_ORDER  1    /* cfg.rgb_order = true */

/* Same mandatory rails as the 1.28". */
#define BOARD_LCD_RAIL_A        1
#define BOARD_LCD_RAIL_B        2

#define BOARD_BL_GPIO           46
#define BOARD_BL_HAS_PWM        1
#define BOARD_BL_PWM_HZ         5000
#define BOARD_BL_PWM_BITS       8

/* Touch: the panel datasheet says CST816D, the vendor firmware uses a CST816T
 * library. Same family, same 0x15 address, same register map -- either
 * driver works. */
#define BOARD_TOUCH_I2C_SDA     6
#define BOARD_TOUCH_I2C_SCL     7
#define BOARD_TOUCH_I2C_ADDR    0x15
#define BOARD_TOUCH_INT         5
#define BOARD_TOUCH_RST         13

/* Encoder part is NOT documented in this repo. The pinout and product family
 * match the 1.28", so the EC3501 is plausible -- but that is an assumption.
 * Calibrate with CONFIG_CAPSTAN_ENCODER_DEBUG before trusting the default. */
#define BOARD_ENC_A             45
#define BOARD_ENC_B             42
#define BOARD_ENC_PULLUP        0    /* External pull-ups. */
#define BOARD_ENC_BTN           41
#define BOARD_ENC_BTN_PULLUP    1

#define BOARD_HAS_WS2812        1
#define BOARD_WS2812_GPIO       48
#define BOARD_WS2812_COUNT      8
/* See the 1.28" entry. Mapped on hardware 2026-09-29: 0 at 2 o'clock,
 * 1 at 4, 2 at 5, 3 at 7, 4 at 8, 5 at 10, 6 at 11, 7 at 1. The two at 5
 * and 7 are the bottom pair, dark in a two-sided pattern. */
#define BOARD_WS2812_SIDE       { +1, +1, 0, 0, -1, -1, -1, +1 }
#define BOARD_WS2812_EN         17   /* Ring stays dark without this high.
                                        Not present on the 1.28". */
#define BOARD_PWR_LED           40   /* Active low. */

/*
 * NOTE: GPIO43 is wired to a demo "bulb" LED on this board. It is also U0TXD.
 * We do not drive it -- the console is on native USB-Serial/JTAG, but leaving
 * UART0 alone keeps a serial console available as a fallback.
 *
 * Battery: RESOLVED by parsing the Eagle schematic source
 * (Eagle_SCH&PCB/ESP32 Display-1.46-V1.0.sch, which is XML) rather than the
 * vector PDF. Neither the vendor firmware nor the wiki mentions it.
 *
 *   net BAT_CHK -> U1 pin GPIO18, through a 1K/1K divider (R42 high side,
 *                  R45 to ground) -- so the ADC reads Vbat/2.
 *   net BAT_CHG -> U1 pin XTAL_32K_P, which is GPIO15, with a 10K pull-up
 *                  (R1) from the charger's CHG pin. Active low = charging.
 *
 * CAUTION before using it: GPIO18 is ADC2_CH7, and ADC2 is shared with the
 * Wi-Fi radio on the ESP32-S3. A read while Wi-Fi is up returns
 * ESP_ERR_TIMEOUT. Any battery monitoring has to either tolerate failed
 * reads or sample only while the radio is idle. This is why battery
 * monitoring is not wired up yet -- the pin is known, the timing strategy
 * is not.
 */
#define BOARD_BAT_SENSE_GPIO    18   /* ADC2_CH7 -- conflicts with Wi-Fi. */
#define BOARD_BAT_SENSE_NUM_MV  2    /* Divider ratio: Vbat = adc_mv * 2. */
#define BOARD_BAT_SENSE_DEN_MV  1
#define BOARD_BAT_CHG_GPIO      15   /* Active low while charging. */

#else
#error "No Capstan board selected. Run idf.py menuconfig -> Capstan -> Rotary display board."
#endif
