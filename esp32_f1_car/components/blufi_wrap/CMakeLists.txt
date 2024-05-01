idf_component_register(SRCS "./src/blufi_security.c"
                            "./src/blufi_init.c"
                            "./src/blufi_wrap.c"
                            "./src/blufi_events_wifi.c"
                        INCLUDE_DIRS "./inc"
                        REQUIRES bt
                        PRIV_REQUIRES nvs_flash  esp_wifi esp-tls)