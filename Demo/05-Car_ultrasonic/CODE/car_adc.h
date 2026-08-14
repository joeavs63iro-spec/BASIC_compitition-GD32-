/*********************************************************************************************************************
* @file            car_adc.h
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.23, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef __CAR_ADC_H
#define __CAR_ADC_H

#include "headfile.h"

//宏定义
#define ADC_CH_00     ADC_CHANNEL_0
#define ADC_CH_01     ADC_CHANNEL_1
#define ADC_CH_02     ADC_CHANNEL_2
#define ADC_CH_03     ADC_CHANNEL_3
#define ADC_CH_04     ADC_CHANNEL_4
#define ADC_CH_05     ADC_CHANNEL_5
#define ADC_CH_06     ADC_CHANNEL_6
#define ADC_CH_07     ADC_CHANNEL_7
#define ADC_CH_08     ADC_CHANNEL_8
#define ADC_CH_09     ADC_CHANNEL_9
#define ADC_CH_10     ADC_CHANNEL_10
#define ADC_CH_11     ADC_CHANNEL_11
#define ADC_CH_12     ADC_CHANNEL_12
#define ADC_CH_13     ADC_CHANNEL_13
#define ADC_CH_14     ADC_CHANNEL_14
#define ADC_CH_15     ADC_CHANNEL_15
#define ADC_CH_16     ADC_CHANNEL_16
#define ADC_CH_17     ADC_CHANNEL_17


//函数声明
/* adc初始化 */
void adc_channel_init(uint32_t adc_periph, uint8_t adc_ch);
/* adc转换一次 */
unsigned int adc_get(uint32_t adc_periph, uint8_t adc_ch);


//全局变量声明

#endif
