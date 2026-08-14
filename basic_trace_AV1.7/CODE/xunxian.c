/*
 * adc.c
 *
 *  Created on: 2025?12?28?
 *      Author: A0728
 */

#include "xunxian.h"
volatile PID_struct track_PID;
volatile Speed_struct speed_PID_L;
volatile Speed_struct speed_PID_R;

volatile uint16_t adc_value[5] = { 0 };
void adc_getdata(void)
{
        adc_value[0] = adc_get(ADC0, ADC_CH_10);             
        adc_value[1] = adc_get(ADC0, ADC_CH_11);
        adc_value[2] = adc_get(ADC0, ADC_CH_12);
        adc_value[3] = adc_get(ADC0, ADC_CH_13);
        adc_value[4] = adc_get(ADC0, ADC_CH_15); 
}

int cal_err(void)
{
    if(adc_value[2]*2>4095)
    {
        adc_value[2]=4095;
    }
    int sum = adc_value[0] + adc_value[1] + 2*adc_value[2] + adc_value[4] + adc_value[3];
    if (sum == 0)
    {
        return 0;
    }
    return (int) ((adc_value[0] + adc_value[1] - adc_value[4] - adc_value[3]) * 100 / sum);
}

void track_para_init(void)
{
    track_PID.P = 60.0f;
    track_PID.I = 0.0f;
    track_PID.D = 0.0f;
    track_PID.error = 0;
    track_PID.last_error = 0;
    track_PID.last_last_error = 0;
    track_PID.add_pwm = 0;
    track_PID.output_pwm = 0;
    base_pwm = 6000;
}


void incremental_PID_track(int real_error, int target_error, PID_struct *PID)
{
    PID->error = target_error - real_error;
    PID->add_pwm = (int16_t)((int) (PID->P * (float) (PID->error - PID->last_error)) + PID->I * (float) PID->error
            + PID->D * (float) (PID->error + PID->last_last_error - 2 * PID->last_error));
    PID->output_pwm += PID->add_pwm;
    if (PID->output_pwm >= 10000)
    {
        PID->output_pwm = 10000;
    }
    if (PID->output_pwm <= -10000)
    {
        PID->output_pwm = -10000;
    }
    PID->last_last_error = PID->last_error;
    PID->last_error = PID->error;
}

void positional_PD_track(int real_error, int target_error, volatile PID_struct *PID)
{
    PID->error = target_error - real_error;

    PID->output_pwm = (int16_t)((int)(PID->P * (float)PID->error) +
                      (int)(PID->D * (float)(PID->error - PID->last_error)));

    PID->last_error = PID->error;

    if(PID->output_pwm > 10000)  PID->output_pwm = 10000;
    if(PID->output_pwm < -10000) PID->output_pwm = -10000;
}

