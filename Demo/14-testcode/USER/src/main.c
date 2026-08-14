/*********************************************************************************************************************
* @file            main.c
* @brief           Provincial competition debug demo
* @Target core     GD32F303RCT6
********************************************************************************************************************/

#include "main.h"
#include "headfile.h"

#define DEMO_LOOP_DELAY_MS        5U
#define DEMO_SENSOR_PERIOD_MS     200U
#define DEMO_ENCODER_PERIOD_MS    200U
#define DEMO_TOF_PERIOD_MS        200U
#define DEMO_LINE_PERIOD_MS       20U
#define DEMO_MOTION_RUN_MS        2000U
#define DEMO_REED_ALERT_MS        500U
#define DEMO_MOTION_PWM           10000U
#define DEMO_ENCODER_DUTY_TEST_ENABLE   1U
#define DEMO_ENCODER_DUTY_TEST_SAMPLE_MS 10U
#define DEMO_ENCODER_DUTY_TEST_KEY      0x06U
#define DEMO_ENCODER_DUTY_LOW           5000U
#define DEMO_ENCODER_DUTY_HIGH          10000U

static uint8_t sensor_view_enabled = 0;
static uint8_t encoder_view_enabled = 0;
static uint8_t tof_view_enabled = 0;
static uint8_t line_follow_enabled = 0;
static uint8_t motion_active = 0;
static uint8_t buzzer_active = 0;
static uint8_t rgb_index = 0;
static uint8_t motion_step = 0;

static uint64_t sensor_last_time_us = 0;
static uint64_t encoder_last_time_us = 0;
static uint64_t tof_last_time_us = 0;
static uint64_t line_last_time_us = 0;
static uint64_t motion_stop_time_us = 0;
static uint64_t buzzer_stop_time_us = 0;

static const rgb_color rgb_table[] =
{
    red,
    yellow,
    white,
    green,
    blue,
    cyan,
    purple,
};

static uint8_t demo_period_elapsed(uint64_t *last_time_us, uint32_t period_ms)
{
    uint64_t now_us = get_current_time_us();
    uint64_t period_us = (uint64_t)period_ms * 1000ULL;

    if((now_us - *last_time_us) >= period_us)
    {
        *last_time_us = now_us;
        return 1;
    }

    return 0;
}

static void demo_show_status(const char *text)
{
    oled_clear();
    oled_show_string(0, 0, (uint8_t *)"DEBUG DEMO", 12);
    oled_show_string(0, 1, (uint8_t *)text, 12);
}

static void demo_stop_motion(void)
{
    motion_active = 0;
    car_stop();
}

static void demo_reset_encoder_sample(void)
{
    car_stop();
    timer_counter_value_config(TIMER1, 0);
    timer_counter_value_config(TIMER2, 0);

    /*
     * update_encoder_values() keeps the previous hardware counter in static
     * variables, so call it once after clearing TIMER_CNT to sync that base.
     */
    update_encoder_values();
    motor1_count = 0;
    motor2_count = 0;
}

static void demo_run_encoder_duty_test(void)
{
    unsigned char key_value;
    uint16_t duty = DEMO_ENCODER_DUTY_LOW;
    uint8_t motor_running = 0;
    int32_t last_motor1_count = 0;
    int32_t last_motor2_count = 0;
    uint64_t last_sample_time_us = 0;

    demo_show_status("ENC DUTY TEST");
    printf("\r\nEncoder duty test: continuous forward, print pulse delta every 10 ms.\r\n");
    printf("Press K6 to start at PWM 5000, then switch between PWM 5000 and PWM 10000.\r\n");
    printf("Lift the car before running.\r\n\r\n");

    while(1)
    {
        key_value = key_check();
        if(key_value == DEMO_ENCODER_DUTY_TEST_KEY)
        {
            if(motor_running == 0)
            {
                duty = DEMO_ENCODER_DUTY_LOW;
                demo_reset_encoder_sample();
                last_motor1_count = 0;
                last_motor2_count = 0;
                last_sample_time_us = get_current_time_us();
                motor_running = 1;
            }
            else
            {
                duty = (duty == DEMO_ENCODER_DUTY_LOW) ? DEMO_ENCODER_DUTY_HIGH : DEMO_ENCODER_DUTY_LOW;
            }

            demo_show_status((duty == DEMO_ENCODER_DUTY_LOW) ? "K6 PWM 5000" : "K6 PWM 10000");
            oled_show_number(72, 2, duty, 5, 12);
            car_forward(duty);
            printf("PWM switched to %u\r\n", duty);
        }

        if((motor_running != 0) && (demo_period_elapsed(&last_sample_time_us, DEMO_ENCODER_DUTY_TEST_SAMPLE_MS) != 0))
        {
            int32_t delta1;
            int32_t delta2;

            update_encoder_values();
            delta1 = motor1_count - last_motor1_count;
            delta2 = motor2_count - last_motor2_count;
            last_motor1_count = motor1_count;
            last_motor2_count = motor2_count;

            printf("PWM=%u, dt=%u ms, d1=%ld, d2=%ld\r\n",
                   duty,
                   DEMO_ENCODER_DUTY_TEST_SAMPLE_MS,
                   (long)delta1,
                   (long)delta2);
        }

        delay_1ms(DEMO_LOOP_DELAY_MS);
    }
}

static void demo_start_reed_alert(void)
{
    /*
     * Reed feedback is non-blocking: RGB auto-off is handled by car_rgb_task(),
     * and buzzer shutoff is handled by demo_service_buzzer().
     */
    buzzer_freq(SO);
    buzzer_duty(50);
    buzzer_active = 1;
    buzzer_stop_time_us = get_current_time_us() + ((uint64_t)DEMO_REED_ALERT_MS * 1000ULL);
    car_both_rgb_on(red, DEMO_REED_ALERT_MS);
}

static void demo_service_buzzer(void)
{
    if((buzzer_active != 0) && (get_current_time_us() >= buzzer_stop_time_us))
    {
        buzzer_duty(0);
        buzzer_active = 0;
    }
}

static void demo_cycle_rgb(void)
{
    /* Show the current color first so the first K1 press starts from red. */
    car_rgb_set_both(rgb_table[rgb_index]);
    demo_show_status("K1 RGB COLOR");
    oled_show_number(72, 2, rgb_index + 1, 1, 12);

    rgb_index++;
    if(rgb_index >= (sizeof(rgb_table) / sizeof(rgb_table[0])))
    {
        rgb_index = 0;
    }
}

static void demo_toggle_sensor_view(void)
{
    sensor_view_enabled = !sensor_view_enabled;
    if(sensor_view_enabled != 0)
    {
        line_follow_enabled = 0;
        tof_view_enabled = 0;
        demo_stop_motion();
        demo_show_status("K2 SENSOR ON");
    }
    else
    {
        demo_show_status("K2 SENSOR OFF");
    }
}

static void demo_show_tof_distance(void)
{
    unsigned int distance = Get_Distance_TOF400C();

    printf("TOF400C Distance: %u mm\r\n", distance);
    oled_show_number(0, 2, distance, 4, 12);
    oled_show_string(32, 2, (uint8_t *)"mm", 12);
}

static void demo_toggle_tof_view(void)
{
    tof_view_enabled = !tof_view_enabled;
    if(tof_view_enabled != 0)
    {
        sensor_view_enabled = 0;
        encoder_view_enabled = 0;
        line_follow_enabled = 0;
        demo_stop_motion();
        tof_last_time_us = 0;
        demo_show_status("K3 TOF ON");
        demo_show_tof_distance();
    }
    else
    {
        demo_show_status("K3 TOF OFF");
    }
}

static void demo_start_motion_step(void)
{
    line_follow_enabled = 0;
    sensor_view_enabled = 0;
    tof_view_enabled = 0;
    motion_active = 1;
    motion_stop_time_us = get_current_time_us() + ((uint64_t)DEMO_MOTION_RUN_MS * 1000ULL);

    /*
     * Use parameterized car_motion APIs so province debug can start safely
     * on a lifted car.
     */
    if(motion_step == 0)
    {
        car_forward(DEMO_MOTION_PWM);
        demo_show_status("K4 MOTOR FWD");
    }
    else if(motion_step == 1)
    {
        car_backward(DEMO_MOTION_PWM);
        demo_show_status("K4 MOTOR BACK");
    }
    else if(motion_step == 2)
    {
        car_turnleft(0, DEMO_MOTION_PWM);
        demo_show_status("K4 MOTOR LEFT");
    }
    else
    {
        car_turnright(0, DEMO_MOTION_PWM);
        demo_show_status("K4 MOTOR RIGHT");
    }

    motion_step++;
    if(motion_step >= 4)
    {
        motion_step = 0;
    }
}

static void demo_toggle_encoder_view(void)
{
    encoder_view_enabled = !encoder_view_enabled;
    if(encoder_view_enabled != 0)
    {
        sensor_view_enabled = 0;
        tof_view_enabled = 0;
        line_follow_enabled = 0;
        motor1_count = 0;
        motor2_count = 0;
        demo_show_status("K5 ENCODER ON");
    }
    else
    {
        demo_show_status("K5 ENCODER OFF");
    }
}

static void demo_toggle_line_follow(void)
{
    line_follow_enabled = !line_follow_enabled;
    if(line_follow_enabled != 0)
    {
        sensor_view_enabled = 0;
        encoder_view_enabled = 0;
        tof_view_enabled = 0;
        demo_stop_motion();
        car_rgb_set_both(green);
        demo_show_status("K6 LINE ON");
    }
    else
    {
        demo_stop_motion();
        car_both_rgb_on(red, 200);
        demo_show_status("K6 LINE OFF");
    }
}

static void demo_handle_key(unsigned char key_value)
{
    if(key_value == 0x01)
    {
        demo_cycle_rgb();
    }
    else if(key_value == 0x02)
    {
        demo_toggle_sensor_view();
    }
    else if(key_value == 0x03)
    {
        demo_toggle_tof_view();
    }
    else if(key_value == 0x04)
    {
        demo_start_motion_step();
    }
    else if(key_value == 0x05)
    {
        demo_toggle_encoder_view();
    }
    else if(key_value == 0x06)
    {
        demo_toggle_line_follow();
    }
}

static void demo_service_motion(void)
{
    if((motion_active != 0) && (get_current_time_us() >= motion_stop_time_us))
    {
        demo_stop_motion();
    }
}

static void demo_service_sensor_view(void)
{
    uint8_t reed_state;

    if((sensor_view_enabled == 0) || (demo_period_elapsed(&sensor_last_time_us, DEMO_SENSOR_PERIOD_MS) == 0))
    {
        return;
    }

    all_adc_get();
    five_unification();
    oled_show_adc();
    oled_show_gyh();

    reed_state = reed_check();
    oled_show_string(72, 6, (uint8_t *)"REED", 12);
    oled_show_number(104, 6, reed_state, 1, 12);

    if((reed_state != 0) && (buzzer_active == 0))
    {
        demo_start_reed_alert();
    }
}

static void demo_service_tof_view(void)
{
    if((tof_view_enabled == 0) || (demo_period_elapsed(&tof_last_time_us, DEMO_TOF_PERIOD_MS) == 0))
    {
        return;
    }

    demo_show_tof_distance();
}

static void demo_service_encoder_view(void)
{
    if((encoder_view_enabled == 0) || (demo_period_elapsed(&encoder_last_time_us, DEMO_ENCODER_PERIOD_MS) == 0))
    {
        return;
    }

    update_encoder_values();
    printf("Encoder1: Count=%ld, Dir=%u\r\n", (long)motor1_count, motor1_dir);
    printf("Encoder2: Count=%ld, Dir=%u\r\n\r\n", (long)motor2_count, motor2_dir);

    oled_show_string(0, 1, (uint8_t *)"E1", 12);
    oled_show_number(20, 1, (uint32_t)((motor1_count < 0) ? -motor1_count : motor1_count), 6, 12);
    oled_show_string(0, 3, (uint8_t *)"E2", 12);
    oled_show_number(20, 3, (uint32_t)((motor2_count < 0) ? -motor2_count : motor2_count), 6, 12);
    oled_show_string(0, 5, (uint8_t *)"D", 12);
    oled_show_number(20, 5, motor1_dir, 1, 12);
    oled_show_number(40, 5, motor2_dir, 1, 12);
}

static void demo_service_line_follow(void)
{
    if((line_follow_enabled == 0) || (demo_period_elapsed(&line_last_time_us, DEMO_LINE_PERIOD_MS) == 0))
    {
        return;
    }

    /*
     * This uses a timestamp scheduler first. Moving the control call into a
     * hardware timer interrupt should be the next step after Keil/hardware QA.
     */
    xunxian();
}

int main(void)
{
// #if DEMO_ENCODER_DUTY_TEST_ENABLE == 0
    unsigned char key_value;
// #endif
    car_init();
    buzzer_duty(0);
    car_both_rgb_off();
    demo_show_status("READY");
    printf("Provincial competition debug demo ready.\r\n");

// #if DEMO_ENCODER_DUTY_TEST_ENABLE != 0
//     demo_run_encoder_duty_test();
// #else
    while(1)
    {
        key_value = key_check();
        if(key_value != 0xFF)
        {
            demo_handle_key(key_value);
        }

        car_rgb_task();
        demo_service_buzzer();
        demo_service_motion();
        demo_service_sensor_view();
        demo_service_tof_view();
        demo_service_encoder_view();
        demo_service_line_follow();

        delay_1ms(DEMO_LOOP_DELAY_MS);
    }
// #endif
}
