#include <stdio.h>
#include <app_remote_control.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>
#include <esp_check.h>
#include <esp_wifi.h>

#include <esp_mac.h>

#include <espnow.h>
#include <espnow_storage.h>
#include <espnow_ctrl.h>

#include <iot_button.h>

static const char *TAG = "remote control";

#define BIND_UNBIND_RSSI -55
#define BIND_WAIT_MS 3000

extern uint8_t controller_mac[6];

static void app_wifi_init()
{
    esp_event_loop_create_default();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static bool ctrl_bind_cb(espnow_attribute_t init_attr, uint8_t mac[6], int8_t rssi){
    ESP_LOGI(TAG, "Bind init attr: %d, mac: " MACSTR ", rssi: %d", init_attr, MAC2STR(mac), rssi);
    return rssi >= BIND_UNBIND_RSSI;
}

static void app_bind(bool bind){
    espnow_ctrl_responder_bind(BIND_WAIT_MS, BIND_UNBIND_RSSI, ctrl_bind_cb);

    vTaskDelay(pdMS_TO_TICKS(100));

    espnow_ctrl_initiator_bind(ESPNOW_ATTRIBUTE_BASE, bind);
}

static void button_double_click_cb(void *button_handle, void *usr_data)
{
    ESP_LOGI(TAG, "BUTTON_DOUBLE_CLICK");

    app_bind(true);
}

static void button_long_press_cb(void *button_handle, void *usr_data)
{
    ESP_LOGI(TAG, "BUTTON_LONG_PRESS_CLICK");

    app_bind(false);
}

static void button_single_click_cb(void *button_handle, void *usr_data)
{
    ESP_LOGI(TAG, "BUTTON_SINGLE_CLICK");

    espnow_ctrl_initiator_send(ESPNOW_ATTRIBUTE_BASE, ESPNOW_ATTRIBUTE_BASE, esp_log_timestamp());
}

static void button_multiple_click_cb(void *button_handle, void *usr_data)
{
    ESP_LOGI(TAG, "BUTTON_MULTIPLE_CLICK");

    espnow_ctrl_responder_clear_bindlist();
}

static void ctrl_data_cb(espnow_attribute_t init_attr, espnow_attribute_t resp_attr, uint32_t resp_val)
{
    ESP_LOGI(TAG, "Init attr: %d, Resp attr: %d, Val: %lu", init_attr, resp_attr, resp_val);

    if ((espnow_f1_attribute_t)resp_attr == ESPNOW_ATTRIBUTE_F1_CONTROL)
    {
        ESP_LOGI(TAG, "Got control info");
    } else if ((espnow_f1_attribute_t)resp_attr == ESPNOW_ATTRIBUTE_F1_LIMITER)
    {
        ESP_LOGI(TAG, "Got limiter command");
    }
}

void initialize_remote_control(void){
    espnow_storage_init();

    app_wifi_init();

    espnow_config_t espnow_config = ESPNOW_INIT_CONFIG_DEFAULT();

    espnow_init(&espnow_config);

    button_config_t button_conf = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = GPIO_NUM_9,
            .active_level = 0
        }
    };

    button_handle_t button = iot_button_create(&button_conf);
    iot_button_register_cb(button, BUTTON_DOUBLE_CLICK, button_double_click_cb, NULL);
    iot_button_register_cb(button, BUTTON_LONG_PRESS_UP, button_long_press_cb, NULL);
    iot_button_register_cb(button, BUTTON_SINGLE_CLICK, button_single_click_cb, NULL);

    button_event_config_t button_event = {
        .event = BUTTON_MULTIPLE_CLICK,
        .event_data.multiple_clicks.clicks = 3
    };

    iot_button_register_event_cb(button, button_event, button_multiple_click_cb, NULL);

    espnow_ctrl_responder_data(ctrl_data_cb);
}
