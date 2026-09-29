/*
 * The Kconfig values the UI code reads, for the EEZ Studio simulator only.
 *
 * Mirrors the defaults in main/Kconfig.projbuild. If a default changes there,
 * change it here -- nothing checks the two agree.
 *
 * The board is chosen from DISPLAY_WIDTH, which the simulator build passes
 * from the project's displayWidth. That is the same one-resolution-per-board
 * mapping main/CMakeLists.txt uses, run backwards.
 */
#pragma once

#ifndef EEZ_LVGL_SIMULATOR
#  error "this sdkconfig.h is the simulator's stand-in; the firmware generates its own"
#endif

#if !defined(DISPLAY_WIDTH) || DISPLAY_WIDTH >= 480
#  define CONFIG_CAPSTAN_BOARD_MATOUCH_21   1
#  define CONFIG_CAPSTAN_BOARD_NAME         "matouch21"
#  define CONFIG_CAPSTAN_LCD_H_RES          480
#  define CONFIG_CAPSTAN_LCD_V_RES          480
#  define CONFIG_CAPSTAN_RGB_LED_COUNT      0
#elif DISPLAY_WIDTH >= 360
#  define CONFIG_CAPSTAN_BOARD_CROWPANEL_146 1
#  define CONFIG_CAPSTAN_BOARD_NAME         "crowpanel146"
#  define CONFIG_CAPSTAN_LCD_H_RES          360
#  define CONFIG_CAPSTAN_LCD_V_RES          360
#  define CONFIG_CAPSTAN_HAS_RGB_LEDS       1
#  define CONFIG_CAPSTAN_RGB_LED_COUNT      8
#else
#  define CONFIG_CAPSTAN_BOARD_CROWPANEL_128 1
#  define CONFIG_CAPSTAN_BOARD_NAME         "crowpanel128"
#  define CONFIG_CAPSTAN_LCD_H_RES          240
#  define CONFIG_CAPSTAN_LCD_V_RES          240
#  define CONFIG_CAPSTAN_HAS_RGB_LEDS       1
#  define CONFIG_CAPSTAN_RGB_LED_COUNT      5
#endif

#define CONFIG_CAPSTAN_RGB_LEDS_MAX_BRIGHTNESS  40
#define CONFIG_CAPSTAN_LONG_PRESS_MS            700
#define CONFIG_CAPSTAN_IDLE_TIMEOUT_S           30
#define CONFIG_CAPSTAN_ALARM_SNOOZE_MIN         10
#define CONFIG_CAPSTAN_MQTT_DEFAULT_PORT        8883
/* CONFIG_CAPSTAN_DEFAULT_UNITS_CELSIUS and CONFIG_CAPSTAN_DEFAULT_THEME_DARK
 * default to n, which Kconfig expresses by leaving them undefined. */

/* Only named in a comment in ui_data.c; kept so a future use still builds. */
#define CONFIG_ESP_SYSTEM_EVENT_TASK_STACK_SIZE 2304
