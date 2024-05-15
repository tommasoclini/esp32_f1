#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <esp_types.h>
#include <esp_err.h>

/**
 * @brief Init steering and esc servos
*/
esp_err_t init_servos(void);

/**
 * @brief Deinit steering and esc servos
*/
esp_err_t deinit_servos(void);

/**
 * @brief Write a uint16 to steering servo
*/
esp_err_t steering_servo_write_u16(uint16_t);

/**
 * @brief Write a uint16 to esc motor servo
*/
esp_err_t esc_motor_servo_write_u16(uint16_t);

#ifdef __cplusplus
}
#endif
