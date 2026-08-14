/*********************************************************************************************************************
* @file            car_adc.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.23, V1.0
* @modify          none
********************************************************************************************************************/

#include "headfile.h"

/*
 * A normal ADC conversion only takes a few microseconds.  Keep the polling
 * bounded so an ADC/peripheral fault cannot trap the control interrupt
 * forever.  The loop count deliberately leaves a generous margin for the
 * configured 55.5-cycle sample time.
 */
#define ADC_CONVERSION_TIMEOUT_LOOPS    (10000U)

//-------------------------------------------------------------------------------------------------------------------
// @brief        adc??'??   
// @param        adc_periph        ???ADC????ADCx(x = 0,1,2)
// @param        adc_ch            ???ADC?????ADC_CHANNEL_x(x = 0..17)
// @return       void
// Sample usage:       adc_channel_init(ADC0, ADC_CHANNEL_8);//??'??ADC0???8
//-------------------------------------------------------------------------------------------------------------------
void adc_channel_init(uint32_t adc_periph, uint8_t adc_ch)
{
    adc_init(adc_periph, adc_ch);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        adc???h??
// @param        adc_periph        ???ADC????ADCx(x = 0,1,2)
// @param        adc_ch            ???ADC?????ADC_CHANNEL_x(x = 0..17)
// @return       void 
// Sample usage:        adc_get(ADC0, ADC_CH_08);//ADC0???8???h??
//-------------------------------------------------------------------------------------------------------------------
unsigned int adc_get(uint32_t adc_periph, uint8_t adc_ch)
{
    uint32_t timeout = ADC_CONVERSION_TIMEOUT_LOOPS;

    adc_regular_channel_config(adc_periph, 0, adc_ch, ADC_SAMPLETIME_55POINT5);

    /* Clear a stale EOC before starting a new conversion. */
    adc_flag_clear(adc_periph, ADC_FLAG_EOC);
    adc_software_trigger_enable(adc_periph, ADC_REGULAR_CHANNEL);

    while(!adc_flag_get(adc_periph, ADC_FLAG_EOC))
    {
        if(timeout-- == 0U)
        {
            /* Leave the peripheral in a known state for the next sample. */
            adc_flag_clear(adc_periph, ADC_FLAG_EOC);
            return 0U;
        }
    }

    return adc_regular_data_read(adc_periph);
}
