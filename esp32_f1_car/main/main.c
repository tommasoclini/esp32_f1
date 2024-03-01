#include <stdio.h>
#include <app_console.h>

void app_main(void)
{
    ESP_ERROR_CHECK(initialize_console());
    ESP_ERROR_CHECK(start_console());
}