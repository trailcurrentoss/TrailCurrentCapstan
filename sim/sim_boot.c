/*
 * Boot for the EEZ Studio simulator: the part of app_main() that runs after
 * ui_init().
 *
 * The simulator's main.c is EEZ Studio's, not ours. It calls lv_init(),
 * ui_init() and then ui_tick() every frame, and offers no hook. So this
 * schedules itself from a constructor, waits for the generated screens to
 * exist, and then brings the rest of the firmware's UI up in the same order
 * main.c does -- settings, model, theme, navigation, the data refresh, the
 * clock -- plus the simulated network and rig in place of Wi-Fi and MQTT.
 *
 * ui_tick() is NOT put on a timer here, unlike main.c: the simulator's loop
 * already calls it every frame, and a second caller would tick twice.
 *
 * Simulator only -- see docs/simulator.md.
 */
#ifdef EEZ_LVGL_SIMULATOR

#include <emscripten.h>

#include "lvgl.h"

#include "capstan_board.h"
#include "capstan_config.h"
#include "capstan_model.h"
#include "esp_log.h"
#include "screens.h"
#include "ui_clock.h"
#include "ui_data.h"
#include "ui_nav.h"
#include "ui_settings.h"
#include "sim.h"

static const char *TAG = "sim_boot";

/* main.c runs these on its service task every 50 ms; here they share the
 * one thread with everything else. */
static void service_tick(lv_timer_t *t)
{
    (void)t;
    ui_data_service_tick();
    capstan_model_expire_stale();
}

static void boot(void)
{
    capstan_config_init();
    capstan_board_init();
    sim_board_attach_inputs();

    /* What Headwaters would have delivered and NVS would have kept. */
    sim_feed_seed_config();

    sim_net_init();
    capstan_model_init();

    ui_settings_apply_theme();
    ui_nav_init();
    ui_data_init();
    ui_clock_init();

    lv_timer_create(service_tick, 50, NULL);
    sim_feed_start();

    ESP_LOGI(TAG, "Capstan simulator running");
}

static void wait_for_ui(void *arg)
{
    (void)arg;
    /* ui_init() runs inside the simulator's first frame. Until it has, there
     * are no screens to drive and no indevs to take over. */
    if (!lv_is_initialized() || !objects.page_idle) {
        emscripten_async_call(wait_for_ui, NULL, 20);
        return;
    }
    boot();
}

__attribute__((constructor))
static void sim_schedule_boot(void)
{
    emscripten_async_call(wait_for_ui, NULL, 0);
}

#endif /* EEZ_LVGL_SIMULATOR */
