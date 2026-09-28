/*
 * Touch calibration.
 *
 * Fits a per-axis affine correction, screen = scale * raw + offset, from a
 * handful of known target points, and stores it in NVS via capstan_config.
 *
 * WHY THIS IS NEEDED
 *
 * esp_lcd_touch applies no scaling whatsoever: x_max/y_max are used only
 * for mirroring, so whatever range the controller reports is what LVGL
 * sees. On the MaTouch that measured as a systematic ~19 px Y bias and
 * roughly 10% X stretch -- 22 px mean error, 30 px worst case, against
 * keyboard keys 34 px wide.
 *
 * WHY AFFINE AND NOT SOMETHING CLEVERER
 *
 * Two error modes are worth correcting: a constant offset (the touch layer
 * is bonded slightly shifted) and a scale mismatch (the controller's
 * coordinate range does not equal the panel's). Both are affine. Anything
 * beyond that -- pincushion, per-region warp -- is not present on a bonded
 * capacitive stack in any quantity worth the code, and fitting it from a
 * handful of finger presses would model the user's aim rather than the
 * panel.
 *
 * X and Y are fitted independently. Cross terms would capture rotation,
 * which a bonded layer does not have to any measurable degree.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "capstan_config.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Five points is the practical minimum that can distinguish offset from
 * scale on both axes: the four compass points give each axis two widely
 * separated samples, and the centre catches a gross error in the fit.
 *
 * Corners are deliberately not used -- this is a round panel and they do
 * not exist.
 */
#define CAPSTAN_TOUCH_CAL_POINTS 5

/*
 * Samples averaged per point. A single press carries the user's aiming
 * error and the finger-centroid offset; averaging several presses at the
 * same target cancels the random part and leaves the systematic part,
 * which is the only part worth correcting.
 */
#define CAPSTAN_TOUCH_CAL_SAMPLES 4

typedef struct {
    int16_t expect_x, expect_y;   /**< where the crosshair is drawn */
    int32_t sum_x, sum_y;         /**< accumulated raw readings */
    uint8_t samples;
} capstan_touch_cal_point_t;

typedef struct {
    capstan_touch_cal_point_t pt[CAPSTAN_TOUCH_CAL_POINTS];
    uint8_t current;              /**< index being collected */
    bool    complete;
} capstan_touch_cal_session_t;

/**
 * Begin a calibration. Fills in the target positions for a panel of the
 * given size (inset from the rim so every point is on glass) and resets
 * the accumulators.
 */
void capstan_touch_cal_begin(capstan_touch_cal_session_t *s,
                             uint16_t h_res, uint16_t v_res);

/**
 * Feed one RAW touch sample for the current point.
 *
 * Returns true when the current point has collected enough samples and the
 * session has advanced (or completed). The caller redraws the crosshair at
 * the next target when that happens.
 */
bool capstan_touch_cal_add_sample(capstan_touch_cal_session_t *s,
                                  int raw_x, int raw_y);

/**
 * Fit the transform from a completed session.
 *
 * Rejects a fit that is obviously wrong rather than storing it -- a scale
 * outside 0.5..2.0 or a non-finite coefficient means the samples were
 * garbage (a stuck finger, a dead axis), and writing that to NVS would
 * leave the device unusable with no obvious way back. Returns
 * ESP_ERR_INVALID_RESPONSE in that case and leaves the stored calibration
 * untouched.
 */
esp_err_t capstan_touch_cal_finish(const capstan_touch_cal_session_t *s,
                                   capstan_touch_cal_t *out);

/** Apply the stored calibration in place. No-op when uncalibrated. */
void capstan_touch_cal_apply(int *x, int *y);

/** Re-read the stored calibration from capstan_config. Call after saving. */
void capstan_touch_cal_reload(void);

#ifdef __cplusplus
}
#endif
