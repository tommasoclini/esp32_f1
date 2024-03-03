idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES app_common app_console nvs_flash app_remote_control)