/*********************************************************************************************************************
* @file            hf_pit.c
* @author         
* @Target core     GD32F303RCT6
* @revisions       2022.09.08, V1.0
* @modify          none
********************************************************************************************************************/

#include "hf_pit.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief        定时器中断初始化(ms)
// @param        timer_periph	选择中断使用的定时器（TIMER1,2,3,4,5,6）
// @param        timer_period	设置中断时间（ms）
// @return       void
// Sample usage:        timer_pit_interrupt_ms(TIMER2, 1000); 
//                      //开启TIMER2 1000ms定时器中断
//-------------------------------------------------------------------------------------------------------------------
void timer_pit_interrupt_ms(uint32_t timer_periph, uint16_t timer_period)
{
    /* TIMER结构体的配置 */
    timer_parameter_struct timer_initpara;
	
    if(timer_periph == TIMER1)
    {
        nvic_irq_enable(TIMER1_IRQn, 0, 0);
        rcu_periph_clock_enable(RCU_TIMER1);
    }
    else if(timer_periph == TIMER2)
    {
        nvic_irq_enable(TIMER2_IRQn, 0, 0);
        rcu_periph_clock_enable(RCU_TIMER2);
    }
    else if(timer_periph == TIMER3)
    {
        nvic_irq_enable(TIMER3_IRQn, 15, 0);
        rcu_periph_clock_enable(RCU_TIMER3);
    }
    else if(timer_periph == TIMER4)
    {
        nvic_irq_enable(TIMER4_IRQn, 0, 0);
        rcu_periph_clock_enable(RCU_TIMER4);
    }
    else if(timer_periph == TIMER5)
    {
        nvic_irq_enable(TIMER5_IRQn, 0, 0);
        rcu_periph_clock_enable(RCU_TIMER5);
    }
    else if(timer_periph == TIMER6)
    {
        nvic_irq_enable(TIMER6_IRQn, 0, 0);
        rcu_periph_clock_enable(RCU_TIMER6);
    }
    /* 复位外设TIMER */
    timer_deinit(timer_periph);
				
    timer_initpara.prescaler         = 120-1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
//    timer_initpara.period            = timer_period*10-1;
    timer_initpara.period            = timer_period * 1000 - 1;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(timer_periph, &timer_initpara);

    /* 清除外设TIMER的更新中断标志位 */
    timer_interrupt_flag_clear(timer_periph, TIMER_INT_FLAG_UP);
				
    /* 使能TIMER中断 */
    timer_interrupt_enable(timer_periph, TIMER_INT_UP);
				
    /* 使能中断更新并使能TIMER */
    timer_enable(timer_periph);	
}


//-------------------------------------------------------------------------------------------------------------------
// @brief        定时器中断初始化(us)
// @param        timer_periph	选择中断使用的定时器（TIMER1,2,3,4,5,6）
// @param        timer_period	设置中断时间（us）
// @return       void
// Sample usage:        timer_pit_interrupt_us(TIMER2, 1000);
//                      //开启TIMER2 1000us定时器中断
//-------------------------------------------------------------------------------------------------------------------
void timer_pit_interrupt_us(uint32_t timer_periph, uint16_t timer_period)
{
    /* TIMER结构体的配置 */
    timer_parameter_struct timer_initpara;
	
    if(timer_periph == TIMER1)
    {
        nvic_irq_enable(TIMER1_IRQn, 0, 0);
        rcu_periph_clock_enable(RCU_TIMER1);
    }
    else if(timer_periph == TIMER2)
    {
        nvic_irq_enable(TIMER2_IRQn, 0, 0);
        rcu_periph_clock_enable(RCU_TIMER2);
    }
    else if(timer_periph == TIMER3)
    {
        nvic_irq_enable(TIMER3_IRQn, 15, 0);
        rcu_periph_clock_enable(RCU_TIMER3);
    }
    else if(timer_periph == TIMER4)
    {
        nvic_irq_enable(TIMER4_IRQn, 9, 0);
        rcu_periph_clock_enable(RCU_TIMER4);
    }
    else if(timer_periph == TIMER5)
    {
        nvic_irq_enable(TIMER5_IRQn, 0, 0);
        rcu_periph_clock_enable(RCU_TIMER5);
    }
    else if(timer_periph == TIMER6)
    {
        nvic_irq_enable(TIMER6_IRQn, 0, 0);
        rcu_periph_clock_enable(RCU_TIMER6);
    }
    /* 复位外设TIMER */
    timer_deinit(timer_periph);
				
    timer_initpara.prescaler         = 9;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = timer_period*72-1;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(timer_periph, &timer_initpara);

    /* 清除外设TIMER的更新中断标志位 */
    timer_interrupt_flag_clear(timer_periph, TIMER_INT_FLAG_UP);
				
    /* 使能TIMER中断 */
    timer_interrupt_enable(timer_periph, TIMER_INT_UP);
				
    /* 使能中断更新并使能TIMER */
    timer_enable(timer_periph);	
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        关闭定时器中断
// @param        timer_periph	选择关闭的定时器中断（TIMER1,2,3,4,5,6）
// @return       void
// Sample usage:        timer_pit_interrupt_us(TIMER2);
//                      //关闭TIMER2定时器中断
//-------------------------------------------------------------------------------------------------------------------
void timer_pit_close(uint32_t timer_periph)
{
    timer_disable(timer_periph);	
}
