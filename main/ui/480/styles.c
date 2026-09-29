#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: ScreenRoot
//

void init_style_screen_root_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rr16);
};

lv_style_t *get_style_screen_root_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_screen_root_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_screen_root(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_screen_root_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_screen_root(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_screen_root_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Card
//

void init_style_card_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 12);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
};

lv_style_t *get_style_card_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_card_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_card_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_border_width(style, 3);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 12);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
};

lv_style_t *get_style_card_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_card_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_card(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_card_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_card_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_card(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_card_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_card_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: CardSelected
//

void init_style_card_selected_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_border_width(style, 2);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 12);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
};

lv_style_t *get_style_card_selected_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_card_selected_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_card_selected(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_card_selected_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_card_selected(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_card_selected_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: LabelTitle
//

void init_style_label_title_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rm26);
};

lv_style_t *get_style_label_title_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_title_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_label_title(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_title_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_title(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_title_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: LabelBody
//

void init_style_label_body_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &ui_font_rr16);
};

lv_style_t *get_style_label_body_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_body_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_label_body(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_body_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_body(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_body_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: LabelMuted
//

void init_style_label_muted_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &ui_font_rr13);
};

lv_style_t *get_style_label_muted_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_muted_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_label_muted(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_muted_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_muted(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_muted_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: LabelHero
//

void init_style_label_hero_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rl72);
};

lv_style_t *get_style_label_hero_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_hero_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_label_hero(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_hero_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_hero(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_hero_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ButtonPrimary
//

void init_style_button_primary_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_radius(style, 8);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][27]));
    lv_style_set_text_font(style, &ui_font_rr16);
};

lv_style_t *get_style_button_primary_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_primary_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_button_primary(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_primary_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_button_primary(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_primary_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ButtonNeutral
//

void init_style_button_neutral_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 8);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rr16);
};

lv_style_t *get_style_button_neutral_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_neutral_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_button_neutral(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_neutral_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_button_neutral(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_neutral_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ScaleRing
//

void init_style_scale_ring_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][25]));
    lv_style_set_line_width(style, 2);
    lv_style_set_line_opa(style, 255);
};

lv_style_t *get_style_scale_ring_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scale_ring_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_scale_ring_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][25]));
    lv_style_set_line_width(style, 2);
    lv_style_set_line_opa(style, 255);
};

lv_style_t *get_style_scale_ring_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scale_ring_ITEMS_DEFAULT(style);
    }
    return style;
};

void init_style_scale_ring_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][16]));
    lv_style_set_line_width(style, 3);
    lv_style_set_line_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][16]));
    lv_style_set_text_font(style, &ui_font_rr13);
};

lv_style_t *get_style_scale_ring_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scale_ring_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_scale_ring(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_scale_ring_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_scale_ring_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_scale_ring_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_scale_ring(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_scale_ring_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_scale_ring_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_scale_ring_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: LabelIcon
//

void init_style_label_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_fa24);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
};

lv_style_t *get_style_label_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_label_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: LabelIconSm
//

void init_style_label_icon_sm_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &ui_font_fa18);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
};

lv_style_t *get_style_label_icon_sm_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_icon_sm_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_label_icon_sm(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_icon_sm_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_icon_sm(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_icon_sm_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: HeroTile
//

void init_style_hero_tile_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_border_width(style, 3);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 1000);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_hero_tile_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_hero_tile_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_hero_tile(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_hero_tile_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_hero_tile(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_hero_tile_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: HeroIcon
//

void init_style_hero_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &ui_font_fh68);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
};

lv_style_t *get_style_hero_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_hero_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_hero_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_hero_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_hero_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_hero_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: NeighbourIcon
//

void init_style_neighbour_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &ui_font_fh44);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
};

lv_style_t *get_style_neighbour_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_neighbour_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_neighbour_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_neighbour_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_neighbour_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_neighbour_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: DeviceTile
//

void init_style_device_tile_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 3);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 1000);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_device_tile_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_device_tile_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_device_tile_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_border_width(style, 3);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 1000);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_device_tile_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_device_tile_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_device_tile(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_device_tile_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_device_tile_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_device_tile(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_device_tile_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_device_tile_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: DeviceIcon
//

void init_style_device_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &ui_font_fa56);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
};

lv_style_t *get_style_device_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_device_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_device_icon_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &ui_font_fa56);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
};

lv_style_t *get_style_device_icon_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_device_icon_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_device_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_device_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_device_icon_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_device_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_device_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_device_icon_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: DeviceNeighbourIcon
//

void init_style_device_neighbour_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &ui_font_fa36);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
};

lv_style_t *get_style_device_neighbour_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_device_neighbour_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_device_neighbour_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_device_neighbour_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_device_neighbour_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_device_neighbour_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: AlertRing
//

void init_style_alert_ring_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
    lv_style_set_border_width(style, 4);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 9999);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
};

lv_style_t *get_style_alert_ring_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_alert_ring_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_alert_ring(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_alert_ring_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_alert_ring(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_alert_ring_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: LevelWell
//

void init_style_level_well_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][27]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 2);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 9999);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_level_well_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_level_well_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_level_well(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_level_well_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_level_well(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_level_well_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: LevelRing
//

void init_style_level_ring_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 9999);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_level_ring_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_level_ring_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_level_ring(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_level_ring_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_level_ring(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_level_ring_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: LevelLine
//

void init_style_level_line_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_line_width(style, 1);
    lv_style_set_line_opa(style, 255);
};

lv_style_t *get_style_level_line_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_level_line_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_level_line(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_level_line_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_level_line(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_level_line_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: LevelBubble
//

void init_style_level_bubble_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][11]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 9999);
    lv_style_set_shadow_color(style, lv_color_hex(theme_colors[active_theme_index][11]));
    lv_style_set_shadow_opa(style, 128);
    lv_style_set_shadow_width(style, 20);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_level_bubble_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_level_bubble_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_level_bubble(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_level_bubble_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_level_bubble(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_level_bubble_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: AlertIcon
//

void init_style_alert_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
    lv_style_set_text_font(style, &ui_font_fa56);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
};

lv_style_t *get_style_alert_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_alert_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_alert_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_alert_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_alert_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_alert_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Dot
//

void init_style_dot_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 1000);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_dot_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_dot_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_dot_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 1000);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_dot_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_dot_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_dot(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_dot_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_dot_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_dot(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_dot_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_dot_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: Plain
//

void init_style_plain_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_plain_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_plain_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_plain(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_plain_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_plain(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_plain_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Pivot
//

void init_style_pivot_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 1000);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_pivot_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_pivot_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_pivot(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_pivot_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_pivot(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_pivot_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: HandHour
//

void init_style_hand_hour_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_line_width(style, 5);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
};

lv_style_t *get_style_hand_hour_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_hand_hour_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_hand_hour(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_hand_hour_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_hand_hour(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_hand_hour_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: HandMinute
//

void init_style_hand_minute_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_line_width(style, 3);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
};

lv_style_t *get_style_hand_minute_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_hand_minute_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_hand_minute(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_hand_minute_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_hand_minute(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_hand_minute_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: HandSecond
//

void init_style_hand_second_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_line_width(style, 2);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
};

lv_style_t *get_style_hand_second_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_hand_second_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_hand_second(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_hand_second_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_hand_second(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_hand_second_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: BarLevel
//

void init_style_bar_level_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][4]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_radius(style, 6);
    lv_style_set_border_width(style, 0);
};

lv_style_t *get_style_bar_level_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_bar_level_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_bar_level_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_radius(style, 6);
};

lv_style_t *get_style_bar_level_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_bar_level_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_bar_level(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_bar_level_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_bar_level_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_bar_level(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_bar_level_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_bar_level_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: BarFresh
//

void init_style_bar_fresh_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][4]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_radius(style, 6);
    lv_style_set_border_width(style, 0);
};

lv_style_t *get_style_bar_fresh_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_bar_fresh_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_bar_fresh_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][17]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_radius(style, 6);
};

lv_style_t *get_style_bar_fresh_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_bar_fresh_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_bar_fresh(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_bar_fresh_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_bar_fresh_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_bar_fresh(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_bar_fresh_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_bar_fresh_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: BarGrey
//

void init_style_bar_grey_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][4]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_radius(style, 6);
    lv_style_set_border_width(style, 0);
};

lv_style_t *get_style_bar_grey_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_bar_grey_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_bar_grey_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][23]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_radius(style, 6);
};

lv_style_t *get_style_bar_grey_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_bar_grey_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_bar_grey(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_bar_grey_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_bar_grey_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_bar_grey(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_bar_grey_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_bar_grey_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: BarBlack
//

void init_style_bar_black_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][4]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_radius(style, 6);
    lv_style_set_border_width(style, 0);
};

lv_style_t *get_style_bar_black_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_bar_black_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_bar_black_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][24]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_radius(style, 6);
};

lv_style_t *get_style_bar_black_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_bar_black_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_bar_black(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_bar_black_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_bar_black_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_bar_black(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_bar_black_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_bar_black_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: Field
//

void init_style_field_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 6);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rr16);
};

lv_style_t *get_style_field_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_field_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_field_TEXTAREA_PLACEHOLDER_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
};

lv_style_t *get_style_field_TEXTAREA_PLACEHOLDER_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_field_TEXTAREA_PLACEHOLDER_DEFAULT(style);
    }
    return style;
};

void init_style_field_CURSOR_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_field_CURSOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_field_CURSOR_DEFAULT(style);
    }
    return style;
};

void add_style_field(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_field_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_field_TEXTAREA_PLACEHOLDER_DEFAULT(), LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_field_CURSOR_DEFAULT(), LV_PART_CURSOR | LV_STATE_DEFAULT);
};

void remove_style_field(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_field_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_field_TEXTAREA_PLACEHOLDER_DEFAULT(), LV_PART_TEXTAREA_PLACEHOLDER | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_field_CURSOR_DEFAULT(), LV_PART_CURSOR | LV_STATE_DEFAULT);
};

//
// Style: ArcValue
//

void init_style_arc_value_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_arc_width(style, 10);
    lv_style_set_arc_opa(style, 255);
};

lv_style_t *get_style_arc_value_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_arc_value_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_arc_value_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_arc_width(style, 10);
    lv_style_set_arc_opa(style, 255);
};

lv_style_t *get_style_arc_value_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_arc_value_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_arc_value(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_arc_value_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_arc_value_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_arc_value(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_arc_value_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_arc_value_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: ArcThin
//

void init_style_arc_thin_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_arc_width(style, 14);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_arc_rounded(style, true);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_arc_thin_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_arc_thin_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_arc_thin_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][15]));
    lv_style_set_arc_width(style, 14);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_arc_rounded(style, true);
};

lv_style_t *get_style_arc_thin_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_arc_thin_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_arc_thin_INDICATOR_CHECKED(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][11]));
    lv_style_set_arc_width(style, 14);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_arc_rounded(style, true);
};

lv_style_t *get_style_arc_thin_INDICATOR_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_arc_thin_INDICATOR_CHECKED(style);
    }
    return style;
};

void init_style_arc_thin_INDICATOR_DISABLED(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
    lv_style_set_arc_width(style, 14);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_arc_rounded(style, true);
};

lv_style_t *get_style_arc_thin_INDICATOR_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_arc_thin_INDICATOR_DISABLED(style);
    }
    return style;
};

void init_style_arc_thin_INDICATOR_PRESSED(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_arc_width(style, 14);
    lv_style_set_arc_opa(style, 0);
    lv_style_set_arc_rounded(style, true);
};

lv_style_t *get_style_arc_thin_INDICATOR_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_arc_thin_INDICATOR_PRESSED(style);
    }
    return style;
};

void init_style_arc_thin_KNOB_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_arc_thin_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_arc_thin_KNOB_DEFAULT(style);
    }
    return style;
};

void add_style_arc_thin(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_arc_thin_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_arc_thin_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_arc_thin_INDICATOR_CHECKED(), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_arc_thin_INDICATOR_DISABLED(), LV_PART_INDICATOR | LV_STATE_DISABLED);
    lv_obj_add_style(obj, get_style_arc_thin_INDICATOR_PRESSED(), LV_PART_INDICATOR | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_arc_thin_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
};

void remove_style_arc_thin(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_arc_thin_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_arc_thin_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_arc_thin_INDICATOR_CHECKED(), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_arc_thin_INDICATOR_DISABLED(), LV_PART_INDICATOR | LV_STATE_DISABLED);
    lv_obj_remove_style(obj, get_style_arc_thin_INDICATOR_PRESSED(), LV_PART_INDICATOR | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_arc_thin_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
};

//
// Style: LabelEyebrow
//

void init_style_label_eyebrow_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &ui_font_rr16);
    lv_style_set_text_letter_space(style, 2);
};

lv_style_t *get_style_label_eyebrow_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_eyebrow_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_label_eyebrow(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_eyebrow_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_eyebrow(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_eyebrow_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: LabelHeroXL
//

void init_style_label_hero_xl_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rn120);
    lv_style_set_text_letter_space(style, -4);
};

lv_style_t *get_style_label_hero_xl_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_hero_xl_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_label_hero_xl(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_hero_xl_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_hero_xl(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_hero_xl_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: LabelMetric
//

void init_style_label_metric_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rr16);
};

lv_style_t *get_style_label_metric_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_metric_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_label_metric(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_metric_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_metric(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_metric_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: LabelStatus
//

void init_style_label_status_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][15]));
    lv_style_set_text_font(style, &ui_font_rm20);
};

lv_style_t *get_style_label_status_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_status_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_label_status_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][11]));
    lv_style_set_text_font(style, &ui_font_rm20);
};

lv_style_t *get_style_label_status_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_status_MAIN_CHECKED(style);
    }
    return style;
};

void init_style_label_status_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
    lv_style_set_text_font(style, &ui_font_rm20);
};

lv_style_t *get_style_label_status_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_status_MAIN_DISABLED(style);
    }
    return style;
};

void init_style_label_status_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &ui_font_rm20);
};

lv_style_t *get_style_label_status_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_status_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_label_status(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_status_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_label_status_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_label_status_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_add_style(obj, get_style_label_status_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_label_status(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_status_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_label_status_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_label_status_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_remove_style(obj, get_style_label_status_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_screen_root,
        add_style_card,
        add_style_card_selected,
        add_style_label_title,
        add_style_label_body,
        add_style_label_muted,
        add_style_label_hero,
        add_style_button_primary,
        add_style_button_neutral,
        add_style_scale_ring,
        add_style_label_icon,
        add_style_label_icon_sm,
        add_style_hero_tile,
        add_style_hero_icon,
        add_style_neighbour_icon,
        add_style_device_tile,
        add_style_device_icon,
        add_style_device_neighbour_icon,
        add_style_alert_ring,
        add_style_level_well,
        add_style_level_ring,
        add_style_level_line,
        add_style_level_bubble,
        add_style_alert_icon,
        add_style_dot,
        add_style_plain,
        add_style_pivot,
        add_style_hand_hour,
        add_style_hand_minute,
        add_style_hand_second,
        add_style_bar_level,
        add_style_bar_fresh,
        add_style_bar_grey,
        add_style_bar_black,
        add_style_field,
        add_style_arc_value,
        add_style_arc_thin,
        add_style_label_eyebrow,
        add_style_label_hero_xl,
        add_style_label_metric,
        add_style_label_status,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_screen_root,
        remove_style_card,
        remove_style_card_selected,
        remove_style_label_title,
        remove_style_label_body,
        remove_style_label_muted,
        remove_style_label_hero,
        remove_style_button_primary,
        remove_style_button_neutral,
        remove_style_scale_ring,
        remove_style_label_icon,
        remove_style_label_icon_sm,
        remove_style_hero_tile,
        remove_style_hero_icon,
        remove_style_neighbour_icon,
        remove_style_device_tile,
        remove_style_device_icon,
        remove_style_device_neighbour_icon,
        remove_style_alert_ring,
        remove_style_level_well,
        remove_style_level_ring,
        remove_style_level_line,
        remove_style_level_bubble,
        remove_style_alert_icon,
        remove_style_dot,
        remove_style_plain,
        remove_style_pivot,
        remove_style_hand_hour,
        remove_style_hand_minute,
        remove_style_hand_second,
        remove_style_bar_level,
        remove_style_bar_fresh,
        remove_style_bar_grey,
        remove_style_bar_black,
        remove_style_field,
        remove_style_arc_value,
        remove_style_arc_thin,
        remove_style_label_eyebrow,
        remove_style_label_hero_xl,
        remove_style_label_metric,
        remove_style_label_status,
    };
    remove_style_funcs[styleIndex](obj);
}