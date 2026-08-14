/*********************************************************************************************************************
* @file            xunxian.c
* @brief           Electromagnetic sensor sampling and normalization
* @Target core     GD32F303RCT6
********************************************************************************************************************/

#include "headfile.h"

#define TRACK_SAMPLE_COUNT        7U
#define TRACK_SENSOR_MAX_DEFAULT  3000.0f

float adc_value[5];
float ll_max = TRACK_SENSOR_MAX_DEFAULT;
float lm_max = TRACK_SENSOR_MAX_DEFAULT;
float mm_max = TRACK_SENSOR_MAX_DEFAULT;
float rm_max = TRACK_SENSOR_MAX_DEFAULT;
float rr_max = TRACK_SENSOR_MAX_DEFAULT;
float ll;
float lm;
float mm;
float rm;
float rr;

static float sensor_limit_float(float value, float min_value, float max_value)
{
    if(value < min_value)
    {
        return min_value;
    }
    if(value > max_value)
    {
        return max_value;
    }
    return value;
}

static float sensor_normalize(float value, float max_value)
{
    float normalized;

    if(max_value < 1.0f)
    {
        return 0.0f;
    }

    normalized = (value * 100.0f) / max_value;
    return sensor_limit_float(normalized, 0.0f, 100.0f);
}

void all_adc_get(void)
{
    adc_value[0] = (float)adc_get(ADC0, ADC_CH_10);
    adc_value[1] = (float)adc_get(ADC0, ADC_CH_11);
    adc_value[2] = (float)adc_get(ADC0, ADC_CH_12);
    adc_value[3] = (float)adc_get(ADC0, ADC_CH_13);
    adc_value[4] = (float)adc_get(ADC0, ADC_CH_15);
    update_max();
}

float seven_filter(uint8_t channel)
{
    float samples[TRACK_SAMPLE_COUNT];
    float sum = 0.0f;
    float temp;
    uint8_t i;
    uint8_t j;

    for(i = 0U; i < TRACK_SAMPLE_COUNT; i++)
    {
        samples[i] = (float)adc_get(ADC0, channel);
    }

    for(i = 0U; i < (TRACK_SAMPLE_COUNT - 1U); i++)
    {
        for(j = 0U; j < (TRACK_SAMPLE_COUNT - 1U - i); j++)
        {
            if(samples[j] > samples[j + 1U])
            {
                temp = samples[j];
                samples[j] = samples[j + 1U];
                samples[j + 1U] = temp;
            }
        }
    }

    for(i = 1U; i <= 5U; i++)
    {
        sum += samples[i];
    }

    return sum / 5.0f;
}

void five_unification(void)
{
    float ll_raw = seven_filter(ADC_CH_10);
    float lm_raw = seven_filter(ADC_CH_11);
    float mm_raw = seven_filter(ADC_CH_12);
    float rm_raw = seven_filter(ADC_CH_13);
    float rr_raw = seven_filter(ADC_CH_15);

    if(ll_raw > ll_max)
    {
        ll_max = ll_raw;
    }
    if(lm_raw > lm_max)
    {
        lm_max = lm_raw;
    }
    if(mm_raw > mm_max)
    {
        mm_max = mm_raw;
    }
    if(rm_raw > rm_max)
    {
        rm_max = rm_raw;
    }
    if(rr_raw > rr_max)
    {
        rr_max = rr_raw;
    }

    ll = sensor_normalize(ll_raw, ll_max);
    lm = sensor_normalize(lm_raw, lm_max);
    mm = sensor_normalize(mm_raw, mm_max);
    rm = sensor_normalize(rm_raw, rm_max);
    rr = sensor_normalize(rr_raw, rr_max);
}

void update_max(void)
{
    if(adc_value[0] > ll_max)
    {
        ll_max = adc_value[0];
    }
    if(adc_value[1] > lm_max)
    {
        lm_max = adc_value[1];
    }
    if(adc_value[2] > mm_max)
    {
        mm_max = adc_value[2];
    }
    if(adc_value[3] > rm_max)
    {
        rm_max = adc_value[3];
    }
    if(adc_value[4] > rr_max)
    {
        rr_max = adc_value[4];
    }
}

void track_sensor_max_reset(void)
{
    all_adc_get();

    ll_max = (adc_value[0] < 1.0f) ? 1.0f : adc_value[0];
    lm_max = (adc_value[1] < 1.0f) ? 1.0f : adc_value[1];
    mm_max = (adc_value[2] < 1.0f) ? 1.0f : adc_value[2];
    rm_max = (adc_value[3] < 1.0f) ? 1.0f : adc_value[3];
    rr_max = (adc_value[4] < 1.0f) ? 1.0f : adc_value[4];
    track_pid_reset();
}

void oled_show_adc(void)
{
    oled_show_number(30, 2, (uint32_t)adc_value[0], 4, 12);
    oled_show_number(30, 3, (uint32_t)adc_value[1], 4, 12);
    oled_show_number(30, 4, (uint32_t)adc_value[2], 4, 12);
    oled_show_number(30, 5, (uint32_t)adc_value[3], 4, 12);
    oled_show_number(30, 6, (uint32_t)adc_value[4], 4, 12);
}

void oled_show_gyh(void)
{
    oled_show_number(5, 2, (uint32_t)ll, 4, 12);
    oled_show_number(5, 3, (uint32_t)lm, 4, 12);
    oled_show_number(5, 4, (uint32_t)mm, 4, 12);
    oled_show_number(5, 5, (uint32_t)rm, 4, 12);
    oled_show_number(5, 6, (uint32_t)rr, 4, 12);
}
