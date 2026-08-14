/*********************************************************************************************************************
* @file            car_init.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.19, V1.0
* @modify          none
********************************************************************************************************************/

#include "car_init.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief        智能车初始化
// @param        void    
// @return       void
// Sample usage:        car_init(); 
                        //智能车初始化
//-------------------------------------------------------------------------------------------------------------------
void car_init(void)
{
    systick_config();							  //时钟初始化
	
    uart0_init(UART0);							//串行口初始化
	
    adc_channel_init(ADC0, ADC_CH_10);    //ADC初始化
    adc_channel_init(ADC0, ADC_CH_11);
    adc_channel_init(ADC0, ADC_CH_12);
    adc_channel_init(ADC0, ADC_CH_13);
    adc_channel_init(ADC0, ADC_CH_15);
	
    key_init(K1);								//6按键初始化
    key_init(K2);
    key_init(K3);
    key_init(K4);
    key_init(K5);
    key_init(K6);
    
    reed_init();								//干簧管初始化
	
    motor_init(left);							//左电机初始化
    motor_init(right);							//右电机初始化
	  encoder_timer_config();        // 编码器初始化
		Motor_PID_Init();
		
    ws2812b_init();								//RGB灯初始化
    car_both_rgb_off();
	
    ultra_init();								//超声波初始化
		Init_TOF400C();            //tof初始化
    oled_init();               //OLED初始化,可根据用户需要自行开启
		oled_display_on();
		
    int_init();									//中断初始化
				
    board_led_init(LED1);					//初始化LED1端口PB2
    board_led_init(LED2);					//初始化LED1端口PB11
}

//其它的初始化均可由用户自行在下方编写，并添加在car_init()函数中
//例如：中断初始化等
//-------------------------------------------------------------------------------------------------------------------
// @brief        中断初始化
// @param        void    
// @return       void
// Sample usage:        int_init();//中断初始化
//-------------------------------------------------------------------------------------------------------------------
void int_init(void)
{
    ;
}

