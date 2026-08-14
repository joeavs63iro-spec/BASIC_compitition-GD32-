/*********************************************************************************************************************
* @file            hf_adc.h
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.05, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef _HF_ADC_H
#define _HF_ADC_H

#include "headfile.h"

/* 函数声明 */

/* ADC初始化 */
void adc_init(uint32_t adc_periph, uint8_t adc_ch);
/* ADC转换一次 */
uint16_t adc_convert(uint32_t adc_periph, uint8_t adc_ch);
/* ADC转换多次 */
uint16_t adc_mean_filter(uint32_t adc_periph, uint8_t adc_ch, uint8_t count);

uint16_t get_adc(uint32_t adc_periph , uint8_t adc_channel);

#endif
