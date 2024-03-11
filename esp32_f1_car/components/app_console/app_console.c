#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <app_common.h>
#include <app_console.h>

#include <esp_err.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_types.h>

#include <esp_console.h>
#include <argtable3/argtable3.h>

static const char *TAG = "app console";

static const esp_console_dev_uart_config_t esp_console_dev_uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
static const esp_console_repl_config_t esp_console_repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();

static esp_console_repl_t *esp_console_repl = NULL;

esp_err_t initialize_console(void)
{
    return esp_console_new_repl_uart(&esp_console_dev_uart_config, &esp_console_repl_config, &esp_console_repl);
}

esp_err_t start_console(void){
    return esp_console_start_repl(esp_console_repl);
}
