#include <stdio.h>
#include <stdlib.h>
#include "median_filter.h"

// 冒泡排序辅助函数
static void bubble_sort(float arr[], int size) {
    for(int i = 0; i < size-1; i++) {
        for(int j = 0; j < size-i-1; j++) {
            if(arr[j] > arr[j+1]) {
                float temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}
float seven_filter1(sampling_function_t sampler, int channel) {
    float samples[7];
    float sum = 0.0f;
    
    // 使用传入的采样函数进行采样
    for(int i = 0; i < 7; i++) {
        samples[i] = sampler(channel);
    }
    
    // 对采样值进行排序
    bubble_sort(samples, 7);
    
    // 计算中间5个值的算术平均（去除首尾极值）
    for(int i = 1; i <= 5; i++) {
        sum += samples[i];
    }
    
    return sum / 5.0f;
}

// 针对带参数的ADC采样函数的七点中值滤波器实现
float seven_filter2(sampling_function_adc_t sampler, int adc_num, int channel) {
    float samples[7];
    float sum = 0.0f;
    
    // 使用传入的采样函数进行采样
    for(int i = 0; i < 7; i++) {
        samples[i] = sampler(adc_num, channel);
    }
    
    // 对采样值进行排序
    bubble_sort(samples, 7);
    
    // 计算中间5个值的算术平均（去除首尾极值）
    for(int i = 1; i <= 5; i++) {
        sum += samples[i];
    }
    
    return sum / 5.0f;
}

// 针对无参数采样函数的七点中值滤波器实现
float seven_filter_void(sampling_function_void_t sampler) {
    float samples[7];
    float sum = 0.0f;
    
    // 使用传入的采样函数进行采样
    for(int i = 0; i < 7; i++) {
        // 注意：这里需要将 uint32_t 转换为 float 以保持一致性
        samples[i] = (float)sampler();
    }
    
    // 对采样值进行排序
    bubble_sort(samples, 7);
    
    // 计算中间5个值的算术平均（去除首尾极值）
    for(int i = 1; i <= 5; i++) {
        sum += samples[i];
    }
    
    return sum / 5.0f;
}

