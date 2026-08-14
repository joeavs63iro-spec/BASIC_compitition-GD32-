/*********************************************************************************************************************
* @file            control.c
* @brief           Position PD track control loop
* @Target core     GD32F303RCT6
********************************************************************************************************************/

#include "headfile.h"

#define TRACK_CONTROL_TIMER       TIMER4
#define TRACK_SENSOR_WEIGHT_LL    (-100.0f)
#define TRACK_SENSOR_WEIGHT_LM    (-50.0f)
#define TRACK_SENSOR_WEIGHT_MM    (0.0f)
#define TRACK_SENSOR_WEIGHT_RM    (50.0f)
#define TRACK_SENSOR_WEIGHT_RR    (100.0f)

volatile TrackControl g_track_control;

static float control_absf(float value)
{
    return (value < 0.0f) ? -value : value;
}

static float control_limit_float(float value, float min_value, float max_value)
{
    if(value < min_value)
    {
        return min_value;
    }
    if(value > max_value)
    {
        return max_value;
    }
    return value;
}

static uint16_t control_limit_pwm(int32_t value)
{
    uint32_t pwm_value;

    track_sanitize_params();
    if(value <= 0)
    {
        return 0U;
    }

    pwm_value = (uint32_t)value;
    if((pwm_value > 0U) && (pwm_value < g_track_control.min_speed))
    {
        pwm_value = g_track_control.min_speed;
    }
    if(pwm_value > g_track_control.max_speed)
    {
        pwm_value = g_track_control.max_speed;
    }
    if(pwm_value > MOTOR_MAX_SPEED)
    {
        pwm_value = MOTOR_MAX_SPEED;
    }

    return (uint16_t)pwm_value;
}

void track_sanitize_params(void)
{
    if(g_track_control.base_speed > MOTOR_MAX_SPEED)
    {
        g_track_control.base_speed = MOTOR_MAX_SPEED;
    }
    if(g_track_control.max_speed > MOTOR_MAX_SPEED)
    {
        g_track_control.max_speed = MOTOR_MAX_SPEED;
    }
    if(g_track_control.min_speed > MOTOR_MAX_SPEED)
    {
        g_track_control.min_speed = MOTOR_MAX_SPEED;
    }
    if(g_track_control.min_speed > g_track_control.max_speed)
    {
        g_track_control.min_speed = g_track_control.max_speed;
    }
    g_track_control.output_limit = control_limit_float(g_track_control.output_limit, 0.0f, (float)MOTOR_MAX_SPEED);
    g_track_control.output_scale = control_limit_float(g_track_control.output_scale, 0.0f, 50.0f);
    g_track_control.lost_threshold = control_limit_float(g_track_control.lost_threshold, 0.0f, 500.0f);
    g_track_control.deadband = control_limit_float(g_track_control.deadband, 0.0f, 50.0f);
}

void track_pid_reset(void)
{
    g_track_control.position = 0.0f;
    g_track_control.error = 0.0f;
    g_track_control.last_error = 0.0f;
    g_track_control.output = 0.0f;
    g_track_control.left_pwm = 0U;
    g_track_control.right_pwm = 0U;
    g_track_control.lost_flag = 0U;
}

void track_init(void)
{
    g_track_control.kp = 1.90f;
    g_track_control.kd = 2.80f;
    g_track_control.output_scale = 7.0f;
    g_track_control.output_limit = (float)MOTOR_MAX_SPEED;
    g_track_control.lost_threshold = 30.0f;
    g_track_control.deadband = 1.0f;
    g_track_control.base_speed = MOTOR_MAX_SPEED;
    g_track_control.min_speed = 0U;
    g_track_control.max_speed = MOTOR_MAX_SPEED;
    g_track_control.run_enable = 1U;
    track_pid_reset();
    track_sanitize_params();
}

void track_timer_init(void)
{
    timer_parameter_struct timer_initpara;

    nvic_irq_enable(TIMER4_IRQn, 0, 0);
    rcu_periph_clock_enable(RCU_TIMER4);
    timer_deinit(TRACK_CONTROL_TIMER);

    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler = 120U - 1U;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = (TRACK_CONTROL_PERIOD_MS * 1000U) - 1U;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_init(TRACK_CONTROL_TIMER, &timer_initpara);

    timer_interrupt_flag_clear(TRACK_CONTROL_TIMER, TIMER_INT_FLAG_UP);
    timer_interrupt_enable(TRACK_CONTROL_TIMER, TIMER_INT_UP);
    timer_enable(TRACK_CONTROL_TIMER);
}

void track_set_run(uint8_t enabled)
{
    g_track_control.run_enable = (enabled != 0U) ? 1U : 0U;
    track_pid_reset();

    if(g_track_control.run_enable == 0U)
    {
        car_stop();
    }
}

uint8_t track_get_run(void)
{
    return g_track_control.run_enable;
}

void Track_SetPD(float kp, float kd)
{
    if(kp >= 0.0f)
    {
        g_track_control.kp = kp;
    }

    if(kd >= 0.0f)
    {
        g_track_control.kd = kd;
    }

    track_pid_reset();
    track_sanitize_params();
}

void positional_PD_track(void)
{
    float signal_sum;
    float weighted_position;
    float derivative;
    float pd_output;
    int32_t correction;
    int32_t left_cmd;
    int32_t right_cmd;

    track_sanitize_params();
    signal_sum = ll + lm + mm + rm + rr;

    if(signal_sum < g_track_control.lost_threshold)
    {
        g_track_control.lost_flag = 1U;
        g_track_control.position = 0.0f;
        g_track_control.error = 0.0f;
        g_track_control.last_error = 0.0f;
        g_track_control.output = 0.0f;
        g_track_control.left_pwm = 0U;
        g_track_control.right_pwm = 0U;
        return;
    }

    weighted_position = (TRACK_SENSOR_WEIGHT_LL * ll) +
                        (TRACK_SENSOR_WEIGHT_LM * lm) +
                        (TRACK_SENSOR_WEIGHT_MM * mm) +
                        (TRACK_SENSOR_WEIGHT_RM * rm) +
                        (TRACK_SENSOR_WEIGHT_RR * rr);

    g_track_control.lost_flag = 0U;
    g_track_control.position = weighted_position / signal_sum;
    g_track_control.error = g_track_control.position;

    if(control_absf(g_track_control.error) < g_track_control.deadband)
    {
        g_track_control.error = 0.0f;
    }

    derivative = g_track_control.error - g_track_control.last_error;
    pd_output = ((g_track_control.kp * g_track_control.error) +
                 (g_track_control.kd * derivative)) * g_track_control.output_scale;
    pd_output = control_limit_float(pd_output, -g_track_control.output_limit, g_track_control.output_limit);

    g_track_control.output = pd_output;
    correction = (int32_t)pd_output;
    left_cmd = (int32_t)g_track_control.base_speed + correction;
    right_cmd = (int32_t)g_track_control.base_speed - correction;
    g_track_control.left_pwm = control_limit_pwm(left_cmd);
    g_track_control.right_pwm = control_limit_pwm(right_cmd);
    g_track_control.last_error = g_track_control.error;
}

void track_motion_output(void)
{
    if((g_track_control.run_enable == 0U) || (g_track_control.lost_flag != 0U))
    {
        car_stop();
        return;
    }

    motor_forward(left, g_track_control.left_pwm);
    motor_forward(right, g_track_control.right_pwm);
}

void track_control_isr(void)
{
    if(g_track_control.run_enable == 0U)
    {
        return;
    }

    all_adc_get();
    five_unification();
    positional_PD_track();
    track_motion_output();
}
