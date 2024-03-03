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

/*
    Read parameters saved in nvs flash and copy them to variables in RAM
*/
extern uint8_t controller_mac[6];
static void init_parameters(void){
    nvs_handle_t handle;
    size_t size;
    esp_err_t res;
    res = nvs_open(APP_NAMESPACE, NVS_READONLY, &handle);
    if (res != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to open namespace, %s", esp_err_to_name(res));
        ESP_LOGW(TAG, "Parameters must be set manually");
        return;
    }
    
    res = nvs_get_blob(handle, CONTROLLER_MAC_ADDRESS_KEY, controller_mac, &size);
    switch (res)
    {
    case ESP_OK:
        ESP_LOGI(TAG, "Controller mac: " MACSTR, MAC2STR(controller_mac));
        break;

    default:
        ESP_LOGW(TAG, "Failed to get controller mac from nvs flash, %s", esp_err_to_name(res));
        break;
    }
    nvs_close(handle);
}

void app_main(void)
{
    initialize_nvs();
    init_parameters();

    initialize_remote_control();

    ESP_ERROR_CHECK(initialize_console());
    ESP_ERROR_CHECK(start_console());
}