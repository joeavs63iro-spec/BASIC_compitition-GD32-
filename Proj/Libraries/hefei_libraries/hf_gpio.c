/*********************************************************************************************************************
* @file            hf_gpio.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.02, V1.0
* @modify          none
********************************************************************************************************************/

#include "hf_gpio.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief        GPIO初始化
// @param        gpio_periph            选择的端口：GPIOx(x = A,B,C,D,E,F,G)
// @param        mode:                  选择的模式: GPIO_mode_enum(详见hf_gpio.h)
// @param        gpio_pin               选择的引脚：GPIO_PIN_x(x=0..15), GPIO_PIN_ALL
// @return       void
// Sample usage:        gpio_init(GPIOA, 1, GPIO_PIN_0);
//                      //PA0初始化为浮空输入模式
//-------------------------------------------------------------------------------------------------------------------
void gpio_pin_init(uint32_t gpio_periph, GPIO_mode_enum mode, uint32_t gpio_pin)
{
    /* 配置GPIO时钟 */
    if(gpio_periph == GPIOA)
    {
        rcu_periph_clock_enable(RCU_GPIOA);
    }
    else if(gpio_periph == GPIOB)
    {
        rcu_periph_clock_enable(RCU_GPIOB);
    }
    else if(gpio_periph == GPIOC)
    {
        rcu_periph_clock_enable(RCU_GPIOC);
    }
    else if(gpio_periph == GPIOD)
    {
        rcu_periph_clock_enable(RCU_GPIOD);
    }
    else if(gpio_periph == GPIOE)
    {
        rcu_periph_clock_enable(RCU_GPIOE);
    }
    else if(gpio_periph == GPIOF)
    {
        rcu_periph_clock_enable(RCU_GPIOF);
    }
    else if(gpio_periph == GPIOG)
    {
        rcu_periph_clock_enable(RCU_GPIOG);
    }

    /* 配置GPIO引脚模式 */
    if(mode == IN_AIN)
    {
        gpio_init(gpio_periph, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, gpio_pin);
    }

    else if(mode == IN_FLOATING)
    {
        gpio_init(gpio_periph, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, gpio_pin);
    }

    else if(mode == IN_PULLDOWN)
    {
        gpio_init(gpio_periph, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, gpio_pin);
    }

    else if(mode == IN_PULLUP)
    {
        gpio_init(gpio_periph, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, gpio_pin);
    }

    else if(mode == OUT_PP)
    {
        gpio_init(gpio_periph, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, gpio_pin);
    }

    else if(mode == OUT_OD)
    {
        gpio_init(gpio_periph, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, gpio_pin);
    }

    else if(mode == OUT_AF_PP)
    {
        gpio_init(gpio_periph, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, gpio_pin);
    }

    else if(mode == OUT_AF_OD)
    {
        gpio_init(gpio_periph, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, gpio_pin);
    }

}

//-------------------------------------------------------------------------------------------------------------------
// @brief        GPIO引脚功能重映射
// @param        remap            选择的复用功能：GPIO_remap_enum(详见hf_gpio.h)
// @return       void
// Sample usage:        gpio_pin_remap(1);
//                      //开启引脚的SPI0重映射
//-------------------------------------------------------------------------------------------------------------------
void gpio_pin_remap(uint32_t remap)
{
    if(remap == SPI0_REMAP)
    {
        gpio_pin_remap_config(GPIO_SPI0_REMAP, ENABLE);
    }
		
    else if(remap == I2C0_REMAP)
    {
        gpio_pin_remap_config(GPIO_I2C0_REMAP, ENABLE);
    }
				
    else if(remap == USART0_REMAP)
    {
        gpio_pin_remap_config(GPIO_USART0_REMAP, ENABLE);
    }
				
    else if(remap == USART1_REMAP)
    {
        gpio_pin_remap_config(GPIO_USART1_REMAP, ENABLE);
    }
				
    else if(remap == USART2_PARTIAL_REMAP)
    {
        gpio_pin_remap_config(GPIO_USART2_PARTIAL_REMAP, ENABLE);
    }
				
    else if(remap == USART2_FULL_REMAP)
    {
        gpio_pin_remap_config(GPIO_USART2_FULL_REMAP, ENABLE);
    }
				
    else if(remap == TIMER0_PARTIAL_REMAP)
    {
        gpio_pin_remap_config(GPIO_TIMER0_PARTIAL_REMAP, ENABLE);
    }
				
    else if(remap == TIMER0_FULL_REMAP)
    {
        gpio_pin_remap_config(GPIO_TIMER0_FULL_REMAP, ENABLE);
    }
				
    else if(remap == TIMER1_PARTIAL_REMAP0)
    {
        gpio_pin_remap_config(GPIO_TIMER1_PARTIAL_REMAP0, ENABLE);
    }
				
    else if(remap == TIMER1_PARTIAL_REMAP1)
    {
        gpio_pin_remap_config(GPIO_TIMER1_PARTIAL_REMAP1, ENABLE);
    }
				
    else if(remap == TIMER1_FULL_REMAP)
    {
        gpio_pin_remap_config(GPIO_TIMER1_FULL_REMAP, ENABLE);
    }		
		
    else if(remap == TIMER2_PARTIAL_REMAP)
    {
        gpio_pin_remap_config(GPIO_TIMER2_PARTIAL_REMAP, ENABLE);
    }
				
    else if(remap == TIMER2_FULL_REMAP)
    {
        gpio_pin_remap_config(GPIO_TIMER2_FULL_REMAP, ENABLE);
    }
			
    else if(remap == TIMER3_REMAP)
    {
        gpio_pin_remap_config(GPIO_TIMER3_REMAP, ENABLE);
    }
		
    else if(remap == CAN_PARTIAL_REMAP)
    {
        gpio_pin_remap_config(GPIO_CAN_PARTIAL_REMAP, ENABLE);
    }		
		
    else if(remap == CAN_FULL_REMAP)
    {
        gpio_pin_remap_config(GPIO_CAN_FULL_REMAP, ENABLE);
    }
				
    else if(remap == PD01_REMAP)
    {
        gpio_pin_remap_config(GPIO_PD01_REMAP, ENABLE);
    }
				
    else if(remap == TIMER4CH3_IREMAP)
    {
        gpio_pin_remap_config(GPIO_TIMER4CH3_IREMAP, ENABLE);
    }
				
    else if(remap == ADC0_ETRGINS_REMAP)
    {
        gpio_pin_remap_config(GPIO_ADC0_ETRGINS_REMAP, ENABLE);
    }
				
    else if(remap == ADC0_ETRGREG_REMAP)
    {
        gpio_pin_remap_config(GPIO_ADC0_ETRGREG_REMAP, ENABLE);
    }
				
    else if(remap == ADC1_ETRGINS_REMAP)
    {
        gpio_pin_remap_config(GPIO_ADC1_ETRGINS_REMAP, ENABLE);
    }
			
    else if(remap == ADC1_ETRGREG_REMAP)
    {
        gpio_pin_remap_config(GPIO_ADC1_ETRGREG_REMAP, ENABLE);
    }
				
    else if(remap == SWJ_NONJTRST_REMAP)
    {
        gpio_pin_remap_config(GPIO_SWJ_NONJTRST_REMAP, ENABLE);
    }
				
    else if(remap == SWJ_SWDPENABLE_REMAP)
    {
        gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
    }
				
    else if(remap == SWJ_DISABLE_REMAP)
    {
        gpio_pin_remap_config(GPIO_SWJ_DISABLE_REMAP, ENABLE);
    }
				
    else if(remap == SPI2_REMAP)
    {
        gpio_pin_remap_config(GPIO_SPI2_REMAP, ENABLE);
    }
				
    else if(remap == TIMER8_REMAP)
    {
        gpio_pin_remap_config(GPIO_TIMER8_REMAP, ENABLE);
    }
		
    else if(remap == TIMER9_REMAP)
    {
        gpio_pin_remap_config(GPIO_TIMER9_REMAP, ENABLE);
    }
		
    else if(remap == TIMER10_REMAP)
    {
        gpio_pin_remap_config(GPIO_TIMER10_REMAP, ENABLE);
    }
		
    else if(remap == TIMER12_REMAP)
    {
        gpio_pin_remap_config(GPIO_TIMER12_REMAP, ENABLE);
    }
		
    else if(remap == TIMER13_REMAP)
    {
        gpio_pin_remap_config(GPIO_TIMER13_REMAP, ENABLE);
    }
		
    else if(remap == EXMC_NADV_REMAP)
    {
        gpio_pin_remap_config(GPIO_EXMC_NADV_REMAP, ENABLE);
    }
		
    else if(remap == CTC_REMAP0)
    {
        gpio_pin_remap_config(GPIO_CTC_REMAP0, ENABLE);
    }
		
    else if(remap == CTC_REMAP1)
    {
        gpio_pin_remap_config(GPIO_CTC_REMAP1, ENABLE);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        GPIO引脚输出设置
// @param        gpio_periph            选择的端口：GPIOx(x = A,B,C,D,E,F,G)
// @param        gpio_pin               选择的引脚：GPIO_PIN_x(x=0..15), GPIO_PIN_ALL
// @param        dat                    0：低电平 1：高电平
// @return       void
// Sample usage:        gpio_bit_output(GPIOA, GPIO_PIN_0, 1);
                        //PA0输出高电平
//-------------------------------------------------------------------------------------------------------------------
void gpio_bit_output(uint32_t gpio_periph, uint32_t gpio_pin, uint8_t dat)
{
    if(RESET != dat)
    {
        gpio_bit_set(gpio_periph, gpio_pin);
    }
    else
    {
        gpio_bit_reset(gpio_periph, gpio_pin);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        GPIO引脚输出翻转
// @param        gpio_periph            选择的端口：GPIOx(x = A,B,C,D,E,F,G)
// @param        gpio_pin               选择的引脚：GPIO_PIN_x(x=0..15), GPIO_PIN_ALL
// @return       void
// Sample usage:        gpio_toggle(GPIOA, GPIO_PIN_0);
                        //PA0输出切换（0变为1，1变为0）
//-------------------------------------------------------------------------------------------------------------------
void gpio_toggle(uint32_t gpio_periph, uint32_t gpio_pin)
{
    if(0 != gpio_input_bit_get(gpio_periph, gpio_pin))    
    {       
        gpio_bit_reset(gpio_periph, gpio_pin);
    }
    else
    {
        gpio_bit_set(gpio_periph, gpio_pin);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        GPIO获取引脚输入
// @param        gpio_periph            选择的端口：GPIOx(x = A,B,C,D,E,F,G)
// @param        gpio_pin               选择的引脚：GPIO_PIN_x(x=0..15), GPIO_PIN_ALL
// @return       uint8                  0：低电平 1：高电平
// Sample usage:        uint8 status;  status = gpio_get_input(GPIOA, GPIO_PIN_0);
                        //获取PA0引脚输入电平
//-------------------------------------------------------------------------------------------------------------------
uint8_t gpio_get_input(uint32_t gpio_periph,uint32_t gpio_pin)
{
    if(0 != gpio_input_bit_get(gpio_periph, gpio_pin))    
    {       
        return 1;
    }
    else
    {
        return 0;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        GPIO获取引脚输出
// @param        gpio_periph            选择的端口：GPIOx(x = A,B,C,D,E,F,G)
// @param        gpio_pin               选择的引脚：GPIO_PIN_x(x=0..15), GPIO_PIN_ALL
// @return       uint8                  0：低电平 1：高电平
// Sample usage:        uint8 status = gpio_get_output(GPIOA, GPIO_PIN_0);
                        //获取PA0引脚输出电平
//-------------------------------------------------------------------------------------------------------------------
uint8_t gpio_get_output(uint32_t gpio_periph, uint32_t gpio_pin)
{
    if(0 != gpio_output_bit_get(gpio_periph, gpio_pin))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
