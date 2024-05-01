#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <esp_err.h>
#include <esp_blufi_api.h>

esp_err_t blufi_wrap_bt_ble_init();
esp_err_t blufi_wrap_init();
esp_err_t blufi_wrap_deinit();

#ifdef __cplusplus
}
#endif
