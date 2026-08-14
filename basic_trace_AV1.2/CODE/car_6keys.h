/*********************************************************************************************************************
* @file            car_6keys.h
* @author         
* @Target core     GD32F303RCT6
* @revisions       2022.09.17, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef __CAR_6KEY_H
#define __CAR_6KEY_H

#include "headfile.h"

// Key level and debounce configuration
#define KEYDOWN    0
#define KEYUP      1
#define KEY_DEBOUNCE_TICKS 2

#define K1         1
#define K2         2
#define K3         3
#define K4         4
#define K5         5
#define K6         6

#define K1_PORT    GPIOA
#define K1_PIN     GPIO_PIN_11
#define K1_MODE    IN_PULLUP

#define K2_PORT    GPIOA
#define K2_PIN     GPIO_PIN_4
#define K2_MODE    IN_PULLUP

#define K3_PORT    GPIOB
#define K3_PIN     GPIO_PIN_0
#define K3_MODE    IN_PULLUP

#define K4_PORT    GPIOB
#define K4_PIN     GPIO_PIN_1
#define K4_MODE    IN_PULLUP

#define K5_PORT    GPIOC
#define K5_PIN     GPIO_PIN_12
#define K5_MODE    IN_PULLUP

#define K6_PORT    GPIOD
#define K6_PIN     GPIO_PIN_2
#define K6_MODE    IN_PULLUP



//????
/* ????? */
void key_init(unsigned char key_num);
/* ???? */
unsigned char key_check(void);

//??????


#endif
