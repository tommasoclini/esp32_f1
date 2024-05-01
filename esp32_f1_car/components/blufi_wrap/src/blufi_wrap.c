#include <esp_err.h>
#include <esp_log.h>
#include <esp_blufi.h>
#include <esp_blufi_api.h>
#include <blufi_example.h>

#include <nvs_flash.h>

extern esp_blufi_callbacks_t blufi_example_callbacks;

esp_err_t blufi_wrap_init(){
    esp_err_t ret;

    // Initialize NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    initialise_wifi();

#if CONFIG_BT_CONTROLLER_ENABLED || !CONFIG_BT_NIMBLE_ENABLED
    ret = esp_blufi_controller_init();
    if (ret) {
        BLUFI_ERROR("%s BLUFI controller init failed: %s\n", __func__, esp_err_to_name(ret));
        return ret;
    }
#endif

    ret = esp_blufi_host_and_cb_init(&blufi_example_callbacks);
    if (ret) {
        BLUFI_ERROR("%s initialise failed: %s\n", __func__, esp_err_to_name(ret));
        return ret;
    }

    BLUFI_INFO("BLUFI VERSION %04x\n", esp_blufi_get_version());
    return ESP_OK;
}
