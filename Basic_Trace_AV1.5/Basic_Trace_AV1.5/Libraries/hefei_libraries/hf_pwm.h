/*********************************************************************************************************************
* @file            hf_pwm.h
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.06, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef _HF_PWM_H
#define _HF_PWM_H

#include "headfile.h"

/* 函数定义 */

/* PWM 初始化 */
void pwm_init(uint32_t timer_periph, uint16_t timer_ch, uint16_t freq_div, uint16_t period_temp, uint16_t duty);
/* PWM 频率设置 */
void pwm_freq(uint32_t timer_periph, uint16_t freq_div, uint16_t period_temp);
/* PWM 占空比设置 */
void pwm_duty(uint32_t timer_periph, uint32_t timer_ch, uint16_t duty);
/* PWM 停止 */
void pwm_stop(uint32_t timer_periph);

#endif
