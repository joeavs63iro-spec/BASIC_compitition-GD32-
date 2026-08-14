/*********************************************************************************************************************
* @file            xunxian.h
* @brief           Electromagnetic sensor sampling and normalization API
* @Target core     GD32F303RCT6
********************************************************************************************************************/

#ifndef _XUNXIAN_H
#define _XUNXIAN_H

#include <stdint.h>

extern float adc_value[5];
extern float ll_max;
extern float lm_max;
extern float mm_max;
extern float rm_max;
extern float rr_max;
extern float ll;
extern float lm;
extern float mm;
extern float rm;
extern float rr;

void all_adc_get(void);
float seven_filter(uint8_t channel);
void five_unification(void);
void update_max(void);
void track_sensor_max_reset(void);
void oled_show_adc(void);
void oled_show_gyh(void);

#endif
