/*********************************************************************************************************************
* @file            car_reed.h
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.22, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef __CAR_REED_H
#define __CAR_REED_H

#include "headfile.h"

#define REED_PORT    GPIOA
#define REED_PIN     GPIO_PIN_5
#define REED_MODE    IN_PULLUP


//函数声明
/* 智能车干簧管初始化 */
void reed_init(void);
/* 智能车磁标检测 */
uint8_t reed_check(void);


//全局变量声明

#endif
