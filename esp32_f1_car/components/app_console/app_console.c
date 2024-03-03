#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <app_common.h>
#include <app_console.h>

#include <esp_err.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_types.h>

#include <nvs.h>

#include <esp_mac.h>

#include <esp_console.h>
#include <argtable3/argtable3.h>

extern uint8_t controller_mac[6];

static const char *TAG = "app console";

static const esp_console_dev_uart_config_t esp_console_dev_uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
static const esp_console_repl_config_t esp_console_repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();

static esp_console_repl_t *esp_console_repl = NULL;

struct {
    struct arg_str *mac_address;
    struct arg_end *end;
} set_mac_args;

static int set_controller_mac(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &set_mac_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, set_mac_args.end, argv[0]);
        return 1;
    }

    const char *mac_address = set_mac_args.mac_address->sval[0];

    uint8_t mac[6];

    if (sscanf(mac_address, MACSTR, MAC2STR(&mac)) == 6)
    {
        memcpy(controller_mac, mac, sizeof(controller_mac));
        ESP_LOGI(TAG, "Controller mac: " MACSTR, MAC2STR(mac));
        nvs_handle_t handle;
        esp_err_t res = nvs_open(APP_NAMESPACE, NVS_READWRITE, &handle);
        if (res != ESP_OK)
        {
            ESP_LOGI(TAG, "Failed to open namespace %s, %s", APP_NAMESPACE, esp_err_to_name(res));
        } else {
            ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_set_blob(handle, CONTROLLER_MAC_ADDRESS_KEY, mac, sizeof(mac)));
            nvs_close(handle);
        }
    } else {
        ESP_LOGW(TAG, "Invalid controller mac");
    }
    return 0;
}

static int get_controller_mac(int argc, char **argv)
{
    ESP_LOGI(TAG, "Controller mac: " MACSTR, MAC2STR(controller_mac));
    return 0;
}

static int update_controller_mac(int argc, char **argv)
{
    nvs_handle_t handle;
    esp_err_t res = nvs_open(APP_NAMESPACE, NVS_READONLY, &handle);
    if (res != ESP_OK)
    {
        ESP_LOGW(TAG, "Failed to open namespace %s, %s", APP_NAMESPACE, esp_err_to_name(res));
        return res;
    } else {
        size_t size;
        res = nvs_get_blob(handle, CONTROLLER_MAC_ADDRESS_KEY, controller_mac, &size);
        switch (res)
        {
        case ESP_OK:
            ESP_LOGI(TAG, "Updated controller mac: " MACSTR, MAC2STR(controller_mac));
            break;
        
        default:
            ESP_LOGW(TAG, "Failed to get controller mac from nvs flash, %s", esp_err_to_name(res));
            break;
        }
        nvs_close(handle);
    }
    return 0;
}

static void register_mac(){
    set_mac_args.mac_address = arg_str1(NULL, NULL, "<mac_address>", "controller mac address");
    set_mac_args.end = arg_end(3);
    const esp_console_cmd_t set_mac_cmd = {
        .command = "set_controller_mac",
        .help = "Sets the controller mac address which the car will be able to communicate with",
        .hint = "ab:cd:ef:01:23:45",
        .func = &set_controller_mac,
        .argtable = &set_mac_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&set_mac_cmd) );

    const esp_console_cmd_t update_mac_cmd = {
        .command = "update_controller_mac",
        .help = "Updates controller mac stored in ram from mac stored in flash(this is done automatically at startup and when setting controller mac)",
        .hint = NULL,
        .func = &update_controller_mac
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&update_mac_cmd) );

    const esp_console_cmd_t get_mac_cmd = {
        .command = "get_controller_mac",
        .help = "Gets the controller mac address",
        .hint = NULL,
        .func = &get_controller_mac
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&get_mac_cmd) );
}

esp_err_t initialize_console(void)
{
    esp_err_t ret = esp_console_new_repl_uart(&esp_console_dev_uart_config, &esp_console_repl_config, &esp_console_repl);
    if (ret == ESP_OK)
    {
        register_mac();
    }
    return ret;
}

esp_err_t start_console(void){
    return esp_console_start_repl(esp_console_repl);
}
