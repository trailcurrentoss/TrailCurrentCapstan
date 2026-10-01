/* Internal interface between the capstan_board translation units. Not part
 * of the public API -- nothing outside this component includes it. */
#pragma once

#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "lvgl.h"

esp_err_t board_display_init(lv_display_t **out_disp);
esp_lcd_panel_handle_t board_display_panel(void);

esp_err_t board_touch_init(lv_display_t *disp, lv_indev_t **out_indev);

esp_err_t board_encoder_init(lv_indev_t **out_indev);

/* Called by the touch and encoder drivers on any user activity, so the idle
 * timeout is driven by real input rather than by LVGL's inactivity timer --
 * which the idle clock's own second hand would otherwise keep resetting. */
void board_note_input(void);

/* WS2812 ring; a no-op on boards without one. board_leds.c */
esp_err_t board_leds_init(void);

/* The ring was turned `detents` (signed, clockwise positive): dip the LED
 * the turn has reached. Called by the encoder driver on every detent. */
void board_leds_turn(int detents);
