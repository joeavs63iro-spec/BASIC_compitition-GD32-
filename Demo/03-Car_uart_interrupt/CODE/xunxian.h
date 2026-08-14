/*********************************************************************************************************************
* @file            xunxian.h
* @author          
* @Target core     GD32F303RCT6
* @revisions       2025.9.15, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef _XUNXIAN_H
#define _XUNXIAN_H

//变量定义、宏定义、函数声明等内容
// 速度控制参数
extern unsigned int st_speed;  // 初始速度 (2500)

// PID调节参数
extern float kp;  // 比例系数 (1.9)
extern float kd;  // 微分系数 (2.8)

// ADC原始数据
extern float adc_value[5];  // 5通道未处理ADC值

// 各通道ADC最大值
extern float ll_max;  // 左外通道 (3000)
extern float lm_max;  // 左中通道 (3000)
extern float mm_max;  // 中间通道 (3000)
extern float rm_max;  // 右中通道 (3000)
extern float rr_max;  // 右外通道 (3000)

// 归一化滤波后的ADC值
extern float ll;  // 左外通道
extern float lm;  // 左中通道
extern float mm;  // 中间通道
extern float rm;  // 右中通道
extern float rr;  // 右外通道

// 误差记录
extern float error[2];  // [0]=上一次误差, [1]=当前误差



void all_adc_get(void);
float seven_filter(float channel);
void five_unification(void);
void gxmax(void); //更新最大值
void oled_show_adc(void); //显示没有处理过的ADC值
void oled_show_gyh(void);  //显示归一化的ADC值
void diancixunxian(void);
void xunxian(void);
//全局变量声明

#endif
