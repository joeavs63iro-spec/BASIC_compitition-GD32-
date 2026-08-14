/*********************************************************************************************************************
* @file            hf_pwm.c
* @author
* @Target core     GD32F303RCT6
* @revisions       2022.09.06, V1.0
* @modify          none
********************************************************************************************************************/

#include "hf_pwm.h"

uint16_t period[6]={0,};

static uint32_t pwm_duty_to_pulse(uint16_t period_temp, uint16_t duty)
{
    if(period_temp == 0)
    {
        return 0;
    }

    if(duty >= 10000)
    {
        /* ARR is Period - 1, so keep full duty inside the valid compare range. */
        return period_temp - 2;
    }

    return ((uint32_t)duty * period_temp) / 10000U;
}

//-------------------------------------------------------------------------------------------------------------------
//@brief        PWM 初始化(引脚对应通道见PWM 引脚初始化部分)
//@param        timer_periph:            TIMERx(x=0,1,2,3,4,7)
//@param        timer_ch:                TIMER_CH_x(x=0..13)
//@param        freq_div                 预分频值(0~65535)
//@param        period_temp              自动重装载值(0~65535)
//@param        duty				     占空比（0-10000）
//@return       void
//Sample usage:        pwm_init(TIMER2, TIMER_CH_0, 120, 10000, 2000);
                       //定时器TIMER2_CH_0输出PWM信号,频率为100Hz(120MHz/120/10000),占空比为20％(2000/10000)
                       //频率计算公式：PWM信号频率 = 时钟频率（72MHz） / 预分频值 / 自动重装载值
//-------------------------------------------------------------------------------------------------------------------
void pwm_init(uint32_t timer_periph, uint16_t timer_ch, uint16_t freq_div, uint16_t period_temp, uint16_t duty)
{
	   /*开启复用功能时钟*/
    rcu_periph_clock_enable(RCU_AF);

    /* 使能定时器时钟,记录自动重装载值 */
    if(timer_periph == TIMER0)
    {
        timer_primary_output_config(TIMER0, ENABLE);
        rcu_periph_clock_enable(RCU_TIMER0);
        period[0] = period_temp;
    }
    else if(timer_periph == TIMER1)
    {
        rcu_periph_clock_enable(RCU_TIMER1);
        period[1] = period_temp;
    }
    else if(timer_periph == TIMER2)
    {
        rcu_periph_clock_enable(RCU_TIMER2);
        period[2] = period_temp;
    }
    else if(timer_periph == TIMER3)
    {
        rcu_periph_clock_enable(RCU_TIMER3);
        period[3] = period_temp;
    }
    else if(timer_periph == TIMER4)
    {
        rcu_periph_clock_enable(RCU_TIMER4);
        period[4] = period_temp;
    }
    else if(timer_periph == TIMER7)
    {
        timer_primary_output_config(TIMER7, ENABLE);
        rcu_periph_clock_enable(RCU_TIMER7);
        period[5] = period_temp;
    }

    /* TIMER结构体配置 */
    timer_parameter_struct timer_initpara;
    /* 写入预分频值 */
    timer_initpara.prescaler         = freq_div-1;
    /* 对齐模式为边缘对齐 */
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    /* 计数方式为向上计数 */
    timer_initpara.counterdirection  = TIMER_COUNTER_DOWN;
    /* 写入自动重装载值 */
    timer_initpara.period            = period_temp-1;
    /* 选择时钟分频因子 */
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    /* 写入重复计数值 */
    timer_initpara.repetitioncounter = 0;
    /* 初始化外设定时器 */
    timer_init(timer_periph,&timer_initpara);

    /* TIMER输出比较模式结构体配置 */
    timer_oc_parameter_struct timer_ocintpara;
    /* 通道输出极性配置为高 */
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    /* 通道输出使能 */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    /* 互补通道输出极性配置为高 */
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    /* 互补通道输出禁能 */
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    /* 空闲状态下通道输出为低 */
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    /* 空闲状态下互补通道输出为低 */
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

	   /* 外设定时器的通道输出配置 */
    timer_channel_output_config(timer_periph,timer_ch,&timer_ocintpara);
    /* 配置外设定时器的通道输出比较值 */
    timer_channel_output_pulse_value_config(timer_periph,timer_ch, pwm_duty_to_pulse(period_temp, duty));
    /* 配置外设定时器通道输出比较模式（PWM0/PWM1) */
    timer_channel_output_mode_config(timer_periph,timer_ch,TIMER_OC_MODE_PWM0);

    /* 定时器通道输出影子寄存器禁能 */
    timer_channel_output_shadow_config(timer_periph,timer_ch,TIMER_OC_SHADOW_DISABLE);
    /* 定时器通道输出使能 */
    timer_primary_output_config(timer_periph,ENABLE);
    /* 定时器自动重载影子使能 */
    timer_auto_reload_shadow_enable(timer_periph);
    /* 定时器使能 */
    timer_enable(timer_periph);
}

//-------------------------------------------------------------------------------------------------------------------
//@brief        PWM 频率设置
//@param        timer_periph:            TIMERx(x=0,1,2,3,4,7)
//@param        freq_div                 预分频值(0~65535)
//@param        period_temp              自动重装载值(0~65535)
//@return       void
//Sample usage:        pwm_freq(TIMER2,72, 10000);
                       //设置TIMER2的通道输出的PWM信号频率为100Hz(72MHz/72/10000)
                       //频率计算公式：PWM信号频率 = 时钟频率（72MHz） / 预分频值 / 自动重装载值
//-------------------------------------------------------------------------------------------------------------------
void pwm_freq(uint32_t timer_periph, uint16_t freq_div, uint16_t period_temp)
{
    timer_prescaler_config (timer_periph, freq_div - 1, TIMER_PSC_RELOAD_NOW);
    timer_autoreload_value_config (timer_periph, period_temp - 1);

    if(timer_periph == TIMER0)
    {
        period[0] = period_temp;
    }
    else if(timer_periph == TIMER1)
    {
        period[1] = period_temp;
    }
    else if(timer_periph == TIMER2)
    {
        period[2] = period_temp;
    }
    else if(timer_periph == TIMER3)
    {
        period[3] = period_temp;
    }
    else if(timer_periph == TIMER4)
    {
        period[4] = period_temp;
    }
    else if(timer_periph == TIMER7)
    {
        period[5] = period_temp;
    }
}

//-------------------------------------------------------------------------------------------------------------------
//@brief        PWM 占空比设置
//@param        timer_periph:            TIMERx(x=0,1,2,3,4,7)
//@param        timer_ch:                TIMER_CH_x(x=0..13)
//@param        duty				     占空比（0-10000）
//@return		void
//Sample usage:        pwm_duty(TIMER2, TIMER_CH_1, 2000);
                       //设置TIMER2通道1PWM输出占空比为20％(2000/10000)
//-------------------------------------------------------------------------------------------------------------------
void pwm_duty(uint32_t timer_periph, uint32_t timer_ch, uint16_t duty)
{
    uint16_t Period;

    if(timer_periph == TIMER0)
    {
        Period = period[0];
    }
    else if(timer_periph == TIMER1)
    {
        Period = period[1];
    }
    else if(timer_periph == TIMER2)
    {
        Period = period[2];
    }
    else if(timer_periph == TIMER3)
    {
        Period = period[3];
    }
    else if(timer_periph == TIMER4)
    {
        Period = period[4];
    }
    else if(timer_periph == TIMER7)
    {
        Period = period[5];
    }

    timer_channel_output_pulse_value_config(timer_periph, timer_ch, pwm_duty_to_pulse(Period, duty));
}

//-------------------------------------------------------------------------------------------------------------------
//@brief        PWM 停止
//@param        timer_periph:            TIMERx(x=0,1,2,3,4,7)
//@return		void
//Sample usage:        pwm_stop(TIMER2);
                       //停止TIMER2所有通道的PWM输出
//-------------------------------------------------------------------------------------------------------------------
void pwm_stop(uint32_t timer_periph)
{
    /* 定时器失能能 */
    timer_disable(timer_periph);
}
