#include <stdio.h>
#include "app_car_params.h"

#include <esp_err.h>
#include <esp_log.h>
#include <esp_check.h>

#include "nvs_flash.h"
#include "nvs.h"

static const char *TAG = "app car params";

static const char *APP_CAR_PARAMS_NS = "app_car_params";

static const char *LIMITER_NAME = "limiter";
#define LIMITER_DEFAULT_VAL (0x7fff + 0x2000)

static uint16_t limiter = 1000;

bool init = false;

esp_err_t init_params(void){
    if (init) return ESP_ERR_INVALID_STATE;
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    nvs_handle_t handle;
    ESP_RETURN_ON_ERROR(nvs_open(APP_CAR_PARAMS_NS, NVS_READWRITE, &handle), TAG, "Failed to open params namspace");
    
    err = nvs_get_u16(handle, LIMITER_NAME, &limiter);
    if (err == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGW(TAG, "Limiter not found in nvs");
        nvs_set_u16(handle, LIMITER_NAME, LIMITER_DEFAULT_VAL);
        limiter = LIMITER_DEFAULT_VAL;
        err = ESP_OK;
    }
    nvs_close(handle);

    init = true;

    return ESP_OK;
}

esp_err_t deinit_params(void){
    init = false;
    return ESP_OK;
}

esp_err_t get_limiter(uint16_t *lim){
    *lim = limiter;
    return ESP_OK;
}

esp_err_t get_limiter_p(uint16_t **lim_p){
    *lim_p = &limiter;
    return ESP_OK;
}

esp_err_t set_limiter(uint16_t lim){
    limiter = lim;
    esp_err_t ret = ESP_OK;
    nvs_handle_t handle;
    ESP_RETURN_ON_ERROR(nvs_open(APP_CAR_PARAMS_NS, NVS_READWRITE, &handle), TAG, "Failed to open params namspace");
    nvs_set_u16(handle, LIMITER_NAME, lim);
    nvs_close(handle);
    return ret;
}
