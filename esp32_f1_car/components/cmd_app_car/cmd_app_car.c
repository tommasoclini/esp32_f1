#include <stdio.h>
#include "cmd_app_car.h"

#include <app_car_params.h>

#include <esp_err.h>
#include <esp_log.h>
#include <esp_check.h>

#include <esp_console.h>
#include <argtable3/argtable3.h>

static const char *TAG = "cmd app car";

static void register_limiter(void);

static int get_lim_fun(int argc, char **argv){
    uint16_t limiter;
    ESP_RETURN_ON_ERROR(get_limiter(&limiter), TAG, "Failed to get limiter");
    printf("Limiter: %u\r\n", limiter);
    return 0;
}

struct
{
    arg_int_t *limiter;
    arg_end_t *end;
} set_lim_args;

static int set_lim_fun(int argc, char **argv){
    int nerrors = arg_parse(argc, argv, (void **) &set_lim_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, set_lim_args.end, argv[0]);
        return 1;
    }

    uint16_t limiter;
    limiter = (uint16_t)set_lim_args.limiter->ival[0];

    ESP_RETURN_ON_ERROR(set_limiter(limiter), TAG, "Failed to set limiter from cmd");

    return 0;
}

static void register_limiter(void){
    const esp_console_cmd_t get_lim = {
        .command = "get_lim",
        .help = "Get the current limiter",
        .hint = NULL,
        .func = &get_lim_fun,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&get_lim));

    set_lim_args.limiter = arg_intn("l", "limiter", "<n>", 1, 1, "limiter value");
    set_lim_args.end = arg_end(5);

    esp_console_cmd_t set_lim = {
        .command = "set_lim",
        .help = "Set the limiter",
        .hint = NULL,
        .func = &set_lim_fun,
        .argtable = &set_lim_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&set_lim));
}

void register_app_car(void)
{
    init_params();
    register_limiter();
}
