idf_component_register(SRCS "pwm_capture.cpp"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES esp_timer
                    REQUIRES driver)
