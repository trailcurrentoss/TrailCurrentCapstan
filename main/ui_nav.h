/*
 * Screen navigation and per-screen input policy.
 *
 * WHY THIS EXISTS
 *
 * Touch and the ring press are the same physical action on this hardware --
 * the whole panel is the button. If both are live, one press produces an
 * encoder ENTER and a touch event, and the press lands on whatever is under
 * the user's finger instead of on whatever the ring had focused.
 *
 * So touch must be live on some screens (the keyboard is unusable without
 * it) and dead on others (anything the ring drives on its own). That is a
 * per-screen decision, and the wrong way to implement it is to have each
 * screen enable touch on entry and remember to disable it on exit -- one
 * missed exit path, one early return, one alert overlay stealing the
 * transition, and touch is silently live on a screen that fights it. That
 * bug would be intermittent and miserable to find.
 *
 * Instead the policy is declared once per screen in a table, and applied
 * centrally on every transition. A screen cannot forget, because a screen is
 * never the thing that decides.
 *
 * The board layer owns the mechanism (capstan_board_touch_set_enabled) --
 * it is where the LVGL indev handle lives. This layer owns the policy.
 */
#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Screens, in the order the prototype's menu carousel presents them. */
typedef enum {
    CAPSTAN_SCREEN_IDLE = 0,   /* analog clock */
    CAPSTAN_SCREEN_MENU,       /* the app carousel -- same on all three */
    CAPSTAN_SCREEN_CLIMATE,
    CAPSTAN_SCREEN_CLIMATE_MODE,
    CAPSTAN_SCREEN_LIGHTS,
    CAPSTAN_SCREEN_HEATER,
    CAPSTAN_SCREEN_ENERGY,
    CAPSTAN_SCREEN_WATER,
    CAPSTAN_SCREEN_AIR,
    CAPSTAN_SCREEN_LEVEL,
    CAPSTAN_SCREEN_DOORS,
    CAPSTAN_SCREEN_SETTINGS,
    CAPSTAN_SCREEN_ALERT,      /* full-screen overlay */
    CAPSTAN_SCREEN_SETUP,      /* soft-AP provisioning instructions */
    CAPSTAN_SCREEN_COUNT
} capstan_screen_t;

/*
 * What a screen needs from the user.
 *
 * RING_ONLY is the default and should stay the overwhelming majority.
 * Reach for TOUCH only when rotating to a target is genuinely worse than
 * hitting it -- which in practice means text entry.
 */
typedef enum {
    CAPSTAN_INPUT_RING_ONLY = 0,  /* touch disabled while this screen shows */
    CAPSTAN_INPUT_RING_AND_TOUCH, /* both live */
} capstan_input_mode_t;

/** Initialise navigation. Call after capstan_board_init(). */
void ui_nav_init(void);

/**
 * Show a screen, applying its declared input policy.
 *
 * This is the ONLY supported way to change screens. Calling loadScreen() or
 * lv_screen_load() directly bypasses the input policy and will leave touch
 * in whatever state the previous screen wanted.
 *
 * Must be called with the LVGL lock held, or from an LVGL callback (which
 * already holds it).
 */
void ui_nav_goto(capstan_screen_t screen);

/** Where a long press or the back chip goes from the current screen. */
void ui_nav_back(void);

/*
 * Ring input.
 *
 * These are wired to the board's encoder callbacks in main.c and are
 * called from inside LVGL's input-device read, so the LVGL lock is
 * already held.
 *
 * `diff` is a DIRECTION AND MAGNITUDE for this event only -- a count of
 * detents since the last callback, never an absolute position. The
 * overshoot is not stored anywhere, so turning past the end and coming
 * back moves by exactly one detent.
 *
 * Past the end the app carousel WRAPS and every list CLAMPS; see the note
 * in ui_nav_rotate() for why those differ.
 */
void ui_nav_rotate(int diff);

/** Ring press: open, confirm or toggle, depending on the screen. */
void ui_nav_press(void);

/** Index of the highlighted item on the current screen, or -1. */
int ui_nav_selection(void);

/**
 * Selection on a NAMED screen, whether or not it is showing.
 *
 * The data bridge refreshes every screen, not just the visible one, so
 * it needs the Energy page index even while Energy is in the
 * background.
 */
int ui_nav_selection_of(capstan_screen_t s);

/**
 * A screen's item list changed underneath the selection.
 *
 * Call after adding, removing, showing or hiding rows -- a Wi-Fi scan
 * finishing is the case this exists for. Re-clamps the selection to the
 * new length and repaints the highlight, so a selection left over from a
 * longer list cannot point past the end.
 *
 * Must be called with the LVGL lock held.
 */
void ui_nav_selection_changed(void);

capstan_screen_t ui_nav_current(void);

/** True if this screen wants touch. Exposed for tests and logging. */
capstan_input_mode_t ui_nav_input_mode(capstan_screen_t screen);

#ifdef __cplusplus
}
#endif
