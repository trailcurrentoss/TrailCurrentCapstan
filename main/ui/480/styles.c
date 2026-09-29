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
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_border_width(style, 2);
    lv_style_set_border_opa(style, 255);
    lv_style_set_shadow_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_shadow_opa(style, 128);
    lv_style_set_shadow_width(style, 20);
    lv_style_set_shadow_spread(style, 0);
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
    lv_style_set_border_width(style, 2);
    lv_style_set_border_opa(style, 255);
    lv_style_set_shadow_opa(style, 0);
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
    lv_style_set_border_width(style, 2);
    lv_style_set_border_opa(style, 255);
    lv_style_set_shadow_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_shadow_opa(style, 128);
    lv_style_set_shadow_width(style, 20);
    lv_style_set_shadow_spread(style, 0);
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
// Style: ClimateTicks
//

void init_style_climate_ticks_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_width(style, 0);
    lv_style_set_arc_opa(style, 0);
    lv_style_set_line_width(style, 0);
    lv_style_set_line_opa(style, 0);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_climate_ticks_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_climate_ticks_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_climate_ticks_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_line_width(style, 3);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
    lv_style_set_length(style, 26);
};

lv_style_t *get_style_climate_ticks_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_climate_ticks_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_climate_ticks_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_line_width(style, 0);
    lv_style_set_line_opa(style, 0);
    lv_style_set_length(style, 0);
};

lv_style_t *get_style_climate_ticks_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_climate_ticks_ITEMS_DEFAULT(style);
    }
    return style;
};

void add_style_climate_ticks(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_climate_ticks_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_climate_ticks_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_climate_ticks_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
};

void remove_style_climate_ticks(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_climate_ticks_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_climate_ticks_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_climate_ticks_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
};

//
// Style: ClimateSectionHeat
//

void init_style_climate_section_heat_MAIN_DEFAULT(lv_style_t *style) {
};

lv_style_t *get_style_climate_section_heat_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_climate_section_heat_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_climate_section_heat_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
    lv_style_set_line_width(style, 4);
    lv_style_set_line_opa(style, 255);
};

lv_style_t *get_style_climate_section_heat_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_climate_section_heat_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_climate_section_heat(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_climate_section_heat_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_climate_section_heat_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_climate_section_heat(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_climate_section_heat_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_climate_section_heat_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: ClimateSectionCool
//

void init_style_climate_section_cool_MAIN_DEFAULT(lv_style_t *style) {
};

lv_style_t *get_style_climate_section_cool_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_climate_section_cool_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_climate_section_cool_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][13]));
    lv_style_set_line_width(style, 4);
    lv_style_set_line_opa(style, 255);
};

lv_style_t *get_style_climate_section_cool_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_climate_section_cool_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_climate_section_cool(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_climate_section_cool_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_climate_section_cool_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_climate_section_cool(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_climate_section_cool_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_climate_section_cool_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: ClimateSectionHold
//

void init_style_climate_section_hold_MAIN_DEFAULT(lv_style_t *style) {
};

lv_style_t *get_style_climate_section_hold_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_climate_section_hold_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_climate_section_hold_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_line_width(style, 4);
    lv_style_set_line_opa(style, 255);
};

lv_style_t *get_style_climate_section_hold_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_climate_section_hold_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_climate_section_hold(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_climate_section_hold_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_climate_section_hold_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_climate_section_hold(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_climate_section_hold_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_climate_section_hold_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: ClimateNeedleInside
//

void init_style_climate_needle_inside_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_line_width(style, 4);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
};

lv_style_t *get_style_climate_needle_inside_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_climate_needle_inside_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_climate_needle_inside(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_climate_needle_inside_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_climate_needle_inside(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_climate_needle_inside_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ClimateNeedleTarget
//

void init_style_climate_needle_target_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_line_width(style, 6);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
};

lv_style_t *get_style_climate_needle_target_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_climate_needle_target_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_climate_needle_target(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_climate_needle_target_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_climate_needle_target(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_climate_needle_target_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ClimateNum
//

void init_style_climate_num_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rn150);
    lv_style_set_text_letter_space(style, -5);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
};

lv_style_t *get_style_climate_num_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_climate_num_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_climate_num(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_climate_num_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_climate_num(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_climate_num_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ClimateModeText
//

void init_style_climate_mode_text_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][28]));
    lv_style_set_text_font(style, &ui_font_rr16);
    lv_style_set_text_letter_space(style, 2);
};

lv_style_t *get_style_climate_mode_text_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_climate_mode_text_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_climate_mode_text_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][31]));
};

lv_style_t *get_style_climate_mode_text_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_climate_mode_text_MAIN_CHECKED(style);
    }
    return style;
};

void init_style_climate_mode_text_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][30]));
};

lv_style_t *get_style_climate_mode_text_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_climate_mode_text_MAIN_PRESSED(style);
    }
    return style;
};

void init_style_climate_mode_text_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
};

lv_style_t *get_style_climate_mode_text_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_climate_mode_text_MAIN_DISABLED(style);
    }
    return style;
};

void add_style_climate_mode_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_climate_mode_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_climate_mode_text_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_climate_mode_text_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_climate_mode_text_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

void remove_style_climate_mode_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_climate_mode_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_climate_mode_text_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_climate_mode_text_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_climate_mode_text_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

//
// Style: ClimateModeIcon
//

void init_style_climate_mode_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][28]));
    lv_style_set_text_font(style, &ui_font_fa18);
};

lv_style_t *get_style_climate_mode_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_climate_mode_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_climate_mode_icon_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][31]));
};

lv_style_t *get_style_climate_mode_icon_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_climate_mode_icon_MAIN_CHECKED(style);
    }
    return style;
};

void init_style_climate_mode_icon_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][30]));
};

lv_style_t *get_style_climate_mode_icon_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_climate_mode_icon_MAIN_PRESSED(style);
    }
    return style;
};

void init_style_climate_mode_icon_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
};

lv_style_t *get_style_climate_mode_icon_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_climate_mode_icon_MAIN_DISABLED(style);
    }
    return style;
};

void add_style_climate_mode_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_climate_mode_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_climate_mode_icon_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_climate_mode_icon_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_climate_mode_icon_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

void remove_style_climate_mode_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_climate_mode_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_climate_mode_icon_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_climate_mode_icon_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_climate_mode_icon_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

//
// Style: EnergyArc
//

void init_style_energy_arc_MAIN_DEFAULT(lv_style_t *style) {
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

lv_style_t *get_style_energy_arc_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_energy_arc_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_energy_arc_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_arc_width(style, 14);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_arc_rounded(style, true);
};

lv_style_t *get_style_energy_arc_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_energy_arc_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_energy_arc_INDICATOR_CHECKED(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][11]));
    lv_style_set_arc_width(style, 14);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_arc_rounded(style, true);
};

lv_style_t *get_style_energy_arc_INDICATOR_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_energy_arc_INDICATOR_CHECKED(style);
    }
    return style;
};

void init_style_energy_arc_INDICATOR_PRESSED(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][13]));
    lv_style_set_arc_width(style, 14);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_arc_rounded(style, true);
};

lv_style_t *get_style_energy_arc_INDICATOR_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_energy_arc_INDICATOR_PRESSED(style);
    }
    return style;
};

void init_style_energy_arc_INDICATOR_DISABLED(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_arc_width(style, 14);
    lv_style_set_arc_opa(style, 0);
    lv_style_set_arc_rounded(style, true);
};

lv_style_t *get_style_energy_arc_INDICATOR_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_energy_arc_INDICATOR_DISABLED(style);
    }
    return style;
};

void init_style_energy_arc_KNOB_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_energy_arc_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_energy_arc_KNOB_DEFAULT(style);
    }
    return style;
};

void add_style_energy_arc(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_energy_arc_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_energy_arc_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_energy_arc_INDICATOR_CHECKED(), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_energy_arc_INDICATOR_PRESSED(), LV_PART_INDICATOR | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_energy_arc_INDICATOR_DISABLED(), LV_PART_INDICATOR | LV_STATE_DISABLED);
    lv_obj_add_style(obj, get_style_energy_arc_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
};

void remove_style_energy_arc(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_energy_arc_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_energy_arc_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_energy_arc_INDICATOR_CHECKED(), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_energy_arc_INDICATOR_PRESSED(), LV_PART_INDICATOR | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_energy_arc_INDICATOR_DISABLED(), LV_PART_INDICATOR | LV_STATE_DISABLED);
    lv_obj_remove_style(obj, get_style_energy_arc_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
};

//
// Style: EnergyHead
//

void init_style_energy_head_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][28]));
    lv_style_set_text_font(style, &ui_font_rr16);
    lv_style_set_text_letter_space(style, 2);
};

lv_style_t *get_style_energy_head_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_energy_head_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_energy_head_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][29]));
};

lv_style_t *get_style_energy_head_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_energy_head_MAIN_CHECKED(style);
    }
    return style;
};

void init_style_energy_head_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][30]));
};

lv_style_t *get_style_energy_head_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_energy_head_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_energy_head(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_energy_head_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_energy_head_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_energy_head_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_energy_head(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_energy_head_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_energy_head_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_energy_head_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: EnergyHeadIcon
//

void init_style_energy_head_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][28]));
    lv_style_set_text_font(style, &ui_font_fa18);
};

lv_style_t *get_style_energy_head_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_energy_head_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_energy_head_icon_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][29]));
};

lv_style_t *get_style_energy_head_icon_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_energy_head_icon_MAIN_CHECKED(style);
    }
    return style;
};

void init_style_energy_head_icon_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][30]));
};

lv_style_t *get_style_energy_head_icon_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_energy_head_icon_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_energy_head_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_energy_head_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_energy_head_icon_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_energy_head_icon_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_energy_head_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_energy_head_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_energy_head_icon_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_energy_head_icon_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: EnergyUnit
//

void init_style_energy_unit_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &ui_font_rr30);
    lv_style_set_translate_y(style, 6);
};

lv_style_t *get_style_energy_unit_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_energy_unit_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_energy_unit(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_energy_unit_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_energy_unit(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_energy_unit_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: SettingsTile
//

void init_style_settings_tile_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_border_width(style, 2);
    lv_style_set_border_opa(style, 255);
    lv_style_set_shadow_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_shadow_opa(style, 128);
    lv_style_set_shadow_width(style, 20);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_radius(style, 1000);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_settings_tile_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_tile_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_settings_tile_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
    lv_style_set_shadow_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
};

lv_style_t *get_style_settings_tile_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_tile_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_settings_tile(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_settings_tile_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_settings_tile_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_settings_tile(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_settings_tile_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_settings_tile_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: SettingsIcon
//

void init_style_settings_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &ui_font_fa56);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
};

lv_style_t *get_style_settings_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_settings_icon_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
};

lv_style_t *get_style_settings_icon_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_icon_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_settings_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_settings_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_settings_icon_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_settings_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_settings_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_settings_icon_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: SettingsValue
//

void init_style_settings_value_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &ui_font_rr20);
};

lv_style_t *get_style_settings_value_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_value_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_settings_value_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][28]));
};

lv_style_t *get_style_settings_value_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_value_MAIN_CHECKED(style);
    }
    return style;
};

void init_style_settings_value_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][29]));
};

lv_style_t *get_style_settings_value_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_value_MAIN_PRESSED(style);
    }
    return style;
};

void init_style_settings_value_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][31]));
};

lv_style_t *get_style_settings_value_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_value_MAIN_DISABLED(style);
    }
    return style;
};

void add_style_settings_value(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_settings_value_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_settings_value_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_settings_value_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_settings_value_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

void remove_style_settings_value(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_settings_value_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_settings_value_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_settings_value_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_settings_value_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

//
// Style: ListRow
//

void init_style_list_row_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 2);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 12);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_list_row_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_list_row_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_list_row_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
};

lv_style_t *get_style_list_row_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_list_row_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_list_row(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_list_row_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_list_row_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_list_row(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_list_row_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_list_row_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: GuideTile
//

void init_style_guide_tile_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_border_width(style, 2);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 1000);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_guide_tile_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_guide_tile_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_guide_tile(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_guide_tile_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_guide_tile(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_guide_tile_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: GuideIcon
//

void init_style_guide_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &ui_font_fa56);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
};

lv_style_t *get_style_guide_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_guide_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_guide_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_guide_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_guide_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_guide_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: GuideTitle
//

void init_style_guide_title_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rm30);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
};

lv_style_t *get_style_guide_title_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_guide_title_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_guide_title(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_guide_title_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_guide_title(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_guide_title_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: GuideBody
//

void init_style_guide_body_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &ui_font_rr18);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_line_space(style, 5);
};

lv_style_t *get_style_guide_body_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_guide_body_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_guide_body(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_guide_body_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_guide_body(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_guide_body_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: GuideHint
//

void init_style_guide_hint_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][28]));
    lv_style_set_text_font(style, &ui_font_rr16);
};

lv_style_t *get_style_guide_hint_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_guide_hint_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_guide_hint(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_guide_hint_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_guide_hint(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_guide_hint_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: GuideHintIcon
//

void init_style_guide_hint_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][28]));
    lv_style_set_text_font(style, &ui_font_fa18);
};

lv_style_t *get_style_guide_hint_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_guide_hint_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_guide_hint_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_guide_hint_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_guide_hint_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_guide_hint_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: GuideDot
//

void init_style_guide_dot_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 1000);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_guide_dot_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_guide_dot_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_guide_dot_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_guide_dot_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_guide_dot_MAIN_DISABLED(style);
    }
    return style;
};

void init_style_guide_dot_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_transform_width(style, 2);
    lv_style_set_transform_height(style, 2);
};

lv_style_t *get_style_guide_dot_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_guide_dot_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_guide_dot(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_guide_dot_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_guide_dot_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_add_style(obj, get_style_guide_dot_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_guide_dot(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_guide_dot_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_guide_dot_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_remove_style(obj, get_style_guide_dot_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: FaceTicks
//

void init_style_face_ticks_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_width(style, 0);
    lv_style_set_arc_opa(style, 0);
    lv_style_set_line_width(style, 0);
    lv_style_set_line_opa(style, 0);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_face_ticks_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_ticks_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_face_ticks_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_line_width(style, 2);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
    lv_style_set_length(style, 10);
};

lv_style_t *get_style_face_ticks_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_ticks_ITEMS_DEFAULT(style);
    }
    return style;
};

void init_style_face_ticks_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_line_width(style, 5);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
    lv_style_set_length(style, 26);
};

lv_style_t *get_style_face_ticks_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_ticks_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_face_ticks(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_ticks_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_face_ticks_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_face_ticks_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_face_ticks(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_ticks_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_face_ticks_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_face_ticks_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: FaceSecTicks
//

void init_style_face_sec_ticks_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_width(style, 0);
    lv_style_set_arc_opa(style, 0);
    lv_style_set_line_width(style, 0);
    lv_style_set_line_opa(style, 0);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_face_sec_ticks_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_sec_ticks_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_face_sec_ticks_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_line_width(style, 3);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
    lv_style_set_length(style, 10);
};

lv_style_t *get_style_face_sec_ticks_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_sec_ticks_ITEMS_DEFAULT(style);
    }
    return style;
};

void init_style_face_sec_ticks_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_line_width(style, 3);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
    lv_style_set_length(style, 18);
};

lv_style_t *get_style_face_sec_ticks_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_sec_ticks_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_face_sec_ticks(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_sec_ticks_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_face_sec_ticks_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_face_sec_ticks_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_face_sec_ticks(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_sec_ticks_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_face_sec_ticks_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_face_sec_ticks_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: FaceSecOn
//

void init_style_face_sec_on_MAIN_DEFAULT(lv_style_t *style) {
};

lv_style_t *get_style_face_sec_on_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_sec_on_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_face_sec_on_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_line_width(style, 4);
    lv_style_set_line_opa(style, 255);
};

lv_style_t *get_style_face_sec_on_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_sec_on_ITEMS_DEFAULT(style);
    }
    return style;
};

void init_style_face_sec_on_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_line_width(style, 4);
    lv_style_set_line_opa(style, 255);
};

lv_style_t *get_style_face_sec_on_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_sec_on_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_face_sec_on(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_sec_on_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_face_sec_on_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_face_sec_on_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_face_sec_on(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_sec_on_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_face_sec_on_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_face_sec_on_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: FaceHandH
//

void init_style_face_hand_h_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_line_width(style, 10);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
};

lv_style_t *get_style_face_hand_h_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_hand_h_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_face_hand_h(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_hand_h_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_face_hand_h(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_hand_h_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: FaceHandM
//

void init_style_face_hand_m_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_line_width(style, 6);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
};

lv_style_t *get_style_face_hand_m_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_hand_m_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_face_hand_m(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_hand_m_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_face_hand_m(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_hand_m_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: FaceHandS
//

void init_style_face_hand_s_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_line_width(style, 2);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
};

lv_style_t *get_style_face_hand_s_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_hand_s_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_face_hand_s(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_hand_s_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_face_hand_s(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_hand_s_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: FaceCap
//

void init_style_face_cap_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 1000);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_face_cap_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_cap_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_face_cap(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_cap_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_face_cap(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_cap_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: FaceCapIn
//

void init_style_face_cap_in_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 1000);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_face_cap_in_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_cap_in_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_face_cap_in(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_cap_in_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_face_cap_in(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_cap_in_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: FaceMinArc
//

void init_style_face_min_arc_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_arc_width(style, 4);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_arc_rounded(style, false);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_face_min_arc_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_min_arc_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_face_min_arc_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_arc_width(style, 4);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_arc_rounded(style, true);
};

lv_style_t *get_style_face_min_arc_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_min_arc_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_face_min_arc_KNOB_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 4);
    lv_style_set_pad_bottom(style, 4);
    lv_style_set_pad_left(style, 4);
    lv_style_set_pad_right(style, 4);
};

lv_style_t *get_style_face_min_arc_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_min_arc_KNOB_DEFAULT(style);
    }
    return style;
};

void add_style_face_min_arc(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_min_arc_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_face_min_arc_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_face_min_arc_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
};

void remove_style_face_min_arc(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_min_arc_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_face_min_arc_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_face_min_arc_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
};

//
// Style: FaceRingArc
//

void init_style_face_ring_arc_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_arc_width(style, 3);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_arc_rounded(style, false);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_face_ring_arc_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_ring_arc_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_face_ring_arc_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_arc_width(style, 8);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_arc_rounded(style, true);
};

lv_style_t *get_style_face_ring_arc_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_ring_arc_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_face_ring_arc_KNOB_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_border_width(style, 6);
    lv_style_set_border_opa(style, 255);
    lv_style_set_pad_top(style, 6);
    lv_style_set_pad_bottom(style, 6);
    lv_style_set_pad_left(style, 6);
    lv_style_set_pad_right(style, 6);
};

lv_style_t *get_style_face_ring_arc_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_ring_arc_KNOB_DEFAULT(style);
    }
    return style;
};

void add_style_face_ring_arc(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_ring_arc_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_face_ring_arc_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_face_ring_arc_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
};

void remove_style_face_ring_arc(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_ring_arc_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_face_ring_arc_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_face_ring_arc_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
};

//
// Style: FaceTextLg
//

void init_style_face_text_lg_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &ui_font_rr20);
};

lv_style_t *get_style_face_text_lg_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_text_lg_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_face_text_lg(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_text_lg_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_face_text_lg(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_text_lg_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: FaceTextMd
//

void init_style_face_text_md_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &ui_font_rr18);
};

lv_style_t *get_style_face_text_md_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_text_md_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_face_text_md(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_text_md_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_face_text_md(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_text_md_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: FaceTextMuted
//

void init_style_face_text_muted_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &ui_font_rr18);
};

lv_style_t *get_style_face_text_muted_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_text_muted_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_face_text_muted(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_text_muted_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_face_text_muted(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_text_muted_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: FaceStat
//

void init_style_face_stat_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &ui_font_rr16);
};

lv_style_t *get_style_face_stat_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_stat_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_face_stat(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_stat_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_face_stat(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_stat_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: FaceStatIcon
//

void init_style_face_stat_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &ui_font_fa18);
};

lv_style_t *get_style_face_stat_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_stat_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_face_stat_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_stat_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_face_stat_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_stat_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: FaceBrandName
//

void init_style_face_brand_name_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &ui_font_rr20);
    lv_style_set_text_letter_space(style, 1);
};

lv_style_t *get_style_face_brand_name_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_brand_name_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_face_brand_name(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_brand_name_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_face_brand_name(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_brand_name_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: FaceTime
//

void init_style_face_time_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rn150);
    lv_style_set_text_letter_space(style, -5);
};

lv_style_t *get_style_face_time_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_time_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_face_time(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_time_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_face_time(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_time_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: FaceTimeSm
//

void init_style_face_time_sm_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rn120);
    lv_style_set_text_letter_space(style, -4);
};

lv_style_t *get_style_face_time_sm_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_time_sm_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_face_time_sm(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_time_sm_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_face_time_sm(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_time_sm_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: FaceAmPm
//

void init_style_face_am_pm_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &ui_font_rr24);
    lv_style_set_pad_top(style, 22);
};

lv_style_t *get_style_face_am_pm_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_am_pm_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_face_am_pm(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_am_pm_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_face_am_pm(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_am_pm_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: FaceAmPmSm
//

void init_style_face_am_pm_sm_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &ui_font_rr20);
    lv_style_set_pad_top(style, 16);
};

lv_style_t *get_style_face_am_pm_sm_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_am_pm_sm_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_face_am_pm_sm(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_am_pm_sm_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_face_am_pm_sm(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_am_pm_sm_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: FaceRingTime
//

void init_style_face_ring_time_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &ui_font_rr24);
};

lv_style_t *get_style_face_ring_time_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_ring_time_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_face_ring_time(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_ring_time_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_face_ring_time(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_ring_time_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: FaceMode
//

void init_style_face_mode_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][28]));
    lv_style_set_text_font(style, &ui_font_rr16);
};

lv_style_t *get_style_face_mode_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_mode_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_face_mode_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][31]));
};

lv_style_t *get_style_face_mode_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_mode_MAIN_CHECKED(style);
    }
    return style;
};

void init_style_face_mode_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][30]));
};

lv_style_t *get_style_face_mode_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_mode_MAIN_PRESSED(style);
    }
    return style;
};

void init_style_face_mode_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
};

lv_style_t *get_style_face_mode_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_mode_MAIN_DISABLED(style);
    }
    return style;
};

void add_style_face_mode(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_mode_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_face_mode_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_face_mode_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_face_mode_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

void remove_style_face_mode(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_mode_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_face_mode_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_face_mode_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_face_mode_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

//
// Style: FaceModeIcon
//

void init_style_face_mode_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][28]));
    lv_style_set_text_font(style, &ui_font_fa18);
};

lv_style_t *get_style_face_mode_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_mode_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_face_mode_icon_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][31]));
};

lv_style_t *get_style_face_mode_icon_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_mode_icon_MAIN_CHECKED(style);
    }
    return style;
};

void init_style_face_mode_icon_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][30]));
};

lv_style_t *get_style_face_mode_icon_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_mode_icon_MAIN_PRESSED(style);
    }
    return style;
};

void init_style_face_mode_icon_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
};

lv_style_t *get_style_face_mode_icon_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_mode_icon_MAIN_DISABLED(style);
    }
    return style;
};

void add_style_face_mode_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_mode_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_face_mode_icon_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_face_mode_icon_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_face_mode_icon_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

void remove_style_face_mode_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_mode_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_face_mode_icon_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_face_mode_icon_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_face_mode_icon_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

//
// Style: FaceHourNum
//

void init_style_face_hour_num_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &ui_font_rr20);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
};

lv_style_t *get_style_face_hour_num_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_hour_num_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_face_hour_num_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rm30);
};

lv_style_t *get_style_face_hour_num_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_hour_num_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_face_hour_num(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_hour_num_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_face_hour_num_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_face_hour_num(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_hour_num_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_face_hour_num_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: FacePreview
//

void init_style_face_preview_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 2);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 1000);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_face_preview_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_preview_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_face_preview(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_preview_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_face_preview(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_preview_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: FaceScaled
//

void init_style_face_scaled_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_transform_scale_x(style, 151);
    lv_style_set_transform_scale_y(style, 151);
    lv_style_set_transform_pivot_x(style, 0);
    lv_style_set_transform_pivot_y(style, 0);
};

lv_style_t *get_style_face_scaled_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_scaled_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_face_scaled(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_scaled_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_face_scaled(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_scaled_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: FacePickSub
//

void init_style_face_pick_sub_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &ui_font_rr16);
};

lv_style_t *get_style_face_pick_sub_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_pick_sub_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_face_pick_sub_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][28]));
};

lv_style_t *get_style_face_pick_sub_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_face_pick_sub_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_face_pick_sub(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_face_pick_sub_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_face_pick_sub_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_face_pick_sub(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_face_pick_sub_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_face_pick_sub_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: MenuName
//

void init_style_menu_name_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rm34);
};

lv_style_t *get_style_menu_name_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_menu_name_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_menu_name(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_menu_name_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_menu_name(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_menu_name_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: MenuSummary
//

void init_style_menu_summary_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &ui_font_rr20);
};

lv_style_t *get_style_menu_summary_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_menu_summary_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_menu_summary(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_menu_summary_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_menu_summary(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_menu_summary_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ModeRowHeat
//

void init_style_mode_row_heat_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 2);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 12);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_mode_row_heat_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_mode_row_heat_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_mode_row_heat_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][31]));
};

lv_style_t *get_style_mode_row_heat_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_mode_row_heat_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_mode_row_heat(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_mode_row_heat_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_mode_row_heat_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_mode_row_heat(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_mode_row_heat_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_mode_row_heat_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: ModeRowCool
//

void init_style_mode_row_cool_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 2);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 12);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_mode_row_cool_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_mode_row_cool_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_mode_row_cool_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][13]));
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][30]));
};

lv_style_t *get_style_mode_row_cool_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_mode_row_cool_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_mode_row_cool(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_mode_row_cool_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_mode_row_cool_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_mode_row_cool(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_mode_row_cool_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_mode_row_cool_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: ModeRowAuto
//

void init_style_mode_row_auto_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 2);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 12);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_mode_row_auto_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_mode_row_auto_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_mode_row_auto_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][28]));
};

lv_style_t *get_style_mode_row_auto_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_mode_row_auto_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_mode_row_auto(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_mode_row_auto_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_mode_row_auto_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_mode_row_auto(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_mode_row_auto_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_mode_row_auto_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: ModeRowOff
//

void init_style_mode_row_off_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 2);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 12);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_mode_row_off_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_mode_row_off_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_mode_row_off_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
};

lv_style_t *get_style_mode_row_off_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_mode_row_off_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_mode_row_off(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_mode_row_off_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_mode_row_off_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_mode_row_off(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_mode_row_off_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_mode_row_off_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: ModeRowIcon
//

void init_style_mode_row_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &ui_font_fa24);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
};

lv_style_t *get_style_mode_row_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_mode_row_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_mode_row_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_mode_row_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_mode_row_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_mode_row_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ModeRowText
//

void init_style_mode_row_text_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &ui_font_rr24);
};

lv_style_t *get_style_mode_row_text_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_mode_row_text_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_mode_row_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_mode_row_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_mode_row_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_mode_row_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: LabelBodyMuted
//

void init_style_label_body_muted_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &ui_font_rr16);
};

lv_style_t *get_style_label_body_muted_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_body_muted_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_label_body_muted(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_body_muted_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_body_muted(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_body_muted_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: EnergyDot
//

void init_style_energy_dot_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 1000);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_energy_dot_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_energy_dot_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_energy_dot_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_energy_dot_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_energy_dot_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_energy_dot(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_energy_dot_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_energy_dot_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_energy_dot(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_energy_dot_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_energy_dot_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: AlertIcon
//

void init_style_alert_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][31]));
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
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 12);
    lv_style_set_pad_top(style, 1);
    lv_style_set_pad_bottom(style, 1);
    lv_style_set_pad_left(style, 1);
    lv_style_set_pad_right(style, 1);
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
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][18]));
    lv_style_set_bg_grad_color(style, lv_color_hex(theme_colors[active_theme_index][17]));
    lv_style_set_bg_grad_dir(style, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(style, 255);
    lv_style_set_radius(style, 0);
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
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 12);
    lv_style_set_pad_top(style, 1);
    lv_style_set_pad_bottom(style, 1);
    lv_style_set_pad_left(style, 1);
    lv_style_set_pad_right(style, 1);
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
    lv_style_set_radius(style, 0);
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
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 12);
    lv_style_set_pad_top(style, 1);
    lv_style_set_pad_bottom(style, 1);
    lv_style_set_pad_left(style, 1);
    lv_style_set_pad_right(style, 1);
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
    lv_style_set_radius(style, 0);
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
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
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
    lv_style_set_text_font(style, &ui_font_rr20);
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
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][28]));
    lv_style_set_text_font(style, &ui_font_rr24);
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
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][29]));
    lv_style_set_text_font(style, &ui_font_rr24);
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
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][31]));
    lv_style_set_text_font(style, &ui_font_rr24);
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
    lv_style_set_text_font(style, &ui_font_rr24);
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
        add_style_climate_ticks,
        add_style_climate_section_heat,
        add_style_climate_section_cool,
        add_style_climate_section_hold,
        add_style_climate_needle_inside,
        add_style_climate_needle_target,
        add_style_climate_num,
        add_style_climate_mode_text,
        add_style_climate_mode_icon,
        add_style_energy_arc,
        add_style_energy_head,
        add_style_energy_head_icon,
        add_style_energy_unit,
        add_style_settings_tile,
        add_style_settings_icon,
        add_style_settings_value,
        add_style_list_row,
        add_style_guide_tile,
        add_style_guide_icon,
        add_style_guide_title,
        add_style_guide_body,
        add_style_guide_hint,
        add_style_guide_hint_icon,
        add_style_guide_dot,
        add_style_face_ticks,
        add_style_face_sec_ticks,
        add_style_face_sec_on,
        add_style_face_hand_h,
        add_style_face_hand_m,
        add_style_face_hand_s,
        add_style_face_cap,
        add_style_face_cap_in,
        add_style_face_min_arc,
        add_style_face_ring_arc,
        add_style_face_text_lg,
        add_style_face_text_md,
        add_style_face_text_muted,
        add_style_face_stat,
        add_style_face_stat_icon,
        add_style_face_brand_name,
        add_style_face_time,
        add_style_face_time_sm,
        add_style_face_am_pm,
        add_style_face_am_pm_sm,
        add_style_face_ring_time,
        add_style_face_mode,
        add_style_face_mode_icon,
        add_style_face_hour_num,
        add_style_face_preview,
        add_style_face_scaled,
        add_style_face_pick_sub,
        add_style_menu_name,
        add_style_menu_summary,
        add_style_mode_row_heat,
        add_style_mode_row_cool,
        add_style_mode_row_auto,
        add_style_mode_row_off,
        add_style_mode_row_icon,
        add_style_mode_row_text,
        add_style_label_body_muted,
        add_style_energy_dot,
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
        remove_style_climate_ticks,
        remove_style_climate_section_heat,
        remove_style_climate_section_cool,
        remove_style_climate_section_hold,
        remove_style_climate_needle_inside,
        remove_style_climate_needle_target,
        remove_style_climate_num,
        remove_style_climate_mode_text,
        remove_style_climate_mode_icon,
        remove_style_energy_arc,
        remove_style_energy_head,
        remove_style_energy_head_icon,
        remove_style_energy_unit,
        remove_style_settings_tile,
        remove_style_settings_icon,
        remove_style_settings_value,
        remove_style_list_row,
        remove_style_guide_tile,
        remove_style_guide_icon,
        remove_style_guide_title,
        remove_style_guide_body,
        remove_style_guide_hint,
        remove_style_guide_hint_icon,
        remove_style_guide_dot,
        remove_style_face_ticks,
        remove_style_face_sec_ticks,
        remove_style_face_sec_on,
        remove_style_face_hand_h,
        remove_style_face_hand_m,
        remove_style_face_hand_s,
        remove_style_face_cap,
        remove_style_face_cap_in,
        remove_style_face_min_arc,
        remove_style_face_ring_arc,
        remove_style_face_text_lg,
        remove_style_face_text_md,
        remove_style_face_text_muted,
        remove_style_face_stat,
        remove_style_face_stat_icon,
        remove_style_face_brand_name,
        remove_style_face_time,
        remove_style_face_time_sm,
        remove_style_face_am_pm,
        remove_style_face_am_pm_sm,
        remove_style_face_ring_time,
        remove_style_face_mode,
        remove_style_face_mode_icon,
        remove_style_face_hour_num,
        remove_style_face_preview,
        remove_style_face_scaled,
        remove_style_face_pick_sub,
        remove_style_menu_name,
        remove_style_menu_summary,
        remove_style_mode_row_heat,
        remove_style_mode_row_cool,
        remove_style_mode_row_auto,
        remove_style_mode_row_off,
        remove_style_mode_row_icon,
        remove_style_mode_row_text,
        remove_style_label_body_muted,
        remove_style_energy_dot,
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