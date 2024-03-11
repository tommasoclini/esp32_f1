#include <stdio.h>
#include <string.h>

#include <app_common.h>
#include <app_console.h>
#include <app_remote_control.h>

#include <nvs.h>
#include <nvs_flash.h>

#include <esp_types.h>

#include <esp_log.h>

static const char *TAG = "main";

static void initialize_nvs(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

void app_main(void)
{
    initialize_nvs();

    initialize_remote_control();

    ESP_ERROR_CHECK(initialize_console());
    ESP_ERROR_CHECK(start_console());
}