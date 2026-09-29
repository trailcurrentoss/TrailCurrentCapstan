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

// Style: HeroTile
lv_style_t *get_style_hero_tile_MAIN_DEFAULT();
void add_style_hero_tile(lv_obj_t *obj);
void remove_style_hero_tile(lv_obj_t *obj);

// Style: HeroIcon
lv_style_t *get_style_hero_icon_MAIN_DEFAULT();
void add_style_hero_icon(lv_obj_t *obj);
void remove_style_hero_icon(lv_obj_t *obj);

// Style: NeighbourIcon
lv_style_t *get_style_neighbour_icon_MAIN_DEFAULT();
void add_style_neighbour_icon(lv_obj_t *obj);
void remove_style_neighbour_icon(lv_obj_t *obj);

// Style: DeviceTile
lv_style_t *get_style_device_tile_MAIN_DEFAULT();
lv_style_t *get_style_device_tile_MAIN_CHECKED();
void add_style_device_tile(lv_obj_t *obj);
void remove_style_device_tile(lv_obj_t *obj);

// Style: DeviceIcon
lv_style_t *get_style_device_icon_MAIN_DEFAULT();
lv_style_t *get_style_device_icon_MAIN_CHECKED();
void add_style_device_icon(lv_obj_t *obj);
void remove_style_device_icon(lv_obj_t *obj);

// Style: DeviceNeighbourIcon
lv_style_t *get_style_device_neighbour_icon_MAIN_DEFAULT();
void add_style_device_neighbour_icon(lv_obj_t *obj);
void remove_style_device_neighbour_icon(lv_obj_t *obj);

// Style: AlertRing
lv_style_t *get_style_alert_ring_MAIN_DEFAULT();
void add_style_alert_ring(lv_obj_t *obj);
void remove_style_alert_ring(lv_obj_t *obj);

// Style: LevelWell
lv_style_t *get_style_level_well_MAIN_DEFAULT();
void add_style_level_well(lv_obj_t *obj);
void remove_style_level_well(lv_obj_t *obj);

// Style: LevelRing
lv_style_t *get_style_level_ring_MAIN_DEFAULT();
void add_style_level_ring(lv_obj_t *obj);
void remove_style_level_ring(lv_obj_t *obj);

// Style: LevelLine
lv_style_t *get_style_level_line_MAIN_DEFAULT();
void add_style_level_line(lv_obj_t *obj);
void remove_style_level_line(lv_obj_t *obj);

// Style: LevelBubble
lv_style_t *get_style_level_bubble_MAIN_DEFAULT();
void add_style_level_bubble(lv_obj_t *obj);
void remove_style_level_bubble(lv_obj_t *obj);

// Style: ClimateTicks
lv_style_t *get_style_climate_ticks_MAIN_DEFAULT();
lv_style_t *get_style_climate_ticks_INDICATOR_DEFAULT();
lv_style_t *get_style_climate_ticks_ITEMS_DEFAULT();
void add_style_climate_ticks(lv_obj_t *obj);
void remove_style_climate_ticks(lv_obj_t *obj);

// Style: ClimateSectionHeat
lv_style_t *get_style_climate_section_heat_MAIN_DEFAULT();
lv_style_t *get_style_climate_section_heat_INDICATOR_DEFAULT();
void add_style_climate_section_heat(lv_obj_t *obj);
void remove_style_climate_section_heat(lv_obj_t *obj);

// Style: ClimateSectionCool
lv_style_t *get_style_climate_section_cool_MAIN_DEFAULT();
lv_style_t *get_style_climate_section_cool_INDICATOR_DEFAULT();
void add_style_climate_section_cool(lv_obj_t *obj);
void remove_style_climate_section_cool(lv_obj_t *obj);

// Style: ClimateSectionHold
lv_style_t *get_style_climate_section_hold_MAIN_DEFAULT();
lv_style_t *get_style_climate_section_hold_INDICATOR_DEFAULT();
void add_style_climate_section_hold(lv_obj_t *obj);
void remove_style_climate_section_hold(lv_obj_t *obj);

// Style: ClimateNeedleInside
lv_style_t *get_style_climate_needle_inside_MAIN_DEFAULT();
void add_style_climate_needle_inside(lv_obj_t *obj);
void remove_style_climate_needle_inside(lv_obj_t *obj);

// Style: ClimateNeedleTarget
lv_style_t *get_style_climate_needle_target_MAIN_DEFAULT();
void add_style_climate_needle_target(lv_obj_t *obj);
void remove_style_climate_needle_target(lv_obj_t *obj);

// Style: ClimateNum
lv_style_t *get_style_climate_num_MAIN_DEFAULT();
void add_style_climate_num(lv_obj_t *obj);
void remove_style_climate_num(lv_obj_t *obj);

// Style: ClimateModeText
lv_style_t *get_style_climate_mode_text_MAIN_DEFAULT();
lv_style_t *get_style_climate_mode_text_MAIN_CHECKED();
lv_style_t *get_style_climate_mode_text_MAIN_PRESSED();
lv_style_t *get_style_climate_mode_text_MAIN_DISABLED();
void add_style_climate_mode_text(lv_obj_t *obj);
void remove_style_climate_mode_text(lv_obj_t *obj);

// Style: ClimateModeIcon
lv_style_t *get_style_climate_mode_icon_MAIN_DEFAULT();
lv_style_t *get_style_climate_mode_icon_MAIN_CHECKED();
lv_style_t *get_style_climate_mode_icon_MAIN_PRESSED();
lv_style_t *get_style_climate_mode_icon_MAIN_DISABLED();
void add_style_climate_mode_icon(lv_obj_t *obj);
void remove_style_climate_mode_icon(lv_obj_t *obj);

// Style: EnergyArc
lv_style_t *get_style_energy_arc_MAIN_DEFAULT();
lv_style_t *get_style_energy_arc_INDICATOR_DEFAULT();
lv_style_t *get_style_energy_arc_INDICATOR_CHECKED();
lv_style_t *get_style_energy_arc_INDICATOR_PRESSED();
lv_style_t *get_style_energy_arc_INDICATOR_DISABLED();
lv_style_t *get_style_energy_arc_KNOB_DEFAULT();
void add_style_energy_arc(lv_obj_t *obj);
void remove_style_energy_arc(lv_obj_t *obj);

// Style: EnergyHead
lv_style_t *get_style_energy_head_MAIN_DEFAULT();
lv_style_t *get_style_energy_head_MAIN_CHECKED();
lv_style_t *get_style_energy_head_MAIN_PRESSED();
void add_style_energy_head(lv_obj_t *obj);
void remove_style_energy_head(lv_obj_t *obj);

// Style: EnergyHeadIcon
lv_style_t *get_style_energy_head_icon_MAIN_DEFAULT();
lv_style_t *get_style_energy_head_icon_MAIN_CHECKED();
lv_style_t *get_style_energy_head_icon_MAIN_PRESSED();
void add_style_energy_head_icon(lv_obj_t *obj);
void remove_style_energy_head_icon(lv_obj_t *obj);

// Style: EnergyUnit
lv_style_t *get_style_energy_unit_MAIN_DEFAULT();
void add_style_energy_unit(lv_obj_t *obj);
void remove_style_energy_unit(lv_obj_t *obj);

// Style: LabelBodyMuted
lv_style_t *get_style_label_body_muted_MAIN_DEFAULT();
void add_style_label_body_muted(lv_obj_t *obj);
void remove_style_label_body_muted(lv_obj_t *obj);

// Style: EnergyDot
lv_style_t *get_style_energy_dot_MAIN_DEFAULT();
lv_style_t *get_style_energy_dot_MAIN_CHECKED();
void add_style_energy_dot(lv_obj_t *obj);
void remove_style_energy_dot(lv_obj_t *obj);

// Style: AlertIcon
lv_style_t *get_style_alert_icon_MAIN_DEFAULT();
void add_style_alert_icon(lv_obj_t *obj);
void remove_style_alert_icon(lv_obj_t *obj);

// Style: Dot
lv_style_t *get_style_dot_MAIN_DEFAULT();
lv_style_t *get_style_dot_MAIN_CHECKED();
void add_style_dot(lv_obj_t *obj);
void remove_style_dot(lv_obj_t *obj);

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

// Style: ArcThin
lv_style_t *get_style_arc_thin_MAIN_DEFAULT();
lv_style_t *get_style_arc_thin_INDICATOR_DEFAULT();
lv_style_t *get_style_arc_thin_INDICATOR_CHECKED();
lv_style_t *get_style_arc_thin_INDICATOR_DISABLED();
lv_style_t *get_style_arc_thin_INDICATOR_PRESSED();
lv_style_t *get_style_arc_thin_KNOB_DEFAULT();
void add_style_arc_thin(lv_obj_t *obj);
void remove_style_arc_thin(lv_obj_t *obj);

// Style: LabelEyebrow
lv_style_t *get_style_label_eyebrow_MAIN_DEFAULT();
void add_style_label_eyebrow(lv_obj_t *obj);
void remove_style_label_eyebrow(lv_obj_t *obj);

// Style: LabelHeroXL
lv_style_t *get_style_label_hero_xl_MAIN_DEFAULT();
void add_style_label_hero_xl(lv_obj_t *obj);
void remove_style_label_hero_xl(lv_obj_t *obj);

// Style: LabelMetric
lv_style_t *get_style_label_metric_MAIN_DEFAULT();
void add_style_label_metric(lv_obj_t *obj);
void remove_style_label_metric(lv_obj_t *obj);

// Style: LabelStatus
lv_style_t *get_style_label_status_MAIN_DEFAULT();
lv_style_t *get_style_label_status_MAIN_CHECKED();
lv_style_t *get_style_label_status_MAIN_DISABLED();
lv_style_t *get_style_label_status_MAIN_PRESSED();
void add_style_label_status(lv_obj_t *obj);
void remove_style_label_status(lv_obj_t *obj);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/