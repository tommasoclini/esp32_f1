#include "app_car_servos.h"
#include <app_car_servos_conf.h>

#include <esp_check.h>

#include <driver/ledc.h>

static const char *TAG = "app car servos";

static bool init = false;

esp_err_t init_servos(void){
    ledc_timer_config_t ledc_tim_conf = {
        .clk_cfg = LEDC_AUTO_CLK,
        .duty_resolution = SERVOS_LEDC_TIMER_BIT,
        .freq_hz = SERVOS_LEDC_TIMER_FREQ,
        .speed_mode = SERVOS_LEDC_TIMER_SPEED_MODE,
        .timer_num = SERVOS_LEDC_TIMER
    };
    ESP_RETURN_ON_ERROR(ledc_timer_config(&ledc_tim_conf), TAG, "Failed to config ledc timer for servos");

    ledc_channel_config_t ledc_chan_conf = {
        .gpio_num = STEERING_SERVO_PIN,
        .speed_mode = SERVOS_LEDC_TIMER_SPEED_MODE,
        .channel = STEERING_SERVO_LEDC_CHAN,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = SERVOS_LEDC_TIMER,
        .duty = 0,
        .hpoint = 0
    };
    ESP_RETURN_ON_ERROR(ledc_channel_config(&ledc_chan_conf), TAG, "Failed to config steering servo ledc channel");

    ledc_chan_conf.gpio_num = ESC_MOTOR_SERVO_PIN;
    ledc_chan_conf.channel = ESC_MOTOR_SERVO_LEDC_CHAN;
    ESP_RETURN_ON_ERROR(ledc_channel_config(&ledc_chan_conf), TAG, "Failed to config esc motor servo ledc channel");
    init = true;
    return ESP_OK;
}

esp_err_t deinit_servos(void){
    ESP_RETURN_ON_ERROR(ledc_stop(SERVOS_LEDC_TIMER_SPEED_MODE, STEERING_SERVO_LEDC_CHAN, 0), TAG, "Failed to stop steering servo ledc chan");
    ESP_RETURN_ON_ERROR(ledc_stop(SERVOS_LEDC_TIMER_SPEED_MODE, ESC_MOTOR_SERVO_LEDC_CHAN, 0), TAG, "Failed to stop esc motor servo ledc chan");

    ESP_RETURN_ON_ERROR(ledc_timer_pause(SERVOS_LEDC_TIMER_SPEED_MODE, SERVOS_LEDC_TIMER), TAG, "Failed to pause servos ledc timer");
    ledc_timer_config_t ledc_tim_conf = {
        .deconfigure = true
    };
    ESP_RETURN_ON_ERROR(ledc_timer_config(&ledc_tim_conf), TAG, "Failed to deconfig servos ledc timer");

    return ESP_OK;
}

esp_err_t steering_servo_write_u16(uint16_t angle){
    if (init)
    {
        ESP_RETURN_ON_ERROR(ledc_set_duty(SERVOS_LEDC_TIMER_SPEED_MODE, STEERING_SERVO_LEDC_CHAN, STEERING_SERVO_UINT16_TO_DUTY(angle)), TAG, "Failed to set steering servo ledc channel duty");
        ESP_RETURN_ON_ERROR(ledc_update_duty(SERVOS_LEDC_TIMER_SPEED_MODE, STEERING_SERVO_LEDC_CHAN), TAG, "Failed to set steering servo ledc channel duty");
    } else {
        return ESP_ERR_INVALID_STATE;
    }

    return ESP_OK;
}

esp_err_t esc_motor_servo_write_u16(uint16_t angle){
    if (init)
    {
        ESP_RETURN_ON_ERROR(ledc_set_duty(SERVOS_LEDC_TIMER_SPEED_MODE, ESC_MOTOR_SERVO_LEDC_CHAN, ESC_MOTOR_SERVO_UINT16_TO_DUTY(angle)), TAG, "Failed to set esc motor servo ledc channel duty");
        ESP_RETURN_ON_ERROR(ledc_update_duty(SERVOS_LEDC_TIMER_SPEED_MODE, ESC_MOTOR_SERVO_LEDC_CHAN), TAG, "Failed to set esc motor servo ledc channel duty");
    } else {
        return ESP_ERR_INVALID_STATE;
    }

    return ESP_OK;
}
