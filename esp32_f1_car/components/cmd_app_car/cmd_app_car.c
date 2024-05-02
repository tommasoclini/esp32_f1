#include <stdio.h>
#include "cmd_app_car.h"

#include <esp_console.h>

static void register_limiter(void);

static int get_lim_fun(int argc, char **argv){
    //printf("limiter: %u");
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
}

void register_app_car(void)
{

}
