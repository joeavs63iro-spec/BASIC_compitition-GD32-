#include "headfile.h"
#include "control.h"

typedef struct
{
    float kp;
    float ki;
    float kd;
    float integral;
    float prev_error;
    float output_limit;
} SpeedPid;

static const int16_t sine_table[32] =
{
    0, 195, 383, 556, 707, 831, 924, 981,
    1000, 981, 924, 831, 707, 556, 383, 195,
    0, -195, -383, -556, -707, -831, -924, -981,
    -1000, -981, -924, -831, -707, -556, -383, -195
};

static SpeedPid pid_left;
static SpeedPid pid_right;
static ControlPidParam pid_param = { CONTROL_DEFAULT_KP, CONTROL_DEFAULT_KI, CONTROL_DEFAULT_KD };
static ControlData control_data;

static int32_t last_count_left = 0;
static int32_t last_count_right = 0;
static int16_t sine_base = 0;
static int16_t sine_amplitude = 0;
static uint16_t sine_period_ticks = 1;
static uint16_t sine_tick = 0;

static int32_t control_abs_i32(int32_t value)
{
    return (value < 0) ? -value : value;
}

static int16_t control_limit_i16(int32_t value)
{
    if(value > 32767)
    {
        return 32767;
    }

    if(value < -32768)
    {
        return -32768;
    }

    return (int16_t)value;
}

static void speed_pid_init(SpeedPid *pid, float kp, float ki, float kd, float limit)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->output_limit = limit;
}

static void speed_pid_reset(SpeedPid *pid)
{
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
}

static float speed_pid_update(SpeedPid *pid, float target, float actual)
{
    float error = target - actual;
    float derivative;
    float output;

    pid->integral += error;
    if(pid->integral > pid->output_limit)
    {
        pid->integral = pid->output_limit;
    }
    else if(pid->integral < -pid->output_limit)
    {
        pid->integral = -pid->output_limit;
    }

    derivative = error - pid->prev_error;
    pid->prev_error = error;

    output = (pid->kp * error) + (pid->ki * pid->integral) + (pid->kd * derivative);
    if(output > pid->output_limit)
    {
        output = pid->output_limit;
    }
    else if(output < -pid->output_limit)
    {
        output = -pid->output_limit;
    }

    return output;
}

static int32_t control_update_pwm(SpeedPid *pid, int16_t target, int32_t actual, int32_t last_pwm)
{
    int32_t pwm;
    float correction;

    if(target == 0)
    {
        speed_pid_reset(pid);
        return 0;
    }

    correction = speed_pid_update(pid,
                                  (float)control_abs_i32((int32_t)target),
                                  (float)control_abs_i32(actual));
    pwm = last_pwm + (int32_t)correction;

    if(pwm < CONTROL_MIN_PWM)
    {
        pwm = CONTROL_MIN_PWM;
    }
    else if(pwm > MOTOR_MAX_SPEED)
    {
        pwm = MOTOR_MAX_SPEED;
    }

    return pwm;
}

static void control_drive_motor(uint8_t motor_num, int16_t target, int32_t pwm)
{
    if(target > 0)
    {
        motor_set(motor_num, MOTOR_CW, pwm);
    }
    else if(target < 0)
    {
        motor_set(motor_num, MOTOR_CCW, pwm);
    }
    else
    {
        motor_set(motor_num, MOTOR_STOP, 0);
    }
}

static void control_update_sine_target(void)
{
    uint16_t index;
    int32_t target;

    if(control_data.sine_enabled == 0)
    {
        return;
    }

    index = (uint16_t)(((uint32_t)sine_tick * 32U) / sine_period_ticks);
    if(index >= 32U)
    {
        index = 31U;
    }

    target = (int32_t)sine_base + (((int32_t)sine_amplitude * sine_table[index]) / 1000);
    control_data.target_left = control_limit_i16(target);
    control_data.target_right = control_data.target_left;

    sine_tick++;
    if(sine_tick >= sine_period_ticks)
    {
        sine_tick = 0;
    }
}

void Control_Init(void)
{
    speed_pid_init(&pid_left, pid_param.kp, pid_param.ki, pid_param.kd, CONTROL_PID_OUTPUT_LIMIT);
    speed_pid_init(&pid_right, pid_param.kp, pid_param.ki, pid_param.kd, CONTROL_PID_OUTPUT_LIMIT);

    control_data.target_left = 0;
    control_data.target_right = 0;
    control_data.actual_left = 0;
    control_data.actual_right = 0;
    control_data.pwm_left = 0;
    control_data.pwm_right = 0;
    control_data.enabled = 0;
    control_data.sine_enabled = 0;

    update_encoder_values();
    last_count_left = motor1_count;
    last_count_right = motor2_count;
}

void Control_Task(void)
{
    if(control_data.enabled == 0)
    {
        return;
    }

    control_update_sine_target();

    update_encoder_values();
    control_data.actual_left = motor1_count - last_count_left;
    control_data.actual_right = motor2_count - last_count_right;
    last_count_left = motor1_count;
    last_count_right = motor2_count;

    control_data.pwm_left = control_update_pwm(&pid_left,
                                               control_data.target_left,
                                               control_data.actual_left,
                                               control_data.pwm_left);
    control_data.pwm_right = control_update_pwm(&pid_right,
                                                control_data.target_right,
                                                control_data.actual_right,
                                                control_data.pwm_right);

    control_drive_motor(1, control_data.target_left, control_data.pwm_left);
    control_drive_motor(2, control_data.target_right, control_data.pwm_right);
}

void Control_Stop(void)
{
    control_data.enabled = 0;
    control_data.sine_enabled = 0;
    control_data.target_left = 0;
    control_data.target_right = 0;
    control_data.actual_left = 0;
    control_data.actual_right = 0;
    control_data.pwm_left = 0;
    control_data.pwm_right = 0;
    speed_pid_reset(&pid_left);
    speed_pid_reset(&pid_right);
    motor_set(1, MOTOR_STOP, 0);
    motor_set(2, MOTOR_STOP, 0);
}

void Control_SetSpeedTarget(int16_t left_pulse_per_period, int16_t right_pulse_per_period)
{
    update_encoder_values();
    last_count_left = motor1_count;
    last_count_right = motor2_count;

    control_data.target_left = left_pulse_per_period;
    control_data.target_right = right_pulse_per_period;
    control_data.actual_left = 0;
    control_data.actual_right = 0;
    control_data.pwm_left = 0;
    control_data.pwm_right = 0;
    control_data.sine_enabled = 0;
    speed_pid_reset(&pid_left);
    speed_pid_reset(&pid_right);

    control_data.enabled = ((left_pulse_per_period != 0) || (right_pulse_per_period != 0)) ? 1U : 0U;
    if(control_data.enabled == 0)
    {
        Control_Stop();
    }
}

void Control_SetLeftTarget(int16_t left_pulse_per_period)
{
    Control_SetSpeedTarget(left_pulse_per_period, control_data.target_right);
}

void Control_SetRightTarget(int16_t right_pulse_per_period)
{
    Control_SetSpeedTarget(control_data.target_left, right_pulse_per_period);
}

void Control_SetSineTarget(int16_t base_pulse, int16_t amplitude_pulse, uint16_t period_ms)
{
    if(period_ms < CONTROL_PERIOD_MS)
    {
        period_ms = CONTROL_PERIOD_MS;
    }

    sine_base = base_pulse;
    sine_amplitude = amplitude_pulse;
    if(sine_amplitude < 0)
    {
        sine_amplitude = (int16_t)-sine_amplitude;
    }

    sine_period_ticks = (uint16_t)(period_ms / CONTROL_PERIOD_MS);
    if(sine_period_ticks == 0)
    {
        sine_period_ticks = 1;
    }

    sine_tick = 0;
    control_data.enabled = 1;
    control_data.sine_enabled = 1;
    control_data.actual_left = 0;
    control_data.actual_right = 0;
    control_data.pwm_left = 0;
    control_data.pwm_right = 0;
    update_encoder_values();
    last_count_left = motor1_count;
    last_count_right = motor2_count;
    speed_pid_reset(&pid_left);
    speed_pid_reset(&pid_right);
}

void Control_DisableSineTarget(void)
{
    control_data.sine_enabled = 0;
}

void Control_SetPid(float kp, float ki, float kd)
{
    if(kp >= 0.0f)
    {
        pid_param.kp = kp;
    }

    if(ki >= 0.0f)
    {
        pid_param.ki = ki;
    }

    if(kd >= 0.0f)
    {
        pid_param.kd = kd;
    }

    speed_pid_init(&pid_left, pid_param.kp, pid_param.ki, pid_param.kd, CONTROL_PID_OUTPUT_LIMIT);
    speed_pid_init(&pid_right, pid_param.kp, pid_param.ki, pid_param.kd, CONTROL_PID_OUTPUT_LIMIT);
    last_count_left = motor1_count;
    last_count_right = motor2_count;
}

void Control_GetPid(ControlPidParam *pid)
{
    if(pid != 0)
    {
        *pid = pid_param;
    }
}

void Control_GetData(ControlData *data)
{
    if(data != 0)
    {
        *data = control_data;
    }
}

uint8_t Control_IsEnabled(void)
{
    return control_data.enabled;
}
