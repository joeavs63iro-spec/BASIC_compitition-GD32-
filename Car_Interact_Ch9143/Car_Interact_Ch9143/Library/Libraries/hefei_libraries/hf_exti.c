/*********************************************************************************************************************
* @file            hf_exti.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.08, V1.0
* @modify          none
********************************************************************************************************************/

#include "hf_exti.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief        开启引脚外部中断
// @param        gpio_periph            选择的端口：GPIOx(x = A,B,C,D,E,F,G)
// @param        exti_pin               选择的引脚：GPIO_PIN_x(x=0..15)
// @param        Mode                   选择的模式，只能选择以下参数：
// @arg          CHANGE:                电平改变触发
// @arg          RISING:                上升沿触发
// @arg          FALLING:               下降沿触发
// @return       void
// Sample usage:            exti_enable(GPIOA, GPIO_PIN_0, CHANGE);//开启PA0外部中断，模式为电平改变触发
                            //注：无法同时开启同一引脚数的外部中断（如PA0和PB0）
//-------------------------------------------------------------------------------------------------------------------
void exti_enable(uint32_t gpio_periph, uint32_t exti_pin, exti_Mode_enum Mode)
{
    uint8_t exti_source_gpio;
    uint8_t exti_source_pin;
    uint32_t exti_num;
	
	   /*开启复用功能时钟*/
    rcu_periph_clock_enable(RCU_AF);

    /* 配置中断优先级 */
    nvic_irq_enable(EXTI0_IRQn, 0,0);
    nvic_irq_enable(EXTI1_IRQn, 0,0);
    nvic_irq_enable(EXTI2_IRQn, 0,0);
    nvic_irq_enable(EXTI3_IRQn, 0,0);
    nvic_irq_enable(EXTI4_IRQn, 0,0);
    nvic_irq_enable(EXTI5_9_IRQn, 0,0);
    nvic_irq_enable(EXTI10_15_IRQn, 0,0);

    /* 中断源端口及时钟配置 */
    if(gpio_periph == GPIOA)
    {
        exti_source_gpio = GPIO_PORT_SOURCE_GPIOA;
        rcu_periph_clock_enable(RCU_GPIOA);
    }
    else if(gpio_periph == GPIOB) 
    {
        exti_source_gpio = GPIO_PORT_SOURCE_GPIOB;
        rcu_periph_clock_enable(RCU_GPIOB);
    }
    else if(gpio_periph == GPIOC)
    {
        exti_source_gpio = GPIO_PORT_SOURCE_GPIOC;
        rcu_periph_clock_enable(RCU_GPIOC);
    }
    else if(gpio_periph == GPIOD)
    {
        exti_source_gpio = GPIO_PORT_SOURCE_GPIOD;
        rcu_periph_clock_enable(RCU_GPIOD);
    }
    else if(gpio_periph == GPIOE)
    {
        exti_source_gpio = GPIO_PORT_SOURCE_GPIOE;
        rcu_periph_clock_enable(RCU_GPIOE);
    }
    else if(gpio_periph == GPIOF)
    {
        exti_source_gpio = GPIO_PORT_SOURCE_GPIOF;
        rcu_periph_clock_enable(RCU_GPIOF);
    }
    else if(gpio_periph == GPIOG)
    {
        exti_source_gpio = GPIO_PORT_SOURCE_GPIOG;
        rcu_periph_clock_enable(RCU_GPIOG);
    }

    /* 中断源引脚及线序配置 */
    if(exti_pin == GPIO_PIN_0)
    {
        exti_source_pin = GPIO_PIN_SOURCE_0;
        exti_num = EXTI_0;
    }
    else if(exti_pin == GPIO_PIN_1)
    {
        exti_source_pin = GPIO_PIN_SOURCE_1;
        exti_num = EXTI_1;
    }
    else if(exti_pin == GPIO_PIN_2)
    {
        exti_source_pin = GPIO_PIN_SOURCE_2;
        exti_num = EXTI_2;
    }
    else if(exti_pin == GPIO_PIN_3)
    {
        exti_source_pin = GPIO_PIN_SOURCE_3;
        exti_num = EXTI_3;
    }
    else if(exti_pin == GPIO_PIN_4)
    {
        exti_source_pin = GPIO_PIN_SOURCE_4;
        exti_num = EXTI_4;
    }
    else if(exti_pin == GPIO_PIN_5)
    {
        exti_source_pin = GPIO_PIN_SOURCE_5;
        exti_num = EXTI_5;
    }
    else if(exti_pin == GPIO_PIN_6)
    {
        exti_source_pin = GPIO_PIN_SOURCE_6;
        exti_num = EXTI_6;
    }
    else if(exti_pin == GPIO_PIN_7)
    {
        exti_source_pin = GPIO_PIN_SOURCE_7;
        exti_num = EXTI_7;
    }
    else if(exti_pin == GPIO_PIN_8)
    {
        exti_source_pin = GPIO_PIN_SOURCE_8;
        exti_num = EXTI_8;
    }
    else if(exti_pin == GPIO_PIN_9)
    {
        exti_source_pin = GPIO_PIN_SOURCE_9;
        exti_num = EXTI_9;
    }
    else if(exti_pin == GPIO_PIN_10)
    {
        exti_source_pin = GPIO_PIN_SOURCE_10;
        exti_num = EXTI_10;
    }
    else if(exti_pin == GPIO_PIN_11)
    {
        exti_source_pin = GPIO_PIN_SOURCE_11;
        exti_num = EXTI_11;
    }
    else if(exti_pin == GPIO_PIN_12)
    {
        exti_source_pin = GPIO_PIN_SOURCE_12;
        exti_num = EXTI_12;
    }
    else if(exti_pin == GPIO_PIN_13)
    {
        exti_source_pin = GPIO_PIN_SOURCE_13;
        exti_num = EXTI_13;
    }
    else if(exti_pin == GPIO_PIN_14)
    {
        exti_source_pin = GPIO_PIN_SOURCE_14;
        exti_num = EXTI_14;
    }
    else if(exti_pin == GPIO_PIN_15)
    {
        exti_source_pin = GPIO_PIN_SOURCE_15;
        exti_num = EXTI_15;
    }

    /* 配置GPIO引脚作为中断源 */
    gpio_exti_source_select(exti_source_gpio, exti_source_pin);
    gpio_init(gpio_periph, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, exti_pin);

    /* 中断触发方式初始化 */
    if(Mode == CHANGE)       
    { 
        exti_init((exti_line_enum)exti_num, (exti_mode_enum)EXTI_INTERRUPT, (exti_trig_type_enum)EXTI_TRIG_BOTH);
    }
    else if(Mode == RISING)  
    {
        exti_init((exti_line_enum)exti_num, (exti_mode_enum)EXTI_INTERRUPT,(exti_trig_type_enum) EXTI_TRIG_RISING);
    }
    else if(Mode == FALLING) 
    {
        exti_init((exti_line_enum)exti_num, (exti_mode_enum)EXTI_INTERRUPT, (exti_trig_type_enum)EXTI_TRIG_FALLING);
    }

    /* 清除中断标志位 */
    exti_interrupt_flag_clear((exti_line_enum)exti_num);

    /* 使能外部中断 */
    exti_interrupt_enable((exti_line_enum)exti_num);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        关闭引脚外部中断
// @param        exti_pin            选择的引脚：GPIO_PIN_x(x=0..15)
// @return       void
// Sample usage:        exti_disable(GPIO_PIN_0);
                        //关闭引脚PX0的外部中断（X=A、B、C、F）
//-------------------------------------------------------------------------------------------------------------------
void exti_disable(uint32_t exti_pin)
{
    uint32_t exti_num;

    if(exti_pin == GPIO_PIN_0)        
    {
        exti_num = EXTI_0;
    }
    else if(exti_pin == GPIO_PIN_1)   
    {
        exti_num = EXTI_1;
    }
    else if(exti_pin == GPIO_PIN_2)   
    {
        exti_num = EXTI_2;
    }
    else if(exti_pin == GPIO_PIN_3)   
    {
        exti_num = EXTI_3;
    }
    else if(exti_pin == GPIO_PIN_4)  
    {
        exti_num = EXTI_4;
    }
    else if(exti_pin == GPIO_PIN_5)  
    {
        exti_num = EXTI_5;
    }
    else if(exti_pin == GPIO_PIN_6)   
    {
        exti_num = EXTI_6;
    }
    else if(exti_pin == GPIO_PIN_7)   
    {
        exti_num = EXTI_7;
    }
    else if(exti_pin == GPIO_PIN_8)  
    {
        exti_num = EXTI_8;
    }
    else if(exti_pin == GPIO_PIN_9)   
    {
        exti_num = EXTI_9;
    }
    else if(exti_pin == GPIO_PIN_10)  
    {
        exti_num = EXTI_10;
    }
    else if(exti_pin == GPIO_PIN_11)  
    {
        exti_num = EXTI_11;
    }
    else if(exti_pin == GPIO_PIN_12)  
    {
        exti_num = EXTI_12;
    }
    else if(exti_pin == GPIO_PIN_13)  
    {
        exti_num = EXTI_13;
    }
    else if(exti_pin == GPIO_PIN_13)  
    {
        exti_num = EXTI_13;
    }
    else if(exti_pin == GPIO_PIN_14)  
    {
        exti_num = EXTI_14;
    }
    else if(exti_pin == GPIO_PIN_15)  
    {
        exti_num = EXTI_15;
    }

    /* 失能外部中断 */
    exti_interrupt_disable((exti_line_enum)exti_num);
}
