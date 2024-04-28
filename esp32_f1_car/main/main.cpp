#include <app_console.h>
#include <app_car_servos.h>
#include <pwm_capture.hpp>

#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// #include <math.h>
#include <algorithm>

#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#define MAP(x, l0, h0, l1, h1) (((h1 - l1) / (h0 - l0)) * (x - l0) + l1)

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

    pwm_capture::pwm_cap th_cap(GPIO_NUM_0, "gpio 0 throttle pwm cap");
    pwm_capture::pwm_cap ch3_cap(GPIO_NUM_1, "gpio 1 ch3 pwm cap");
    pwm_capture::pwm_cap ch4_cap(GPIO_NUM_2, "gpio 2 ch4 pwm cap");

    ch3_cap.init();
    ch4_cap.init();
    th_cap.init();

    ch3_cap.start();
    // ch4_cap.start();
    th_cap.start();

    gpio_num_t th_gpio = th_cap.get_gpio();
    gpio_num_t ch3_gpio = ch3_cap.get_gpio();
    //gpio_num_t ch4_gpio = ch4_cap.get_gpio();

    while (1)
    {
        pwm_capture::pwm_item_data_t pwm_th;
        pwm_capture::pwm_item_data_t pwm_ch3;

        ch3_cap.pwm_queue_receive(&pwm_ch3, portMAX_DELAY);
        th_cap.pwm_queue_receive(&pwm_th, portMAX_DELAY);
        uint16_t th = MAP(std::clamp((float)pwm_th.duty / (float)pwm_th.period, 0.05f, 0.10f), 0.05f, 0.10f, (float)0x0, (float)0xffff);
        if (((float)pwm_ch3.duty / (float)pwm_ch3.period) > 0.075f)
        {
            th = std::min(th, limiter_servo_val);
        }
        
        esc_motor_servo_write_u16(th);

        ESP_LOGI(TAG, "ch3 G(%d), P(%llu), D(%llu); th G(%d), P(%llu), D(%llu); esc(%u)", ch3_gpio, pwm_ch3.period, pwm_ch3.duty, th_gpio, pwm_th.period, pwm_th.duty, th);
    }
}