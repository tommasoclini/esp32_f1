#include <app_remote_control.h>
#include <app_remote_control_conf.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

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

/**
 * @brief init wifi
*/
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

/**
 * @brief bind event app cb
*/
static bool ctrl_bind_cb(espnow_attribute_t init_attr, uint8_t mac[6], int8_t rssi){
    ESP_LOGI(TAG, "Bind init attr: %d, mac: " MACSTR ", rssi: %d", init_attr, MAC2STR(mac), rssi);
    return rssi >= BIND_UNBIND_RSSI;
}

/**
 * @brief start bind/unbind operation
*/
static void app_bind(bool bind){
    espnow_ctrl_responder_bind(BIND_WAIT_MS, BIND_UNBIND_RSSI, ctrl_bind_cb);

    vTaskDelay(pdMS_TO_TICKS(100));

    espnow_ctrl_initiator_bind(ESPNOW_ATTRIBUTE_F1_BASE, bind);
}

/* button */
static void button_double_click_cb(void *button_handle, void *usr_data)
{
    ESP_LOGI(TAG, "BUTTON_DOUBLE_CLICK");

    app_bind(true);
    ESP_LOGI(TAG, "Doing bind operation");
}

static void button_long_press_cb(void *button_handle, void *usr_data)
{
    ESP_LOGI(TAG, "BUTTON_LONG_PRESS_CLICK");

    app_bind(false);
    ESP_LOGI(TAG, "Doing unbind operation");
}

static void button_multiple_click_cb(void *button_handle, void *usr_data)
{
    ESP_LOGI(TAG, "BUTTON_MULTIPLE_CLICK");

    espnow_ctrl_responder_clear_bindlist();
    ESP_LOGI(TAG, "Cleared remote control(espnow) bindlist");
}

static void button_single_click_cb(void *button_handle, void *usr_data)
{
    ESP_LOGI(TAG, "BUTTON_SINGLE_CLICK");

    espnow_ctrl_initiator_send(ESPNOW_ATTRIBUTE_F1_BASE, ESPNOW_ATTRIBUTE_F1_TEST, esp_log_timestamp());
    ESP_LOGI(TAG, "Sending test message");
}

static void init_button(void){
    button_config_t button_conf = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = APP_REMOTE_CTRL_BTN_GPIO,
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
}

/* initialization */
esp_err_t initialize_remote_control(void){
    ESP_RETURN_ON_ERROR(espnow_storage_init(), TAG, "Failed to init espnow storage");

    app_wifi_init();

    espnow_config_t espnow_config = ESPNOW_INIT_CONFIG_DEFAULT();
    ESP_RETURN_ON_ERROR(espnow_init(&espnow_config), TAG, "Failed to init espnow");

    init_button();

    return ESP_OK;
}

esp_err_t remote_control_register_cb(espnow_ctrl_data_cb_t data_cb){
    return espnow_ctrl_responder_data(data_cb);
}

esp_err_t remote_control_send_data(espnow_attribute_t resp_attr, uint32_t val){
    return espnow_ctrl_initiator_send(ESPNOW_ATTRIBUTE_F1_BASE, resp_attr, val);
}
