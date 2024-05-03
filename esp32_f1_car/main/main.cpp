#include <app_console.h>
#include <app_car_servos.h>
#include <pwm_capture.hpp>
#include <app_car_params.h>

// #include <blufi_wrap.h>

// #include <esp-libtelnet.h>

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
static uint16_t *limiter_p = NULL;

#define THROTTLE_TO_SERVO(th) (limiter_active ? MIN(th, limiter_servo_val) : th)

#define THROTTLE_RANGE 0x1999

pwm_capture::pwm_cap th_cap(GPIO_NUM_1, "gpio 1 th pwm cap");
pwm_capture::pwm_cap ch3_cap(GPIO_NUM_2, "gpio 2 ch3 pwm cap");
pwm_capture::pwm_cap ch4_cap(GPIO_NUM_3, "gpio 3 ch4 pwm cap");
pwm_capture::pwm_cap st_cap(GPIO_NUM_0, "gpio 0 st pwm cap");

/*static void telnet_rx_cb(const char *buf, size_t len) {
    ESP_LOGI(TAG, "Received %d bytes from telnet: %.*s", len, len, buf);
}*/

extern "C" void app_main(void)
{
    /*ESP_ERROR_CHECK(blufi_wrap_init());

    init_telnet(telnet_rx_cb);
    start_telnet();
    telnet_mirror_to_uart(true);*/

    ESP_ERROR_CHECK(init_params());
    
    ESP_ERROR_CHECK(get_limiter_p(&limiter_p));

    ESP_ERROR_CHECK(initialize_console());
    ESP_ERROR_CHECK(start_console());

    ESP_ERROR_CHECK(init_servos());

    pwm_capture::init_for_all();

    QueueHandle_t queue = xQueueCreate(20, sizeof(pwm_capture::pwm_item_data_t));

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
    // gpio_num_t ch4_gpio = ch4_cap.get_gpio();

    while (1)
    {
        static uint16_t th = 0x7fff;
        static uint16_t st = 0x7fff;

        pwm_capture::pwm_item_data_t pwm;
        xQueueReceive(queue, &pwm, portMAX_DELAY);
        if (pwm.gpio == th_gpio)
        {
            th = MAP(std::clamp((float)pwm.duty / (float)pwm.period, 0.05f, 0.10f), 0.05f, 0.10f, (float)(0x7fff - THROTTLE_RANGE), (float)(0x7fff + THROTTLE_RANGE));
        }
        else if (pwm.gpio == st_gpio) {
            st = MAP(std::clamp((float)pwm.duty / (float)pwm.period, 0.05f, 0.10f), 0.05f, 0.10f, (float)0xffff, (float)0x0);
            steering_servo_write_u16(st);
        }
        else if (pwm.gpio == ch3_gpio)
        {
            limiter_active = ((float)pwm.duty / (float)pwm.period) > 0.075f;
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