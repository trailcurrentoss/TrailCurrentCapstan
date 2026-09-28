#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: ScreenRoot
lv_style_t *get_style_screen_root_MAIN_DEFAULT();
void add_style_screen_root(lv_obj_t *obj);
void remove_style_screen_root(lv_obj_t *obj);

// Style: Card
lv_style_t *get_style_card_MAIN_DEFAULT();
lv_style_t *get_style_card_MAIN_CHECKED();
void add_style_card(lv_obj_t *obj);
void remove_style_card(lv_obj_t *obj);

// Style: CardSelected
lv_style_t *get_style_card_selected_MAIN_DEFAULT();
void add_style_card_selected(lv_obj_t *obj);
void remove_style_card_selected(lv_obj_t *obj);

// Style: LabelTitle
lv_style_t *get_style_label_title_MAIN_DEFAULT();
void add_style_label_title(lv_obj_t *obj);
void remove_style_label_title(lv_obj_t *obj);

// Style: LabelBody
lv_style_t *get_style_label_body_MAIN_DEFAULT();
void add_style_label_body(lv_obj_t *obj);
void remove_style_label_body(lv_obj_t *obj);

// Style: LabelMuted
lv_style_t *get_style_label_muted_MAIN_DEFAULT();
void add_style_label_muted(lv_obj_t *obj);
void remove_style_label_muted(lv_obj_t *obj);

// Style: LabelHero
lv_style_t *get_style_label_hero_MAIN_DEFAULT();
void add_style_label_hero(lv_obj_t *obj);
void remove_style_label_hero(lv_obj_t *obj);

// Style: ButtonPrimary
lv_style_t *get_style_button_primary_MAIN_DEFAULT();
void add_style_button_primary(lv_obj_t *obj);
void remove_style_button_primary(lv_obj_t *obj);

// Style: ButtonNeutral
lv_style_t *get_style_button_neutral_MAIN_DEFAULT();
void add_style_button_neutral(lv_obj_t *obj);
void remove_style_button_neutral(lv_obj_t *obj);

// Style: ScaleRing
lv_style_t *get_style_scale_ring_MAIN_DEFAULT();
lv_style_t *get_style_scale_ring_ITEMS_DEFAULT();
lv_style_t *get_style_scale_ring_INDICATOR_DEFAULT();
void add_style_scale_ring(lv_obj_t *obj);
void remove_style_scale_ring(lv_obj_t *obj);

// Style: LabelIcon
lv_style_t *get_style_label_icon_MAIN_DEFAULT();
void add_style_label_icon(lv_obj_t *obj);
void remove_style_label_icon(lv_obj_t *obj);

// Style: LabelIconSm
lv_style_t *get_style_label_icon_sm_MAIN_DEFAULT();
void add_style_label_icon_sm(lv_obj_t *obj);
void remove_style_label_icon_sm(lv_obj_t *obj);

// Style: Plain
lv_style_t *get_style_plain_MAIN_DEFAULT();
void add_style_plain(lv_obj_t *obj);
void remove_style_plain(lv_obj_t *obj);

// Style: Pivot
lv_style_t *get_style_pivot_MAIN_DEFAULT();
void add_style_pivot(lv_obj_t *obj);
void remove_style_pivot(lv_obj_t *obj);

// Style: HandHour
lv_style_t *get_style_hand_hour_MAIN_DEFAULT();
void add_style_hand_hour(lv_obj_t *obj);
void remove_style_hand_hour(lv_obj_t *obj);

// Style: HandMinute
lv_style_t *get_style_hand_minute_MAIN_DEFAULT();
void add_style_hand_minute(lv_obj_t *obj);
void remove_style_hand_minute(lv_obj_t *obj);

// Style: HandSecond
lv_style_t *get_style_hand_second_MAIN_DEFAULT();
void add_style_hand_second(lv_obj_t *obj);
void remove_style_hand_second(lv_obj_t *obj);

// Style: BarLevel
lv_style_t *get_style_bar_level_MAIN_DEFAULT();
lv_style_t *get_style_bar_level_INDICATOR_DEFAULT();
void add_style_bar_level(lv_obj_t *obj);
void remove_style_bar_level(lv_obj_t *obj);

// Style: BarFresh
lv_style_t *get_style_bar_fresh_MAIN_DEFAULT();
lv_style_t *get_style_bar_fresh_INDICATOR_DEFAULT();
void add_style_bar_fresh(lv_obj_t *obj);
void remove_style_bar_fresh(lv_obj_t *obj);

// Style: BarGrey
lv_style_t *get_style_bar_grey_MAIN_DEFAULT();
lv_style_t *get_style_bar_grey_INDICATOR_DEFAULT();
void add_style_bar_grey(lv_obj_t *obj);
void remove_style_bar_grey(lv_obj_t *obj);

// Style: BarBlack
lv_style_t *get_style_bar_black_MAIN_DEFAULT();
lv_style_t *get_style_bar_black_INDICATOR_DEFAULT();
void add_style_bar_black(lv_obj_t *obj);
void remove_style_bar_black(lv_obj_t *obj);

// Style: Field
lv_style_t *get_style_field_MAIN_DEFAULT();
lv_style_t *get_style_field_TEXTAREA_PLACEHOLDER_DEFAULT();
lv_style_t *get_style_field_CURSOR_DEFAULT();
void add_style_field(lv_obj_t *obj);
void remove_style_field(lv_obj_t *obj);

// Style: ArcValue
lv_style_t *get_style_arc_value_MAIN_DEFAULT();
lv_style_t *get_style_arc_value_INDICATOR_DEFAULT();
void add_style_arc_value(lv_obj_t *obj);
void remove_style_arc_value(lv_obj_t *obj);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/