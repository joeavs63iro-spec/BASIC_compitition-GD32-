#ifndef MEDIAN_FILTER_H
#define MEDIAN_FILTER_H

#include <stdint.h>

// 采样函数指针类型定义
typedef float (*sampling_function_t)(int channel);
typedef float (*sampling_function_adc_t)(int, int); // 用于类似 adc_get(ADC0, channel) 的函数
typedef uint32_t (*sampling_function_void_t)(void);    // 用于类似 ultra_get_distance(void) 的函数
// 函数重载：针对带参数的ADC采样函数
float seven_filter2(sampling_function_adc_t sampler, int adc_num, int channel);

// 函数重载：针对无参数的采样函数
float seven_filter_void(sampling_function_void_t sampler);
// 通用中值滤波器函数声明
float seven_filter1(sampling_function_t sampler, int channel);
// 冒泡排序辅助函数
static void bubble_sort(float arr[], int size);
#endif

