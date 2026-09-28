/*
 * The idle analog clock.
 *
 * The face is authored in EEZ Studio -- the tick ring, three needles and
 * the date label all exist in the export -- but a static export cannot
 * move a needle. This drives it.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Start the 1 Hz face update.
 *
 * Call after ui_init(), with the LVGL lock held.
 */
void ui_clock_init(void);

/** Redraw the face from the current system time. LVGL lock held. */
void ui_clock_refresh(void);

#ifdef __cplusplus
}
#endif
