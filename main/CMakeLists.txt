idf_component_register(SRCS "screen.c" "camera.c" "esp_main.c" "networkConfig.c" "communication.c" "screen.c"
                    INCLUDE_DIRS "include"
                    REQUIRES nvs_flash esp_wifi esp_event )
