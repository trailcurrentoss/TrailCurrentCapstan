#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_PAGE_IDLE = 1,
    SCREEN_ID_PAGE_MENU = 2,
    SCREEN_ID_PAGE_CLIMATE = 3,
    SCREEN_ID_PAGE_CLIMATE_MODE = 4,
    SCREEN_ID_PAGE_LIGHTS = 5,
    SCREEN_ID_PAGE_HEATER = 6,
    SCREEN_ID_PAGE_ENERGY = 7,
    SCREEN_ID_PAGE_WATER = 8,
    SCREEN_ID_PAGE_AIR = 9,
    SCREEN_ID_PAGE_LEVEL = 10,
    SCREEN_ID_PAGE_DOORS = 11,
    SCREEN_ID_PAGE_SETTINGS = 12,
    SCREEN_ID_PAGE_WIFI = 13,
    SCREEN_ID_PAGE_WIFI_SECURITY = 14,
    SCREEN_ID_PAGE_MQTT = 15,
    SCREEN_ID_PAGE_KEYBOARD = 16,
    SCREEN_ID_PAGE_ALERT = 17,
    _SCREEN_ID_LAST = 17
};

typedef struct _objects_t {
    lv_obj_t *page_idle;
    lv_obj_t *page_menu;
    lv_obj_t *page_climate;
    lv_obj_t *page_climate_mode;
    lv_obj_t *page_lights;
    lv_obj_t *page_heater;
    lv_obj_t *page_energy;
    lv_obj_t *page_water;
    lv_obj_t *page_air;
    lv_obj_t *page_level;
    lv_obj_t *page_doors;
    lv_obj_t *page_settings;
    lv_obj_t *page_wifi;
    lv_obj_t *page_wifi_security;
    lv_obj_t *page_mqtt;
    lv_obj_t *page_keyboard;
    lv_obj_t *page_alert;
    lv_obj_t *idle_ring;
    lv_obj_t *idle_date;
    lv_obj_t *idle_hand_hour;
    lv_obj_t *idle_hand_minute;
    lv_obj_t *idle_hand_second;
    lv_obj_t *idle_cap;
    lv_obj_t *menu_item0;
    lv_obj_t *menu_item0_icon;
    lv_obj_t *menu_item1;
    lv_obj_t *menu_item1_icon;
    lv_obj_t *menu_item2;
    lv_obj_t *menu_item2_icon;
    lv_obj_t *menu_item3;
    lv_obj_t *menu_item3_icon;
    lv_obj_t *menu_item4;
    lv_obj_t *menu_item4_icon;
    lv_obj_t *menu_item5;
    lv_obj_t *menu_item5_icon;
    lv_obj_t *menu_item6;
    lv_obj_t *menu_item6_icon;
    lv_obj_t *menu_item7;
    lv_obj_t *menu_item7_icon;
    lv_obj_t *menu_item8;
    lv_obj_t *menu_item8_icon;
    lv_obj_t *menu_title;
    lv_obj_t *menu_summary;
    lv_obj_t *climate_arc;
    lv_obj_t *climate_setpoint;
    lv_obj_t *climate_unit;
    lv_obj_t *climate_current;
    lv_obj_t *climate_state_icon;
    lv_obj_t *climate_state;
    lv_obj_t *climate_eta;
    lv_obj_t *climate_mode;
    lv_obj_t *climate_back;
    lv_obj_t *climate_back_icon;
    lv_obj_t *cmode_title;
    lv_obj_t *cmode_list;
    lv_obj_t *cmode_item0;
    lv_obj_t *cmode_item0_title;
    lv_obj_t *cmode_item1;
    lv_obj_t *cmode_item1_title;
    lv_obj_t *cmode_item2;
    lv_obj_t *cmode_item2_title;
    lv_obj_t *cmode_item3;
    lv_obj_t *cmode_item3_title;
    lv_obj_t *climate_mode_back;
    lv_obj_t *climate_mode_back_icon;
    lv_obj_t *lights_title;
    lv_obj_t *lights_list;
    lv_obj_t *lights_item0;
    lv_obj_t *lights_item0_icon;
    lv_obj_t *lights_item0_title;
    lv_obj_t *lights_item0_value;
    lv_obj_t *lights_item1;
    lv_obj_t *lights_item1_icon;
    lv_obj_t *lights_item1_title;
    lv_obj_t *lights_item1_value;
    lv_obj_t *lights_item2;
    lv_obj_t *lights_item2_icon;
    lv_obj_t *lights_item2_title;
    lv_obj_t *lights_item2_value;
    lv_obj_t *lights_item3;
    lv_obj_t *lights_item3_icon;
    lv_obj_t *lights_item3_title;
    lv_obj_t *lights_item3_value;
    lv_obj_t *lights_item4;
    lv_obj_t *lights_item4_icon;
    lv_obj_t *lights_item4_title;
    lv_obj_t *lights_item4_value;
    lv_obj_t *lights_item5;
    lv_obj_t *lights_item5_icon;
    lv_obj_t *lights_item5_title;
    lv_obj_t *lights_item5_value;
    lv_obj_t *lights_item6;
    lv_obj_t *lights_item6_icon;
    lv_obj_t *lights_item6_title;
    lv_obj_t *lights_item7;
    lv_obj_t *lights_item7_icon;
    lv_obj_t *lights_item7_title;
    lv_obj_t *lights_item8;
    lv_obj_t *lights_item8_icon;
    lv_obj_t *lights_item8_title;
    lv_obj_t *lights_back;
    lv_obj_t *lights_back_icon;
    lv_obj_t *heater_arc;
    lv_obj_t *heater_title;
    lv_obj_t *heater_level;
    lv_obj_t *heater_state;
    lv_obj_t *heater_back;
    lv_obj_t *heater_back_icon;
    lv_obj_t *energy_title;
    lv_obj_t *energy_value;
    lv_obj_t *energy_unit;
    lv_obj_t *energy_sub;
    lv_obj_t *energy_dot0;
    lv_obj_t *energy_dot1;
    lv_obj_t *energy_dot2;
    lv_obj_t *energy_dot3;
    lv_obj_t *energy_dot4;
    lv_obj_t *energy_back;
    lv_obj_t *energy_back_icon;
    lv_obj_t *water_title;
    lv_obj_t *water_fresh_value;
    lv_obj_t *water_fresh_bar;
    lv_obj_t *water_fresh_label;
    lv_obj_t *water_grey_value;
    lv_obj_t *water_grey_bar;
    lv_obj_t *water_grey_label;
    lv_obj_t *water_black_value;
    lv_obj_t *water_black_bar;
    lv_obj_t *water_black_label;
    lv_obj_t *water_back;
    lv_obj_t *water_back_icon;
    lv_obj_t *air_title;
    lv_obj_t *air_rows;
    lv_obj_t *air_temp_label;
    lv_obj_t *air_temp;
    lv_obj_t *air_humidity_label;
    lv_obj_t *air_humidity;
    lv_obj_t *air_eco2_label;
    lv_obj_t *air_eco2;
    lv_obj_t *air_voc_label;
    lv_obj_t *air_voc;
    lv_obj_t *air_back;
    lv_obj_t *air_back_icon;
    lv_obj_t *level_title;
    lv_obj_t *level_well;
    lv_obj_t *level_bubble;
    lv_obj_t *level_pitch;
    lv_obj_t *level_roll;
    lv_obj_t *level_back;
    lv_obj_t *level_back_icon;
    lv_obj_t *doors_title;
    lv_obj_t *doors_list;
    lv_obj_t *doors_item0;
    lv_obj_t *doors_item0_icon;
    lv_obj_t *doors_item0_title;
    lv_obj_t *doors_item0_value;
    lv_obj_t *doors_item1;
    lv_obj_t *doors_item1_icon;
    lv_obj_t *doors_item1_title;
    lv_obj_t *doors_item1_value;
    lv_obj_t *doors_item2;
    lv_obj_t *doors_item2_icon;
    lv_obj_t *doors_item2_title;
    lv_obj_t *doors_item2_value;
    lv_obj_t *doors_item3;
    lv_obj_t *doors_item3_icon;
    lv_obj_t *doors_item3_title;
    lv_obj_t *doors_item3_value;
    lv_obj_t *doors_item4;
    lv_obj_t *doors_item4_icon;
    lv_obj_t *doors_item4_title;
    lv_obj_t *doors_item4_value;
    lv_obj_t *doors_item5;
    lv_obj_t *doors_item5_icon;
    lv_obj_t *doors_item5_title;
    lv_obj_t *doors_item5_value;
    lv_obj_t *doors_back;
    lv_obj_t *doors_back_icon;
    lv_obj_t *settings_title;
    lv_obj_t *settings_list;
    lv_obj_t *settings_item0;
    lv_obj_t *settings_item0_icon;
    lv_obj_t *settings_item0_title;
    lv_obj_t *settings_item0_value;
    lv_obj_t *settings_item1;
    lv_obj_t *settings_item1_icon;
    lv_obj_t *settings_item1_title;
    lv_obj_t *settings_item1_value;
    lv_obj_t *settings_item2;
    lv_obj_t *settings_item2_icon;
    lv_obj_t *settings_item2_title;
    lv_obj_t *settings_item2_value;
    lv_obj_t *settings_item3;
    lv_obj_t *settings_item3_icon;
    lv_obj_t *settings_item3_title;
    lv_obj_t *settings_back;
    lv_obj_t *settings_back_icon;
    lv_obj_t *wifi_title;
    lv_obj_t *wifi_list;
    lv_obj_t *wifi_item0;
    lv_obj_t *wifi_item0_icon;
    lv_obj_t *wifi_item0_title;
    lv_obj_t *wifi_item0_value;
    lv_obj_t *wifi_item1;
    lv_obj_t *wifi_item1_icon;
    lv_obj_t *wifi_item1_title;
    lv_obj_t *wifi_item1_value;
    lv_obj_t *wifi_item2;
    lv_obj_t *wifi_item2_icon;
    lv_obj_t *wifi_item2_title;
    lv_obj_t *wifi_item2_value;
    lv_obj_t *wifi_item3;
    lv_obj_t *wifi_item3_icon;
    lv_obj_t *wifi_item3_title;
    lv_obj_t *wifi_item3_value;
    lv_obj_t *wifi_item4;
    lv_obj_t *wifi_item4_icon;
    lv_obj_t *wifi_item4_title;
    lv_obj_t *wifi_item4_value;
    lv_obj_t *wifi_item5;
    lv_obj_t *wifi_item5_icon;
    lv_obj_t *wifi_item5_title;
    lv_obj_t *wifi_item5_value;
    lv_obj_t *wifi_item6;
    lv_obj_t *wifi_item6_icon;
    lv_obj_t *wifi_item6_title;
    lv_obj_t *wifi_item6_value;
    lv_obj_t *wifi_item7;
    lv_obj_t *wifi_item7_icon;
    lv_obj_t *wifi_item7_title;
    lv_obj_t *wifi_item7_value;
    lv_obj_t *wifi_item8;
    lv_obj_t *wifi_item8_icon;
    lv_obj_t *wifi_item8_title;
    lv_obj_t *wifi_item8_value;
    lv_obj_t *wifi_item9;
    lv_obj_t *wifi_item9_icon;
    lv_obj_t *wifi_item9_title;
    lv_obj_t *wifi_item9_value;
    lv_obj_t *wifi_back;
    lv_obj_t *wifi_back_icon;
    lv_obj_t *wsec_title;
    lv_obj_t *wsec_list;
    lv_obj_t *wsec_item0;
    lv_obj_t *wsec_item0_title;
    lv_obj_t *wsec_item1;
    lv_obj_t *wsec_item1_title;
    lv_obj_t *wsec_item2;
    lv_obj_t *wsec_item2_title;
    lv_obj_t *wsec_item3;
    lv_obj_t *wsec_item3_title;
    lv_obj_t *wifi_security_back;
    lv_obj_t *wifi_security_back_icon;
    lv_obj_t *mqtt_title;
    lv_obj_t *mqtt_list;
    lv_obj_t *mqtt_item0;
    lv_obj_t *mqtt_item0_title;
    lv_obj_t *mqtt_item0_value;
    lv_obj_t *mqtt_item1;
    lv_obj_t *mqtt_item1_title;
    lv_obj_t *mqtt_item1_value;
    lv_obj_t *mqtt_item2;
    lv_obj_t *mqtt_item2_title;
    lv_obj_t *mqtt_item2_value;
    lv_obj_t *mqtt_item3;
    lv_obj_t *mqtt_item3_title;
    lv_obj_t *mqtt_item3_value;
    lv_obj_t *mqtt_item4;
    lv_obj_t *mqtt_item4_title;
    lv_obj_t *mqtt_back;
    lv_obj_t *mqtt_back_icon;
    lv_obj_t *kb_field;
    lv_obj_t *kb_eye;
    lv_obj_t *kb_eye_icon;
    lv_obj_t *kb_keys;
    lv_obj_t *keyboard_back;
    lv_obj_t *keyboard_back_icon;
    lv_obj_t *alert_bg;
    lv_obj_t *alert_icon;
    lv_obj_t *alert_title;
    lv_obj_t *alert_message;
    lv_obj_t *alert_hint;
} objects_t;

extern objects_t objects;

void create_screen_page_idle();
void tick_screen_page_idle();

void create_screen_page_menu();
void tick_screen_page_menu();

void create_screen_page_climate();
void tick_screen_page_climate();

void create_screen_page_climate_mode();
void tick_screen_page_climate_mode();

void create_screen_page_lights();
void tick_screen_page_lights();

void create_screen_page_heater();
void tick_screen_page_heater();

void create_screen_page_energy();
void tick_screen_page_energy();

void create_screen_page_water();
void tick_screen_page_water();

void create_screen_page_air();
void tick_screen_page_air();

void create_screen_page_level();
void tick_screen_page_level();

void create_screen_page_doors();
void tick_screen_page_doors();

void create_screen_page_settings();
void tick_screen_page_settings();

void create_screen_page_wifi();
void tick_screen_page_wifi();

void create_screen_page_wifi_security();
void tick_screen_page_wifi_security();

void create_screen_page_mqtt();
void tick_screen_page_mqtt();

void create_screen_page_keyboard();
void tick_screen_page_keyboard();

void create_screen_page_alert();
void tick_screen_page_alert();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

// Color themes

enum Themes {
    THEME_ID_DEFAULT,
    THEME_ID_DARK,
};
enum Colors {
    COLOR_ID_BG_DESK,
    COLOR_ID_BG_BODY,
    COLOR_ID_BG_CARD,
    COLOR_ID_BG_CARD_HOVER,
    COLOR_ID_BG_BAR,
    COLOR_ID_BORDER_COLOR,
    COLOR_ID_TEXT_PRIMARY,
    COLOR_ID_TEXT_SECONDARY,
    COLOR_ID_TEXT_MUTED,
    COLOR_ID_ACCENT_PRIMARY,
    COLOR_ID_ACCENT_SOFT,
    COLOR_ID_SOLAR,
    COLOR_ID_SOLAR_SOFT,
    COLOR_ID_INFO,
    COLOR_ID_DANGER,
    COLOR_ID_SUCCESS,
    COLOR_ID_SLATE_NEUTRAL,
    COLOR_ID_TANK_FRESH_DARK,
    COLOR_ID_TANK_FRESH_LIGHT,
    COLOR_ID_TANK_GREY_DARK,
    COLOR_ID_TANK_GREY_LIGHT,
    COLOR_ID_TANK_BLACK_DARK,
    COLOR_ID_TANK_BLACK_LIGHT,
    COLOR_ID_GREY_WATER,
    COLOR_ID_BLACK_WATER,
    COLOR_ID_GRID_LINE,
    COLOR_ID_FOREGROUND_WHITE,
    COLOR_ID_FOREGROUND_BLACK,
};
void change_color_theme(uint32_t themeIndex);
extern uint32_t theme_colors[2][28];
extern uint32_t active_theme_index;

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/