/*********************************************************************************************************************
* @file            car_buzzer.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.22, V1.0
* @modify          none
********************************************************************************************************************/

#include "car_buzzer.h"

static uint16_t buzzer_freq_to_div(unsigned int freq)
{
    uint32_t div_value;

    if(freq == 0)
    {
        return FREQ_DIV;
    }

    /*
     * Replace the old 3600/freq magic number with the timer formula:
     * PWM frequency = timer clock / prescaler / auto-reload.
     */
    div_value = BUZZER_TIMER_CLOCK_HZ / ((uint32_t)PERIOD_TEMP * (uint32_t)freq);
    if(div_value == 0)
    {
        div_value = 1;
    }
    else if(div_value > 0xFFFFU)
    {
        div_value = 0xFFFFU;
    }

    return (uint16_t)div_value;
}

//-------------------------------------------------------------------------------------------------------------------
//@brief        buzzer ???(???????PWM ???????)
//@param        TIMER_PORT:              TIMERx(x=0,2,13..16)
//@param        TIMER_CH:                TIMER_CH_x(x=0..15)
//@param        FREQ_DIV                 ????(0~65535)
//@param        PERIOD_TEMP              ??????(0~65535)
//@param        DUTY				     ???(0-10000)
//@return       void
//Sample usage:        pwm_timer_init(TIMER2, TIMER_CH_0, 72, 10000, 2000); 
//-------------------------------------------------------------------------------------------------------------------
void buzzer_init(void)
{
    gpio_pin_init(GPIO_PORT, GPIO_MODE, GPIO_PIN);
    pwm_init(TIMER_PORT, TIMER_CH, FREQ_DIV, PERIOD_TEMP, 0);
}

//-------------------------------------------------------------------------------------------------------------------
//@brief        buzzer ????
//@param        freq              ??(20Hz-3600Hz)
//@return       
//Sample usage:        buzzer_freq(400);    //??buzzer???400Hz
                       //??????:buzzer?? = PWM???? = ????(72MHz) / ???? / ??????
//-------------------------------------------------------------------------------------------------------------------
void buzzer_freq(unsigned int freq)
{
    pwm_freq(TIMER_PORT, buzzer_freq_to_div(freq), PERIOD_TEMP);
}

//-------------------------------------------------------------------------------------------------------------------
//@brief        PWM ?????
//@param        duty				     ???(0-100)
//@return		
//Sample usage:        buzzer_duty(50);//??buzzer??????50%
//-------------------------------------------------------------------------------------------------------------------
void buzzer_duty(unsigned char duty)
{
    pwm_duty(TIMER_PORT, TIMER_CH, (duty*100));
}
