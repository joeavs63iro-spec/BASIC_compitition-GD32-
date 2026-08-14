/*********************************************************************************************************************
* @file            hf_qtimer.c
* @author         
* @Target core     GD32F303RCT6
* @revisions       2022.09.08, V1.0
* @modify          none
********************************************************************************************************************/

#include "hf_qtimer.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief        编码器初始化
// @param        Encodernum            选择的编码器：Encoder1、2，Encoder_ALL
// @param        mode                  选择的编码器编码模式：Mode1(带方向)、2(正交)
// @return       void
// Sample usage:        Encoder_Init(Encoder_ALL, Mode1);
//                      //初始化编码器1和编码器2，编码模式为带方向
//-------------------------------------------------------------------------------------------------------------------
void timer_quad_init(encoder_typedef_enum Encodernum)
{
    /*开启复用功能时钟*/
    rcu_periph_clock_enable(RCU_AF);
	
    /* 定义定时器结构体 */
    timer_parameter_struct timer_initpara;
	
    rcu_periph_clock_enable(RCU_GPIOA);

    /* 配置A,B引脚及复用功能，作为TIMER通道 */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);

    /* 配置定时器时钟 */
    rcu_periph_clock_enable(RCU_TIMER2);

    /* 初始化TIMER2 */
    timer_deinit(TIMER2);

    /* 配置定时器结构体并配置定时器 */
    timer_initpara.prescaler         = 9;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 5000;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2,&timer_initpara);

    /* 使能定时器重装载 */
    timer_auto_reload_shadow_enable(TIMER2);

    /* 配置定时器正交解码器 */
    timer_quadrature_decoder_mode_config(TIMER2,TIMER_ENCODER_MODE2,TIMER_IC_POLARITY_RISING,TIMER_IC_POLARITY_RISING);

    /* 使能定时器 */
    timer_enable(TIMER2);


    /* 配置A,B引脚及复用功能，作为TIMER通道 */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);

    /* 配置定时器时钟 */
    rcu_periph_clock_enable(RCU_TIMER0);

    /* 初始化TIMER */
    timer_deinit(TIMER0);

    /* 配置定时器结构体并配置定时器 */
    timer_initpara.prescaler         = 9;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 5000;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0,&timer_initpara);

    /* 使能定时器重装载 */
    timer_auto_reload_shadow_enable(TIMER0);

    /* 配置定时器正交解码器 */
    timer_quadrature_decoder_mode_config(TIMER0,TIMER_ENCODER_MODE2,TIMER_IC_POLARITY_RISING,TIMER_IC_POLARITY_RISING);

    /* 使能定时器 */
    timer_enable(TIMER0);

}

//-------------------------------------------------------------------------------------------------------------------
// @brief        获取编码器值
// @param        Encodernum            选择的编码器：Encoder1、2，Encoder_ALL
// @return       int16
// Sample usage:        int16 value;value = Encoder_Read(Encoder1);
//                      //读取编码器1的值，存入value
//-------------------------------------------------------------------------------------------------------------------
int16_t timer_quad_get(encoder_typedef_enum Encodernum)
{
    int16_t value, value1, value2;

    value1 = 0;
    value2 = 0;
    if(Encodernum == Encoder1)
    {
        value1 = timer_counter_read(TIMER0)-10000;
        timer_counter_value_config(TIMER0,10000);
        value = value1;
    }
    else if(Encodernum == Encoder2)
    {
        value2 = timer_counter_read(TIMER2)-10000;
        timer_counter_value_config(TIMER2,10000);
        value = value2;
    }
    return value;
}
