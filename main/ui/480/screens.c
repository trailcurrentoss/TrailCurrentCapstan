#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;

screen_page_climate_state_t screen_page_climate_state;

//
// Event handlers
//

lv_obj_t *tick_value_change_obj;

//
// Screens
//

void create_screen_page_idle() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_idle = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 480);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // idle_ring
            lv_obj_t *obj = lv_scale_create(parent_obj);
            objects.idle_ring = obj;
            lv_obj_set_pos(obj, LV_PCT(2), LV_PCT(2));
            lv_obj_set_size(obj, LV_PCT(96), LV_PCT(96));
            lv_scale_set_mode(obj, LV_SCALE_MODE_ROUND_INNER);
            lv_scale_set_range(obj, 0, 60);
            lv_scale_set_angle_range(obj, 360);
            lv_scale_set_rotation(obj, 270);
            lv_scale_set_total_tick_count(obj, 61);
            lv_scale_set_major_tick_every(obj, 5);
            lv_scale_set_label_show(obj, false);
            add_style_scale_ring(obj);
        }
        {
            // idle_date
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.idle_date = obj;
            lv_obj_set_pos(obj, LV_PCT(20), LV_PCT(24));
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(8));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_muted(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "SAT 27 SEP");
        }
        {
            // idle_hand_hour
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.idle_hand_hour = obj;
            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
            lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
            static lv_point_precise_t line_points[] = {
                { 240, 240 },
                { 240, 105 }
            };
            lv_line_set_points(obj, line_points, 2);
            add_style_hand_hour(obj);
        }
        {
            // idle_hand_minute
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.idle_hand_minute = obj;
            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
            lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
            static lv_point_precise_t line_points[] = {
                { 240, 240 },
                { 240, 48 }
            };
            lv_line_set_points(obj, line_points, 2);
            add_style_hand_minute(obj);
        }
        {
            // idle_hand_second
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.idle_hand_second = obj;
            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
            lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
            static lv_point_precise_t line_points[] = {
                { 240, 240 },
                { 240, 28 }
            };
            lv_line_set_points(obj, line_points, 2);
            add_style_hand_second(obj);
        }
        {
            // idle_cap
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.idle_cap = obj;
            lv_obj_set_pos(obj, LV_PCT(48), LV_PCT(48));
            lv_obj_set_size(obj, LV_PCT(4), LV_PCT(4));
            add_style_pivot(obj);
        }
    }
    
    tick_screen_page_idle();
}

void tick_screen_page_idle() {
}

void create_screen_page_menu() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_menu = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 480);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // menu_hero
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_hero = obj;
            lv_obj_set_pos(obj, LV_PCT(36), LV_PCT(26));
            lv_obj_set_size(obj, LV_PCT(29), LV_PCT(29));
            add_style_hero_tile(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // menu_hero_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.menu_hero_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_hero_icon(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
        {
            // menu_prev
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_prev = obj;
            lv_obj_set_pos(obj, LV_PCT(5), LV_PCT(42));
            lv_obj_set_size(obj, LV_PCT(14), LV_PCT(14));
            add_style_plain(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // menu_prev_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.menu_prev_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_neighbour_icon(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
        {
            // menu_next
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_next = obj;
            lv_obj_set_pos(obj, LV_PCT(81), LV_PCT(42));
            lv_obj_set_size(obj, LV_PCT(14), LV_PCT(14));
            add_style_plain(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // menu_next_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.menu_next_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_neighbour_icon(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
        {
            // menu_title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.menu_title = obj;
            lv_obj_set_pos(obj, LV_PCT(22), LV_PCT(59));
            lv_obj_set_size(obj, LV_PCT(56), LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_menu_name(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Climate");
        }
        {
            // menu_summary
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.menu_summary = obj;
            lv_obj_set_pos(obj, LV_PCT(22), LV_PCT(69));
            lv_obj_set_size(obj, LV_PCT(56), LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_menu_summary(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Heating · 72°");
        }
        {
            // menu_dot0
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_dot0 = obj;
            lv_obj_set_pos(obj, 312, 435);
            lv_obj_set_size(obj, 10, 10);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // menu_dot1
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_dot1 = obj;
            lv_obj_set_pos(obj, 290, 442);
            lv_obj_set_size(obj, 10, 10);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // menu_dot2
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_dot2 = obj;
            lv_obj_set_pos(obj, 268, 446);
            lv_obj_set_size(obj, 10, 10);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // menu_dot3
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_dot3 = obj;
            lv_obj_set_pos(obj, 246, 449);
            lv_obj_set_size(obj, 10, 10);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // menu_dot4
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_dot4 = obj;
            lv_obj_set_pos(obj, 224, 449);
            lv_obj_set_size(obj, 10, 10);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // menu_dot5
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_dot5 = obj;
            lv_obj_set_pos(obj, 202, 446);
            lv_obj_set_size(obj, 10, 10);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // menu_dot6
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_dot6 = obj;
            lv_obj_set_pos(obj, 180, 442);
            lv_obj_set_size(obj, 10, 10);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // menu_dot7
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_dot7 = obj;
            lv_obj_set_pos(obj, 158, 435);
            lv_obj_set_size(obj, 10, 10);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_page_menu();
}

void tick_screen_page_menu() {
}

void create_screen_page_climate() {
    screen_page_climate_state_t *state = &screen_page_climate_state;
    (void)state;
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_climate = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 480);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // climate_ticks
            lv_obj_t *obj = lv_scale_create(parent_obj);
            objects.climate_ticks = obj;
            lv_obj_set_pos(obj, LV_PCT(4), LV_PCT(4));
            lv_obj_set_size(obj, LV_PCT(93), LV_PCT(93));
            lv_scale_set_mode(obj, LV_SCALE_MODE_ROUND_INNER);
            lv_scale_set_range(obj, 0, 80);
            lv_scale_set_angle_range(obj, 270);
            lv_scale_set_rotation(obj, 135);
            lv_scale_set_total_tick_count(obj, 81);
            lv_scale_set_major_tick_every(obj, 1);
            lv_scale_set_label_show(obj, false);
            {
                state->scale_section = lv_scale_add_section(obj);
                lv_scale_section_set_style(state->scale_section, LV_PART_MAIN, get_style_climate_section_heat_MAIN_DEFAULT());
                lv_scale_section_set_style(state->scale_section, LV_PART_INDICATOR, get_style_climate_section_heat_INDICATOR_DEFAULT());
            }
            {
                state->scale_section1 = lv_scale_add_section(obj);
                lv_scale_section_set_style(state->scale_section1, LV_PART_MAIN, get_style_climate_section_cool_MAIN_DEFAULT());
                lv_scale_section_set_style(state->scale_section1, LV_PART_INDICATOR, get_style_climate_section_cool_INDICATOR_DEFAULT());
            }
            {
                state->scale_section2 = lv_scale_add_section(obj);
                lv_scale_section_set_style(state->scale_section2, LV_PART_MAIN, get_style_climate_section_hold_MAIN_DEFAULT());
                lv_scale_section_set_style(state->scale_section2, LV_PART_INDICATOR, get_style_climate_section_hold_INDICATOR_DEFAULT());
            }
            add_style_climate_ticks(obj);
        }
        {
            // climate_needle_inside
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.climate_needle_inside = obj;
            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
            lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
            static lv_point_precise_t line_points[] = {
                { 182, 58 },
                { 173, 28 }
            };
            lv_line_set_points(obj, line_points, 2);
            add_style_climate_needle_inside(obj);
        }
        {
            // climate_needle_target
            lv_obj_t *obj = lv_line_create(parent_obj);
            objects.climate_needle_target = obj;
            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
            lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
            static lv_point_precise_t line_points[] = {
                { 281, 68 },
                { 292, 20 }
            };
            lv_line_set_points(obj, line_points, 2);
            add_style_climate_needle_target(obj);
        }
        {
            // climate_mode_row
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.climate_mode_row = obj;
            lv_obj_set_pos(obj, LV_PCT(15), LV_PCT(29));
            lv_obj_set_size(obj, LV_PCT(70), LV_PCT(4));
            add_style_plain(obj);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // climate_mode_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.climate_mode_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_climate_mode_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // climate_mode
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.climate_mode = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_climate_mode_text(obj);
                    lv_label_set_text(obj, "HEATING");
                }
            }
        }
        {
            // climate_setpoint
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.climate_setpoint = obj;
            lv_obj_set_pos(obj, LV_PCT(10), LV_PCT(37));
            lv_obj_set_size(obj, LV_PCT(80), LV_PCT(23));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
            add_style_climate_num(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "72");
        }
        {
            // climate_info_row
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.climate_info_row = obj;
            lv_obj_set_pos(obj, LV_PCT(15), LV_PCT(62));
            lv_obj_set_size(obj, LV_PCT(70), LV_PCT(4));
            add_style_plain(obj);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 18, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // climate_inside
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.climate_inside = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_label_body(obj);
                    lv_label_set_text(obj, "Inside 67°");
                }
                {
                    // climate_eta_group
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.climate_eta_group = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_plain(obj);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // climate_eta_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.climate_eta_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // climate_eta
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.climate_eta = obj;
                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_label_set_text(obj, "28 min");
                        }
                    }
                }
            }
        }
        {
            // climate_back
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.climate_back = obj;
            lv_obj_set_pos(obj, LV_PCT(42), LV_PCT(84));
            lv_obj_set_size(obj, LV_PCT(16), LV_PCT(11));
            lv_obj_add_event_cb(obj, action_nav_back, LV_EVENT_CLICKED, (void *)0);
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // climate_back_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.climate_back_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(20));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(60));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_label_icon_sm(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
    }
    
    tick_screen_page_climate();
}

void tick_screen_page_climate() {
    screen_page_climate_state_t *state = &screen_page_climate_state;
    (void)state;
    {
        int32_t new_min_val = get_var_climate_heat_min();
        int32_t new_max_val = get_var_climate_heat_max();
        lv_scale_section_set_range(state->scale_section, new_min_val, new_max_val);
    }
    {
        int32_t new_min_val = get_var_climate_cool_min();
        int32_t new_max_val = get_var_climate_cool_max();
        lv_scale_section_set_range(state->scale_section1, new_min_val, new_max_val);
    }
    {
        int32_t new_min_val = get_var_climate_hold_min();
        int32_t new_max_val = get_var_climate_hold_max();
        lv_scale_section_set_range(state->scale_section2, new_min_val, new_max_val);
    }
}

void create_screen_page_climate_mode() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_climate_mode = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 480);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // cmode_title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.cmode_title = obj;
            lv_obj_set_pos(obj, LV_PCT(20), LV_PCT(18));
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(4));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_eyebrow(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "MODE");
        }
        {
            // cmode_list
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.cmode_list = obj;
            lv_obj_set_pos(obj, LV_PCT(25), LV_PCT(25));
            lv_obj_set_size(obj, LV_PCT(50), LV_PCT(56));
            add_style_plain(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // cmode_item0
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.cmode_item0 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(22));
                    add_style_mode_row_heat(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // cmode_item0_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.cmode_item0_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(8), LV_PCT(0));
                            lv_obj_set_size(obj, LV_PCT(10), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_mode_row_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // cmode_item0_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.cmode_item0_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(0));
                            lv_obj_set_size(obj, LV_PCT(68), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_mode_row_text(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Heat");
                        }
                    }
                }
                {
                    // cmode_item1
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.cmode_item1 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(26));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(22));
                    add_style_mode_row_cool(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // cmode_item1_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.cmode_item1_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(8), LV_PCT(0));
                            lv_obj_set_size(obj, LV_PCT(10), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_mode_row_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // cmode_item1_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.cmode_item1_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(0));
                            lv_obj_set_size(obj, LV_PCT(68), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_mode_row_text(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Cool");
                        }
                    }
                }
                {
                    // cmode_item2
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.cmode_item2 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(52));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(22));
                    add_style_mode_row_auto(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // cmode_item2_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.cmode_item2_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(8), LV_PCT(0));
                            lv_obj_set_size(obj, LV_PCT(10), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_mode_row_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // cmode_item2_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.cmode_item2_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(0));
                            lv_obj_set_size(obj, LV_PCT(68), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_mode_row_text(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Auto");
                        }
                    }
                }
                {
                    // cmode_item3
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.cmode_item3 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(78));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(22));
                    add_style_mode_row_off(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // cmode_item3_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.cmode_item3_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(8), LV_PCT(0));
                            lv_obj_set_size(obj, LV_PCT(10), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_mode_row_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // cmode_item3_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.cmode_item3_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(0));
                            lv_obj_set_size(obj, LV_PCT(68), LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_mode_row_text(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Off");
                        }
                    }
                }
            }
        }
        {
            // climate_mode_back
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.climate_mode_back = obj;
            lv_obj_set_pos(obj, LV_PCT(42), LV_PCT(84));
            lv_obj_set_size(obj, LV_PCT(16), LV_PCT(11));
            lv_obj_add_event_cb(obj, action_nav_back, LV_EVENT_CLICKED, (void *)0);
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // climate_mode_back_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.climate_mode_back_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(20));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(60));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_label_icon_sm(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
    }
    
    tick_screen_page_climate_mode();
}

void tick_screen_page_climate_mode() {
}

void create_screen_page_devices() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_devices = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 480);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // devices_hero
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.devices_hero = obj;
            lv_obj_set_pos(obj, LV_PCT(34), LV_PCT(25));
            lv_obj_set_size(obj, LV_PCT(31), LV_PCT(31));
            add_style_device_tile(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // devices_hero_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.devices_hero_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_device_icon(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
        {
            // devices_prev
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.devices_prev = obj;
            lv_obj_set_pos(obj, LV_PCT(5), LV_PCT(42));
            lv_obj_set_size(obj, LV_PCT(14), LV_PCT(14));
            add_style_plain(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // devices_prev_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.devices_prev_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_device_neighbour_icon(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
        {
            // devices_next
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.devices_next = obj;
            lv_obj_set_pos(obj, LV_PCT(81), LV_PCT(42));
            lv_obj_set_size(obj, LV_PCT(14), LV_PCT(14));
            add_style_plain(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // devices_next_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.devices_next_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_device_neighbour_icon(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
        {
            // devices_name
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.devices_name = obj;
            lv_obj_set_pos(obj, LV_PCT(22), LV_PCT(60));
            lv_obj_set_size(obj, LV_PCT(56), LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_title(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Kitchen");
        }
        {
            // devices_value
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.devices_value = obj;
            lv_obj_set_pos(obj, LV_PCT(22), LV_PCT(70));
            lv_obj_set_size(obj, LV_PCT(56), LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_body(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "On");
        }
        {
            // devices_dot0
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.devices_dot0 = obj;
            lv_obj_set_pos(obj, 158, 35);
            lv_obj_set_size(obj, 10, 10);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // devices_dot1
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.devices_dot1 = obj;
            lv_obj_set_pos(obj, 169, 31);
            lv_obj_set_size(obj, 10, 10);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // devices_dot2
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.devices_dot2 = obj;
            lv_obj_set_pos(obj, 180, 28);
            lv_obj_set_size(obj, 10, 10);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // devices_dot3
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.devices_dot3 = obj;
            lv_obj_set_pos(obj, 190, 26);
            lv_obj_set_size(obj, 10, 10);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // devices_dot4
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.devices_dot4 = obj;
            lv_obj_set_pos(obj, 202, 24);
            lv_obj_set_size(obj, 10, 10);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // devices_dot5
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.devices_dot5 = obj;
            lv_obj_set_pos(obj, 213, 22);
            lv_obj_set_size(obj, 10, 10);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // devices_dot6
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.devices_dot6 = obj;
            lv_obj_set_pos(obj, 224, 21);
            lv_obj_set_size(obj, 10, 10);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // devices_dot7
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.devices_dot7 = obj;
            lv_obj_set_pos(obj, 235, 21);
            lv_obj_set_size(obj, 10, 10);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // devices_dot8
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.devices_dot8 = obj;
            lv_obj_set_pos(obj, 246, 21);
            lv_obj_set_size(obj, 10, 10);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // devices_dot9
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.devices_dot9 = obj;
            lv_obj_set_pos(obj, 257, 22);
            lv_obj_set_size(obj, 10, 10);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // devices_dot10
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.devices_dot10 = obj;
            lv_obj_set_pos(obj, 268, 24);
            lv_obj_set_size(obj, 10, 10);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // devices_dot11
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.devices_dot11 = obj;
            lv_obj_set_pos(obj, 280, 26);
            lv_obj_set_size(obj, 10, 10);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // devices_dot12
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.devices_dot12 = obj;
            lv_obj_set_pos(obj, 290, 28);
            lv_obj_set_size(obj, 10, 10);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // devices_dot13
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.devices_dot13 = obj;
            lv_obj_set_pos(obj, 301, 31);
            lv_obj_set_size(obj, 10, 10);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // devices_dot14
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.devices_dot14 = obj;
            lv_obj_set_pos(obj, 312, 35);
            lv_obj_set_size(obj, 10, 10);
            add_style_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // devices_back
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.devices_back = obj;
            lv_obj_set_pos(obj, LV_PCT(42), LV_PCT(84));
            lv_obj_set_size(obj, LV_PCT(16), LV_PCT(11));
            lv_obj_add_event_cb(obj, action_nav_back, LV_EVENT_CLICKED, (void *)0);
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // devices_back_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.devices_back_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(20));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(60));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_label_icon_sm(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
    }
    
    tick_screen_page_devices();
}

void tick_screen_page_devices() {
}

void create_screen_page_energy() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_energy = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 480);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // energy_arc
            lv_obj_t *obj = lv_arc_create(parent_obj);
            objects.energy_arc = obj;
            lv_obj_set_pos(obj, LV_PCT(4), LV_PCT(4));
            lv_obj_set_size(obj, LV_PCT(92), LV_PCT(92));
            lv_arc_set_value(obj, 82);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            add_style_energy_arc(obj);
        }
        {
            // energy_stack
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.energy_stack = obj;
            lv_obj_set_pos(obj, LV_PCT(16), LV_PCT(16));
            lv_obj_set_size(obj, LV_PCT(68), LV_PCT(68));
            add_style_plain(obj);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // energy_head
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.energy_head = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_plain(obj);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // energy_head_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.energy_head_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_energy_head_icon(obj);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // energy_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.energy_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_energy_head(obj);
                            lv_label_set_text(obj, "BATTERY");
                        }
                    }
                }
                {
                    // energy_value_row
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.energy_value_row = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_plain(obj);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_END, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // energy_value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.energy_value = obj;
                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_hero_xl(obj);
                            lv_label_set_text(obj, "82");
                        }
                        {
                            // energy_unit
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.energy_unit = obj;
                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_energy_unit(obj);
                            lv_label_set_text(obj, "%");
                        }
                    }
                }
                {
                    // energy_sub1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.energy_sub1 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_label_body(obj);
                    lv_label_set_text(obj, "13.4 V · Float");
                }
                {
                    // energy_sub2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.energy_sub2 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_label_body_muted(obj);
                    lv_label_set_text(obj, "Time Remaining 14h 20m");
                }
            }
        }
        {
            // energy_dot0
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.energy_dot0 = obj;
            lv_obj_set_pos(obj, 215, 435);
            lv_obj_set_size(obj, 8, 8);
            add_style_energy_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // energy_dot1
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.energy_dot1 = obj;
            lv_obj_set_pos(obj, 236, 436);
            lv_obj_set_size(obj, 8, 8);
            add_style_energy_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // energy_dot2
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.energy_dot2 = obj;
            lv_obj_set_pos(obj, 257, 435);
            lv_obj_set_size(obj, 8, 8);
            add_style_energy_dot(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // energy_back
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.energy_back = obj;
            lv_obj_set_pos(obj, LV_PCT(42), LV_PCT(84));
            lv_obj_set_size(obj, LV_PCT(16), LV_PCT(11));
            lv_obj_add_event_cb(obj, action_nav_back, LV_EVENT_CLICKED, (void *)0);
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // energy_back_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.energy_back_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(20));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(60));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_label_icon_sm(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
    }
    
    tick_screen_page_energy();
}

void tick_screen_page_energy() {
}

void create_screen_page_water() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_water = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 480);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // water_title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.water_title = obj;
            lv_obj_set_pos(obj, LV_PCT(20), LV_PCT(15));
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(4));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_eyebrow(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "WATER TANKS");
        }
        {
            // water_fresh_value
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.water_fresh_value = obj;
            lv_obj_set_pos(obj, LV_PCT(17), LV_PCT(25));
            lv_obj_set_size(obj, LV_PCT(22), LV_PCT(7));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_title(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "--");
        }
        {
            // water_fresh_bar
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.water_fresh_bar = obj;
            lv_obj_set_pos(obj, LV_PCT(21), LV_PCT(33));
            lv_obj_set_size(obj, LV_PCT(13), LV_PCT(35));
            add_style_bar_fresh(obj);
        }
        {
            // water_fresh_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.water_fresh_label = obj;
            lv_obj_set_pos(obj, LV_PCT(17), LV_PCT(71));
            lv_obj_set_size(obj, LV_PCT(22), LV_PCT(4));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_body(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Fresh");
        }
        {
            // water_grey_value
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.water_grey_value = obj;
            lv_obj_set_pos(obj, LV_PCT(39), LV_PCT(25));
            lv_obj_set_size(obj, LV_PCT(22), LV_PCT(7));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_title(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "--");
        }
        {
            // water_grey_bar
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.water_grey_bar = obj;
            lv_obj_set_pos(obj, LV_PCT(43), LV_PCT(33));
            lv_obj_set_size(obj, LV_PCT(13), LV_PCT(35));
            add_style_bar_grey(obj);
        }
        {
            // water_grey_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.water_grey_label = obj;
            lv_obj_set_pos(obj, LV_PCT(39), LV_PCT(71));
            lv_obj_set_size(obj, LV_PCT(22), LV_PCT(4));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_body(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Grey");
        }
        {
            // water_black_value
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.water_black_value = obj;
            lv_obj_set_pos(obj, LV_PCT(62), LV_PCT(25));
            lv_obj_set_size(obj, LV_PCT(22), LV_PCT(7));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_title(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "--");
        }
        {
            // water_black_bar
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.water_black_bar = obj;
            lv_obj_set_pos(obj, LV_PCT(66), LV_PCT(33));
            lv_obj_set_size(obj, LV_PCT(13), LV_PCT(35));
            add_style_bar_black(obj);
        }
        {
            // water_black_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.water_black_label = obj;
            lv_obj_set_pos(obj, LV_PCT(62), LV_PCT(71));
            lv_obj_set_size(obj, LV_PCT(22), LV_PCT(4));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_body(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Black");
        }
        {
            // water_back
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.water_back = obj;
            lv_obj_set_pos(obj, LV_PCT(42), LV_PCT(84));
            lv_obj_set_size(obj, LV_PCT(16), LV_PCT(11));
            lv_obj_add_event_cb(obj, action_nav_back, LV_EVENT_CLICKED, (void *)0);
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // water_back_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.water_back_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(20));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(60));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_label_icon_sm(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
    }
    
    tick_screen_page_water();
}

void tick_screen_page_water() {
}

void create_screen_page_air() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_air = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 480);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // air_arc
            lv_obj_t *obj = lv_arc_create(parent_obj);
            objects.air_arc = obj;
            lv_obj_set_pos(obj, LV_PCT(4), LV_PCT(4));
            lv_obj_set_size(obj, LV_PCT(92), LV_PCT(92));
            lv_arc_set_range(obj, 400, 2000);
            lv_arc_set_value(obj, 640);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            add_style_arc_thin(obj);
        }
        {
            // air_title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.air_title = obj;
            lv_obj_set_pos(obj, LV_PCT(25), LV_PCT(24));
            lv_obj_set_size(obj, LV_PCT(50), LV_PCT(4));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_eyebrow(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "AIR QUALITY");
        }
        {
            // air_value
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.air_value = obj;
            lv_obj_set_pos(obj, LV_PCT(15), LV_PCT(30));
            lv_obj_set_size(obj, LV_PCT(70), LV_PCT(19));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
            add_style_label_hero_xl(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "640");
        }
        {
            // air_unit
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.air_unit = obj;
            lv_obj_set_pos(obj, LV_PCT(40), LV_PCT(53));
            lv_obj_set_size(obj, LV_PCT(20), LV_PCT(4));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_muted(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "ppm");
        }
        {
            // air_status
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.air_status = obj;
            lv_obj_set_pos(obj, LV_PCT(20), LV_PCT(58));
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(5));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_status(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Good");
        }
        {
            // air_metrics
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.air_metrics = obj;
            lv_obj_set_pos(obj, LV_PCT(18), LV_PCT(68));
            lv_obj_set_size(obj, LV_PCT(64), LV_PCT(8));
            add_style_plain(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // air_voc_col
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.air_voc_col = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_PCT(33), LV_PCT(100));
                    add_style_plain(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // air_voc_label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.air_voc_label = obj;
                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                            lv_obj_set_size(obj, LV_PCT(100), LV_PCT(46));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_muted(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "VOC");
                        }
                        {
                            // air_voc
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.air_voc = obj;
                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(46));
                            lv_obj_set_size(obj, LV_PCT(100), LV_PCT(54));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_metric(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "120 ppb");
                        }
                    }
                }
                {
                    // air_humidity_col
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.air_humidity_col = obj;
                    lv_obj_set_pos(obj, LV_PCT(33), LV_PCT(0));
                    lv_obj_set_size(obj, LV_PCT(33), LV_PCT(100));
                    add_style_plain(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // air_humidity_label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.air_humidity_label = obj;
                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                            lv_obj_set_size(obj, LV_PCT(100), LV_PCT(46));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_muted(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "Humidity");
                        }
                        {
                            // air_humidity
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.air_humidity = obj;
                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(46));
                            lv_obj_set_size(obj, LV_PCT(100), LV_PCT(54));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_metric(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "44%");
                        }
                    }
                }
                {
                    // air_temp_col
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.air_temp_col = obj;
                    lv_obj_set_pos(obj, LV_PCT(67), LV_PCT(0));
                    lv_obj_set_size(obj, LV_PCT(33), LV_PCT(100));
                    add_style_plain(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // air_temp_label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.air_temp_label = obj;
                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                            lv_obj_set_size(obj, LV_PCT(100), LV_PCT(46));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_muted(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "Temp");
                        }
                        {
                            // air_temp
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.air_temp = obj;
                            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(46));
                            lv_obj_set_size(obj, LV_PCT(100), LV_PCT(54));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_metric(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "70°F");
                        }
                    }
                }
            }
        }
        {
            // air_back
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.air_back = obj;
            lv_obj_set_pos(obj, LV_PCT(42), LV_PCT(84));
            lv_obj_set_size(obj, LV_PCT(16), LV_PCT(11));
            lv_obj_add_event_cb(obj, action_nav_back, LV_EVENT_CLICKED, (void *)0);
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // air_back_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.air_back_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(20));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(60));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_label_icon_sm(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
    }
    
    tick_screen_page_air();
}

void tick_screen_page_air() {
}

void create_screen_page_level() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_level = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 480);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // level_title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.level_title = obj;
            lv_obj_set_pos(obj, LV_PCT(20), LV_PCT(13));
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(4));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_eyebrow(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "LEVELING");
        }
        {
            // level_well
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.level_well = obj;
            lv_obj_set_pos(obj, LV_PCT(27), LV_PCT(23));
            lv_obj_set_size(obj, LV_PCT(46), LV_PCT(46));
            add_style_level_well(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // level_cross_v
                    lv_obj_t *obj = lv_line_create(parent_obj);
                    objects.level_cross_v = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
                    static lv_point_precise_t line_points[] = {
                        { 108, 0 },
                        { 108, 216 }
                    };
                    lv_line_set_points(obj, line_points, 2);
                    add_style_level_line(obj);
                }
                {
                    // level_cross_h
                    lv_obj_t *obj = lv_line_create(parent_obj);
                    objects.level_cross_h = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
                    static lv_point_precise_t line_points[] = {
                        { 0, 108 },
                        { 216, 108 }
                    };
                    lv_line_set_points(obj, line_points, 2);
                    add_style_level_line(obj);
                }
                {
                    // level_ring
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.level_ring = obj;
                    lv_obj_set_pos(obj, 78, 78);
                    lv_obj_set_size(obj, 60, 60);
                    add_style_level_ring(obj);
                }
                {
                    // level_bubble
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.level_bubble = obj;
                    lv_obj_set_pos(obj, 86, 86);
                    lv_obj_set_size(obj, 44, 44);
                    add_style_level_bubble(obj);
                }
            }
        }
        {
            // level_status
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.level_status = obj;
            lv_obj_set_pos(obj, LV_PCT(18), LV_PCT(71));
            lv_obj_set_size(obj, LV_PCT(64), LV_PCT(5));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_status(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "--");
        }
        {
            // level_detail
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.level_detail = obj;
            lv_obj_set_pos(obj, LV_PCT(18), LV_PCT(77));
            lv_obj_set_size(obj, LV_PCT(64), LV_PCT(4));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_body(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "--");
        }
        {
            // level_back
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.level_back = obj;
            lv_obj_set_pos(obj, LV_PCT(42), LV_PCT(84));
            lv_obj_set_size(obj, LV_PCT(16), LV_PCT(11));
            lv_obj_add_event_cb(obj, action_nav_back, LV_EVENT_CLICKED, (void *)0);
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // level_back_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.level_back_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(20));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(60));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_label_icon_sm(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
    }
    
    tick_screen_page_level();
}

void tick_screen_page_level() {
}

void create_screen_page_settings() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_settings = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 480);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // settings_title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.settings_title = obj;
            lv_obj_set_pos(obj, LV_PCT(20), LV_PCT(13));
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(4));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_eyebrow(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "SETTINGS");
        }
        {
            // settings_list
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_list = obj;
            lv_obj_set_pos(obj, LV_PCT(16), LV_PCT(24));
            lv_obj_set_size(obj, LV_PCT(68), LV_PCT(60));
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
            add_style_plain(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // settings_item0
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.settings_item0 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // settings_item0_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_item0_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(26));
                            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(48));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // settings_item0_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_item0_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(42), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Wi-Fi");
                        }
                        {
                            // settings_item0_value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_item0_value = obj;
                            lv_obj_set_pos(obj, LV_PCT(66), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(32), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_muted(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "--");
                        }
                    }
                }
                {
                    // settings_item1
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.settings_item1 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(25));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // settings_item1_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_item1_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(26));
                            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(48));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // settings_item1_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_item1_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(42), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "MQTT");
                        }
                        {
                            // settings_item1_value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_item1_value = obj;
                            lv_obj_set_pos(obj, LV_PCT(66), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(32), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_muted(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "--");
                        }
                    }
                }
                {
                    // settings_item2
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.settings_item2 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(50));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // settings_item2_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_item2_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(26));
                            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(48));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // settings_item2_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_item2_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(42), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Theme");
                        }
                        {
                            // settings_item2_value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_item2_value = obj;
                            lv_obj_set_pos(obj, LV_PCT(66), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(32), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_muted(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Light");
                        }
                    }
                }
                {
                    // settings_item3
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.settings_item3 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(75));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // settings_item3_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_item3_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(26));
                            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(48));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // settings_item3_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_item3_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(42), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Alarm Snooze");
                        }
                        {
                            // settings_item3_value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_item3_value = obj;
                            lv_obj_set_pos(obj, LV_PCT(66), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(32), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_muted(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "--");
                        }
                    }
                }
                {
                    // settings_item4
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.settings_item4 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(100));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // settings_item4_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_item4_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(26));
                            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(48));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // settings_item4_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_item4_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(42), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Factory Reset");
                        }
                    }
                }
            }
        }
        {
            // settings_back
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_back = obj;
            lv_obj_set_pos(obj, LV_PCT(42), LV_PCT(84));
            lv_obj_set_size(obj, LV_PCT(16), LV_PCT(11));
            lv_obj_add_event_cb(obj, action_nav_back, LV_EVENT_CLICKED, (void *)0);
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // settings_back_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.settings_back_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(20));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(60));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_label_icon_sm(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
    }
    
    tick_screen_page_settings();
}

void tick_screen_page_settings() {
}

void create_screen_page_alert() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_alert = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 480);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // alert_bg
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.alert_bg = obj;
            lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
            lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
            add_style_alert_ring(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // alert_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.alert_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(30), LV_PCT(27));
                    lv_obj_set_size(obj, LV_PCT(40), LV_PCT(16));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_alert_icon(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // alert_title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.alert_title = obj;
                    lv_obj_set_pos(obj, LV_PCT(12), LV_PCT(46));
                    lv_obj_set_size(obj, LV_PCT(76), LV_PCT(9));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_label_title(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "--");
                }
                {
                    // alert_message
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.alert_message = obj;
                    lv_obj_set_pos(obj, LV_PCT(12), LV_PCT(56));
                    lv_obj_set_size(obj, LV_PCT(76), LV_PCT(13));
                    add_style_label_body(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "--");
                }
                {
                    // alert_hint
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.alert_hint = obj;
                    lv_obj_set_pos(obj, LV_PCT(15), LV_PCT(70));
                    lv_obj_set_size(obj, LV_PCT(70), LV_PCT(7));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_label_muted(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Press to dismiss");
                }
            }
        }
    }
    
    tick_screen_page_alert();
}

void tick_screen_page_alert() {
}

void create_screen_page_setup() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_setup = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 480);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // setup_title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.setup_title = obj;
            lv_obj_set_pos(obj, LV_PCT(20), LV_PCT(13));
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(4));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_eyebrow(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "WI-FI SETUP");
        }
        {
            // setup_ssid_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.setup_ssid_label = obj;
            lv_obj_set_pos(obj, LV_PCT(16), LV_PCT(25));
            lv_obj_set_size(obj, LV_PCT(68), LV_PCT(7));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_muted(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Join this network");
        }
        {
            // setup_ssid
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.setup_ssid = obj;
            lv_obj_set_pos(obj, LV_PCT(12), LV_PCT(32));
            lv_obj_set_size(obj, LV_PCT(76), LV_PCT(10));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_title(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Capstan-------");
        }
        {
            // setup_pass_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.setup_pass_label = obj;
            lv_obj_set_pos(obj, LV_PCT(16), LV_PCT(45));
            lv_obj_set_size(obj, LV_PCT(68), LV_PCT(7));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_muted(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Password");
        }
        {
            // setup_pass
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.setup_pass = obj;
            lv_obj_set_pos(obj, LV_PCT(12), LV_PCT(52));
            lv_obj_set_size(obj, LV_PCT(76), LV_PCT(10));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_title(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "--------");
        }
        {
            // setup_url
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.setup_url = obj;
            lv_obj_set_pos(obj, LV_PCT(10), LV_PCT(65));
            lv_obj_set_size(obj, LV_PCT(80), LV_PCT(8));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_body(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "http://192.168.4.1");
        }
        {
            // setup_status
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.setup_status = obj;
            lv_obj_set_pos(obj, LV_PCT(12), LV_PCT(74));
            lv_obj_set_size(obj, LV_PCT(76), LV_PCT(8));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_muted(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Waiting for phone...");
        }
    }
    
    tick_screen_page_setup();
}

void tick_screen_page_setup() {
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_page_idle,
    tick_screen_page_menu,
    tick_screen_page_climate,
    tick_screen_page_climate_mode,
    tick_screen_page_devices,
    tick_screen_page_energy,
    tick_screen_page_water,
    tick_screen_page_air,
    tick_screen_page_level,
    tick_screen_page_settings,
    tick_screen_page_alert,
    tick_screen_page_setup,
};
void tick_screen(int screen_index) {
    if (screen_index >= 0 && screen_index < 12) {
        tick_screen_funcs[screen_index]();
    }
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen(screenId - 1);
}

//
// Fonts
//

ext_font_desc_t fonts[] = {
    { "rl72", &ui_font_rl72 },
    { "rn150", &ui_font_rn150 },
    { "rn120", &ui_font_rn120 },
    { "rm34", &ui_font_rm34 },
    { "rm26", &ui_font_rm26 },
    { "rm20", &ui_font_rm20 },
    { "rr30", &ui_font_rr30 },
    { "rr24", &ui_font_rr24 },
    { "rr20", &ui_font_rr20 },
    { "rr16", &ui_font_rr16 },
    { "rr13", &ui_font_rr13 },
    { "fa56", &ui_font_fa56 },
    { "fa36", &ui_font_fa36 },
    { "fa24", &ui_font_fa24 },
    { "fa18", &ui_font_fa18 },
    { "fa14", &ui_font_fa14 },
    { "fh68", &ui_font_fh68 },
    { "fh44", &ui_font_fh44 },
#if LV_FONT_MONTSERRAT_8
    { "MONTSERRAT_8", &lv_font_montserrat_8 },
#endif
#if LV_FONT_MONTSERRAT_10
    { "MONTSERRAT_10", &lv_font_montserrat_10 },
#endif
#if LV_FONT_MONTSERRAT_12
    { "MONTSERRAT_12", &lv_font_montserrat_12 },
#endif
#if LV_FONT_MONTSERRAT_14
    { "MONTSERRAT_14", &lv_font_montserrat_14 },
#endif
#if LV_FONT_MONTSERRAT_16
    { "MONTSERRAT_16", &lv_font_montserrat_16 },
#endif
#if LV_FONT_MONTSERRAT_18
    { "MONTSERRAT_18", &lv_font_montserrat_18 },
#endif
#if LV_FONT_MONTSERRAT_20
    { "MONTSERRAT_20", &lv_font_montserrat_20 },
#endif
#if LV_FONT_MONTSERRAT_22
    { "MONTSERRAT_22", &lv_font_montserrat_22 },
#endif
#if LV_FONT_MONTSERRAT_24
    { "MONTSERRAT_24", &lv_font_montserrat_24 },
#endif
#if LV_FONT_MONTSERRAT_26
    { "MONTSERRAT_26", &lv_font_montserrat_26 },
#endif
#if LV_FONT_MONTSERRAT_28
    { "MONTSERRAT_28", &lv_font_montserrat_28 },
#endif
#if LV_FONT_MONTSERRAT_30
    { "MONTSERRAT_30", &lv_font_montserrat_30 },
#endif
#if LV_FONT_MONTSERRAT_32
    { "MONTSERRAT_32", &lv_font_montserrat_32 },
#endif
#if LV_FONT_MONTSERRAT_34
    { "MONTSERRAT_34", &lv_font_montserrat_34 },
#endif
#if LV_FONT_MONTSERRAT_36
    { "MONTSERRAT_36", &lv_font_montserrat_36 },
#endif
#if LV_FONT_MONTSERRAT_38
    { "MONTSERRAT_38", &lv_font_montserrat_38 },
#endif
#if LV_FONT_MONTSERRAT_40
    { "MONTSERRAT_40", &lv_font_montserrat_40 },
#endif
#if LV_FONT_MONTSERRAT_42
    { "MONTSERRAT_42", &lv_font_montserrat_42 },
#endif
#if LV_FONT_MONTSERRAT_44
    { "MONTSERRAT_44", &lv_font_montserrat_44 },
#endif
#if LV_FONT_MONTSERRAT_46
    { "MONTSERRAT_46", &lv_font_montserrat_46 },
#endif
#if LV_FONT_MONTSERRAT_48
    { "MONTSERRAT_48", &lv_font_montserrat_48 },
#endif
};

//
// Color themes
//

uint32_t active_theme_index = 0;
void change_color_theme(uint32_t theme_index) {
    active_theme_index = theme_index;
    
    lv_style_set_bg_color(get_style_screen_root_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][1]));
    lv_style_set_text_color(get_style_screen_root_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_bg_color(get_style_card_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_card_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_card_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_bg_color(get_style_card_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][10]));
    lv_style_set_border_color(get_style_card_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_card_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_bg_color(get_style_card_selected_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][10]));
    lv_style_set_border_color(get_style_card_selected_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_card_selected_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_text_color(get_style_label_title_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_text_color(get_style_label_body_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_text_color(get_style_label_muted_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_text_color(get_style_label_hero_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_bg_color(get_style_button_primary_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_button_primary_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][27]));
    lv_style_set_bg_color(get_style_button_neutral_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_button_neutral_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_button_neutral_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_line_color(get_style_scale_ring_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][25]));
    lv_style_set_line_color(get_style_scale_ring_ITEMS_DEFAULT(), lv_color_hex(theme_colors[theme_index][25]));
    lv_style_set_line_color(get_style_scale_ring_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][16]));
    lv_style_set_text_color(get_style_scale_ring_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][16]));
    lv_style_set_text_color(get_style_label_icon_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_text_color(get_style_label_icon_sm_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_bg_color(get_style_hero_tile_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_hero_tile_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_shadow_color(get_style_hero_tile_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_hero_icon_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_neighbour_icon_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_bg_color(get_style_device_tile_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_device_tile_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_bg_color(get_style_device_tile_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][10]));
    lv_style_set_border_color(get_style_device_tile_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_shadow_color(get_style_device_tile_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_device_icon_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_text_color(get_style_device_icon_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_device_neighbour_icon_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_bg_color(get_style_alert_ring_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][1]));
    lv_style_set_border_color(get_style_alert_ring_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][14]));
    lv_style_set_text_color(get_style_alert_ring_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_bg_color(get_style_level_well_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][27]));
    lv_style_set_border_color(get_style_level_well_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_border_color(get_style_level_ring_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_line_color(get_style_level_line_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_bg_color(get_style_level_bubble_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][11]));
    lv_style_set_shadow_color(get_style_level_bubble_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][11]));
    lv_style_set_line_color(get_style_climate_ticks_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_line_color(get_style_climate_section_heat_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][14]));
    lv_style_set_line_color(get_style_climate_section_cool_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][13]));
    lv_style_set_line_color(get_style_climate_section_hold_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_line_color(get_style_climate_needle_inside_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_line_color(get_style_climate_needle_target_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_text_color(get_style_climate_num_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_text_color(get_style_climate_mode_text_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][28]));
    lv_style_set_text_color(get_style_climate_mode_text_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][31]));
    lv_style_set_text_color(get_style_climate_mode_text_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][30]));
    lv_style_set_text_color(get_style_climate_mode_text_MAIN_DISABLED(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_text_color(get_style_climate_mode_icon_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][28]));
    lv_style_set_text_color(get_style_climate_mode_icon_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][31]));
    lv_style_set_text_color(get_style_climate_mode_icon_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][30]));
    lv_style_set_text_color(get_style_climate_mode_icon_MAIN_DISABLED(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_arc_color(get_style_energy_arc_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_arc_color(get_style_energy_arc_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_arc_color(get_style_energy_arc_INDICATOR_CHECKED(), lv_color_hex(theme_colors[theme_index][11]));
    lv_style_set_arc_color(get_style_energy_arc_INDICATOR_PRESSED(), lv_color_hex(theme_colors[theme_index][13]));
    lv_style_set_arc_color(get_style_energy_arc_INDICATOR_DISABLED(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_energy_head_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][28]));
    lv_style_set_text_color(get_style_energy_head_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][29]));
    lv_style_set_text_color(get_style_energy_head_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][30]));
    lv_style_set_text_color(get_style_energy_head_icon_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][28]));
    lv_style_set_text_color(get_style_energy_head_icon_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][29]));
    lv_style_set_text_color(get_style_energy_head_icon_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][30]));
    lv_style_set_text_color(get_style_energy_unit_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_text_color(get_style_menu_name_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_text_color(get_style_menu_summary_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_border_color(get_style_mode_row_heat_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_mode_row_heat_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_bg_color(get_style_mode_row_heat_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_mode_row_heat_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][14]));
    lv_style_set_text_color(get_style_mode_row_heat_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][31]));
    lv_style_set_border_color(get_style_mode_row_cool_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_mode_row_cool_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_bg_color(get_style_mode_row_cool_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_mode_row_cool_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][13]));
    lv_style_set_text_color(get_style_mode_row_cool_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][30]));
    lv_style_set_border_color(get_style_mode_row_auto_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_mode_row_auto_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_bg_color(get_style_mode_row_auto_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_mode_row_auto_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_mode_row_auto_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][28]));
    lv_style_set_border_color(get_style_mode_row_off_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_mode_row_off_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_bg_color(get_style_mode_row_off_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_mode_row_off_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_text_color(get_style_mode_row_off_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_text_color(get_style_label_body_muted_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_bg_color(get_style_energy_dot_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_bg_color(get_style_energy_dot_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_text_color(get_style_alert_icon_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][31]));
    lv_style_set_bg_color(get_style_dot_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_bg_color(get_style_dot_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_bg_color(get_style_pivot_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_line_color(get_style_hand_hour_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_line_color(get_style_hand_minute_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_line_color(get_style_hand_second_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_bg_color(get_style_bar_level_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][4]));
    lv_style_set_bg_color(get_style_bar_level_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_bg_color(get_style_bar_fresh_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_bar_fresh_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_bg_color(get_style_bar_fresh_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][18]));
    lv_style_set_bg_grad_color(get_style_bar_fresh_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][17]));
    lv_style_set_bg_color(get_style_bar_grey_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_bar_grey_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_bg_color(get_style_bar_grey_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][23]));
    lv_style_set_bg_color(get_style_bar_black_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_bar_black_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_bg_color(get_style_bar_black_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][24]));
    lv_style_set_bg_color(get_style_field_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_field_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_field_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_text_color(get_style_field_TEXTAREA_PLACEHOLDER_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_bg_color(get_style_field_CURSOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_arc_color(get_style_arc_value_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_arc_color(get_style_arc_value_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_arc_color(get_style_arc_thin_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_arc_color(get_style_arc_thin_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_arc_color(get_style_arc_thin_INDICATOR_CHECKED(), lv_color_hex(theme_colors[theme_index][11]));
    lv_style_set_arc_color(get_style_arc_thin_INDICATOR_DISABLED(), lv_color_hex(theme_colors[theme_index][14]));
    lv_style_set_arc_color(get_style_arc_thin_INDICATOR_PRESSED(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_label_eyebrow_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_text_color(get_style_label_hero_xl_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_text_color(get_style_label_metric_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_text_color(get_style_label_status_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][28]));
    lv_style_set_text_color(get_style_label_status_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][29]));
    lv_style_set_text_color(get_style_label_status_MAIN_DISABLED(), lv_color_hex(theme_colors[theme_index][31]));
    lv_style_set_text_color(get_style_label_status_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][8]));
    lv_obj_invalidate(objects.page_idle);
    lv_obj_invalidate(objects.page_menu);
    lv_obj_invalidate(objects.page_climate);
    lv_obj_invalidate(objects.page_climate_mode);
    lv_obj_invalidate(objects.page_devices);
    lv_obj_invalidate(objects.page_energy);
    lv_obj_invalidate(objects.page_water);
    lv_obj_invalidate(objects.page_air);
    lv_obj_invalidate(objects.page_level);
    lv_obj_invalidate(objects.page_settings);
    lv_obj_invalidate(objects.page_alert);
    lv_obj_invalidate(objects.page_setup);
}
uint32_t theme_colors[2][32] = {
    { 0xffe8e8e8, 0xffe4e4e4, 0xffffffff, 0xffededed, 0xffffffff, 0xffc8c8c8, 0xff1a1a1a, 0xff4a4a4a, 0xff696969, 0xff52a441, 0xffcbe3c6, 0xffffc107, 0xfffff4d1, 0xff48e6fe, 0xffff5453, 0xff74fe00, 0xff505050, 0xff0088cc, 0xff48e6fe, 0xff777777, 0xffb5b5b5, 0xff333333, 0xff666666, 0xff9e9e9e, 0xff424242, 0xffe5e5e5, 0xffffffff, 0xff000000, 0xff3d7b31, 0xff7d5800, 0xff00697d, 0xffb3261e },
    { 0xff0a0a0a, 0xff000000, 0xff1a1a1a, 0xff252525, 0xff0a0a0a, 0xff333333, 0xffffffff, 0xffaaaaaa, 0xff6f6f6f, 0xff7bc96a, 0xff2e4a2a, 0xffffc107, 0xff3a2f00, 0xff48e6fe, 0xffff5453, 0xff74fe00, 0xffbdbdbd, 0xff0088cc, 0xff48e6fe, 0xff777777, 0xffb5b5b5, 0xff333333, 0xff666666, 0xff9e9e9e, 0xff424242, 0xff1e1e1e, 0xffffffff, 0xff000000, 0xff7bc96a, 0xffffc107, 0xff48e6fe, 0xffff5453 },
};

//
//
//

void create_screens() {

// Set default LVGL theme
    lv_display_t *dispp = lv_display_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_display_set_theme(dispp, theme);
    
    // Initialize screens
    // Create screens
    create_screen_page_idle();
    create_screen_page_menu();
    create_screen_page_climate();
    create_screen_page_climate_mode();
    create_screen_page_devices();
    create_screen_page_energy();
    create_screen_page_water();
    create_screen_page_air();
    create_screen_page_level();
    create_screen_page_settings();
    create_screen_page_alert();
    create_screen_page_setup();
}