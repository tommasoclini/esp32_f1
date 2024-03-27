#pragma once

#include <esp_err.h>
#include <espnow.h>
#include <espnow_ctrl.h>

#define ESPNOW_ATTRIBUTE_F1_BASE        0x4000

#define ESPNOW_ATTRIBUTE_F1_TEST        (ESPNOW_ATTRIBUTE_F1_BASE + 0x01)
#define ESPNOW_ATTRIBUTE_F1_CONTROL     (ESPNOW_ATTRIBUTE_F1_BASE + 0x02)
#define ESPNOW_ATTRIBUTE_F1_LIMITER     (ESPNOW_ATTRIBUTE_F1_BASE + 0x03)

/**
 * @brief Initialize remote control based on espnow ctrl
*/
esp_err_t initialize_remote_control(void);

/**
 * @brief Register a callback for incoming data
*/
esp_err_t remote_control_register_cb(espnow_ctrl_data_cb_t);

/**
 * @brief Send data to bound devices
*/
esp_err_t remote_control_send_data(espnow_attribute_t resp_attr, uint32_t val);
