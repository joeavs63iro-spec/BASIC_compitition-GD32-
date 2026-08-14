/*********************************************************************************************************************
* @file            hf_gpio.h
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.02, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef _HF_GPIO_H
#define _HF_GPIO_H

#include "headfile.h"

/* 枚举声明 */

typedef enum    //枚举GPIO配置
{
    IN_AIN = 0,            //模拟输入模式
    IN_FLOATING = 1,       //浮空输入模式
    IN_PULLDOWN = 2,       //下拉输入
    IN_PULLUP = 3,         //上拉输入

    OUT_PP = 4,            //通用推挽输出模式
    OUT_OD = 5,            //通用开漏输出模式
    OUT_AF_PP = 6,         //复用功能推挽输出模式
    OUT_AF_OD = 7,         //复用功能开漏输出模式
   
}GPIO_mode_enum;


typedef enum    //枚举GPIO引脚重映射
{
    SPI0_REMAP = 1,                     //SPIO重映射
    I2C0_REMAP = 2,                     //I2C0重映射
    USART0_REMAP = 3,                   //USART0重映射
    USART1_REMAP = 4,                   //USART1重映射
    USART2_PARTIAL_REMAP = 5,           //USART2部分重映射
    USART2_FULL_REMAP = 6,              //USART2完全重映射
    TIMER0_PARTIAL_REMAP = 7,           //TIMER0部分重映射
    TIMER0_FULL_REMAP = 8,              //TIMER0完全重映射
    TIMER1_PARTIAL_REMAP0 = 9,          //TIMER1部分重映射
    TIMER1_PARTIAL_REMAP1 = 10,         //TIMER1部分重映射
    TIMER1_FULL_REMAP = 11,             //TIMER1完全重映射
    TIMER2_PARTIAL_REMAP = 12,          //TIMER2部分重映射
    TIMER2_FULL_REMAP = 13,             //TIMER2完全重映射
    TIMER3_REMAP = 14,                  //TIMER3重映射
    CAN_PARTIAL_REMAP = 15,             //CAN部分重映射
    CAN_FULL_REMAP = 16,                //CAN完全重映射
    PD01_REMAP = 17,                    //PD01重映射
    TIMER4CH3_IREMAP = 18,              //TIMER4内部通道3重映射
    ADC0_ETRGINS_REMAP = 19,            //ADC0注入转换外部触发重映射
    ADC0_ETRGREG_REMAP = 20,            //ADC0规则转换外部触发重映射
    ADC1_ETRGINS_REMAP = 21,            //ADC1注入转换外部触发重映射
    ADC1_ETRGREG_REMAP = 22,            //ADC1规则转换外部触发重映射
    SWJ_NONJTRST_REMAP = 23,            //全部的SWJ(JTAG-DP + SW-DP),但是不包括NJTRST
    SWJ_SWDPENABLE_REMAP = 24,          //JTAG-DP禁止,SW-DP使能
    SWJ_DISABLE_REMAP = 25,             //JTAG-DP禁止,SW-DP禁止
    SPI2_REMAP = 26,                    //SPI2重映射
    TIMER8_REMAP = 27,                  //TIMER8重映射
    TIMER9_REMAP = 28,                  //TIMER9重映射
    TIMER10_REMAP = 29,                 //TIMER10重映射
    TIMER12_REMAP = 30,                 //TIMER12重映射
    TIMER13_REMAP = 31,                 //TIMER13重映射
    EXMC_NADV_REMAP = 32,               //EXMC_NADV连接或断开
    CTC_REMAP0 = 33,                    //CTC重映射（PD15）
    CTC_REMAP1 = 34,                    //CTC重映射（PF0）
		
}GPIO_remap_enum;


/* 函数声明 */

/* GPIO初始化 */
void gpio_pin_init(uint32_t gpio_periph, GPIO_mode_enum mode, uint32_t gpio_pin);
/* GPIO引脚功能重映射 */
void gpio_pin_remap(uint32_t remap);
/* GPIO引脚输出设置 */
void gpio_bit_output(uint32_t gpio_periph, uint32_t gpio_pin, uint8_t dat);
/* GPIO引脚输出翻转 */
void gpio_toggle(uint32_t gpio_periph, uint32_t gpio_pin);
/* GPIO获取引脚输入 */
uint8_t gpio_get_input(uint32_t gpio_periph,uint32_t gpio_pin);
/* GPIO获取引脚输出 */
uint8_t gpio_get_output(uint32_t gpio_periph,uint32_t gpio_pin);

#endif
