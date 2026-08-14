/*********************************************************************************************************************
* @file            hf_systick.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.07, V1.0
* @modify          none
********************************************************************************************************************/
#include "hf_systick.h"

volatile uint32_t g_millis = 0;
volatile uint32_t g_micros = 0;
static uint32_t sysTick_clk;       // SysTick时钟频率（Hz）
static uint32_t ticks_per_us;      // 1微秒对应的整数滴答数
static uint32_t ticks_per_us_frac; // 1微秒对应的小数滴答数（用于补偿）

void systick_config(void)
{
    // 配置SysTick时钟源为HCLK/8
    systick_clksource_set(SYSTICK_CLKSOURCE_HCLK_DIV8);
    sysTick_clk = SystemCoreClock / 8;  // 计算实际时钟频率
    
    // 预计算1微秒对应的滴答数（提升效率）
    ticks_per_us = sysTick_clk / 1000000;
    ticks_per_us_frac = sysTick_clk % 1000000;
    
    // 配置1ms中断（检查24位计数器上限）
    uint32_t load_val = sysTick_clk / 1000;  // 1ms = 1000us
    if (load_val > 0xFFFFFF) {
			printf("时钟过高，无法配置1ms中断，需调整\n");
//        while (1);  // 时钟过高，无法配置1ms中断，需调整
    }
    if (SysTick_Config(load_val)) {
			printf("配置失败\n");
//        while (1);  // 配置失败
    }
    
    // 设置最高优先级，避免中断延迟
    NVIC_SetPriority(SysTick_IRQn, 0x00);
}

// 高精度微秒延时（适合1~1000us）
void delay_1us(uint32_t us)
{
    if (us == 0) return;
    
    uint32_t start = SysTick->VAL;
    // 计算总滴答数（含整数+小数补偿）
    uint32_t total_ticks = us * ticks_per_us;
    total_ticks += (us * ticks_per_us_frac) / 1000000;  // 补偿小数部分
    
    int32_t remaining = total_ticks;
    while (remaining > 0) {
        uint32_t current = SysTick->VAL;
        // 计算已流逝的滴答数（处理计数器溢出）
        if (current <= start) {
            remaining -= (start - current);
        } else {
            remaining -= (start + (SysTick->LOAD - current));
        }
        start = current;
    }
}

// 毫秒延时（适合1ms以上）
void delay_1ms(uint32_t ms)
{
    if (ms == 0) return;
    uint32_t start = g_millis;
    while (g_millis - start < ms);
}

// 获取当前微秒数（精度<1us）
uint64_t get_current_time_us(void)
{
    uint32_t ms, cnt;
	uint64_t total_us;
    // 确保ms和cnt在同一周期（避免中断干扰）
    do {
        ms = g_millis;
        cnt = SysTick->VAL;
    } while (ms != g_millis);
    
    uint32_t elapsed_ticks = SysTick->LOAD - cnt;
    uint64_t us_part = (uint64_t)elapsed_ticks * 1000000ULL / sysTick_clk;
		total_us=ms * 1000ULL + us_part;
    return total_us;
}

// SysTick中断服务函数（1ms一次）
void SysTick_Handler(void)
{
    g_millis++;
	if(g_millis <= 0){
		g_millis=0;
		printf("g_millis发生溢出，需调整\n");
	}
//    g_micros += 1000;  // 累计微秒（可选）
}
