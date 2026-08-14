#include "base_time.h"

void Tim_Init(void)
{
	timer_parameter_struct tim_struct;
	rcu_periph_clock_enable(RCU_TIMER5);
	
	tim_struct.prescaler = 120-1;//预分频值
	tim_struct.alignedmode = TIMER_COUNTER_EDGE;//对齐模式
	tim_struct.counterdirection = TIMER_COUNTER_UP;//计数器模式
	tim_struct.period = 1000-1;//自动重装载值
	tim_struct.clockdivision = TIMER_CKDIV_DIV1;//时钟分频因子
	timer_init(TIMER5,&tim_struct);

	timer_auto_reload_shadow_enable(TIMER5);
	timer_interrupt_flag_clear(TIMER5,TIMER_INT_FLAG_UP);
	timer_interrupt_enable(TIMER5,TIMER_INT_UP);
	
	nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
	nvic_irq_enable(TIMER5_IRQn,1,1);
	
	timer_enable(TIMER5);
	
}



