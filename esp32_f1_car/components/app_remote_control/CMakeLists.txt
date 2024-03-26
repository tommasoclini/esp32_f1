idf_component_register(SRCS "app_remote_control.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_wifi driver esp-now button)
