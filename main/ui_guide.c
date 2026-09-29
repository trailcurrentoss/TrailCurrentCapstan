/*
 * Getting Started. See ui_guide.h.
 */

#include <stdio.h>

#include "sdkconfig.h"
#include "capstan_config.h"
#include "ui_guide.h"

#ifndef CAPSTAN_HAVE_UI
#  error "CAPSTAN_HAVE_UI is not defined -- main/CMakeLists.txt must set it"
#endif

#define STEP_COUNT 6

/*
 * What gets pressed. On both Elecrow CrowPanels (1.28", 1.46") the display
 * itself is the button -- you push the screen until it clicks; on the
 * MaTouch 2.1" it is the ring. The copy names the right one per board.
 */
#if CONFIG_CAPSTAN_BOARD_MATOUCH_21
#  define PRESSED   "the ring"
#  define INTRO     "Turn the ring to move and press it to select. The " \
                    "screen does not respond to touch."
#else
#  define PRESSED   "the screen"
#  define INTRO     "Turn the ring to move and press the screen to select. " \
                    "The screen does not respond to touch."
#endif

static int s_step;

#if CAPSTAN_HAVE_UI

#include "screens.h"
#include "ui_lv.h"

/* Glyphs from the full `fa` face, all in the house set. The design's own
 * icons (hand, circle-dot, undo arrow, check-circle) are not, and the
 * nearest shared glyph was chosen over adding them. */
#define G_INFO   "\xEF\x81\x9A"   /* 0xF05A circle-info  */
#define G_TURN   "\xEF\x80\xA1"   /* 0xF021 rotate       */
#define G_PRESS  "\xEF\x80\x8C"   /* 0xF00C check        */
#define G_HOLD   "\xEF\x81\x93"   /* 0xF053 chevron-left */
#define G_CLOCK  "\xEF\x80\x97"   /* 0xF017 clock        */
#define G_READY  "\xEF\x85\xA4"   /* 0xF164 thumbs-up    */

/*
 * The newer prototype's copy, adapted where Capstan differs: the hold step
 * says nothing about an edge filling green (there is no hold-progress
 * feedback), and the idle step's body is built from the Clock Timeout
 * setting -- see idle_body(). GUIDE_STEPS in screens_layout.py carries
 * step 1 for the canvas.
 */
static const struct {
    const char *icon;
    const char *title;
    const char *body;       /* NULL: built at runtime */
    const char *hint_icon;
    const char *hint;
} s_steps[STEP_COUNT] = {
    { G_INFO,  "Getting Started", INTRO,
      G_TURN,  "Turn the ring to continue" },
    { G_TURN,  "Turn the ring",
      "Turning moves through items one at a time. The dots along the edge "
      "show your position.",
      G_TURN,  "Turn or press to continue" },
    { G_PRESS, "Press to select",
      "Press " PRESSED " until it clicks to open, toggle or set the item "
      "in the center.",
      G_PRESS, "Press to continue" },
    { G_HOLD,  "Hold to go back",
      "Press and hold " PRESSED " to go back. Here it goes back one step.",
      G_PRESS, "Press to continue" },
    { G_CLOCK, "Idle clock", NULL,
      G_PRESS, "Press to continue" },
    { G_READY, "Ready",
      "Getting Started stays in the menu if you need it again.",
      G_HOLD,  "Hold " PRESSED " to go back to the apps" },
};

/* "After 30 seconds without input..." -- the timeout the user actually has,
 * which Settings can change or turn off. */
static void idle_body(char *out, size_t len)
{
    capstan_display_cfg_t d;
    capstan_config_get_display(&d);
    const unsigned s = d.idle_timeout_s;
    if (s == 0) {
        snprintf(out, len, "The clock returns when you choose Clock in the "
                           "apps, or set a Clock Timeout in Settings.");
        return;
    }
    char t[16];
    if (s < 60) {
        snprintf(t, sizeof(t), "%u seconds", s);
    } else if (s == 60) {
        snprintf(t, sizeof(t), "1 minute");
    } else {
        snprintf(t, sizeof(t), "%u minutes", s / 60);
    }
    snprintf(out, len, "After %s without input the display shows the "
                       "clock. Any turn or press wakes it.", t);
}

static void refresh(void)
{
    char step[32];
    snprintf(step, sizeof(step), "STEP %d OF %d", s_step + 1, STEP_COUNT);
    ui_lv_set_text(objects.guide_step, step);

    char body[160];
    const char *b = s_steps[s_step].body;
    if (!b) {
        idle_body(body, sizeof(body));
        b = body;
    }
    ui_lv_set_text(objects.guide_icon, s_steps[s_step].icon);
    ui_lv_set_text(objects.guide_title, s_steps[s_step].title);
    ui_lv_set_text(objects.guide_body, b);
    ui_lv_set_text(objects.guide_hint_icon, s_steps[s_step].hint_icon);
    ui_lv_set_text(objects.guide_hint, s_steps[s_step].hint);

    /* Done steps DISABLED (green), the current CHECKED (green, larger),
     * the rest DEFAULT -- see GuideDot. */
    lv_obj_t *const dots[STEP_COUNT] = {
        objects.guide_dot0, objects.guide_dot1, objects.guide_dot2,
        objects.guide_dot3, objects.guide_dot4, objects.guide_dot5,
    };
    for (int i = 0; i < STEP_COUNT; i++) {
        ui_lv_set_state_in(dots[i], LV_STATE_CHECKED | LV_STATE_DISABLED,
                           i < s_step  ? LV_STATE_DISABLED :
                           i == s_step ? LV_STATE_CHECKED  : 0);
    }
}

#else

static void refresh(void) { }

#endif

void ui_guide_enter(void)
{
    s_step = 0;
    refresh();
}

void ui_guide_rotate(int diff)
{
    int n = s_step + diff;
    if (n < 0)              { n = 0; }
    if (n > STEP_COUNT - 1) { n = STEP_COUNT - 1; }
    if (n != s_step) {
        s_step = n;
        refresh();
    }
}

void ui_guide_press(void)
{
    if (s_step < STEP_COUNT - 1) {
        s_step++;
        refresh();
    }
}

bool ui_guide_back(void)
{
    /* The design: back one step, except on the first step and on Ready,
     * where a hold leaves -- which is what step 6 tells the user to do. */
    if (s_step > 0 && s_step < STEP_COUNT - 1) {
        s_step--;
        refresh();
        return true;
    }
    return false;
}
