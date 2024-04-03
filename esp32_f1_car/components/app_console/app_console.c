#include <app_console.h>

#include <esp_check.h>
#include <esp_types.h>

#include <esp_console.h>
#include <argtable3/argtable3.h>

#include <cmd_system.h>

static const char *TAG = "app console";

static const esp_console_dev_uart_config_t esp_console_dev_uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
static const esp_console_repl_config_t esp_console_repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();

static esp_console_repl_t *esp_console_repl = NULL;

esp_err_t initialize_console(void)
{
    ESP_RETURN_ON_ERROR(esp_console_new_repl_uart(&esp_console_dev_uart_config, &esp_console_repl_config, &esp_console_repl), TAG, "Failed to create console on uart");
    ESP_LOGI(TAG, "Created console on uart");
    register_system();
    return ESP_OK;
}

esp_err_t start_console(void){
    ESP_RETURN_ON_ERROR(esp_console_start_repl(esp_console_repl), TAG, "Failed to start uart console");
    ESP_LOGI(TAG, "Started console on uart");
    return ESP_OK;
}

esp_err_t stop_and_deinit_console(void){
    ESP_RETURN_ON_ERROR(esp_console_deinit(), TAG, "Failed to stop and deinit uart console");
    ESP_LOGI(TAG, "Stopped and deinitialized uart console");
    return ESP_OK;
}
