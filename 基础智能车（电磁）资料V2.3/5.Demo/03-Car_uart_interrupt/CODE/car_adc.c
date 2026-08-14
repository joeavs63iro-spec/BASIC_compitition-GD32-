/*********************************************************************************************************************
* @file            car_adc.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.23, V1.0
* @modify          none
********************************************************************************************************************/

#include "headfile.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief        adc初始化   
// @param        adc_periph        选择ADC外设：ADCx(x = 0,1,2)
// @param        adc_ch            选择ADC通道：ADC_CHANNEL_x(x = 0..17)
// @return       void
// Sample usage:       adc_channel_init(ADC0, ADC_CHANNEL_8);//初始化ADC0通道8
//-------------------------------------------------------------------------------------------------------------------
void adc_channel_init(uint32_t adc_periph, uint8_t adc_ch)
{
    adc_init(adc_periph, adc_ch);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        adc转换一次
// @param        adc_periph        选择ADC外设：ADCx(x = 0,1,2)
// @param        adc_ch            选择ADC通道：ADC_CHANNEL_x(x = 0..17)
// @return       void 
// Sample usage:        adc_get(ADC0, ADC_CH_08);//ADC0通道8转换一次
//-------------------------------------------------------------------------------------------------------------------
unsigned int adc_get(uint32_t adc_periph, uint8_t adc_ch)
{
    unsigned int adc_value;
                    
    adc_value = adc_convert(adc_periph, adc_ch);       //adc转换一次

    return adc_value;
}
