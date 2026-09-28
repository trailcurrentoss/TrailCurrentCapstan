/*
 * Screen navigation and per-screen input policy.
 *
 * See ui_nav.h for why the touch policy lives here rather than in each
 * screen, and why the board layer only provides the mechanism.
 */

#include "esp_log.h"
#include "esp_timer.h"

#include "capstan_board.h"
#include "ui_nav.h"
#include "ui_clock.h"
#include "ui_settings.h"
#include "ui_setup.h"

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
 * Most screens are now RING_AND_TOUCH, which REVERSES the original default
 * of RING_ONLY. The reason is the Back chip.
 *
 * Long-pressing the ring went back, and that was the only way out of a
 * screen. Nothing on the display says so, so in practice a user who has
 * not been told is stuck. Every screen that carries a Back chip therefore
 * needs touch live, or the chip is decoration.
 *
 * The press-vs-touch conflict the original default avoided is real and has
 * not gone away: on the CrowPanels the whole display is the encoder
 * button, so a firm press on the Back chip produces a touch click AND an
 * encoder press. What stops that acting twice is the suppression window in
 * ui_nav_press(), not the policy.
 *
 * Three screens stay RING_ONLY, each for its own reason -- see their rows.
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

    /* Carousel or list. Rotating through nine items is the whole
     * interaction; touch here would just fight the ring press. */
    [CAPSTAN_SCREEN_MENU]         = { CAPSTAN_INPUT_RING_ONLY,
                                      CAPSTAN_SCREEN_IDLE,     "menu" },

    /* The ring IS the setpoint dial. Touch would be actively harmful. */
    [CAPSTAN_SCREEN_CLIMATE]      = { CAPSTAN_INPUT_RING_AND_TOUCH,
                                      CAPSTAN_SCREEN_MENU,     "climate" },
    [CAPSTAN_SCREEN_CLIMATE_MODE] = { CAPSTAN_INPUT_RING_AND_TOUCH,
                                      CAPSTAN_SCREEN_CLIMATE,  "climate.mode" },

    [CAPSTAN_SCREEN_LIGHTS]       = { CAPSTAN_INPUT_RING_AND_TOUCH,
                                      CAPSTAN_SCREEN_MENU,     "lights" },
    [CAPSTAN_SCREEN_HEATER]       = { CAPSTAN_INPUT_RING_AND_TOUCH,
                                      CAPSTAN_SCREEN_MENU,     "heater" },

    /* Read-only status screens. Nothing to press at all. */
    [CAPSTAN_SCREEN_ENERGY]       = { CAPSTAN_INPUT_RING_AND_TOUCH,
                                      CAPSTAN_SCREEN_MENU,     "energy" },
    [CAPSTAN_SCREEN_WATER]        = { CAPSTAN_INPUT_RING_AND_TOUCH,
                                      CAPSTAN_SCREEN_MENU,     "water" },
    [CAPSTAN_SCREEN_AIR]          = { CAPSTAN_INPUT_RING_AND_TOUCH,
                                      CAPSTAN_SCREEN_MENU,     "air" },
    [CAPSTAN_SCREEN_LEVEL]        = { CAPSTAN_INPUT_RING_AND_TOUCH,
                                      CAPSTAN_SCREEN_MENU,     "level" },
    [CAPSTAN_SCREEN_DOORS]        = { CAPSTAN_INPUT_RING_AND_TOUCH,
                                      CAPSTAN_SCREEN_MENU,     "doors" },

    [CAPSTAN_SCREEN_SETTINGS]     = { CAPSTAN_INPUT_RING_AND_TOUCH,
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
    [CAPSTAN_SCREEN_LIGHTS]       = SCREEN_ID_PAGE_LIGHTS,
    [CAPSTAN_SCREEN_HEATER]       = SCREEN_ID_PAGE_HEATER,
    [CAPSTAN_SCREEN_ENERGY]       = SCREEN_ID_PAGE_ENERGY,
    [CAPSTAN_SCREEN_WATER]        = SCREEN_ID_PAGE_WATER,
    [CAPSTAN_SCREEN_AIR]          = SCREEN_ID_PAGE_AIR,
    [CAPSTAN_SCREEN_LEVEL]        = SCREEN_ID_PAGE_LEVEL,
    [CAPSTAN_SCREEN_DOORS]        = SCREEN_ID_PAGE_DOORS,
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

/*
 * A touch-driven Back also presses the ring on this hardware.
 *
 * On both CrowPanels the display IS the encoder button, so pressing the
 * Back chip hard enough to register a touch also closes the button. Left
 * alone that means "go back, then immediately open whatever was selected
 * on the screen we just left" -- which reads as the Back chip being
 * broken, and is worse than broken, because it navigates somewhere the
 * user did not ask to go.
 *
 * So a Back consumes the ring press that arrives alongside it. The window
 * is generous: the two events are one physical action and nothing
 * guarantees which order they arrive in.
 */
#define BACK_PRESS_SUPPRESS_US 600000   /* 600 ms */

static int64_t s_back_us;

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
    /* Swallow the ring press that a touch on the Back chip also makes. */
    s_back_us = esp_timer_get_time();

    ui_nav_goto(s_policy[s_current].back);
}

/* ----------------------------------------------------------------------
 * Selection
 * ---------------------------------------------------------------------- */

#if HAVE_GENERATED_UI

#define MENU_ITEM_COUNT 9

/* Energy shows one reading at a time. Must match ENERGY_PAGES in
 * GUI/tmp/screens_layout.py -- the dots are authored from that list. */
#define ENERGY_PAGE_COUNT 5

static lv_obj_t *energy_dot(int i)
{
    switch (i) {
    case 0: return objects.energy_dot0;
    case 1: return objects.energy_dot1;
    case 2: return objects.energy_dot2;
    case 3: return objects.energy_dot3;
    case 4: return objects.energy_dot4;
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

/*
 * The menu items, in the order screens_layout.py lays them out. The
 * identifiers are deliberately the same on all three resolutions even
 * though the 240 draws a list and the others a ring, so this table does
 * not have to know which panel it is running on.
 */
static lv_obj_t *menu_item(int i)
{
    switch (i) {
    case 0: return objects.menu_item0;
    case 1: return objects.menu_item1;
    case 2: return objects.menu_item2;
    case 3: return objects.menu_item3;
    case 4: return objects.menu_item4;
    case 5: return objects.menu_item5;
    case 6: return objects.menu_item6;
    case 7: return objects.menu_item7;
    case 8: return objects.menu_item8;
    default: return NULL;
    }
}

/* Must stay in step with MENU_ITEMS in GUI/tmp/screens_layout.py. */
static const capstan_screen_t s_menu_dest[MENU_ITEM_COUNT] = {
    CAPSTAN_SCREEN_CLIMATE, CAPSTAN_SCREEN_LIGHTS,  CAPSTAN_SCREEN_HEATER,
    CAPSTAN_SCREEN_ENERGY,  CAPSTAN_SCREEN_WATER,   CAPSTAN_SCREEN_AIR,
    CAPSTAN_SCREEN_LEVEL,   CAPSTAN_SCREEN_DOORS,   CAPSTAN_SCREEN_SETTINGS,
};

/*
 * Selection is shown by setting LV_STATE_CHECKED, which the project's Card
 * style already defines a look for. Setting a state is one of the few
 * things C is allowed to do to an EEZ-authored widget -- restyling or
 * moving one from here would put the device out of step with the canvas.
 */
static void apply_menu_highlight(void)
{
    for (int i = 0; i < MENU_ITEM_COUNT; i++) {
        lv_obj_t *o = menu_item(i);
        if (!o) {
            continue;
        }
        if (i == s_sel[CAPSTAN_SCREEN_MENU]) {
            lv_obj_add_state(o, LV_STATE_CHECKED);
            lv_obj_scroll_to_view(o, LV_ANIM_ON);   /* no-op on the ring */
        } else {
            lv_obj_remove_state(o, LV_STATE_CHECKED);
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
 * The menu is not in here: on the 480 and 360 it is a ring of faces with
 * no container at all, so it keeps its explicit accessor above.
 */
static lv_obj_t *list_container(capstan_screen_t s)
{
#if HAVE_GENERATED_UI
    switch (s) {
    case CAPSTAN_SCREEN_CLIMATE_MODE: return objects.cmode_list;
    case CAPSTAN_SCREEN_LIGHTS:       return objects.lights_list;
    case CAPSTAN_SCREEN_DOORS:        return objects.doors_list;
    case CAPSTAN_SCREEN_SETTINGS:     return objects.settings_list;
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
        apply_menu_highlight();
        return;
    }
    if (s == CAPSTAN_SCREEN_ENERGY) {
        apply_energy_dots();
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

    const int n = selectable_count(s_current);
    if (n <= 0) {
        return;     /* nothing to move; the ring is simply inert here */
    }

    /*
     * CLAMP, DO NOT WRAP -- and never store the overshoot.
     *
     * `diff` is a count of detents for THIS event only. It is applied to
     * the displayed index and the result clamped, so past either end the
     * selection just stops. Crucially nothing remembers how far past the
     * end the user kept turning: two full turns beyond the last item then
     * one detent back moves by exactly one. Keeping a private running
     * total anywhere -- here, in the board layer, or in LVGL -- is what
     * reintroduces the wind-back, so there is none.
     */
    int sel = s_sel[s_current] + diff;
    if (sel < 0)      { sel = 0; }
    if (sel > n - 1)  { sel = n - 1; }

    if (sel != s_sel[s_current]) {
        /* Moving off the Factory Reset row cancels an armed reset --
         * turning away from it is as clear a "no" as any. */
        if (s_current == CAPSTAN_SCREEN_SETTINGS) {
            ui_settings_disarm_reset();
        }
        s_sel[s_current] = sel;
        refresh_selection(s_current);
    }
}

void ui_nav_press(void)
{
    if (s_back_us &&
        (esp_timer_get_time() - s_back_us) < BACK_PRESS_SUPPRESS_US) {
        ESP_LOGD(TAG, "press suppressed -- arrived with a Back touch");
        return;
    }

    switch (s_current) {
    case CAPSTAN_SCREEN_IDLE:
        ui_nav_goto(CAPSTAN_SCREEN_MENU);
        return;

#if HAVE_GENERATED_UI
    case CAPSTAN_SCREEN_MENU: {
        const int sel = s_sel[CAPSTAN_SCREEN_MENU];
        if (sel >= 0 && sel < MENU_ITEM_COUNT) {
            ui_nav_goto(s_menu_dest[sel]);
        }
        return;
    }

    case CAPSTAN_SCREEN_SETTINGS:
        /* Rows, in the order page_settings() lays them out. Factory reset
         * is deliberately last and is NOT wired here: it is unrecoverable,
         * so it gets a confirmation step rather than acting on the press
         * that lands on it. */
        switch (s_sel[CAPSTAN_SCREEN_SETTINGS]) {
        case 0:
            /*
             * Wi-Fi means PHONE SETUP. There is no on-device editor
             * any more: PageWifi, PageWifiSecurity, PageMqtt and
             * PageKeyboard are gone, along with the modules that drove
             * them. Entering a WPA2 passphrase by rotating a ring was
             * built, tried on the bench, and does not work on a panel
             * this size.
             *
             * Raising the portal from here is a convenience. The
             * documented route is a factory reset, which is what an
             * unprovisioned device does by itself at boot.
             */
            ui_setup_enter();
            return;
        case 1:
            /* Read-only status. Broker details are set in the portal. */
            ESP_LOGD(TAG, "MQTT row is status only -- use setup mode");
            return;
        case 3: ui_settings_factory_reset_pressed(); return;
        default:
            ESP_LOGD(TAG, "settings row %d has no action yet",
                     s_sel[CAPSTAN_SCREEN_SETTINGS]);
            return;
        }

    case CAPSTAN_SCREEN_ALERT:
        ui_nav_back();      /* a press dismisses an alert from anywhere */
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
}
