idf_component_register(
    SRCS "src/capstan_wifi.c"
    INCLUDE_DIRS "include"
    REQUIRES capstan_config esp_wifi esp_netif esp_event
)
