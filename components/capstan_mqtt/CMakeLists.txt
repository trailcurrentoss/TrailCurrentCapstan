idf_component_register(
    SRCS "src/capstan_mqtt.c"
    INCLUDE_DIRS "include"
    REQUIRES capstan_config capstan_model mqtt json esp_timer esp_hw_support
)
