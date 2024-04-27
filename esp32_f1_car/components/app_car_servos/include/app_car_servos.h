#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <esp_types.h>
#include <esp_err.h>

/**
 * @brief Init steering and esc servos
*/
esp_err_t esc_motor_servo_init(void);

/**
 * @brief Deinit steering and esc servos
*/
esp_err_t esc_motor_servo_deinit(void);

/**
 * @brief Write a uint16 to esc motor servo
*/
esp_err_t esc_motor_servo_write_u16(uint16_t);

#ifdef __cplusplus
}
#endif
