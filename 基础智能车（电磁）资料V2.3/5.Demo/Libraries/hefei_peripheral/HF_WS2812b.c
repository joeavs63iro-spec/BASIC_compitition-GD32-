/*********************************************************************************************************************
* @file            HF_WS2812b.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.13, V1.0
* @modify          none
********************************************************************************************************************/

#include "HF_WS2812b.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief        RGB初始化
// @param        void
// @return       void
// Sample usage:        ws2812b_init(); 
                        //RGB灯初始化
//-------------------------------------------------------------------------------------------------------------------
void ws2812b_init()
{   
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_TIMER0);
    gpio_pin_init(GPIOA, OUT_AF_PP,GPIO_PIN_8);
    pwm_init(TIMER0, TIMER_CH_0, 15, 20, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        RGB写入
// @param        GRB             写入的数组
// @return       void
// Sample usage:        uint8_t RGB[3] = {0xff,0x00,0x00};	ws2812b_write(RGB); 
                        //向RGB写入数组，显示红色
//-------------------------------------------------------------------------------------------------------------------
void ws2812b_write(unsigned char* rgb_value)	
{
    unsigned char grb_value[3];
    unsigned int j, i;
	
    grb_value[0] = rgb_value[1];
    grb_value[1] = rgb_value[0];
    grb_value[2] = rgb_value[2];

    for(j=0; j<3; j++)
    {
        for(i=8; i>0; i--)
        {
            if((*(grb_value+j) >> (i-1)) & 0x01)
            {
                //设置阈值(比较值,调节占空比的时候调节这个值)
                pwm_duty(TIMER0, TIMER_CH_0, 3000);/*发1*/
            }
            else
            {
                pwm_duty(TIMER0, TIMER_CH_0, 1000);/*发0*/
            }
            timer_enable(TIMER0);
						
            while(!timer_flag_get(TIMER0, TIMER_FLAG_UP))
            {
                ;
            }
            timer_disable(TIMER0);
            timer_flag_clear(TIMER0, TIMER_FLAG_UP);
        }
    }
}
