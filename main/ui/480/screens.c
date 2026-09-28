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
            // menu_item0
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_item0 = obj;
            lv_obj_set_pos(obj, LV_PCT(41), LV_PCT(5));
            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(18));
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // menu_item0_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.menu_item0_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(25), LV_PCT(25));
                    lv_obj_set_size(obj, LV_PCT(50), LV_PCT(50));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_label_icon(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
        {
            // menu_item1
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_item1 = obj;
            lv_obj_set_pos(obj, LV_PCT(64), LV_PCT(13));
            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(18));
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // menu_item1_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.menu_item1_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(25), LV_PCT(25));
                    lv_obj_set_size(obj, LV_PCT(50), LV_PCT(50));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_label_icon(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
        {
            // menu_item2
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_item2 = obj;
            lv_obj_set_pos(obj, LV_PCT(76), LV_PCT(35));
            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(18));
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // menu_item2_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.menu_item2_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(25), LV_PCT(25));
                    lv_obj_set_size(obj, LV_PCT(50), LV_PCT(50));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_label_icon(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
        {
            // menu_item3
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_item3 = obj;
            lv_obj_set_pos(obj, LV_PCT(72), LV_PCT(59));
            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(18));
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // menu_item3_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.menu_item3_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(25), LV_PCT(25));
                    lv_obj_set_size(obj, LV_PCT(50), LV_PCT(50));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_label_icon(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
        {
            // menu_item4
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_item4 = obj;
            lv_obj_set_pos(obj, LV_PCT(53), LV_PCT(75));
            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(18));
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // menu_item4_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.menu_item4_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(25), LV_PCT(25));
                    lv_obj_set_size(obj, LV_PCT(50), LV_PCT(50));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_label_icon(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
        {
            // menu_item5
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_item5 = obj;
            lv_obj_set_pos(obj, LV_PCT(29), LV_PCT(75));
            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(18));
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // menu_item5_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.menu_item5_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(25), LV_PCT(25));
                    lv_obj_set_size(obj, LV_PCT(50), LV_PCT(50));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_label_icon(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
        {
            // menu_item6
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_item6 = obj;
            lv_obj_set_pos(obj, LV_PCT(10), LV_PCT(59));
            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(18));
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // menu_item6_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.menu_item6_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(25), LV_PCT(25));
                    lv_obj_set_size(obj, LV_PCT(50), LV_PCT(50));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_label_icon(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
        {
            // menu_item7
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_item7 = obj;
            lv_obj_set_pos(obj, LV_PCT(6), LV_PCT(35));
            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(18));
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // menu_item7_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.menu_item7_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(25), LV_PCT(25));
                    lv_obj_set_size(obj, LV_PCT(50), LV_PCT(50));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_label_icon(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
        {
            // menu_item8
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.menu_item8 = obj;
            lv_obj_set_pos(obj, LV_PCT(18), LV_PCT(13));
            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(18));
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // menu_item8_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.menu_item8_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(25), LV_PCT(25));
                    lv_obj_set_size(obj, LV_PCT(50), LV_PCT(50));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_label_icon(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
        {
            // menu_title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.menu_title = obj;
            lv_obj_set_pos(obj, LV_PCT(22), LV_PCT(42));
            lv_obj_set_size(obj, LV_PCT(56), LV_PCT(10));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_title(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Climate");
        }
        {
            // menu_summary
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.menu_summary = obj;
            lv_obj_set_pos(obj, LV_PCT(22), LV_PCT(54));
            lv_obj_set_size(obj, LV_PCT(56), LV_PCT(8));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_muted(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Heating · 72°");
        }
    }
    
    tick_screen_page_menu();
}

void tick_screen_page_menu() {
}

void create_screen_page_climate() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_climate = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 480);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // climate_arc
            lv_obj_t *obj = lv_arc_create(parent_obj);
            objects.climate_arc = obj;
            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(3));
            lv_obj_set_size(obj, LV_PCT(94), LV_PCT(94));
            lv_arc_set_range(obj, 50, 90);
            lv_arc_set_value(obj, 72);
            add_style_arc_value(obj);
        }
        {
            // climate_setpoint
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.climate_setpoint = obj;
            lv_obj_set_pos(obj, LV_PCT(20), LV_PCT(36));
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(26));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_hero(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "72");
        }
        {
            // climate_unit
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.climate_unit = obj;
            lv_obj_set_pos(obj, LV_PCT(56), LV_PCT(36));
            lv_obj_set_size(obj, LV_PCT(16), LV_PCT(10));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_body(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "°F");
        }
        {
            // climate_current
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.climate_current = obj;
            lv_obj_set_pos(obj, LV_PCT(25), LV_PCT(27));
            lv_obj_set_size(obj, LV_PCT(50), LV_PCT(8));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_muted(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Now 68°");
        }
        {
            // climate_state_icon
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.climate_state_icon = obj;
            lv_obj_set_pos(obj, LV_PCT(34), LV_PCT(64));
            lv_obj_set_size(obj, LV_PCT(10), LV_PCT(9));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
            add_style_label_icon(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // climate_state
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.climate_state = obj;
            lv_obj_set_pos(obj, LV_PCT(45), LV_PCT(64));
            lv_obj_set_size(obj, LV_PCT(26), LV_PCT(9));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_title(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Heating");
        }
        {
            // climate_eta
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.climate_eta = obj;
            lv_obj_set_pos(obj, LV_PCT(20), LV_PCT(72));
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(7));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_muted(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "about 20 min");
        }
        {
            // climate_mode
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.climate_mode = obj;
            lv_obj_set_pos(obj, LV_PCT(30), LV_PCT(79));
            lv_obj_set_size(obj, LV_PCT(40), LV_PCT(7));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_body(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "HEAT");
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
            lv_obj_set_pos(obj, LV_PCT(20), LV_PCT(13));
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(9));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_title(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Mode");
        }
        {
            // cmode_list
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.cmode_list = obj;
            lv_obj_set_pos(obj, LV_PCT(16), LV_PCT(24));
            lv_obj_set_size(obj, LV_PCT(68), LV_PCT(60));
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
            add_style_plain(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // cmode_item0
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.cmode_item0 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // cmode_item0_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.cmode_item0_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(4), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(62), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Heat");
                        }
                    }
                }
                {
                    // cmode_item1
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.cmode_item1 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(25));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // cmode_item1_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.cmode_item1_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(4), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(62), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Cool");
                        }
                    }
                }
                {
                    // cmode_item2
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.cmode_item2 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(50));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // cmode_item2_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.cmode_item2_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(4), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(62), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Auto");
                        }
                    }
                }
                {
                    // cmode_item3
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.cmode_item3 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(75));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // cmode_item3_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.cmode_item3_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(4), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(62), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
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

void create_screen_page_lights() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_lights = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 480);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // lights_title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lights_title = obj;
            lv_obj_set_pos(obj, LV_PCT(20), LV_PCT(13));
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(9));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_title(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Lights");
        }
        {
            // lights_list
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.lights_list = obj;
            lv_obj_set_pos(obj, LV_PCT(16), LV_PCT(24));
            lv_obj_set_size(obj, LV_PCT(68), LV_PCT(60));
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
            add_style_plain(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lights_item0
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.lights_item0 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lights_item0_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item0_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(26));
                            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(48));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // lights_item0_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item0_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(42), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Light 1");
                        }
                        {
                            // lights_item0_value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item0_value = obj;
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
                    // lights_item1
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.lights_item1 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(25));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lights_item1_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item1_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(26));
                            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(48));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // lights_item1_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item1_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(42), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Light 2");
                        }
                        {
                            // lights_item1_value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item1_value = obj;
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
                    // lights_item2
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.lights_item2 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(50));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lights_item2_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item2_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(26));
                            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(48));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // lights_item2_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item2_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(42), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Light 3");
                        }
                        {
                            // lights_item2_value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item2_value = obj;
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
                    // lights_item3
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.lights_item3 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(75));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lights_item3_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item3_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(26));
                            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(48));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // lights_item3_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item3_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(42), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Light 4");
                        }
                        {
                            // lights_item3_value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item3_value = obj;
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
                    // lights_item4
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.lights_item4 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(100));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lights_item4_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item4_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(26));
                            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(48));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // lights_item4_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item4_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(42), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Light 5");
                        }
                        {
                            // lights_item4_value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item4_value = obj;
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
                    // lights_item5
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.lights_item5 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(125));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lights_item5_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item5_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(26));
                            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(48));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // lights_item5_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item5_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(42), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Light 6");
                        }
                        {
                            // lights_item5_value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item5_value = obj;
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
                    // lights_item6
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.lights_item6 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(150));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lights_item6_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item6_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(26));
                            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(48));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // lights_item6_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item6_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(42), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Evening");
                        }
                    }
                }
                {
                    // lights_item7
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.lights_item7 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(175));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lights_item7_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item7_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(26));
                            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(48));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // lights_item7_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item7_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(42), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Night");
                        }
                    }
                }
                {
                    // lights_item8
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.lights_item8 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(200));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lights_item8_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item8_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(26));
                            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(48));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // lights_item8_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lights_item8_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(42), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "All Off");
                        }
                    }
                }
            }
        }
        {
            // lights_back
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.lights_back = obj;
            lv_obj_set_pos(obj, LV_PCT(42), LV_PCT(84));
            lv_obj_set_size(obj, LV_PCT(16), LV_PCT(11));
            lv_obj_add_event_cb(obj, action_nav_back, LV_EVENT_CLICKED, (void *)0);
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lights_back_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lights_back_icon = obj;
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
    
    tick_screen_page_lights();
}

void tick_screen_page_lights() {
}

void create_screen_page_heater() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_heater = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 480);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // heater_arc
            lv_obj_t *obj = lv_arc_create(parent_obj);
            objects.heater_arc = obj;
            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(3));
            lv_obj_set_size(obj, LV_PCT(94), LV_PCT(94));
            lv_arc_set_range(obj, 0, 5);
            lv_arc_set_value(obj, 0);
            add_style_arc_value(obj);
        }
        {
            // heater_title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.heater_title = obj;
            lv_obj_set_pos(obj, LV_PCT(20), LV_PCT(13));
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(9));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_title(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Heater");
        }
        {
            // heater_level
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.heater_level = obj;
            lv_obj_set_pos(obj, LV_PCT(20), LV_PCT(38));
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(26));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_hero(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "0");
        }
        {
            // heater_state
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.heater_state = obj;
            lv_obj_set_pos(obj, LV_PCT(25), LV_PCT(66));
            lv_obj_set_size(obj, LV_PCT(50), LV_PCT(9));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_body(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Off");
        }
        {
            // heater_back
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.heater_back = obj;
            lv_obj_set_pos(obj, LV_PCT(42), LV_PCT(84));
            lv_obj_set_size(obj, LV_PCT(16), LV_PCT(11));
            lv_obj_add_event_cb(obj, action_nav_back, LV_EVENT_CLICKED, (void *)0);
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // heater_back_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.heater_back_icon = obj;
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
    
    tick_screen_page_heater();
}

void tick_screen_page_heater() {
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
            // energy_title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.energy_title = obj;
            lv_obj_set_pos(obj, LV_PCT(20), LV_PCT(13));
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(9));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_title(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Battery");
        }
        {
            // energy_value
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.energy_value = obj;
            lv_obj_set_pos(obj, LV_PCT(15), LV_PCT(36));
            lv_obj_set_size(obj, LV_PCT(70), LV_PCT(24));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_hero(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "--");
        }
        {
            // energy_unit
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.energy_unit = obj;
            lv_obj_set_pos(obj, LV_PCT(40), LV_PCT(60));
            lv_obj_set_size(obj, LV_PCT(20), LV_PCT(8));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_muted(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "V");
        }
        {
            // energy_sub
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.energy_sub = obj;
            lv_obj_set_pos(obj, LV_PCT(20), LV_PCT(69));
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(8));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_muted(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "--");
        }
        {
            // energy_dot0
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.energy_dot0 = obj;
            lv_obj_set_pos(obj, LV_PCT(37), LV_PCT(79));
            lv_obj_set_size(obj, LV_PCT(3), LV_PCT(3));
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // energy_dot1
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.energy_dot1 = obj;
            lv_obj_set_pos(obj, LV_PCT(43), LV_PCT(79));
            lv_obj_set_size(obj, LV_PCT(3), LV_PCT(3));
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // energy_dot2
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.energy_dot2 = obj;
            lv_obj_set_pos(obj, LV_PCT(49), LV_PCT(79));
            lv_obj_set_size(obj, LV_PCT(3), LV_PCT(3));
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // energy_dot3
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.energy_dot3 = obj;
            lv_obj_set_pos(obj, LV_PCT(55), LV_PCT(79));
            lv_obj_set_size(obj, LV_PCT(3), LV_PCT(3));
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // energy_dot4
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.energy_dot4 = obj;
            lv_obj_set_pos(obj, LV_PCT(61), LV_PCT(79));
            lv_obj_set_size(obj, LV_PCT(3), LV_PCT(3));
            add_style_card(obj);
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
            lv_obj_set_pos(obj, LV_PCT(20), LV_PCT(13));
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(9));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_title(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Water");
        }
        {
            // water_fresh_value
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.water_fresh_value = obj;
            lv_obj_set_pos(obj, LV_PCT(19), LV_PCT(22));
            lv_obj_set_size(obj, LV_PCT(22), LV_PCT(7));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_muted(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "--");
        }
        {
            // water_fresh_bar
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.water_fresh_bar = obj;
            lv_obj_set_pos(obj, LV_PCT(23), LV_PCT(30));
            lv_obj_set_size(obj, LV_PCT(14), LV_PCT(44));
            add_style_bar_fresh(obj);
        }
        {
            // water_fresh_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.water_fresh_label = obj;
            lv_obj_set_pos(obj, LV_PCT(19), LV_PCT(75));
            lv_obj_set_size(obj, LV_PCT(22), LV_PCT(7));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_muted(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Fresh");
        }
        {
            // water_grey_value
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.water_grey_value = obj;
            lv_obj_set_pos(obj, LV_PCT(39), LV_PCT(22));
            lv_obj_set_size(obj, LV_PCT(22), LV_PCT(7));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_muted(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "--");
        }
        {
            // water_grey_bar
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.water_grey_bar = obj;
            lv_obj_set_pos(obj, LV_PCT(43), LV_PCT(30));
            lv_obj_set_size(obj, LV_PCT(14), LV_PCT(44));
            add_style_bar_grey(obj);
        }
        {
            // water_grey_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.water_grey_label = obj;
            lv_obj_set_pos(obj, LV_PCT(39), LV_PCT(75));
            lv_obj_set_size(obj, LV_PCT(22), LV_PCT(7));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_muted(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Grey");
        }
        {
            // water_black_value
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.water_black_value = obj;
            lv_obj_set_pos(obj, LV_PCT(59), LV_PCT(22));
            lv_obj_set_size(obj, LV_PCT(22), LV_PCT(7));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_muted(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "--");
        }
        {
            // water_black_bar
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.water_black_bar = obj;
            lv_obj_set_pos(obj, LV_PCT(63), LV_PCT(30));
            lv_obj_set_size(obj, LV_PCT(14), LV_PCT(44));
            add_style_bar_black(obj);
        }
        {
            // water_black_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.water_black_label = obj;
            lv_obj_set_pos(obj, LV_PCT(59), LV_PCT(75));
            lv_obj_set_size(obj, LV_PCT(22), LV_PCT(7));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_muted(obj);
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
            // air_title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.air_title = obj;
            lv_obj_set_pos(obj, LV_PCT(20), LV_PCT(13));
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(9));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_title(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Air");
        }
        {
            // air_rows
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.air_rows = obj;
            lv_obj_set_pos(obj, LV_PCT(16), LV_PCT(26));
            lv_obj_set_size(obj, LV_PCT(68), LV_PCT(56));
            add_style_plain(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // air_temp_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.air_temp_label = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(3));
                    lv_obj_set_size(obj, LV_PCT(48), LV_PCT(19));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_label_muted(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Temp");
                }
                {
                    // air_temp
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.air_temp = obj;
                    lv_obj_set_pos(obj, LV_PCT(50), LV_PCT(2));
                    lv_obj_set_size(obj, LV_PCT(50), LV_PCT(20));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_label_body(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "--");
                }
                {
                    // air_humidity_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.air_humidity_label = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(28));
                    lv_obj_set_size(obj, LV_PCT(48), LV_PCT(19));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_label_muted(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Humidity");
                }
                {
                    // air_humidity
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.air_humidity = obj;
                    lv_obj_set_pos(obj, LV_PCT(50), LV_PCT(27));
                    lv_obj_set_size(obj, LV_PCT(50), LV_PCT(20));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_label_body(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "--");
                }
                {
                    // air_eco2_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.air_eco2_label = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(53));
                    lv_obj_set_size(obj, LV_PCT(48), LV_PCT(19));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_label_muted(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "eCO2");
                }
                {
                    // air_eco2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.air_eco2 = obj;
                    lv_obj_set_pos(obj, LV_PCT(50), LV_PCT(52));
                    lv_obj_set_size(obj, LV_PCT(50), LV_PCT(20));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_label_body(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "--");
                }
                {
                    // air_voc_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.air_voc_label = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(78));
                    lv_obj_set_size(obj, LV_PCT(48), LV_PCT(19));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_label_muted(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "VOC");
                }
                {
                    // air_voc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.air_voc = obj;
                    lv_obj_set_pos(obj, LV_PCT(50), LV_PCT(77));
                    lv_obj_set_size(obj, LV_PCT(50), LV_PCT(20));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_label_body(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "--");
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
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(9));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_title(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Level");
        }
        {
            // level_well
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.level_well = obj;
            lv_obj_set_pos(obj, LV_PCT(30), LV_PCT(26));
            lv_obj_set_size(obj, LV_PCT(40), LV_PCT(40));
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // level_bubble
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.level_bubble = obj;
                    lv_obj_set_pos(obj, LV_PCT(40), LV_PCT(40));
                    lv_obj_set_size(obj, LV_PCT(20), LV_PCT(20));
                    add_style_pivot(obj);
                }
            }
        }
        {
            // level_pitch
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.level_pitch = obj;
            lv_obj_set_pos(obj, LV_PCT(16), LV_PCT(70));
            lv_obj_set_size(obj, LV_PCT(34), LV_PCT(9));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_body(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "--");
        }
        {
            // level_roll
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.level_roll = obj;
            lv_obj_set_pos(obj, LV_PCT(50), LV_PCT(70));
            lv_obj_set_size(obj, LV_PCT(34), LV_PCT(9));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_body(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
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

void create_screen_page_doors() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_doors = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 480);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // doors_title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.doors_title = obj;
            lv_obj_set_pos(obj, LV_PCT(20), LV_PCT(13));
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(9));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_title(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Doors");
        }
        {
            // doors_list
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.doors_list = obj;
            lv_obj_set_pos(obj, LV_PCT(16), LV_PCT(24));
            lv_obj_set_size(obj, LV_PCT(68), LV_PCT(60));
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
            add_style_plain(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // doors_item0
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.doors_item0 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(0));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // doors_item0_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.doors_item0_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(26));
                            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(48));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // doors_item0_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.doors_item0_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(42), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Main Door");
                        }
                        {
                            // doors_item0_value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.doors_item0_value = obj;
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
                    // doors_item1
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.doors_item1 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(25));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // doors_item1_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.doors_item1_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(26));
                            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(48));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // doors_item1_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.doors_item1_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(42), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Cargo Bay");
                        }
                        {
                            // doors_item1_value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.doors_item1_value = obj;
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
                    // doors_item2
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.doors_item2 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(50));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // doors_item2_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.doors_item2_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(26));
                            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(48));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // doors_item2_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.doors_item2_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(42), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Window L");
                        }
                        {
                            // doors_item2_value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.doors_item2_value = obj;
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
                    // doors_item3
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.doors_item3 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(75));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // doors_item3_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.doors_item3_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(26));
                            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(48));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // doors_item3_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.doors_item3_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(42), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Window R");
                        }
                        {
                            // doors_item3_value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.doors_item3_value = obj;
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
                    // doors_item4
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.doors_item4 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(100));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // doors_item4_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.doors_item4_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(26));
                            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(48));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // doors_item4_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.doors_item4_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(42), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Roof Hatch");
                        }
                        {
                            // doors_item4_value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.doors_item4_value = obj;
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
                    // doors_item5
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.doors_item5 = obj;
                    lv_obj_set_pos(obj, LV_PCT(0), LV_PCT(125));
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(23));
                    add_style_card(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // doors_item5_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.doors_item5_icon = obj;
                            lv_obj_set_pos(obj, LV_PCT(3), LV_PCT(26));
                            lv_obj_set_size(obj, LV_PCT(18), LV_PCT(48));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_label_icon_sm(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // doors_item5_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.doors_item5_title = obj;
                            lv_obj_set_pos(obj, LV_PCT(24), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(42), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_body(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Storage");
                        }
                        {
                            // doors_item5_value
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.doors_item5_value = obj;
                            lv_obj_set_pos(obj, LV_PCT(66), LV_PCT(24));
                            lv_obj_set_size(obj, LV_PCT(32), LV_PCT(52));
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_label_muted(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "--");
                        }
                    }
                }
            }
        }
        {
            // doors_back
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.doors_back = obj;
            lv_obj_set_pos(obj, LV_PCT(42), LV_PCT(84));
            lv_obj_set_size(obj, LV_PCT(16), LV_PCT(11));
            lv_obj_add_event_cb(obj, action_nav_back, LV_EVENT_CLICKED, (void *)0);
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // doors_back_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.doors_back_icon = obj;
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
    
    tick_screen_page_doors();
}

void tick_screen_page_doors() {
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
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(9));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_title(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Settings");
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
                            lv_label_set_text_static(obj, "");
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
            add_style_card(obj);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // alert_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.alert_icon = obj;
                    lv_obj_set_pos(obj, LV_PCT(40), LV_PCT(24));
                    lv_obj_set_size(obj, LV_PCT(20), LV_PCT(14));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_label_icon(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // alert_title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.alert_title = obj;
                    lv_obj_set_pos(obj, LV_PCT(16), LV_PCT(42));
                    lv_obj_set_size(obj, LV_PCT(68), LV_PCT(10));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_label_title(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Alert");
                }
                {
                    // alert_message
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.alert_message = obj;
                    lv_obj_set_pos(obj, LV_PCT(16), LV_PCT(54));
                    lv_obj_set_size(obj, LV_PCT(68), LV_PCT(18));
                    add_style_label_body(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "--");
                }
                {
                    // alert_hint
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.alert_hint = obj;
                    lv_obj_set_pos(obj, LV_PCT(16), LV_PCT(76));
                    lv_obj_set_size(obj, LV_PCT(68), LV_PCT(8));
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_label_muted(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "press to dismiss");
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
            lv_obj_set_size(obj, LV_PCT(60), LV_PCT(9));
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_label_title(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Wi-Fi Setup");
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
    tick_screen_page_lights,
    tick_screen_page_heater,
    tick_screen_page_energy,
    tick_screen_page_water,
    tick_screen_page_air,
    tick_screen_page_level,
    tick_screen_page_doors,
    tick_screen_page_settings,
    tick_screen_page_alert,
    tick_screen_page_setup,
};
void tick_screen(int screen_index) {
    if (screen_index >= 0 && screen_index < 14) {
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
    { "rm26", &ui_font_rm26 },
    { "rm20", &ui_font_rm20 },
    { "rr16", &ui_font_rr16 },
    { "rr13", &ui_font_rr13 },
    { "fa24", &ui_font_fa24 },
    { "fa18", &ui_font_fa18 },
    { "fa14", &ui_font_fa14 },
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
    lv_style_set_bg_color(get_style_pivot_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_line_color(get_style_hand_hour_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_line_color(get_style_hand_minute_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_line_color(get_style_hand_second_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_bg_color(get_style_bar_level_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][4]));
    lv_style_set_bg_color(get_style_bar_level_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_bg_color(get_style_bar_fresh_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][4]));
    lv_style_set_bg_color(get_style_bar_fresh_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][17]));
    lv_style_set_bg_color(get_style_bar_grey_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][4]));
    lv_style_set_bg_color(get_style_bar_grey_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][23]));
    lv_style_set_bg_color(get_style_bar_black_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][4]));
    lv_style_set_bg_color(get_style_bar_black_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][24]));
    lv_style_set_bg_color(get_style_field_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_field_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_field_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_text_color(get_style_field_TEXTAREA_PLACEHOLDER_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_bg_color(get_style_field_CURSOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_arc_color(get_style_arc_value_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_arc_color(get_style_arc_value_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_obj_invalidate(objects.page_idle);
    lv_obj_invalidate(objects.page_menu);
    lv_obj_invalidate(objects.page_climate);
    lv_obj_invalidate(objects.page_climate_mode);
    lv_obj_invalidate(objects.page_lights);
    lv_obj_invalidate(objects.page_heater);
    lv_obj_invalidate(objects.page_energy);
    lv_obj_invalidate(objects.page_water);
    lv_obj_invalidate(objects.page_air);
    lv_obj_invalidate(objects.page_level);
    lv_obj_invalidate(objects.page_doors);
    lv_obj_invalidate(objects.page_settings);
    lv_obj_invalidate(objects.page_alert);
    lv_obj_invalidate(objects.page_setup);
}
uint32_t theme_colors[2][28] = {
    { 0xffe8e8e8, 0xffe4e4e4, 0xffffffff, 0xffededed, 0xffffffff, 0xffc8c8c8, 0xff1a1a1a, 0xff4a4a4a, 0xff696969, 0xff52a441, 0xffcbe3c6, 0xffffc107, 0xfffff4d1, 0xff48e6fe, 0xffff5453, 0xff74fe00, 0xff505050, 0xff0088cc, 0xff48e6fe, 0xff777777, 0xffb5b5b5, 0xff333333, 0xff666666, 0xff9e9e9e, 0xff424242, 0xffe5e5e5, 0xffffffff, 0xff000000 },
    { 0xff0a0a0a, 0xff000000, 0xff1a1a1a, 0xff252525, 0xff0a0a0a, 0xff333333, 0xffffffff, 0xffaaaaaa, 0xff6f6f6f, 0xff7bc96a, 0xff2e4a2a, 0xffffc107, 0xff3a2f00, 0xff48e6fe, 0xffff5453, 0xff74fe00, 0xffbdbdbd, 0xff0088cc, 0xff48e6fe, 0xff777777, 0xffb5b5b5, 0xff333333, 0xff666666, 0xff9e9e9e, 0xff424242, 0xff1e1e1e, 0xffffffff, 0xff000000 },
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
    create_screen_page_lights();
    create_screen_page_heater();
    create_screen_page_energy();
    create_screen_page_water();
    create_screen_page_air();
    create_screen_page_level();
    create_screen_page_doors();
    create_screen_page_settings();
    create_screen_page_alert();
    create_screen_page_setup();
}