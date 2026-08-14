/*********************************************************************************************************************
* @file            car_init.h
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.19, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef __CAR_INIT_H
#define __CAR_INIT_H

#include "headfile.h"

// TOF400C GPIO1/INT is routed to PA12 on the 2026 smart-car adapter board.
// The VL53L1X vendor configuration drives this signal active-low.
#define TOF400C_INT_PORT          GPIOA
#define TOF400C_INT_PIN           GPIO_PIN_12
#define TOF400C_INT_EXTI_LINE     EXTI_12
#define TOF400C_INT_IRQn          EXTI10_15_IRQn
#define TOF400C_INT_PRIORITY      2U


//函数声明
/* 智能车初始化 */
void car_init(void);
/* 中断初始化 */
void int_init(void);


//全局变量声明
extern volatile uint8_t tof_data_ready;

#endif
