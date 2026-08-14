/*
 * adc.h
 *
 *  Created on: 2025?12?28?
 *      Author: A0728
 */

#ifndef CODE_CONTROL_H_
#define CODE_CONTROL_H_
#include "headfile.h"
typedef struct
{
    float P;
    float I;
    float D;
    int error;
    int last_error;
    int last_last_error;
    int16_t add_pwm;
    int16_t output_pwm;
} PID_struct;


typedef struct
{
    float P;
    float I;
    float D;
    int error;
    int last_error;
    int last_last_error;
    int16_t add_pwm;
    int16_t output_pwm;
} Speed_struct;



extern volatile uint16_t adc_value[5];
extern volatile int base_pwm;
extern uint32_t used_speed_duty;
extern uint32_t target_speed_duty;

extern volatile Speed_struct speed_PID_L;
extern volatile Speed_struct speed_PID_R;

void adc_getdata(void);
int cal_err(void);
void track_para_init(void);
void speed_pid_init(void);
void incremental_PID_track(int real_error,int target_error,PID_struct*PID);
void positional_PD_track(int real_error, int target_error, volatile PID_struct *PID);


#endif /* CODE_CONTROL_H_ */
