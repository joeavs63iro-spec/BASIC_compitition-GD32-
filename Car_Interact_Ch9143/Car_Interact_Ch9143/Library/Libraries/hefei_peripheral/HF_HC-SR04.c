/*********************************************************************************************************************
* @file            HF_HC-SR04.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.13, V1.0
* @modify          none
********************************************************************************************************************/

#include "HF_HC-SR04.h"

#define max     5000U
#define prescal 120U
#define speed   334.52
#define MAX_TIMEOUT_US 30000
//unsigned int  g_overflow_times;
//unsigned int  g_current_time_value;
//unsigned char g_flag_unhandler;

//-------------------------------------------------------------------------------------------------------------------
// @brief        超声波初始化
// @param        void
// @return       void
// Sample usage:        ultrasonic_init(); 
                        //初始化超声波功能
//-------------------------------------------------------------------------------------------------------------------
void ultra_init(void)
{
    /* 使能CFGCMP时钟 */
    rcu_periph_clock_enable(RCU_AF);

    /* 配置PC11为超声波trig引脚 */
    gpio_pin_init(TRIG_PORT, TRIG_MODE, TRIG_PIN);
    gpio_bit_output(TRIG_PORT, TRIG_PIN, 0);

    /* 配置PC10为超声波ehco引脚 */
    gpio_pin_init(ECHO_PORT, ECHO_MODE, ECHO_PIN);
	
}

uint32_t ultra_get_distance(void)
{
    uint32_t distance = 0;
    uint32_t echo_time = 0;
    uint32_t end_time;
    // 发送触发脉冲
	gpio_bit_reset(TRIG_PORT, TRIG_PIN);
	delay_1us(10);
    gpio_bit_set(TRIG_PORT, TRIG_PIN);
    delay_1us(20);  // 保持10-20us高电平
    gpio_bit_reset(TRIG_PORT, TRIG_PIN);
    // （开始计时）等待TRIG引脚变为低电平
    // 开始计时
    uint32_t start_time = get_current_time_us();
	  uint32_t  timeout = MAX_TIMEOUT_US;
    while (gpio_input_bit_get(ECHO_PORT, ECHO_PIN) == 0) {
        if (--timeout == 0) {
            return 44444;  // 超时，无回波（超量程）
        }
    }
    // （结束计时）等待ECHO引脚变为高电平
		  while (gpio_input_bit_get(ECHO_PORT, ECHO_PIN) == 1) {
        if (--timeout == 0) {
            return 44444;  // 超时，无回波（超量程）
        }
    }
			end_time = get_current_time_us();// 结束计时
    if (end_time > start_time) {
        echo_time = end_time - start_time;// 计算高电平持续时间（微秒）
    } else {
        // 处理时间溢出情况
        echo_time = (0xFFFFFFFF - start_time) + end_time;
    }
    
    // 计算距离：距离(cm) = 时间(us) / 58
    distance = echo_time / 58;
//    if(prescal>330&&distance<max)
//		if(distance<max)
//    {distance = distance;}
//		else{distance = 44444;}  //错误代码号44444
		return distance;
}

