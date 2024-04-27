#include <app_console.h>
#include <app_car_servos.h>
#include <pwm_capture.hpp>

#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>

#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

static const char *TAG = "main";

static bool limiter_active;
static uint16_t limiter_servo_val = 0x7fff + 0x2000;

#define THROTTLE_TO_SERVO(th) (limiter_active ? MIN(th, limiter_servo_val) : th)

extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(initialize_console());
    ESP_ERROR_CHECK(esc_motor_servo_init());

    ESP_ERROR_CHECK(start_console());

    pwm_capture::init_for_all();
    pwm_capture::pwm_cap cap(GPIO_NUM_0, "gpio 0 pwm cap");
    cap.init();
    cap.start();

    TickType_t start = xTaskGetTickCount();

    while (xTaskGetTickCount() < start + pdMS_TO_TICKS(5000))
    {
        pwm_capture::pwm_item_data_t pwm_item;
        cap.pwm_queue_receive(&pwm_item, portMAX_DELAY);
        ESP_LOGI(TAG, "GPIO(%d), Period(%llu), Duty(%llu), t0(%llu)", pwm_item.gpio, pwm_item.period, pwm_item.duty, pwm_item.t0);

        uint16_t th = ( (float)MIN(MAX(pwm_item.duty - 1000, 0 ), 1000) ) * 65.535f;
        ESP_LOGI(TAG, "esc motor servo(%u)", th);
        esc_motor_servo_write_u16(th);
    }

    cap.stop();
    cap.deinit();
    pwm_capture::deinit_for_all();
}