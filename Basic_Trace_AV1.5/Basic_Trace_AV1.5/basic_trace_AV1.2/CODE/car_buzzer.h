/*********************************************************************************************************************
* @file            car_buzzer.h
* @author         
* @Target core     GD32F303RCT6
* @revisions       2022.09.22, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef __CAR_BUZZER_H
#define __CAR_BUZZER_H

#include "headfile.h"

//???
#define TIMER_PORT    TIMER0
#define TIMER_CH      TIMER_CH_0
#define FREQ_DIV      14
#define PERIOD_TEMP   20000
#define BUZZER_TIMER_CLOCK_HZ 72000000U

/* PA6 is used by encoder 2 A phase; move buzzer to TIMER0_CH0 on PA8. */
#define GPIO_PORT    GPIOA
#define GPIO_PIN     GPIO_PIN_8
#define GPIO_MODE    OUT_AF_PP

#define DO           262
#define RE           294
#define MI           330
#define FA           370
#define SO           415
#define LA           466
#define SI           521


//????
/* ?????? */
void buzzer_init(void);
/* buzzer???? */
void buzzer_freq(unsigned int freq);
/* PWM ????? */
void buzzer_duty(unsigned char duty);


//??????

#endif
