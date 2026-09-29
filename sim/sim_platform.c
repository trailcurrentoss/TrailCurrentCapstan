/*
 * ESP-IDF services the UI code calls, for the EEZ Studio simulator:
 * esp_timer, esp_restart, esp_err_to_name, and an in-memory NVS.
 *
 * Simulator only -- see docs/simulator.md. The firmware links the real ones.
 */
#ifdef EEZ_LVGL_SIMULATOR

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include <emscripten.h>

#include "lvgl.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "nvs.h"

/* ---- time ----------------------------------------------------------- */

int64_t esp_timer_get_time(void)
{
    /* Monotonic, from page load -- the same shape as the chip's counter,
     * which starts at boot. */
    return (int64_t)(emscripten_get_now() * 1000.0);
}

struct sim_esp_timer {
    esp_timer_cb_t cb;
    void          *arg;
};

static void one_shot_cb(lv_timer_t *t)
{
    struct sim_esp_timer *et = lv_timer_get_user_data(t);
    lv_timer_delete(t);
    if (et && et->cb) {
        et->cb(et->arg);
    }
}

esp_err_t esp_timer_create(const esp_timer_create_args_t *args,
                           esp_timer_handle_t *out)
{
    if (!args || !out) {
        return ESP_ERR_INVALID_ARG;
    }
    struct sim_esp_timer *et = calloc(1, sizeof(*et));
    if (!et) {
        return ESP_ERR_NO_MEM;
    }
    et->cb  = args->callback;
    et->arg = args->arg;
    *out = et;
    return ESP_OK;
}

esp_err_t esp_timer_start_once(esp_timer_handle_t t, uint64_t timeout_us)
{
    if (!t) {
        return ESP_ERR_INVALID_ARG;
    }
    uint32_t ms = (uint32_t)(timeout_us / 1000u);
    lv_timer_create(one_shot_cb, ms ? ms : 1, t);
    return ESP_OK;
}

/*
 * capstan_model_set_gps_time() steps the system clock from the GNSS fix.
 * Emscripten has no settimeofday(), and the browser's clock cannot be set
 * anyway -- but it is also what sim_feed.c derives the "GNSS" time from, so
 * the clock is already right and accepting the call is the honest answer.
 */
struct timezone;
int settimeofday(const struct timeval *tv, const struct timezone *tz)
{
    (void)tv;
    (void)tz;
    return 0;
}

/* ---- system --------------------------------------------------------- */

void esp_restart(void)
{
    /* A reboot, as far as the simulator can have one: every setting lives in
     * memory, so reloading comes back exactly like a factory-fresh panel. */
    ESP_LOGI("sim", "esp_restart() -> reloading the simulator");
    emscripten_run_script("location.reload()");
}

uint32_t esp_get_free_heap_size(void)
{
    return 0;   /* not meaningful here; only ever logged */
}

const char *esp_err_to_name(esp_err_t code)
{
    switch (code) {
    case ESP_OK:                   return "ESP_OK";
    case ESP_FAIL:                 return "ESP_FAIL";
    case ESP_ERR_NO_MEM:           return "ESP_ERR_NO_MEM";
    case ESP_ERR_INVALID_ARG:      return "ESP_ERR_INVALID_ARG";
    case ESP_ERR_INVALID_STATE:    return "ESP_ERR_INVALID_STATE";
    case ESP_ERR_INVALID_SIZE:     return "ESP_ERR_INVALID_SIZE";
    case ESP_ERR_NOT_FOUND:        return "ESP_ERR_NOT_FOUND";
    case ESP_ERR_NOT_SUPPORTED:    return "ESP_ERR_NOT_SUPPORTED";
    case ESP_ERR_TIMEOUT:          return "ESP_ERR_TIMEOUT";
    case ESP_ERR_INVALID_RESPONSE: return "ESP_ERR_INVALID_RESPONSE";
    case ESP_ERR_NVS_NOT_FOUND:    return "ESP_ERR_NVS_NOT_FOUND";
    case ESP_ERR_NVS_KEY_TOO_LONG: return "ESP_ERR_NVS_KEY_TOO_LONG";
    default:                       return "ESP_ERR_UNKNOWN";
    }
}

/* ---- NVS ------------------------------------------------------------ */
/*
 * Just enough of NVS for capstan_config.c: one namespace, typed entries, a
 * 15-character key limit enforced the way the real one enforces it (at
 * runtime), and "namespace not found" until the first write -- which is what
 * sends capstan_config down its first-boot path.
 */

#define NVS_KEY_MAX   15
#define NVS_ENTRIES   48

typedef enum { T_U8, T_U16, T_STR, T_BLOB } nvs_type_t;

typedef struct {
    bool       used;
    char       key[NVS_KEY_MAX + 1];
    nvs_type_t type;
    size_t     len;
    uint8_t   *data;
} nvs_entry_t;

static nvs_entry_t s_nvs[NVS_ENTRIES];
static bool        s_ns_exists;

static nvs_entry_t *find(const char *key)
{
    for (int i = 0; i < NVS_ENTRIES; i++) {
        if (s_nvs[i].used && strcmp(s_nvs[i].key, key) == 0) {
            return &s_nvs[i];
        }
    }
    return NULL;
}

static esp_err_t put(const char *key, nvs_type_t type, const void *v, size_t len)
{
    if (!key || strlen(key) > NVS_KEY_MAX) {
        return ESP_ERR_NVS_KEY_TOO_LONG;
    }
    nvs_entry_t *e = find(key);
    if (!e) {
        for (int i = 0; i < NVS_ENTRIES && !e; i++) {
            if (!s_nvs[i].used) {
                e = &s_nvs[i];
            }
        }
        if (!e) {
            return ESP_ERR_NO_MEM;
        }
        memset(e, 0, sizeof(*e));
        e->used = true;
        strcpy(e->key, key);
    }
    uint8_t *copy = malloc(len ? len : 1);
    if (!copy) {
        return ESP_ERR_NO_MEM;
    }
    memcpy(copy, v, len);
    free(e->data);
    e->data = copy;
    e->len  = len;
    e->type = type;
    s_ns_exists = true;
    return ESP_OK;
}

static esp_err_t get(const char *key, nvs_type_t type, void *out, size_t *len)
{
    const nvs_entry_t *e = key ? find(key) : NULL;
    if (!e || e->type != type) {
        return ESP_ERR_NVS_NOT_FOUND;
    }
    if (len) {
        if (!out) {                 /* size query, as the real API allows */
            *len = e->len;
            return ESP_OK;
        }
        if (*len < e->len) {
            return ESP_ERR_NVS_INVALID_LENGTH;
        }
        *len = e->len;
    }
    memcpy(out, e->data, e->len);
    return ESP_OK;
}

esp_err_t nvs_open(const char *ns, nvs_open_mode_t mode, nvs_handle_t *out)
{
    (void)ns;
    if (mode == NVS_READONLY && !s_ns_exists) {
        return ESP_ERR_NVS_NOT_FOUND;
    }
    *out = 1;
    return ESP_OK;
}

void      nvs_close(nvs_handle_t h)  { (void)h; }
esp_err_t nvs_commit(nvs_handle_t h) { (void)h; return ESP_OK; }

esp_err_t nvs_erase_all(nvs_handle_t h)
{
    (void)h;
    for (int i = 0; i < NVS_ENTRIES; i++) {
        free(s_nvs[i].data);
        memset(&s_nvs[i], 0, sizeof(s_nvs[i]));
    }
    return ESP_OK;
}

esp_err_t nvs_get_u8(nvs_handle_t h, const char *k, uint8_t *o)
{ (void)h; size_t n = sizeof(*o); return get(k, T_U8, o, &n); }
esp_err_t nvs_get_u16(nvs_handle_t h, const char *k, uint16_t *o)
{ (void)h; size_t n = sizeof(*o); return get(k, T_U16, o, &n); }
esp_err_t nvs_get_str(nvs_handle_t h, const char *k, char *o, size_t *len)
{ (void)h; return get(k, T_STR, o, len); }
esp_err_t nvs_get_blob(nvs_handle_t h, const char *k, void *o, size_t *len)
{ (void)h; return get(k, T_BLOB, o, len); }

esp_err_t nvs_set_u8(nvs_handle_t h, const char *k, uint8_t v)
{ (void)h; return put(k, T_U8, &v, sizeof(v)); }
esp_err_t nvs_set_u16(nvs_handle_t h, const char *k, uint16_t v)
{ (void)h; return put(k, T_U16, &v, sizeof(v)); }
esp_err_t nvs_set_str(nvs_handle_t h, const char *k, const char *v)
{ (void)h; return put(k, T_STR, v, strlen(v) + 1); }
esp_err_t nvs_set_blob(nvs_handle_t h, const char *k, const void *v, size_t len)
{ (void)h; return put(k, T_BLOB, v, len); }

#endif /* EEZ_LVGL_SIMULATOR */
