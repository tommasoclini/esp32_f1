#include <app_console.h>
#include <app_car_servos.h>
#include <pwm_capture.hpp>
#include <app_car_params.h>

#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <algorithm>

#define MAP(x, l0, h0, l1, h1) (((h1 - l1) / (h0 - l0)) * (x - l0) + l1)

static const char *TAG = "main";

static bool limiter_active;
static uint16_t *limiter_p = NULL;

static const float duty_min = 0.05f;
static const float duty_max = 0.10f;
static const float duty_mid = (duty_min + duty_max) / 2.0f;

#define THROTTLE_COEFF_DEFAULT_VAL      0.15f // min 0.0 max 1.0
#define THROTTLE_COEFF_1_DEFAULT_VAL    0.21f
#define THROTTLE_NOS_COEFF              1.5f
#define BRAKE_COEFF_DEFAULT_VAL         0.6f // min 0.0 max 1.0

static float throttle_coeff = THROTTLE_COEFF_DEFAULT_VAL;
static float throttle_coeff_1 = THROTTLE_COEFF_1_DEFAULT_VAL;
static float brake_coeff = BRAKE_COEFF_DEFAULT_VAL;

pwm_capture::pwm_cap st_cap(GPIO_NUM_0, "gpio 0 st pwm cap");
pwm_capture::pwm_cap th_cap(GPIO_NUM_1, "gpio 1 th pwm cap");
pwm_capture::pwm_cap ch3_cap(GPIO_NUM_2, "gpio 2 ch3 pwm cap");
pwm_capture::pwm_cap ch4_cap(GPIO_NUM_3, "gpio 3 ch4 pwm cap");


extern "C" void app_main(void)
{
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
    ch4_cap.start();

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
            if (duty >= duty_mid){
                // th = MAP(duty, duty_min, duty_max, (float)(0x7fff - THROTTLE_RANGE), (float)(0x7fff + THROTTLE_RANGE));
                float x = (duty - duty_mid);
                if (x <= (duty_max + duty_mid) / 2.0f)
                {
                    duty = duty_mid + throttle_coeff * x;
                } else {
                    duty = duty_mid + throttle_coeff_1 * x;
                }
            } else {
                // th = MAP(duty, duty_min, duty_max, (float)(0x0), (float)(0xffff));
                duty = duty_mid - (duty_mid - duty) * brake_coeff;
            }
            th = MAP(duty, duty_min, duty_max, (float)0x0, (float)0xffff);
        }
        else if (pwm.gpio == st_gpio)
        {
            st = MAP(std::clamp((float)pwm.duty / (float)pwm.period, duty_min, duty_max), duty_min, duty_max, (float)0xffff, (float)0x0);
            steering_servo_write_u16(st);
        }
        else if (pwm.gpio == ch3_gpio)
        {
            limiter_active = ((float)pwm.duty / (float)pwm.period) > duty_mid;
        }
        else if (pwm.gpio == ch4_gpio)
        {
            static bool last_boost;
            bool boost = ((float)pwm.duty / (float)pwm.period) >= duty_mid;
            if (boost != last_boost)
            {
                if (boost)
                {
                    throttle_coeff = std::min(THROTTLE_COEFF_DEFAULT_VAL * THROTTLE_NOS_COEFF, 1.0f);
                }
                else
                {
                    throttle_coeff = THROTTLE_COEFF_DEFAULT_VAL;
                }
            }
            last_boost = boost;
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