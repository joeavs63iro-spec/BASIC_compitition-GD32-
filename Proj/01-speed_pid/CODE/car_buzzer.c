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
//@brief        buzzer 初始化(引脚对应通道见PWM 引脚初始化部分)
//@param        TIMER_PORT:              TIMERx(x=0,2,13..16)
//@param        TIMER_CH:                TIMER_CH_x(x=0..15)
//@param        FREQ_DIV                 预分频值(0~65535)
//@param        PERIOD_TEMP              自动重装载值(0~65535)
//@param        DUTY				     占空比（0-10000）
//@return       void
//Sample usage:        pwm_timer_init(TIMER2, TIMER_CH_0, 72, 10000, 2000); 
//-------------------------------------------------------------------------------------------------------------------
void buzzer_init(void)
{
    gpio_pin_init(GPIO_PORT, GPIO_MODE, GPIO_PIN);
    pwm_init(TIMER_PORT, TIMER_CH, FREQ_DIV, PERIOD_TEMP, 0);
}

//-------------------------------------------------------------------------------------------------------------------
//@brief        buzzer 频率设置
//@param        freq              频率（20Hz-3600Hz）
//@return       
//Sample usage:        buzzer_freq(400);    //设置buzzer频率为400Hz
                       //频率计算公式：buzzer频率 = PWM信号频率 = 时钟频率（72MHz） / 预分频值 / 自动重装载值
//-------------------------------------------------------------------------------------------------------------------
void buzzer_freq(unsigned int freq)
{
    pwm_freq(TIMER_PORT, buzzer_freq_to_div(freq), PERIOD_TEMP);
}

//-------------------------------------------------------------------------------------------------------------------
//@brief        PWM 占空比设置
//@param        duty				     占空比（0-100）
//@return		
//Sample usage:        buzzer_duty(50);//设置buzzer输出占空比为50％
//-------------------------------------------------------------------------------------------------------------------
void buzzer_duty(unsigned char duty)
{
    pwm_duty(TIMER_PORT, TIMER_CH, (duty*100));
}
