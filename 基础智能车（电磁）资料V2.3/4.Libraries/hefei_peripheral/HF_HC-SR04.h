/*********************************************************************************************************************
* @file            HF_HC-SR04.h
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.13, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef _HF_HC_SR04_H
#define _HF_HC_SR04_H

#include "headfile.h"

/* 资源定义 */
#define TRIG_PORT    GPIOC
#define TRIG_PIN     GPIO_PIN_11
#define TRIG_MODE    OUT_PP

#define ECHO_PORT    GPIOC
#define ECHO_PIN     GPIO_PIN_10
#define ECHO_MODE    IN_PULLDOWN

#define EXTI_PORT    GPIO_PORT_SOURCE_GPIOC
#define EXTI_PIN     GPIO_PIN_SOURCE_10
#define EXTI_NUM     EXTI_10


/* 函数声明 */
/* 超声波初始化 */
void ultra_init(void);
/* 超声波发射一次并返回距离 */
unsigned int ultra_get_distance(void);

//全局变量声明

#endif
