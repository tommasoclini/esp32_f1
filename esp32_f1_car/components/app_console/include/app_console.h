#pragma once

#include <esp_err.h>

/**
 * @brief Initialize uart console
*/
esp_err_t initialize_console(void);

/**
 * @brief Start uart console
*/
esp_err_t start_console(void);

/**
 * @brief Stop and deinitialize uart console
*/
esp_err_t stop_and_deinit_console(void);
