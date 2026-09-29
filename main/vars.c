/*
 * EEZ Studio variable accessors.
 *
 * Expression-bound widget properties read through these on every ui_tick().
 * Hand-written, and deliberately in main/ rather than main/ui/, which is
 * disposable.
 *
 * Keep these cheap: ui_tick() calls every one of them at the UI frame rate.
 * They should read an already-updated value out of the data model, never
 * parse, allocate or block.
 */

/* From main/CMakeLists.txt. Never __has_include -- it records no
 * dependency on an absent file, so a unit compiled before the first EEZ
 * Studio export is never rebuilt when the export arrives. See main.c. */
#ifndef CAPSTAN_HAVE_UI
#  error "CAPSTAN_HAVE_UI is not defined -- main/CMakeLists.txt must set it"
#endif

#if CAPSTAN_HAVE_UI

#include "vars.h"
#include "ui_climate.h"
#include "ui_clock.h"

/*
 * Climate: the three active-range sections' bounds (see GLOBAL_VARIABLES in
 * GUI/tmp/gen_eez_project.py). EEZ calls these every ui_tick() and applies
 * them with lv_scale_section_set_range(). The values are owned by
 * ui_climate.c; the setters exist because EEZ declares them, and are unused.
 */
#define CLIMATE_SECTION_VAR(name, sec, which)                             \
    int32_t get_var_##name(void)                                          \
    {                                                                     \
        int32_t lo, hi;                                                   \
        ui_climate_section(sec, &lo, &hi);                                \
        return which;                                                     \
    }                                                                     \
    void set_var_##name(int32_t value) { (void)value; }

CLIMATE_SECTION_VAR(climate_heat_min, UI_CLIMATE_SEC_HEAT, lo)
CLIMATE_SECTION_VAR(climate_heat_max, UI_CLIMATE_SEC_HEAT, hi)
CLIMATE_SECTION_VAR(climate_cool_min, UI_CLIMATE_SEC_COOL, lo)
CLIMATE_SECTION_VAR(climate_cool_max, UI_CLIMATE_SEC_COOL, hi)
CLIMATE_SECTION_VAR(climate_hold_min, UI_CLIMATE_SEC_HOLD, lo)
CLIMATE_SECTION_VAR(climate_hold_max, UI_CLIMATE_SEC_HOLD, hi)

/* The Digital clock face's elapsed-seconds section: 0..now (ui_clock.c). */
int32_t get_var_clock_sec_min(void) { return 0; }
void set_var_clock_sec_min(int32_t value) { (void)value; }
int32_t get_var_clock_sec_max(void) { return ui_clock_second(); }
void set_var_clock_sec_max(int32_t value) { (void)value; }

#else
typedef int capstan_vars_placeholder;
#endif
