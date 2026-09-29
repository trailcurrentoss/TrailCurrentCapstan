/*
 * The idle clock: four faces from the newer prototype (DOCS/GettingStarted),
 * one showing -- Settings > Clock Face picks it -- and the picker screen
 * that previews them. See face_*() and page_clock_face() in
 * GUI/tmp/screens_layout.py for the layouts this fills in.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    UI_CLOCK_FACE_CLASSIC = 0,
    UI_CLOCK_FACE_DIGITAL,
    UI_CLOCK_FACE_BRAND,
    UI_CLOCK_FACE_RING,
    UI_CLOCK_FACE_COUNT
};

void ui_clock_init(void);

/** Repaint the idle face now (on entry, so it never shows a stale time). */
void ui_clock_refresh(void);

/** The face chosen in Settings, and a face's display name. */
int         ui_clock_face(void);
const char *ui_clock_face_name(int face);

/** The picker: open on the current face, preview another, set the one
 *  showing (saved to NVS). LVGL lock held. */
void ui_clock_pick_enter(void);
void ui_clock_pick_rotate(int diff);
void ui_clock_pick_press(void);

/** Seconds past the minute, 0..59, for the Digital face's section. */
int32_t ui_clock_second(void);

#ifdef __cplusplus
}
#endif
