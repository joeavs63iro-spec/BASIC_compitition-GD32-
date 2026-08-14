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

/* Reed switch IO is PA5 on the GD32F303 smart car adapter board. */
#define REED_PORT    GPIOA
#define REED_PIN     GPIO_PIN_5
#define REED_MODE    IN_PULLUP


//????
/* ????????? */
void reed_init(void);
/* ??????? */
uint8_t reed_check(void);


//??????

#endif
