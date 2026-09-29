/*
 * Getting Started: six steps that teach the ring -- turn, press, hold --
 * now that nothing on the glass is touchable and there is no Back chip.
 * From the newer prototype (DOCS/GettingStarted); see page_guide() in
 * GUI/tmp/screens_layout.py for the layout this fills in.
 *
 * Its own input rules, from the design:
 *   rotate  one step forward or back, clamped to the six
 *   press   next step (nothing on the last)
 *   hold    previous step; on the first step or Ready, leave to the menu
 */
#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Start at step 1 and paint. Called when the screen is opened. */
void ui_guide_enter(void);

void ui_guide_rotate(int diff);
void ui_guide_press(void);

/**
 * A long press. Returns true if the guide used it (stepped back); false
 * means leave the screen, which the navigator then does.
 */
bool ui_guide_back(void);

#ifdef __cplusplus
}
#endif
