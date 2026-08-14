/*********************************************************************************************************************
* @file            control.h
* @brief           Position PD track control parameters and APIs
* @Target core     GD32F303RCT6
********************************************************************************************************************/

#ifndef __CONTROL_H
#define __CONTROL_H

#include <stdint.h>

#define TRACK_CONTROL_PERIOD_MS  10U

typedef struct
{
    float kp;
    float kd;
    float output_scale;
    float output_limit;
    float lost_threshold;
    float deadband;
    float position;
    float error;
    float last_error;
    float output;
    uint16_t base_speed;
    uint16_t min_speed;
    uint16_t max_speed;
    uint16_t left_pwm;
    uint16_t right_pwm;
    uint8_t lost_flag;
    uint8_t run_enable;
} TrackControl;

extern volatile TrackControl g_track_control;

void track_init(void);
void track_timer_init(void);
void track_set_run(uint8_t enabled);
uint8_t track_get_run(void);
void Track_SetPD(float kp, float kd);
void track_pid_reset(void);
void track_sanitize_params(void);
void positional_PD_track(void);
void track_motion_output(void);
void track_control_isr(void);

#endif
