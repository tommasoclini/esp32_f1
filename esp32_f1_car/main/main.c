#include <app_console.h>
#include <app_remote_control.h>
#include <app_car_servos.h>

#include <nvs.h>
#include <nvs_flash.h>

#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

static const char *TAG = "main";

/*static void initialize_nvs(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}*/

TimerHandle_t car_remote_control_tim;

typedef struct __packed {
    uint16_t throttle;
    uint16_t steering;
} control_data_t;

static bool limiter_active;
static uint16_t limiter_servo_val = 0x7fff + 0x2000;

#define THROTTLE_TO_SERVO(th) (limiter_active ? MIN(th, limiter_servo_val) : th)

static void app_remote_control_data_cb(espnow_attribute_t init_attr, espnow_attribute_t resp_attr, uint32_t resp_val)
{
    ESP_LOGI(TAG, "Init attr: %d, Resp attr: 0x%x, Val: %lu", init_attr, resp_attr, resp_val);

    if (init_attr == ESPNOW_ATTRIBUTE_F1_BASE)
    {
        static control_data_t control_data;
        if (resp_attr == ESPNOW_ATTRIBUTE_F1_CONTROL)
        {
            ESP_LOGI(TAG, "Got control info");
            control_data = *(control_data_t*)&resp_val;

            uint16_t throttle_servo = THROTTLE_TO_SERVO(control_data.throttle);

            ESP_LOGI(TAG, "Throttle: %d -> esc motor servo: %d, Steering: %d",
                            control_data.throttle,
                            throttle_servo,
                            control_data.steering);

            esc_motor_servo_write_u16(throttle_servo);
            steering_servo_write_u16(control_data.steering);
            xTimerStart(car_remote_control_tim, 0);
        } else if (resp_attr == ESPNOW_ATTRIBUTE_F1_LIMITER)
        {
            ESP_LOGI(TAG, "Got limiter command");
            limiter_active = (resp_val != 0);
            ESP_LOGI(TAG, "Limiter is %s", limiter_active ? "active" : "not active");
            if (limiter_active) esc_motor_servo_write_u16(MIN(control_data.throttle, limiter_servo_val));
        } else if (resp_attr == ESPNOW_ATTRIBUTE_F1_TEST)
        {
            ESP_LOGI(TAG, "Got test message");
        }
    }
}

static void car_remote_control_tim_cb(TimerHandle_t tim){
    esc_motor_servo_write_u16(0x7fff);
    steering_servo_write_u16(0x7ffff);
}

void app_main(void)
{
    //initialize_nvs();

    ESP_ERROR_CHECK(initialize_remote_control());
    ESP_ERROR_CHECK(initialize_console());
    ESP_ERROR_CHECK(init_servos());

    ESP_ERROR_CHECK(start_console());

    xTimerCreate("car remote control", pdMS_TO_TICKS(300), pdFALSE, NULL, car_remote_control_tim_cb);

    ESP_ERROR_CHECK(remote_control_register_cb(app_remote_control_data_cb));
}