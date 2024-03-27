#include <stdio.h>
#include <string.h>

#include <app_console.h>
#include <app_remote_control.h>
#include <app_car_servos.h>

#include <nvs.h>
#include <nvs_flash.h>

#include <esp_types.h>

#include <esp_log.h>

static const char *TAG = "main";

/*static void initialize_nvs(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}*/

static void app_remote_control_data_cb(espnow_attribute_t init_attr, espnow_attribute_t resp_attr, uint32_t resp_val)
{
    ESP_LOGI(TAG, "Init attr: %d, Resp attr: 0x%x, Val: %lu", init_attr, resp_attr, resp_val);

    if (resp_attr == ESPNOW_ATTRIBUTE_F1_CONTROL)
    {
        ESP_LOGI(TAG, "Got control info");
    } else if (resp_attr == ESPNOW_ATTRIBUTE_F1_LIMITER)
    {
        ESP_LOGI(TAG, "Got limiter command");
    } else if (resp_attr == ESPNOW_ATTRIBUTE_F1_TEST)
    {
        ESP_LOGI(TAG, "Got test message");
    }
}

void app_main(void)
{
    //initialize_nvs();

    ESP_ERROR_CHECK(initialize_remote_control());
    ESP_ERROR_CHECK(initialize_console());
    ESP_ERROR_CHECK(init_servos());

    ESP_ERROR_CHECK(start_console());

    ESP_ERROR_CHECK(remote_control_register_cb(app_remote_control_data_cb));
}