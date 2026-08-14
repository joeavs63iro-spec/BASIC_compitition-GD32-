/*********************************************************************************************************************
* @file            hf_systick.h
* @author          
* @Target core     GD32E230C8T6
* @revisions       2022.09.07, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef _HF_SYSTICK_H
#define _HF_SYSTICK_H

#include "headfile.h"

/* 函数定义 */

/* 配置时钟 */
void systick_config(void);
/* 微秒级延时 */
void delay_1us(uint32_t count);
/* 毫秒级延时 */
void delay_1ms(uint32_t count);
uint64_t get_current_time_us(void);
void SysTick_Handler(void);

#endif

