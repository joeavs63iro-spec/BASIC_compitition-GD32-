/*********************************************************************************************************************
* @file            hf_adc.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.05, V1.0
* @modify          none
********************************************************************************************************************/

#include "hf_adc.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief        ADC初始化
// @param        adc_periph        选择ADC外设：ADCx(x = 0,1,2)
// @param        adc_ch            选择ADC通道：ADC_CHANNEL_x(x = 0..17)
// @return       void
// Sample usage:        adc_init(ADC0, ADC_CHANNEL_1); 
                        //初始化通道1为ADC0输入通道
//-------------------------------------------------------------------------------------------------------------------
void adc_init(uint32_t adc_periph, uint8_t adc_ch)
{
    /*开启复用功能时钟*/
    rcu_periph_clock_enable(RCU_AF);
	
    /* 配置滴答计时器，用于1ms延时 */
    systick_config();

    /* 使能GPIO时钟 */
    if(adc_ch == ADC_CHANNEL_0 || adc_ch == ADC_CHANNEL_1 || adc_ch == ADC_CHANNEL_2 || adc_ch == ADC_CHANNEL_3 || adc_ch == ADC_CHANNEL_4 || adc_ch == ADC_CHANNEL_5 || adc_ch == ADC_CHANNEL_6 || adc_ch == ADC_CHANNEL_7)
    {
        rcu_periph_clock_enable(RCU_GPIOA);
    }
    else if(adc_ch == ADC_CHANNEL_8 || adc_ch == ADC_CHANNEL_9)
    {
        rcu_periph_clock_enable(RCU_GPIOB);
    }
    else if(adc_ch == ADC_CHANNEL_10 || adc_ch == ADC_CHANNEL_11 || adc_ch == ADC_CHANNEL_12 || adc_ch == ADC_CHANNEL_13 || adc_ch == ADC_CHANNEL_14 || adc_ch == ADC_CHANNEL_15)
    {
        rcu_periph_clock_enable(RCU_GPIOC);
    }
		
    if(adc_ch == ADC_CHANNEL_0)
    {
        /* 配置GPIO模式 */
        gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    }
    else if(adc_ch == ADC_CHANNEL_1)
    {
        /* 配置GPIO模式 */
        gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    }
    else if(adc_ch == ADC_CHANNEL_2)
    {
        /* 配置GPIO模式 */
        gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    }
    else if(adc_ch == ADC_CHANNEL_3)
    {
        /* 配置GPIO模式 */
        gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    }
    else if(adc_ch == ADC_CHANNEL_4)
    {
        /* 配置GPIO模式 */
        gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
    }
    else if(adc_ch == ADC_CHANNEL_5)
    {
        /* 配置GPIO模式 */
        gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
    }
    else if(adc_ch == ADC_CHANNEL_6)
    {
        /* 配置GPIO模式 */
        gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    }
    else if(adc_ch == ADC_CHANNEL_7)
    {
        /* 配置GPIO模式 */
        gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
    }
    else if(adc_ch == ADC_CHANNEL_8)
    {
        /* 配置GPIO模式 */
        gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    }
    else if(adc_ch == ADC_CHANNEL_9)
    {
        /* 配置GPIO模式 */
        gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    }
    else if(adc_ch == ADC_CHANNEL_10)
    {
        /* 配置GPIO模式 */
        gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    }
    else if(adc_ch == ADC_CHANNEL_11)
    {
        /* 配置GPIO模式 */
        gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    }
    else if(adc_ch == ADC_CHANNEL_12)
    {
        /* 配置GPIO模式 */
        gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    }
    else if(adc_ch == ADC_CHANNEL_13)
    {
        /* 配置GPIO模式 */
        gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    }
    else if(adc_ch == ADC_CHANNEL_14)
    {
        /* 配置GPIO模式 */
        gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
    }
    else if(adc_ch == ADC_CHANNEL_15)
    {
        /* 配置GPIO模式 */
        gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
    }

    /* 使能ADC时钟 */
    if(adc_periph == ADC0)
    {
        rcu_periph_clock_enable(RCU_ADC0);
    }
    else if(adc_periph == ADC1)
    {
        rcu_periph_clock_enable(RCU_ADC1);
    }
    else if(adc_periph == ADC2)
    {
        rcu_periph_clock_enable(RCU_ADC2);
    }
		
    /* 配置ADC时钟来源 */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);
		
    /* 复位 ADC */
    adc_deinit(adc_periph);

    /* ADC 连续功能禁用 */
    adc_special_function_config(adc_periph, ADC_CONTINUOUS_MODE, DISABLE);
				
    /* ADC 扫描模式禁用 */
    adc_special_function_config(adc_periph, ADC_SCAN_MODE, DISABLE);

    /* ADC数据右对齐 */
    adc_data_alignment_config(adc_periph, ADC_DATAALIGN_RIGHT);

    /* ADC转换由软件触发而不是外部触发 */
    adc_external_trigger_source_config(adc_periph, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
    adc_external_trigger_config(adc_periph, ADC_REGULAR_CHANNEL, ENABLE);

    /* ADC使能 */
    adc_enable(adc_periph);
		
    /* 延时1ms等待使能完成 */
    delay_1ms(1);
		
    /* ADC校准和复位校准 */
    adc_calibration_enable(adc_periph);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        ADC转换一次
// @param        adc_periph        选择ADC外设：ADCx(x = 0,1,2)
// @param        adc_ch            选择ADC通道：ADC_CHANNEL_x(x = 0..17)
// @return       uint16            转换的模拟量数值
// Sample usage:        uint16_t data; data = adc_convert(ADC0, ADC_CHANNEL_1); 
                        //获取ADC0通道1转换值
//-------------------------------------------------------------------------------------------------------------------
uint16_t adc_convert(uint32_t adc_periph, uint8_t adc_ch)
{
    /* 配置ADC通道和转换时间 */
    adc_regular_channel_config(adc_periph, 0, adc_ch, ADC_SAMPLETIME_55POINT5);
	
    /* 使能ADC软件触发 */
    adc_software_trigger_enable(adc_periph, ADC_REGULAR_CHANNEL);
	
    /* 清除转换完成标志位 */
    adc_flag_clear(adc_periph, ADC_FLAG_EOC);
	
    /* 等待ADC转换完成 */
    while (!adc_flag_get(adc_periph, ADC_FLAG_EOC))
    {
        ;
    }
	
    /* 返回ADC的值 */
    return(adc_regular_data_read(adc_periph));
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        ADC转换多次
// @param        adc_ch            选择ADC通道：ADC_CHANNEL_x(x = 0..9)
// @param        count             均值滤波次数
// @return       uint16            转换的模拟量数值
// Sample usage:        uint16_t data; data = adc_mean_filter(ADC_CHANNEL_1, 5); 
                        //对ADC通道1采样5次求平均值
//-------------------------------------------------------------------------------------------------------------------
uint16_t adc_mean_filter(uint32_t adc_periph, uint8_t adc_ch, uint8_t count)
{
    uint8_t i;
    uint16_t sum;

    sum = 0;
	
    if(count < 1)
    {
        return 0;
    }

    for (i=0; i<count; i++)
    {
        sum += adc_convert(adc_periph, adc_ch);
    }
				
    return sum / count;
}

uint16_t get_adc(uint32_t adc_periph , uint8_t adc_channel)
{
    /* 配置规则通道采集*/
    adc_regular_channel_config(adc_periph, 0, adc_channel, ADC_SAMPLETIME_55POINT5);
		// 由于没有采用外部触发，所以使用软件触发ADC转换
		adc_software_trigger_enable(adc_periph, ADC_REGULAR_CHANNEL);   
				
		while(!adc_flag_get(adc_periph, ADC_FLAG_EOC));                       // 等待采样完成
		adc_flag_clear(adc_periph, ADC_FLAG_EOC);                             // 清除结束标志
				
		return  adc_regular_data_read(adc_periph);                         // 读取ADC数据
}


