# Capstan main component.
#
# GENERATED UI: main/ui/<res>/ is written by EEZ Studio's Build (Ctrl+B) and
# is entirely disposable -- delete the whole `main/ui` tree at any time and
# re-export to recreate it. Nothing hand-written lives there, which is why
# `actions.c` and `vars.c` sit here in main/ instead.
#
# One .eez-project per panel resolution exports into its own subdirectory;
# only the selected board's is compiled.

# ESP-IDF evaluates every component CMakeLists in CMake *script mode* first,
# purely to collect REQUIRES. Kconfig has not been processed at that point, so
# none of the CONFIG_* variables exist yet. Erroring on "no board selected"
# here would abort the configure before menuconfig ever gets a chance to run,
# which is a chicken-and-egg failure that looks like a broken project.
if(CONFIG_CAPSTAN_BOARD_MATOUCH_21)
    set(UI_VARIANT "480")
elseif(CONFIG_CAPSTAN_BOARD_CROWPANEL_146)
    set(UI_VARIANT "360")
elseif(CONFIG_CAPSTAN_BOARD_CROWPANEL_128)
    set(UI_VARIANT "240")
elseif(CMAKE_SCRIPT_MODE_FILE)
    set(UI_VARIANT "480")   # placeholder; the real value is resolved below
else()
    message(FATAL_ERROR
        "No Capstan board selected. Run: idf.py menuconfig -> Capstan")
endif()

set(UI_DIR "ui/${UI_VARIANT}")

# CONFIGURE_DEPENDS makes CMake re-check the glob when the file set changes,
# so a fresh EEZ Studio export is picked up without a manual reconfigure.
# It is illegal in CMake script mode, which ESP-IDF uses for its first pass
# over every component to collect REQUIRES -- hence the guard.
if(CMAKE_SCRIPT_MODE_FILE)
    set(UI_GLOB_MODE "")
else()
    set(UI_GLOB_MODE CONFIGURE_DEPENDS)
endif()

file(GLOB UI_CORE   ${UI_GLOB_MODE} "${UI_DIR}/screens.c" "${UI_DIR}/styles.c"
                                    "${UI_DIR}/images.c"  "${UI_DIR}/ui.c")
file(GLOB UI_FONTS  ${UI_GLOB_MODE} "${UI_DIR}/ui_font_*.c")
file(GLOB UI_IMAGES ${UI_GLOB_MODE} "${UI_DIR}/ui_image_*.c")

# A stale glob is the nastiest failure here: if CMake cached an empty result
# from before the first export, the build links a firmware with NO UI in it
# and still reports success. The board then boots to a blank screen and the
# hunt starts in the display driver. Fail loudly instead.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${UI_DIR}/ui.h" AND NOT UI_CORE)
    message(FATAL_ERROR
        "${UI_DIR}/ui.h exists but no generated sources were found -- the "
        "glob is stale. Run `idf.py reconfigure` after an EEZ Studio export.")
endif()

if(NOT UI_CORE)
    message(WARNING
        "No generated UI in main/${UI_DIR}. Building a placeholder firmware. "
        "Open GUI/Capstan${UI_VARIANT}.eez-project in EEZ Studio and press "
        "Ctrl+B to export.")
endif()

# Whether the generated UI is present is decided HERE, by CMake, and passed
# to the compiler as a define.
#
# It used to be decided in C with `#if __has_include("ui.h")`. That is
# broken in a way that takes a long time to see: __has_include records NO
# dependency on a file that does not exist, so a translation unit compiled
# before the first export bakes in "no UI" and is never rebuilt when the
# export finally appears. Nothing is stale by ninja's reckoning -- no input
# it knows about changed -- so the firmware keeps shipping the placeholder
# bring-up screens forever, through any number of exports and rebuilds,
# while the export sits on disk looking correct.
#
# It cost a debugging round trip: screens.c compiled, its objects linked,
# and the board still logged "no generated UI".
#
# A define computed from the glob does not have that hole. When the glob
# result changes, CMake regenerates, the compile command changes, and ninja
# rebuilds every file that sees it.
if(UI_CORE)
    set(CAPSTAN_HAVE_UI 1)
else()
    set(CAPSTAN_HAVE_UI 0)
endif()

idf_component_register(
    SRCS
        "main.c"
        "ui_nav.c"
        "ui_settings.c"
        "ui_data.c"
        "ui_setup.c"
        "ui_clock.c"
        "ui_lights.c"
        "discovery.c"
        "actions.c"
        "vars.c"
        ${UI_CORE}
        ${UI_FONTS}
        ${UI_IMAGES}
    INCLUDE_DIRS
        "."
        "${UI_DIR}"
    REQUIRES
        capstan_board
        capstan_config
        capstan_model
        capstan_wifi
        capstan_mqtt
        capstan_portal
        nvs_flash
        esp_timer
        # discovery.c: mDNS advertisement plus the one-endpoint HTTP server
        # Headwaters GETs to confirm a module. esp_app_format supplies
        # esp_app_get_description() for the firmware version in the TXT record.
        espressif__mdns
        esp_http_server
        esp_app_format
)

target_compile_definitions(${COMPONENT_LIB} PRIVATE
    CAPSTAN_HAVE_UI=${CAPSTAN_HAVE_UI})
