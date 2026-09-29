/*
 * Screen navigation and per-screen input policy.
 *
 * See ui_nav.h for why the touch policy lives here rather than in each
 * screen, and why the board layer only provides the mechanism.
 */

#include "esp_log.h"
#include "esp_timer.h"

#include "capstan_board.h"
#include "capstan_config.h"
#include "ui_nav.h"
#include "ui_clock.h"
#include "ui_icons.h"
#include "ui_alerts.h"
#include "ui_climate.h"
#include "ui_devices.h"
#include "ui_settings.h"

/* From main/CMakeLists.txt -- see the note there and in main.c. Never
 * __has_include: it cannot notice an export that appears later. */
#ifndef CAPSTAN_HAVE_UI
#  error "CAPSTAN_HAVE_UI is not defined -- main/CMakeLists.txt must set it"
#endif

#define HAVE_GENERATED_UI CAPSTAN_HAVE_UI

#if HAVE_GENERATED_UI
#  include "screens.h"
#  include "ui.h"
#endif

static const char *TAG = "nav";

/* ---------------------------------------------------------------------- *
 * The policy table.
 *
 * ONE line per screen, and it is the whole of the touch decision. A screen
 * never enables or disables touch itself -- ui_nav_goto() applies whatever
 * this table says on every transition, so there is no exit path that can
 * leak the previous screen's setting.
 *
 * Every screen is RING_ONLY. For a while most were RING_AND_TOUCH, for
 * the sake of an on-screen Back chip -- long-pressing the ring goes back,
 * and nothing on the glass says so. The chip was removed as clutter once
 * the long press was the known gesture (a "Getting Started" item under
 * Settings will teach it). With nothing touchable left, live touch could
 * only produce stray input: on the CrowPanels the whole display is the
 * encoder button, so every press is also a touch. The mechanism stays for
 * touch calibration and for any future touch target.
 * ---------------------------------------------------------------------- */
typedef struct {
    capstan_input_mode_t input;
    capstan_screen_t     back;    /* where a long press goes */
    const char          *name;    /* for logs */
} screen_policy_t;

static const screen_policy_t s_policy[CAPSTAN_SCREEN_COUNT] = {
    /* The clock. Any input wakes it; nothing to point at. */
    [CAPSTAN_SCREEN_IDLE]         = { CAPSTAN_INPUT_RING_ONLY,
                                      CAPSTAN_SCREEN_IDLE,     "idle" },

    /* The app carousel. Rotating through the ten items is the whole
     * interaction; touch here would just fight the ring press. */
    [CAPSTAN_SCREEN_MENU]         = { CAPSTAN_INPUT_RING_ONLY,
                                      CAPSTAN_SCREEN_IDLE,     "menu" },

    /* The ring IS the setpoint dial. Touch would be actively harmful. */
    [CAPSTAN_SCREEN_CLIMATE]      = { CAPSTAN_INPUT_RING_ONLY,
                                      CAPSTAN_SCREEN_MENU,     "climate" },
    [CAPSTAN_SCREEN_CLIMATE_MODE] = { CAPSTAN_INPUT_RING_ONLY,
                                      CAPSTAN_SCREEN_CLIMATE,  "climate.mode" },

    /* The devices carousel. The tile is deliberately not touchable: on both
     * CrowPanels the display IS the encoder button, so a tap on it would
     * toggle the device twice and land back where it started. */
    [CAPSTAN_SCREEN_DEVICES]      = { CAPSTAN_INPUT_RING_ONLY,
                                      CAPSTAN_SCREEN_MENU,     "devices" },

    /* Read-only status screens. Nothing to press at all. */
    [CAPSTAN_SCREEN_ENERGY]       = { CAPSTAN_INPUT_RING_ONLY,
                                      CAPSTAN_SCREEN_MENU,     "energy" },
    [CAPSTAN_SCREEN_WATER]        = { CAPSTAN_INPUT_RING_ONLY,
                                      CAPSTAN_SCREEN_MENU,     "water" },
    [CAPSTAN_SCREEN_AIR]          = { CAPSTAN_INPUT_RING_ONLY,
                                      CAPSTAN_SCREEN_MENU,     "air" },
    [CAPSTAN_SCREEN_LEVEL]        = { CAPSTAN_INPUT_RING_ONLY,
                                      CAPSTAN_SCREEN_MENU,     "level" },

    [CAPSTAN_SCREEN_SETTINGS]     = { CAPSTAN_INPUT_RING_ONLY,
                                      CAPSTAN_SCREEN_MENU,     "settings" },



    /*
     * Alert overlay. Dismissed by a press, from anywhere. Ring-only, so a
     * stray touch cannot dismiss an alarm the user has not read.
     */
    /*
     * Setup. Ring-only: everything the user does here happens on their
     * phone, and the display is a set of instructions. A touch target
     * would suggest otherwise.
     */
    [CAPSTAN_SCREEN_SETUP]        = { CAPSTAN_INPUT_RING_ONLY,
                                      CAPSTAN_SCREEN_SETTINGS, "setup" },

    [CAPSTAN_SCREEN_ALERT]        = { CAPSTAN_INPUT_RING_ONLY,
                                      CAPSTAN_SCREEN_MENU,     "alert" },
};

static capstan_screen_t s_current = CAPSTAN_SCREEN_IDLE;
static capstan_screen_t s_keyboard_return = CAPSTAN_SCREEN_SETTINGS;

capstan_input_mode_t ui_nav_input_mode(capstan_screen_t screen)
{
    if (screen < 0 || screen >= CAPSTAN_SCREEN_COUNT) {
        return CAPSTAN_INPUT_RING_ONLY;
    }
    return s_policy[screen].input;
}

capstan_screen_t ui_nav_current(void) { return s_current; }

void ui_nav_set_keyboard_return(capstan_screen_t screen)
{
    s_keyboard_return = screen;
}

#if HAVE_GENERATED_UI
/*
 * Maps our screen enum onto the enum EEZ Studio generates.
 *
 * The two happen to be in the same order today, so this could be
 * `(int)s + 1`. It is spelled out anyway: the EEZ order is whatever
 * `PAGES` in GUI/tmp/gen_eez_project.py emits, and nothing stops someone
 * reordering that list or inserting a page in the middle. An arithmetic
 * mapping would then silently load the wrong screen -- navigation that
 * goes to the wrong place is far harder to spot than navigation that
 * fails loudly.
 *
 * The _Static_assert below breaks the build if a screen is added to one
 * enum and not the other.
 */
static const int s_eez_id[CAPSTAN_SCREEN_COUNT] = {
    [CAPSTAN_SCREEN_IDLE]         = SCREEN_ID_PAGE_IDLE,
    [CAPSTAN_SCREEN_MENU]         = SCREEN_ID_PAGE_MENU,
    [CAPSTAN_SCREEN_CLIMATE]      = SCREEN_ID_PAGE_CLIMATE,
    [CAPSTAN_SCREEN_CLIMATE_MODE] = SCREEN_ID_PAGE_CLIMATE_MODE,
    [CAPSTAN_SCREEN_DEVICES]      = SCREEN_ID_PAGE_DEVICES,
    [CAPSTAN_SCREEN_ENERGY]       = SCREEN_ID_PAGE_ENERGY,
    [CAPSTAN_SCREEN_WATER]        = SCREEN_ID_PAGE_WATER,
    [CAPSTAN_SCREEN_AIR]          = SCREEN_ID_PAGE_AIR,
    [CAPSTAN_SCREEN_LEVEL]        = SCREEN_ID_PAGE_LEVEL,
    [CAPSTAN_SCREEN_SETTINGS]     = SCREEN_ID_PAGE_SETTINGS,
    [CAPSTAN_SCREEN_ALERT]        = SCREEN_ID_PAGE_ALERT,
    [CAPSTAN_SCREEN_SETUP]        = SCREEN_ID_PAGE_SETUP,
};

_Static_assert((int)CAPSTAN_SCREEN_COUNT == (int)_SCREEN_ID_LAST,
               "screen enums are out of step -- a page was added to "
               "ui_nav.h or to gen_eez_project.py but not the other");

static int eez_screen_id(capstan_screen_t s)
{
    return s_eez_id[s];
}
#endif

/*
 * The highlighted item on each screen.
 *
 * This is the ONLY selection state that exists. There is deliberately no
 * running encoder total anywhere -- see ui_nav_rotate() for why.
 */
static int s_sel[CAPSTAN_SCREEN_COUNT];

/* Defined below, with the selection code. */
static void refresh_selection(capstan_screen_t s);
static int  selectable_count(capstan_screen_t s);

void ui_nav_goto(capstan_screen_t screen)
{
    if (screen < 0 || screen >= CAPSTAN_SCREEN_COUNT) {
        ESP_LOGE(TAG, "bad screen id %d", (int)screen);
        return;
    }

    /* Leaving Settings cancels an armed reset, so it cannot be completed
     * by a press that lands on the row after coming back later. */
    if (s_current == CAPSTAN_SCREEN_SETTINGS && screen != s_current) {
        ui_settings_disarm_reset();
    }

    const screen_policy_t *p = &s_policy[screen];

    /*
     * Apply the input policy BEFORE the screen appears, so the first frame
     * is already in the right input mode. Doing it afterwards leaves a
     * window in which a touch can land on a screen that did not want one.
     */
    capstan_board_touch_set_enabled(p->input == CAPSTAN_INPUT_RING_AND_TOUCH);

#if HAVE_GENERATED_UI
    const int id = eez_screen_id(screen);
    if (id >= 0) {
        loadScreen((enum ScreensEnum)id);
    } else {
        ESP_LOGW(TAG, "screen '%s' has no generated page yet", p->name);
    }
#endif

    s_current = screen;

    /* The LED ring follows the screen immediately: leaving an app for the
     * carousel darkens it now, not on the next refresh (ui_alerts.c). */
    ui_alerts_leds_now();

    /*
     * Re-apply the highlight AFTER the load. EEZ Studio's generated
     * create_screen_* runs on first show and builds the widgets with their
     * authored states, so a highlight set before the screen existed is
     * discarded.
     */
    /*
     * Clamp before highlighting. A list can be SHORTER than it was last
     * visit -- a Wi-Fi rescan finding fewer networks is the obvious case --
     * and a selection left over from the longer list would index past the
     * end.
     */
    const int n = selectable_count(screen);
    if (s_sel[screen] > n - 1) { s_sel[screen] = (n > 0) ? n - 1 : 0; }
    if (s_sel[screen] < 0)     { s_sel[screen] = 0; }

    refresh_selection(screen);

#if HAVE_GENERATED_UI
    /*
     * Per-screen entry work. Kept to a dispatch: the navigator says WHEN a
     * screen appears, the screen's own module says what that means.
     */
    if (screen == CAPSTAN_SCREEN_IDLE) {
        /* The face only ticks while it is showing, so coming back to it
         * after ten minutes elsewhere would otherwise display ten-minute
         * old hands until the next second boundary. */
        ui_clock_refresh();
    }
#endif

    ESP_LOGD(TAG, "-> %s (%s)", p->name,
             p->input == CAPSTAN_INPUT_RING_AND_TOUCH ? "ring+touch" : "ring");
}

void ui_nav_back(void)
{
    /* Leaving the alert by any route acknowledges it. A long press that
     * simply closed the overlay would silence the alarm with no snooze
     * timer behind it -- until it happened to clear. */
    if (s_current == CAPSTAN_SCREEN_ALERT) {
        ui_alerts_acknowledge();
        return;
    }

    ui_nav_goto(s_policy[s_current].back);
}

/* ----------------------------------------------------------------------
 * Selection
 * ---------------------------------------------------------------------- */

#if HAVE_GENERATED_UI

/*
 * Eight carousel items -- the seven apps plus Clock. Must stay in step with
 * MENU_ITEMS in GUI/tmp/screens_layout.py, which is what the tiles, the
 * labels and the page dots are all authored from.
 */
#define MENU_ITEM_COUNT 8

/* Energy shows one reading at a time. Must match ENERGY_PAGES in
 * GUI/tmp/screens_layout.py -- the dots are authored from that list. */
#define ENERGY_PAGE_COUNT 3   /* Battery, Solar Input, Loads */

static lv_obj_t *energy_dot(int i)
{
    switch (i) {
    case 0: return objects.energy_dot0;
    case 1: return objects.energy_dot1;
    case 2: return objects.energy_dot2;
    default: return NULL;
    }
}

static void apply_energy_dots(void)
{
    for (int i = 0; i < ENERGY_PAGE_COUNT; i++) {
        lv_obj_t *d = energy_dot(i);
        if (!d) {
            continue;
        }
        if (i == s_sel[CAPSTAN_SCREEN_ENERGY]) {
            lv_obj_add_state(d, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(d, LV_STATE_CHECKED);
        }
    }
}

/* ---------------------------------------------------------------------- *
 * The app carousel.
 *
 * The screen shows THREE of the ten items -- the selected one in the centre
 * tile and its two neighbours either side -- through slots whose position
 * and size never change. Rotating the ring rewrites the GLYPH in each slot
 * and moves the CHECKED state along the row of dots.
 *
 * That split is deliberate and is the rule for this whole file: C sets text
 * and states, never geometry or style. A carousel that slid nine authored
 * tiles across the screen from here would look right on the device and show
 * nothing of the sort on EEZ Studio's canvas, which is the divergence the
 * GUI pipeline exists to prevent. See docs/gui.md.
 *
 * It also means the layout is identical on all three panels. The radial ring
 * this replaced was not: nine icons spaced around the edge need more radius
 * than a 240 px panel has, so the 240 drew a scrolling list instead and the
 * three boards navigated three different ways.
 * ---------------------------------------------------------------------- */

/*
 * Item table. Index is the carousel position, and the three arrays are
 * indexed together, so nothing here may be reordered on its own.
 *
 * Clock is last. It goes to the idle face rather than to a page of its own,
 * which is why it is the one entry whose destination is a screen the
 * carousel can also be reached FROM.
 */
static const struct {
    const char       *icon;     /* from ui_icons.h */
    const char       *title;
    capstan_screen_t  dest;
} s_menu[MENU_ITEM_COUNT] = {
    { UI_ICON_CLIMATE,  "Climate",  CAPSTAN_SCREEN_CLIMATE  },
    { UI_ICON_DEVICES,  "Devices",  CAPSTAN_SCREEN_DEVICES  },
    { UI_ICON_ENERGY,   "Energy",   CAPSTAN_SCREEN_ENERGY   },
    { UI_ICON_WATER,    "Water Tanks", CAPSTAN_SCREEN_WATER },
    { UI_ICON_AIR,      "Air Quality", CAPSTAN_SCREEN_AIR   },
    { UI_ICON_LEVEL,    "Leveling",    CAPSTAN_SCREEN_LEVEL },
    { UI_ICON_SETTINGS, "Settings", CAPSTAN_SCREEN_SETTINGS },
    { UI_ICON_CLOCK,    "Clock",    CAPSTAN_SCREEN_IDLE     },
};

static lv_obj_t *menu_dot(int i)
{
    switch (i) {
    case 0: return objects.menu_dot0;
    case 1: return objects.menu_dot1;
    case 2: return objects.menu_dot2;
    case 3: return objects.menu_dot3;
    case 4: return objects.menu_dot4;
    case 5: return objects.menu_dot5;
    case 6: return objects.menu_dot6;
    case 7: return objects.menu_dot7;
    default: return NULL;
    }
}

/* Index `n` places from `i`, wrapping. See ui_nav_rotate() for why the
 * carousel wraps where every other screen clamps. */
static int menu_wrap(int i)
{
    const int n = MENU_ITEM_COUNT;
    return ((i % n) + n) % n;
}

static void apply_menu_carousel(void)
{
    const int sel = menu_wrap(s_sel[CAPSTAN_SCREEN_MENU]);

    if (objects.menu_hero_icon) {
        lv_label_set_text(objects.menu_hero_icon, s_menu[sel].icon);
    }
    if (objects.menu_prev_icon) {
        lv_label_set_text(objects.menu_prev_icon,
                          s_menu[menu_wrap(sel - 1)].icon);
    }
    if (objects.menu_next_icon) {
        lv_label_set_text(objects.menu_next_icon,
                          s_menu[menu_wrap(sel + 1)].icon);
    }
    if (objects.menu_title) {
        lv_label_set_text(objects.menu_title, s_menu[sel].title);
    }

    /*
     * The summary is NOT set here.
     *
     * It is a live reading -- "3 on", "13.2 V" -- so it belongs to the
     * refresh that already reads the model four times a second, in
     * ui_data.c. Writing it from here as well would leave whichever of the
     * two ran last on the screen, and the one that runs on rotation is the
     * one with no data behind it.
     */

    for (int i = 0; i < MENU_ITEM_COUNT; i++) {
        lv_obj_t *d = menu_dot(i);
        if (!d) {
            continue;
        }
        if (i == sel) {
            lv_obj_add_state(d, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(d, LV_STATE_CHECKED);
        }
    }
}
#endif /* HAVE_GENERATED_UI */

/*
 * Every other selectable screen is a list inside a named container, so
 * instead of a switch per screen there is one switch naming the container
 * and the rest is generic: the item count is the container's child count
 * and the items are its children, in order.
 *
 * That matters for the ones filled at runtime. The Wi-Fi list is authored
 * with six placeholder rows but shows however many networks the scan
 * found, and a hard-coded 6 would either strand rows past the sixth or
 * let the selection run off the end of a short list.
 *
 * Neither carousel is in here. The menu and the devices screen both show
 * three items through fixed slots with no container to count, so each has
 * its own count and its own apply function above.
 */
static lv_obj_t *list_container(capstan_screen_t s)
{
#if HAVE_GENERATED_UI
    switch (s) {
    case CAPSTAN_SCREEN_CLIMATE_MODE: return objects.cmode_list;
    default:                          return NULL;
    }
#else
    (void)s;
    return NULL;
#endif
}

/** How many items the given screen offers to the ring. */
static int selectable_count(capstan_screen_t s)
{
#if HAVE_GENERATED_UI
    if (s == CAPSTAN_SCREEN_MENU) {
        return MENU_ITEM_COUNT;
    }
    if (s == CAPSTAN_SCREEN_ENERGY) {
        /* Not a list -- the "items" are pages through one readout. */
        return ENERGY_PAGE_COUNT;
    }
    if (s == CAPSTAN_SCREEN_SETTINGS) {
        return UI_SETTINGS_ITEM_COUNT;
    }
    if (s == CAPSTAN_SCREEN_DEVICES) {
        /* However many devices Headwaters gave this dial. Not a widget
         * count: the carousel has three slots whatever the list length, so
         * the length lives in the config payload and nowhere else. */
        return ui_devices_count();
    }
    lv_obj_t *c = list_container(s);
    if (c) {
        /*
         * VISIBLE rows only.
         *
         * A list whose length is decided at runtime is authored with the
         * most rows it could ever need and the surplus hidden -- the
         * Wi-Fi screen has ten rows and usually fewer networks. Counting
         * every child would let the ring select blank rows below the last
         * real one, which looks like the selection getting stuck.
         *
         * Hidden rows are always the tail, so a visible count doubles as
         * the highest valid index.
         */
        const uint32_t n = lv_obj_get_child_count(c);
        int vis = 0;
        for (uint32_t i = 0; i < n; i++) {
            if (!lv_obj_has_flag(lv_obj_get_child(c, i), LV_OBJ_FLAG_HIDDEN)) {
                vis++;
            }
        }
        return vis;
    }
#else
    (void)s;
#endif
    return 0;   /* nothing to point at on this screen */
}

static void refresh_selection(capstan_screen_t s)
{
#if HAVE_GENERATED_UI
    if (s == CAPSTAN_SCREEN_MENU) {
        apply_menu_carousel();
        return;
    }
    if (s == CAPSTAN_SCREEN_SETTINGS) {
        ui_settings_refresh();
        return;
    }
    if (s == CAPSTAN_SCREEN_ENERGY) {
        apply_energy_dots();
        return;
    }
    if (s == CAPSTAN_SCREEN_DEVICES) {
        /* Glyphs, name, state and dots in one pass -- the selected device's
         * state is part of what the carousel shows, so there is nothing
         * useful to split out the way the menu splits off its summary. */
        ui_devices_refresh();
        return;
    }

    lv_obj_t *c = list_container(s);
    if (!c) {
        return;
    }
    const uint32_t n = lv_obj_get_child_count(c);
    for (uint32_t i = 0; i < n; i++) {
        lv_obj_t *row = lv_obj_get_child(c, i);
        if ((int)i == s_sel[s]) {
            lv_obj_add_state(row, LV_STATE_CHECKED);
            lv_obj_scroll_to_view(row, LV_ANIM_ON);
        } else {
            lv_obj_remove_state(row, LV_STATE_CHECKED);
        }
    }
#else
    (void)s;
#endif
}

void ui_nav_selection_changed(void)
{
    const int n = selectable_count(s_current);
    if (s_sel[s_current] > n - 1) { s_sel[s_current] = (n > 0) ? n - 1 : 0; }
    if (s_sel[s_current] < 0)     { s_sel[s_current] = 0; }
    refresh_selection(s_current);
}

int ui_nav_selection(void)
{
    return selectable_count(s_current) > 0 ? s_sel[s_current] : -1;
}

int ui_nav_selection_of(capstan_screen_t s)
{
    if (s < 0 || s >= CAPSTAN_SCREEN_COUNT) {
        return 0;
    }
    return s_sel[s];
}

void ui_nav_rotate(int diff)
{
    if (diff == 0) {
        return;
    }

    /*
     * Any input wakes the clock, and the waking input is CONSUMED -- the
     * first detent after the screen has been dark should not also move a
     * selection the user cannot see yet.
     */
    if (s_current == CAPSTAN_SCREEN_IDLE) {
        ui_nav_goto(CAPSTAN_SCREEN_MENU);
        return;
    }

    /* Climate: the ring IS the setpoint, as in the prototype. */
    if (s_current == CAPSTAN_SCREEN_CLIMATE) {
        ui_climate_rotate(diff);
        return;
    }

    const int n = selectable_count(s_current);
    if (n <= 0) {
        return;     /* nothing to move; the ring is simply inert here */
    }

    /*
     * NEVER STORE THE OVERSHOOT.
     *
     * `diff` is a count of detents for THIS event only, applied to the
     * DISPLAYED index. Nothing remembers how far past the end the user kept
     * turning, so two full turns beyond the last item then one detent back
     * moves by exactly one. Keeping a private running total anywhere --
     * here, in the board layer, or in LVGL -- is what reintroduces the
     * wind-back, so there is none.
     *
     * Past the end the index WRAPS on the two CAROUSELS and CLAMPS
     * everywhere else. That is a deliberate split, not an inconsistency:
     *
     *   A carousel is a ring with no first or last item -- it shows three at
     *   a time and the neighbouring glyphs and the dots make the wrap
     *   visible before the user reaches it. The physical ring has no end
     *   stops either, so stopping dead at the last app reads as the input
     *   having jammed. Wrapping is also what puts the clock one detent
     *   backwards from Climate, which is the whole reason Clock is the last
     *   menu item.
     *
     *   All three carousels (menu, devices, settings) wrap for the same
     *   reason, so the ring feels the same on each. A new carousel wraps
     *   too -- the test is whether the neighbours are on screen, not which
     *   screen it is.
     *
     *   A list, like Climate Mode's four rows, genuinely has a top and a
     *   bottom, and wrapping one means a user who holds the ring the wrong
     *   way ends up somewhere they cannot account for. Those still clamp.
     *
     * Either way the overshoot is discarded rather than accumulated, which
     * is the invariant that matters.
     */
    int sel = s_sel[s_current] + diff;
    if (s_current == CAPSTAN_SCREEN_MENU ||
        s_current == CAPSTAN_SCREEN_DEVICES ||
        s_current == CAPSTAN_SCREEN_SETTINGS) {
        sel = ((sel % n) + n) % n;
    } else {
        if (sel < 0)      { sel = 0; }
        if (sel > n - 1)  { sel = n - 1; }
    }

    if (sel != s_sel[s_current]) {
        /* Moving off the Factory Reset item cancels an armed reset --
         * turning away from it is as clear a "no" as any. */
        if (s_current == CAPSTAN_SCREEN_SETTINGS) {
            ui_settings_disarm_reset();
        }
        s_sel[s_current] = sel;
        refresh_selection(s_current);
        ui_alerts_leds_now();   /* Devices: green follows the selection */
    }
}

void ui_nav_press(void)
{
    switch (s_current) {
    case CAPSTAN_SCREEN_IDLE:
        ui_nav_goto(CAPSTAN_SCREEN_MENU);
        return;

#if HAVE_GENERATED_UI
    case CAPSTAN_SCREEN_MENU: {
        const int sel = s_sel[CAPSTAN_SCREEN_MENU];
        if (sel >= 0 && sel < MENU_ITEM_COUNT) {
            ui_nav_goto(s_menu[sel].dest);
        }
        return;
    }

    case CAPSTAN_SCREEN_CLIMATE:
        /* Press opens Mode with the current mode highlighted (P:525). */
        s_sel[CAPSTAN_SCREEN_CLIMATE_MODE] = (int)ui_climate_mode();
        ui_nav_goto(CAPSTAN_SCREEN_CLIMATE_MODE);
        return;

    case CAPSTAN_SCREEN_CLIMATE_MODE:
        /* Press applies the highlighted mode and returns. */
        ui_climate_set_mode((ui_climate_mode_t)s_sel[CAPSTAN_SCREEN_CLIMATE_MODE]);
        ui_nav_goto(CAPSTAN_SCREEN_CLIMATE);
        return;

    case CAPSTAN_SCREEN_DEVICES:
        /* The carousel holds the controls Headwaters assigned to this dial;
         * the press commands the selected one. See ui_devices.c. */
        ui_devices_press();
        return;

    case CAPSTAN_SCREEN_SETTINGS:
        /* The carousel's items (ui_settings.h). Factory reset is last and
         * goes through its own two-press confirmation, because it is
         * unrecoverable. */
        switch (s_sel[CAPSTAN_SCREEN_SETTINGS]) {
        case UI_SETTINGS_WIFI:
        case UI_SETTINGS_MQTT:
            /*
             * Status only. Both are set through the phone portal, and the
             * way into it is a factory reset -- what an unprovisioned
             * device does by itself at boot. The Wi-Fi item used to raise
             * the portal directly, but the Setup screen has no way back
             * (by design: on a new device there is nowhere to go back to),
             * so an accidental press with Clock Timeout at Never left the
             * dial stranded until it was power-cycled.
             */
            return;
        case UI_SETTINGS_THEME:   ui_settings_theme_pressed();   break;
        case UI_SETTINGS_SNOOZE:  ui_settings_snooze_pressed();  break;
        case UI_SETTINGS_TIMEOUT: ui_settings_timeout_pressed(); break;
        case UI_SETTINGS_RESET:   ui_settings_factory_reset_pressed(); break;
        default:
            return;
        }
        ui_settings_refresh();   /* the value line shows the change now */
        return;

    case CAPSTAN_SCREEN_ALERT:
        ui_alerts_acknowledge();   /* snooze, back to the interrupted screen */
        return;
#endif

    default:
        /*
         * Screens whose press behaviour arrives with their layout.
         * Doing nothing is correct for now and is not silent: the log
         * says which screen swallowed the press.
         */
        ESP_LOGD(TAG, "press on '%s' has no action yet",
                 s_policy[s_current].name);
        return;
    }
}

static void on_ring_rotate(int diff, void *ctx)
{
    (void)ctx;
    /* Runs from the LVGL read callback, which already holds the lock. */
    ui_nav_rotate(diff);
}

static void on_ring_press(void *ctx)
{
    (void)ctx;
    ui_nav_press();
}

static void on_ring_long_press(void *ctx)
{
    (void)ctx;
    ui_nav_back();
}

/* ----------------------------------------------------------------------
 * Returning to the clock on its own.
 *
 * CONFIG_CAPSTAN_IDLE_TIMEOUT_S has existed since the first commit, and
 * capstan_config carries it through to NVS so the user can change it -- but
 * nothing read it, so no screen ever went back to the idle face. A panel left
 * on the Water screen stayed there until somebody touched it, which on a
 * wall-mounted display means until somebody walked past. It also meant the
 * clock was only ever seen at boot.
 *
 * The timeout is read from settings on every tick rather than cached, so
 * changing it takes effect immediately and there is no second copy to keep in
 * step. A tick is once a second and reading it is a struct copy out of RAM.
 * ---------------------------------------------------------------------- */
static void idle_timer_cb(lv_timer_t *t)
{
    (void)t;

    if (s_current == CAPSTAN_SCREEN_IDLE) {
        return;
    }

    /*
     * Two screens are never timed out.
     *
     * SETUP is a set of instructions the user is following on their phone --
     * the network name and password are on the glass, and taking them away
     * mid-typing is the one thing this screen must not do. ALERT is an alarm
     * that has not been acknowledged; it is dismissed by a press and by
     * nothing else, which is the same reason it is ring-only.
     */
    if (s_current == CAPSTAN_SCREEN_SETUP || s_current == CAPSTAN_SCREEN_ALERT) {
        return;
    }

    capstan_display_cfg_t disp;
    capstan_config_get_display(&disp);
    if (disp.idle_timeout_s == 0) {
        return;             /* 0 disables it */
    }

    if (capstan_board_ms_since_input() < (uint32_t)disp.idle_timeout_s * 1000u) {
        return;
    }

    ESP_LOGD(TAG, "idle for %us -> clock", (unsigned)disp.idle_timeout_s);
    ui_nav_goto(CAPSTAN_SCREEN_IDLE);
}

void ui_nav_init(void)
{
    /*
     * All three ring events are claimed here.
     *
     * Registering a rotate callback also takes rotation AWAY from LVGL's
     * own encoder handling -- the board layer delivers to one or the
     * other, never both, so detents cannot be counted twice. That is why
     * navigation owns all three rather than leaving rotation to LVGL and
     * taking only the press.
     */
    capstan_board_set_rotate_callback(on_ring_rotate, NULL);
    capstan_board_set_press_callback(on_ring_press, NULL);
    capstan_board_set_back_callback(on_ring_long_press, NULL);

    /* Whatever the board defaulted to, the policy table is authoritative
     * from here on. */
    ui_nav_goto(CAPSTAN_SCREEN_IDLE);

    /* One second is plenty: the timeout is tens of seconds, so the worst
     * case is returning to the clock a second late. */
    lv_timer_create(idle_timer_cb, 1000, NULL);
}
