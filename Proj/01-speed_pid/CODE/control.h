#ifndef __CONTROL_H
#define __CONTROL_H

#include <stdint.h>

#define CONTROL_PERIOD_MS              10U
#define CONTROL_MIN_PWM                300
#define CONTROL_DEFAULT_KP             45.0f
#define CONTROL_DEFAULT_KI             2.0f
#define CONTROL_DEFAULT_KD             8.0f
#define CONTROL_PID_OUTPUT_LIMIT       2500.0f

typedef struct
{
    float kp;
    float ki;
    float kd;
} ControlPidParam;

typedef struct
{
    int16_t target_left;
    int16_t target_right;
    int32_t actual_left;
    int32_t actual_right;
    int32_t pwm_left;
    int32_t pwm_right;
    uint8_t enabled;
    uint8_t sine_enabled;
} ControlData;

void Control_Init(void);
void Control_Task(void);
void Control_Stop(void);

void Control_SetSpeedTarget(int16_t left_pulse_per_period, int16_t right_pulse_per_period);
void Control_SetLeftTarget(int16_t left_pulse_per_period);
void Control_SetRightTarget(int16_t right_pulse_per_period);
void Control_SetSineTarget(int16_t base_pulse, int16_t amplitude_pulse, uint16_t period_ms);
void Control_DisableSineTarget(void);

void Control_SetPid(float kp, float ki, float kd);
void Control_GetPid(ControlPidParam *pid);
void Control_GetData(ControlData *data);
uint8_t Control_IsEnabled(void);

#endif
