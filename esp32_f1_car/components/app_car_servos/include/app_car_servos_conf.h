#pragma once

#define SERVOS_LEDC_TIMER_FREQ                  (50)
#define SERVOS_LEDC_TIMER                       LEDC_TIMER_0
#define SERVOS_LEDC_TIMER_BIT                   LEDC_TIMER_14_BIT
#define SERVOS_LEDC_TIMER_SPEED_MODE            LEDC_LOW_SPEED_MODE

#define STEERING_SERVO_LEDC_CHAN                LEDC_CHANNEL_0
#define ESC_MOTOR_SERVO_LEDC_CHAN               LEDC_CHANNEL_1

#define ESC_MOTOR_SERVO_PIN                     GPIO_NUM_4
#define STEERING_SERVO_PIN                      GPIO_NUM_5

/**
 * 0 - 65535
 * to
 * 410 - 2048
*/
#define STEERING_SERVO_UINT16_TO_DUTY(x) (819 + ((uint16_t)x) / 80)

/**
 * 0 - 65535
 * to
 * 819 - 1638
*/
#define ESC_MOTOR_SERVO_UINT16_TO_DUTY(x) (819 + ((uint16_t)x) / 80)