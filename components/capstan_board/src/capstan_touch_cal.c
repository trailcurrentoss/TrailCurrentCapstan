/*
 * Touch calibration. See capstan_touch_cal.h for the rationale.
 */

#include <math.h>
#include <string.h>

#include "esp_check.h"
#include "esp_log.h"

#include "capstan_touch_cal.h"

static const char *TAG = "touch.cal";

/* Cached so the hot path does not take the config mutex on every sample. */
static capstan_touch_cal_t s_cal = { 1.0f, 0.0f, 1.0f, 0.0f, false };

void capstan_touch_cal_reload(void)
{
    capstan_config_get_touch_cal(&s_cal);
    ESP_LOGI(TAG, "calibration %s: x = %.4f*raw %+.1f,  y = %.4f*raw %+.1f",
             s_cal.valid ? "loaded" : "absent (identity)",
             s_cal.x_scale, s_cal.x_offset, s_cal.y_scale, s_cal.y_offset);
}

void capstan_touch_cal_apply(int *x, int *y)
{
    if (!s_cal.valid || !x || !y) {
        return;
    }
    *x = (int)lroundf(s_cal.x_scale * (float)*x + s_cal.x_offset);
    *y = (int)lroundf(s_cal.y_scale * (float)*y + s_cal.y_offset);
}

void capstan_touch_cal_begin(capstan_touch_cal_session_t *s,
                             uint16_t h_res, uint16_t v_res)
{
    if (!s) { return; }
    memset(s, 0, sizeof(*s));

    const int cx = h_res / 2, cy = v_res / 2;

    /*
     * Inset 30% of the width from centre. Far enough apart that the scale
     * term is well determined, and still comfortably inside the circle --
     * a target near the rim would be partly under the bezel and would be
     * pressed at an angle, which biases the sample.
     */
    const int inset = (h_res * 3) / 10;

    const int16_t xs[CAPSTAN_TOUCH_CAL_POINTS] = { cx, cx + inset, cx, cx - inset, cx };
    const int16_t ys[CAPSTAN_TOUCH_CAL_POINTS] = { cy - inset, cy, cy + inset, cy, cy };

    for (int i = 0; i < CAPSTAN_TOUCH_CAL_POINTS; i++) {
        s->pt[i].expect_x = xs[i];
        s->pt[i].expect_y = ys[i];
    }
    ESP_LOGI(TAG, "calibration started: %d points x %d samples",
             CAPSTAN_TOUCH_CAL_POINTS, CAPSTAN_TOUCH_CAL_SAMPLES);
}

bool capstan_touch_cal_add_sample(capstan_touch_cal_session_t *s,
                                  int raw_x, int raw_y)
{
    if (!s || s->complete) { return false; }

    capstan_touch_cal_point_t *p = &s->pt[s->current];
    p->sum_x += raw_x;
    p->sum_y += raw_y;
    p->samples++;

    if (p->samples < CAPSTAN_TOUCH_CAL_SAMPLES) {
        return false;
    }

    ESP_LOGI(TAG, "point %d/%d done: target %d,%d  mean raw %d,%d",
             s->current + 1, CAPSTAN_TOUCH_CAL_POINTS,
             p->expect_x, p->expect_y,
             (int)(p->sum_x / p->samples), (int)(p->sum_y / p->samples));

    s->current++;
    if (s->current >= CAPSTAN_TOUCH_CAL_POINTS) {
        s->complete = true;
        s->current  = CAPSTAN_TOUCH_CAL_POINTS - 1;
    }
    return true;
}

/* Least-squares fit of expect = a*raw + b over n points. */
static bool fit_axis(const double *raw, const double *expect, int n,
                     float *a, float *b)
{
    double sr = 0, se = 0, sre = 0, srr = 0;
    for (int i = 0; i < n; i++) {
        sr  += raw[i];
        se  += expect[i];
        sre += raw[i] * expect[i];
        srr += raw[i] * raw[i];
    }
    const double denom = (double)n * srr - sr * sr;

    /* Degenerate: every sample landed at the same raw coordinate, so the
     * axis is dead or the user pressed one spot five times. */
    if (fabs(denom) < 1e-6) {
        return false;
    }

    const double aa = ((double)n * sre - sr * se) / denom;
    const double bb = (se - aa * sr) / (double)n;
    if (!isfinite(aa) || !isfinite(bb)) {
        return false;
    }
    *a = (float)aa;
    *b = (float)bb;
    return true;
}

esp_err_t capstan_touch_cal_finish(const capstan_touch_cal_session_t *s,
                                   capstan_touch_cal_t *out)
{
    ESP_RETURN_ON_FALSE(s && out, ESP_ERR_INVALID_ARG, TAG, "null arg");
    ESP_RETURN_ON_FALSE(s->complete, ESP_ERR_INVALID_STATE, TAG,
                        "calibration not complete");

    double rx[CAPSTAN_TOUCH_CAL_POINTS], ex[CAPSTAN_TOUCH_CAL_POINTS];
    double ry[CAPSTAN_TOUCH_CAL_POINTS], ey[CAPSTAN_TOUCH_CAL_POINTS];
    for (int i = 0; i < CAPSTAN_TOUCH_CAL_POINTS; i++) {
        const capstan_touch_cal_point_t *p = &s->pt[i];
        if (p->samples == 0) {
            return ESP_ERR_INVALID_STATE;
        }
        rx[i] = (double)p->sum_x / p->samples;
        ry[i] = (double)p->sum_y / p->samples;
        ex[i] = p->expect_x;
        ey[i] = p->expect_y;
    }

    capstan_touch_cal_t cal = { 1.0f, 0.0f, 1.0f, 0.0f, false };
    if (!fit_axis(rx, ex, CAPSTAN_TOUCH_CAL_POINTS, &cal.x_scale, &cal.x_offset) ||
        !fit_axis(ry, ey, CAPSTAN_TOUCH_CAL_POINTS, &cal.y_scale, &cal.y_offset)) {
        ESP_LOGE(TAG, "fit failed -- samples are degenerate");
        return ESP_ERR_INVALID_RESPONSE;
    }

    /*
     * Sanity gate. A wild scale means the samples were garbage, and storing
     * it would leave the panel unusable with no obvious route back --
     * including no way to reach the settings screen to re-calibrate. Refuse
     * rather than persist it.
     */
    if (cal.x_scale < 0.5f || cal.x_scale > 2.0f ||
        cal.y_scale < 0.5f || cal.y_scale > 2.0f) {
        ESP_LOGE(TAG, "rejecting implausible fit: x_scale=%.3f y_scale=%.3f",
                 cal.x_scale, cal.y_scale);
        return ESP_ERR_INVALID_RESPONSE;
    }

    cal.valid = true;

    /* Report the residual so the operator can see whether it actually
     * helped, rather than trusting that a fit happened. */
    double worst = 0;
    for (int i = 0; i < CAPSTAN_TOUCH_CAL_POINTS; i++) {
        const double fx = cal.x_scale * rx[i] + cal.x_offset - ex[i];
        const double fy = cal.y_scale * ry[i] + cal.y_offset - ey[i];
        const double e = sqrt(fx * fx + fy * fy);
        if (e > worst) { worst = e; }
    }
    ESP_LOGW(TAG, "fit: x = %.4f*raw %+.1f,  y = %.4f*raw %+.1f,  worst residual %.1f px",
             cal.x_scale, cal.x_offset, cal.y_scale, cal.y_offset, worst);

    *out = cal;
    return ESP_OK;
}
