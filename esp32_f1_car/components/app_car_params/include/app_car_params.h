#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <esp_err.h>

esp_err_t init_params(void);

esp_err_t get_limiter(uint16_t *lim);
esp_err_t get_limiter_p(uint16_t **lim_p);
esp_err_t set_limiter(uint16_t lim);

esp_err_t deinit_params(void);

#ifdef __cplusplus
}
#endif
