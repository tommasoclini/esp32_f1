#include <app_console.h>
#include <app_car_servos.h>
#include <pwm_capture.hpp>
#include <app_car_params.h>

#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <algorithm>

static const char *TAG = "main";

#define POINT_MIDDLE        0.2f
#define POINT_END           0.3f

#define ACCEL_LIMIT         0.2f

static bool limiter_active;
static uint16_t *limiter_p = NULL;

static const float duty_min = 0.05f;
static const float duty_max = 0.10f;
static const float duty_mid = (duty_min + duty_max) / 2.0f;

static const float diff_max_mid = duty_max - duty_mid;
static const float diff_max_mid_2 = diff_max_mid / 2.0;

static float a, b, c;

static const float brake_coeff = 0.8f;

pwm_capture::pwm_cap st_cap(GPIO_NUM_0, "gpio 0 st pwm cap");
pwm_capture::pwm_cap th_cap(GPIO_NUM_1, "gpio 1 th pwm cap");
pwm_capture::pwm_cap ch3_cap(GPIO_NUM_2, "gpio 2 ch3 pwm cap");
pwm_capture::pwm_cap ch4_cap(GPIO_NUM_3, "gpio 3 ch4 pwm cap");

template<typename T>
static T map(T x, T l0, T h0, T l1, T h1){
    return (((h1 - l1) / (h0 - l0)) * (x - l0) + l1);
}

template<typename T>
void parameters(T *a, T *b, T *c, T x1, T y1, T x2, T y2, T x3, T y3){
    T denom = (x1 - x2) * (x1 - x3) * (x2 - x3);
    *a = (x3 * (y2 - y1) + x2 * (y1 - y3) + x1 * (y3 - y2)) / denom;
    *b = (x3*x3 * (y1 - y2) + x2*x2 * (y3 - y1) + x1*x1 * (y2 - y3)) / denom;
    *c = (x2 * x3 * (x2 - x3) * y1 + x3 * x1 * (x3 - x1) * y2 + x1 * x2 * (x1 - x2) * y3) / denom;
}

template<typename T>
static T process_duty(T duty, int64_t t0){
    float x = std::abs(duty - duty_mid);
    float offset = 0.0f;
    if (duty >= duty_mid)
    {
        static float last_offset = 0.0f;
        static int64_t last_t0 = 0;
        float dt = (float)(t0 - last_t0) / 1000.0f;

        offset = std::min(a * x*x + b * x + c, last_offset + (ACCEL_LIMIT * diff_max_mid * POINT_END) / dt);
        
        last_offset = offset;
        last_t0 = t0;
    } else {
        offset = - x * brake_coeff;
    }

    duty = duty_mid + offset;
    return duty;
}

extern "C" void app_main(void)
{
    parameters(&a, &b, &c, 0.0f, 0.0f, diff_max_mid_2, diff_max_mid_2 * POINT_MIDDLE, diff_max_mid, diff_max_mid * POINT_END);
    ESP_ERROR_CHECK(init_params());
    
    ESP_ERROR_CHECK(get_limiter_p(&limiter_p));

    ESP_ERROR_CHECK(initialize_console());
    ESP_ERROR_CHECK(start_console());

    ESP_ERROR_CHECK(init_servos());

    pwm_capture::init_for_all();

    QueueHandle_t queue = xQueueCreate(25, sizeof(pwm_capture::pwm_item_data_t));

    st_cap.init(queue);
    th_cap.init(queue);
    ch3_cap.init(queue);
    ch4_cap.init(queue);

    st_cap.start();
    th_cap.start();
    ch3_cap.start();
    // ch4_cap.start();

    gpio_num_t st_gpio = st_cap.get_gpio();
    gpio_num_t th_gpio = th_cap.get_gpio();
    gpio_num_t ch3_gpio = ch3_cap.get_gpio();
    gpio_num_t ch4_gpio = ch4_cap.get_gpio();

    while (1)
    {
        static uint16_t th = 0x7fff;
        static uint16_t st = 0x7fff;

        pwm_capture::pwm_item_data_t pwm;
        xQueueReceive(queue, &pwm, portMAX_DELAY);
        if (pwm.gpio == th_gpio)
        {
            float duty = std::clamp((float)pwm.duty / (float)pwm.period, duty_min, duty_max);
            
            duty = process_duty(duty, pwm.t0);

            th = map(duty, duty_min, duty_max, (float)0x0, (float)0xffff);
        }
        else if (pwm.gpio == st_gpio)
        {
            st = map(std::clamp((float)pwm.duty / (float)pwm.period, duty_min, duty_max), duty_min, duty_max, (float)0xffff, (float)0x0);
            steering_servo_write_u16(st);
        }
        else if (pwm.gpio == ch3_gpio)
        {
            limiter_active = ((float)pwm.duty / (float)pwm.period) > duty_mid;
        }

        if (limiter_active)
            th = std::min(th, *limiter_p);

        static uint16_t last_th = 0x7fff;
        if (th != last_th){
            esc_motor_servo_write_u16(th);
            last_th = th;
        }

        //ESP_LOGI(TAG, "gpio(%d), duty(%llu), period(%llu), t0(%llu) esc(%u)", pwm.gpio, pwm.duty, pwm.period, pwm.t0, th);
    }
}