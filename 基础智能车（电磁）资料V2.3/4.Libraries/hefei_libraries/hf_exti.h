/*********************************************************************************************************************
* @file            hf_exti.h
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.08, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef _HF_EXTI_H
#define _HF_EXTI_H

#include "headfile.h"

/* 变量声明 */

typedef enum 
{
    CHANGE = 0,
    RISING = 1,
    FALLING = 2
} exti_Mode_enum;


/* 函数声明 */

/* 开启引脚外部中断 */
void exti_enable(uint32_t gpio_periph, uint32_t exti_pin, exti_Mode_enum Mode);
/* 关闭引脚外部中断 */
void exti_disable(uint32_t exti_pin);

#endif
