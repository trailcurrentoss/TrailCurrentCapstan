#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations

// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_CLIMATE_HEAT_MIN = 0,
    FLOW_GLOBAL_VARIABLE_CLIMATE_HEAT_MAX = 1,
    FLOW_GLOBAL_VARIABLE_CLIMATE_COOL_MIN = 2,
    FLOW_GLOBAL_VARIABLE_CLIMATE_COOL_MAX = 3,
    FLOW_GLOBAL_VARIABLE_CLIMATE_HOLD_MIN = 4,
    FLOW_GLOBAL_VARIABLE_CLIMATE_HOLD_MAX = 5,
    FLOW_GLOBAL_VARIABLE_CLOCK_SEC_MIN = 6,
    FLOW_GLOBAL_VARIABLE_CLOCK_SEC_MAX = 7
};

// Native global variables

extern int32_t get_var_climate_heat_min();
extern void set_var_climate_heat_min(int32_t value);
extern int32_t get_var_climate_heat_max();
extern void set_var_climate_heat_max(int32_t value);
extern int32_t get_var_climate_cool_min();
extern void set_var_climate_cool_min(int32_t value);
extern int32_t get_var_climate_cool_max();
extern void set_var_climate_cool_max(int32_t value);
extern int32_t get_var_climate_hold_min();
extern void set_var_climate_hold_min(int32_t value);
extern int32_t get_var_climate_hold_max();
extern void set_var_climate_hold_max(int32_t value);
extern int32_t get_var_clock_sec_min();
extern void set_var_clock_sec_min(int32_t value);
extern int32_t get_var_clock_sec_max();
extern void set_var_clock_sec_max(int32_t value);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/