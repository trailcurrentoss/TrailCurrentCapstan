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
    SCREEN_ID_PAGE_DEVICES = 5,
    SCREEN_ID_PAGE_ENERGY = 6,
    SCREEN_ID_PAGE_WATER = 7,
    SCREEN_ID_PAGE_AIR = 8,
    SCREEN_ID_PAGE_LEVEL = 9,
    SCREEN_ID_PAGE_SETTINGS = 10,
    SCREEN_ID_PAGE_ALERT = 11,
    SCREEN_ID_PAGE_SETUP = 12,
    _SCREEN_ID_LAST = 12
};

typedef struct _objects_t {
    lv_obj_t *page_idle;
    lv_obj_t *page_menu;
    lv_obj_t *page_climate;
    lv_obj_t *page_climate_mode;
    lv_obj_t *page_devices;
    lv_obj_t *page_energy;
    lv_obj_t *page_water;
    lv_obj_t *page_air;
    lv_obj_t *page_level;
    lv_obj_t *page_settings;
    lv_obj_t *page_alert;
    lv_obj_t *page_setup;
    lv_obj_t *idle_ring;
    lv_obj_t *idle_date;
    lv_obj_t *idle_hand_hour;
    lv_obj_t *idle_hand_minute;
    lv_obj_t *idle_hand_second;
    lv_obj_t *idle_cap;
    lv_obj_t *menu_hero;
    lv_obj_t *menu_hero_icon;
    lv_obj_t *menu_prev;
    lv_obj_t *menu_prev_icon;
    lv_obj_t *menu_next;
    lv_obj_t *menu_next_icon;
    lv_obj_t *menu_title;
    lv_obj_t *menu_summary;
    lv_obj_t *menu_dot0;
    lv_obj_t *menu_dot1;
    lv_obj_t *menu_dot2;
    lv_obj_t *menu_dot3;
    lv_obj_t *menu_dot4;
    lv_obj_t *menu_dot5;
    lv_obj_t *menu_dot6;
    lv_obj_t *menu_dot7;
    lv_obj_t *climate_ticks;
    lv_obj_t *climate_needle_inside;
    lv_obj_t *climate_needle_target;
    lv_obj_t *climate_mode_row;
    lv_obj_t *climate_mode_icon;
    lv_obj_t *climate_mode;
    lv_obj_t *climate_setpoint;
    lv_obj_t *climate_info_row;
    lv_obj_t *climate_inside;
    lv_obj_t *climate_eta_group;
    lv_obj_t *climate_eta_icon;
    lv_obj_t *climate_eta;
    lv_obj_t *climate_back;
    lv_obj_t *climate_back_icon;
    lv_obj_t *cmode_title;
    lv_obj_t *cmode_list;
    lv_obj_t *cmode_item0;
    lv_obj_t *cmode_item0_icon;
    lv_obj_t *cmode_item0_title;
    lv_obj_t *cmode_item1;
    lv_obj_t *cmode_item1_icon;
    lv_obj_t *cmode_item1_title;
    lv_obj_t *cmode_item2;
    lv_obj_t *cmode_item2_icon;
    lv_obj_t *cmode_item2_title;
    lv_obj_t *cmode_item3;
    lv_obj_t *cmode_item3_icon;
    lv_obj_t *cmode_item3_title;
    lv_obj_t *climate_mode_back;
    lv_obj_t *climate_mode_back_icon;
    lv_obj_t *devices_hero;
    lv_obj_t *devices_hero_icon;
    lv_obj_t *devices_prev;
    lv_obj_t *devices_prev_icon;
    lv_obj_t *devices_next;
    lv_obj_t *devices_next_icon;
    lv_obj_t *devices_name;
    lv_obj_t *devices_value;
    lv_obj_t *devices_dot0;
    lv_obj_t *devices_dot1;
    lv_obj_t *devices_dot2;
    lv_obj_t *devices_dot3;
    lv_obj_t *devices_dot4;
    lv_obj_t *devices_dot5;
    lv_obj_t *devices_dot6;
    lv_obj_t *devices_dot7;
    lv_obj_t *devices_dot8;
    lv_obj_t *devices_dot9;
    lv_obj_t *devices_dot10;
    lv_obj_t *devices_dot11;
    lv_obj_t *devices_dot12;
    lv_obj_t *devices_dot13;
    lv_obj_t *devices_dot14;
    lv_obj_t *devices_back;
    lv_obj_t *devices_back_icon;
    lv_obj_t *energy_arc;
    lv_obj_t *energy_stack;
    lv_obj_t *energy_head;
    lv_obj_t *energy_head_icon;
    lv_obj_t *energy_title;
    lv_obj_t *energy_value_row;
    lv_obj_t *energy_value;
    lv_obj_t *energy_unit;
    lv_obj_t *energy_sub1;
    lv_obj_t *energy_sub2;
    lv_obj_t *energy_dot0;
    lv_obj_t *energy_dot1;
    lv_obj_t *energy_dot2;
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
    lv_obj_t *air_arc;
    lv_obj_t *air_title;
    lv_obj_t *air_value;
    lv_obj_t *air_unit;
    lv_obj_t *air_status;
    lv_obj_t *air_metrics;
    lv_obj_t *air_voc_col;
    lv_obj_t *air_voc_label;
    lv_obj_t *air_voc;
    lv_obj_t *air_humidity_col;
    lv_obj_t *air_humidity_label;
    lv_obj_t *air_humidity;
    lv_obj_t *air_temp_col;
    lv_obj_t *air_temp_label;
    lv_obj_t *air_temp;
    lv_obj_t *air_back;
    lv_obj_t *air_back_icon;
    lv_obj_t *level_title;
    lv_obj_t *level_well;
    lv_obj_t *level_cross_v;
    lv_obj_t *level_cross_h;
    lv_obj_t *level_ring;
    lv_obj_t *level_bubble;
    lv_obj_t *level_status;
    lv_obj_t *level_detail;
    lv_obj_t *level_back;
    lv_obj_t *level_back_icon;
    lv_obj_t *settings_title;
    lv_obj_t *settings_hero;
    lv_obj_t *settings_hero_icon;
    lv_obj_t *settings_prev;
    lv_obj_t *settings_prev_icon;
    lv_obj_t *settings_next;
    lv_obj_t *settings_next_icon;
    lv_obj_t *settings_name;
    lv_obj_t *settings_value;
    lv_obj_t *settings_dot0;
    lv_obj_t *settings_dot1;
    lv_obj_t *settings_dot2;
    lv_obj_t *settings_dot3;
    lv_obj_t *settings_dot4;
    lv_obj_t *settings_dot5;
    lv_obj_t *settings_back;
    lv_obj_t *settings_back_icon;
    lv_obj_t *alert_bg;
    lv_obj_t *alert_icon;
    lv_obj_t *alert_title;
    lv_obj_t *alert_message;
    lv_obj_t *alert_hint;
    lv_obj_t *setup_title;
    lv_obj_t *setup_ssid_label;
    lv_obj_t *setup_ssid;
    lv_obj_t *setup_pass_label;
    lv_obj_t *setup_pass;
    lv_obj_t *setup_url;
    lv_obj_t *setup_status;
} objects_t;

extern objects_t objects;

typedef struct {
    lv_scale_section_t *scale_section;
    lv_scale_section_t *scale_section1;
    lv_scale_section_t *scale_section2;
} screen_page_climate_state_t;

extern screen_page_climate_state_t screen_page_climate_state;

void create_screen_page_idle();
void tick_screen_page_idle();

void create_screen_page_menu();
void tick_screen_page_menu();

void create_screen_page_climate();
void tick_screen_page_climate();

void create_screen_page_climate_mode();
void tick_screen_page_climate_mode();

void create_screen_page_devices();
void tick_screen_page_devices();

void create_screen_page_energy();
void tick_screen_page_energy();

void create_screen_page_water();
void tick_screen_page_water();

void create_screen_page_air();
void tick_screen_page_air();

void create_screen_page_level();
void tick_screen_page_level();

void create_screen_page_settings();
void tick_screen_page_settings();

void create_screen_page_alert();
void tick_screen_page_alert();

void create_screen_page_setup();
void tick_screen_page_setup();

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
    COLOR_ID_ACCENT_TEXT,
    COLOR_ID_SOLAR_TEXT,
    COLOR_ID_INFO_TEXT,
    COLOR_ID_DANGER_TEXT,
};
void change_color_theme(uint32_t themeIndex);
extern uint32_t theme_colors[2][32];
extern uint32_t active_theme_index;

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/