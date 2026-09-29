#ifndef EEZ_LVGL_UI_FONTS_H
#define EEZ_LVGL_UI_FONTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_font_t ui_font_rl72;
extern const lv_font_t ui_font_rn150;
extern const lv_font_t ui_font_rn120;
extern const lv_font_t ui_font_rm26;
extern const lv_font_t ui_font_rm20;
extern const lv_font_t ui_font_rr30;
extern const lv_font_t ui_font_rr16;
extern const lv_font_t ui_font_rr13;
extern const lv_font_t ui_font_fa56;
extern const lv_font_t ui_font_fa36;
extern const lv_font_t ui_font_fa24;
extern const lv_font_t ui_font_fa18;
extern const lv_font_t ui_font_fa14;
extern const lv_font_t ui_font_fh68;
extern const lv_font_t ui_font_fh44;

#ifndef EXT_FONT_DESC_T
#define EXT_FONT_DESC_T
typedef struct _ext_font_desc_t {
    const char *name;
    const void *font_ptr;
} ext_font_desc_t;
#endif

extern ext_font_desc_t fonts[];

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_FONTS_H*/