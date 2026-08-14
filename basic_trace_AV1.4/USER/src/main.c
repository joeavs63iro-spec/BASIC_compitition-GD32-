/*********************************************************************************************************************
* @file            encoder.c
* @brief           智能车电机编码器例程
* @author          
* @Target core     GD32F303RCT6
* @revisions       2025.6.13, V1.0
* @modify          none
********************************************************************************************************************/

/********************************************详细说明********************************************

* 1. 如果需要从外部调用库函数，只需将所用到的库函数放在CODE文件中，在工程中添加即可成功编译
* 2. 对于用户而言，只需要更改main函数的内容即可实现想要的功能
* 3. 如果需要声明变量、宏定义、函数等内容，可以放在main.h文件中
* 4. 如果需要用到中断，中断服务函数可以在gd32f30x_it.c文件中找到，只需要添加内容即可
* 5. 如果需要写一些说明性的文档，可以放在doc文件夹的readme.txt文件中

* 例程说明：
Move_Distance(100);
Move_ASYtance(-90,90); 
Move_Single_Motor(1, 90)
-此例程基于GD32F303学习板与扩展板, 展示编码器部分功能的使用。

************************************************************************************************/
#include "main.h"
#include "headfile.h"
#include "stdbool.h"
#include "string.h"
volatile int real_error=0;
volatile int base_pwm=6000;
volatile RoundState_t g_round_state = ROUND_NONE;
volatile BridgeState_t g_bridge_state = BRIDGE_NONE;
volatile ParkingState_t g_parking_state = PARKING_NONE;
volatile uint8_t avoid_cnt = 0;
volatile uint8_t track_cnt = 0;
volatile uint8_t parking_cnt = 0;

volatile uint8_t g_mag_cnt = 0;
volatile uint32_t g_round_pulse=0;
volatile uint32_t g_park_pulse=0;
volatile uint32_t g_bridge_pulse=0;
volatile uint32_t g_avoid_pulse=0;
volatile uint32_t g_parking_pulse = 0;
volatile uint32_t g_avoid_haas_pulse = 0;
volatile uint32_t g_msec_count = 0;
volatile uint32_t last_mag_time = 0;

volatile uint8_t reed_pending = 0;
volatile uint32_t reed_trigger_time = 0;
volatile uint8_t reed_last_state = 0;
volatile uint32_t reed_print_time = 0;
volatile uint8_t rgb_request = black;
volatile uint8_t rgb_last = 255;
volatile uint32_t ble_send_timer = 0;
volatile bool check_dist_flag = false;
volatile  int round_cnt = 0;
volatile  int park_cnt = 0;
uint32_t wait_time = 0;
volatile int base_pwm;
volatile int16_t left_pwm;
volatile int16_t right_pwm;
volatile uint32_t g_total_distance = 0;
volatile uint32_t g_stop_count = 0;
uint8_t obs_confirm_cnt = 0;
volatile uint16_t avoid_step_ticks = 0;
bool has_avoided_obs = false;
bool has_bridge = false;
bool has_parked = false;
bool has_roundabout = false;
int bridge_target_error =68;
int avoid_target_error = 100;
uint32_t g_pulse_init = 1;
volatile bool g_flag_cnt[6] = { 0 };
int is_checking_round = 1;
int is_checking_bridge = 1;
int is_checking_obs = 0;
int is_checking_parking=0;
int start_round = 0;
int current_mag_cnt=0;
uint8_t g_check_diff = 0;
volatile uint8_t start_avoid_flag = 1;
extern volatile uint32_t g_millis ;
char send_buffer[64] = {0};
char recv_buffer[64] = {0};
bool recv_data_flag = false;
bool flag=0;
uint8_t set_off=0;
volatile uint8_t has_round_flag = 0;
int main(void)
{
	car_init();
	
//	car_rgb_set_both(red);
//	delay_1ms(1000);
//	car_rgb_set_both(green);
//	delay_1ms(1000);
	track_cnt = 1;
	timer_pit_interrupt_ms(TIMER3,10);
	nvic_irq_enable(TIMER3_IRQn, 15, 0); //TIMER3最低优先级
	while(1)
	{
		ui_task();
		if(reed_pending &&((uint32_t)(g_millis - reed_trigger_time) >= 20))
		{
				reed_pending = 0;
				if(reed_check())
				{
						reed_last_state = 1;
						g_mag_cnt++;
				}
				else
				{
						exti_interrupt_flag_clear(EXTI_5);
						exti_interrupt_enable(EXTI_5);
				}
		}

		if((reed_last_state != 0) && (reed_check() == 0))
		{
				reed_last_state = 0;
				exti_interrupt_flag_clear(EXTI_5);
				exti_interrupt_enable(EXTI_5);
		}
//        reed_print_time++;
//        if(reed_print_time >= 10)
//        {
//            reed_print_time = 0;
//            adc_getdata();
//            printf("loop mag=%d reed=%d dist=%d\r\n", (int)g_mag_cnt, (int)reed_check(), (int)g_total_distance);
//            printf("flag=%d obs=%d avoid=%d adc2=%d\r\n", (int)check_dist_flag, (int)is_checking_obs, (int)avoid_cnt, (int)adc_value[2]);
//        }
	  check_obstacle_trigger();
		check_bridge_trigger();
		check_parking_trigger();
		check_roundabout_trigger();
		if((start_round == 1) && (has_round_flag == 0))
		{
			
				g_round_state = ROUND_OUT;
				delay_1ms(200);
				start_round = 0;
				has_round_flag = 1;
				track_cnt = 1;
				track_PID.error = 0;
				track_PID.last_error = 0;
				track_PID.output_pwm = 0;
				g_round_state = ROUND_NONE;
			has_roundabout=true;
		}
		if(is_checking_obs)
		{
			avoid_obs();
		}
		if (avoid_cnt == 0 && g_bridge_state != BRIDGE_NONE && is_checking_bridge == 1)
		{
			cross_bridge ();
		}
		if(parking_cnt==3&&is_checking_parking == 1)
		{
			delay_1ms(3000);
			g_millis = 0;
			parking_cnt = 4;
		}
		 else if(avoid_cnt == 0&&g_parking_state == PARKING_IN&&(g_millis-g_stop_count)>10000)
		 {
				start_avoid_flag=1;
		 }
		if (g_mag_cnt == 4)
		{
			delay_1ms(300);
			track_cnt = 0;
			motor_forward(right, 0);
			motor_backward(left, 0);
			timer_pit_close(TIMER3);
			break;
		}
	}
	 
}
void TIMER3_IRQHandler(void)
{
if(SET == timer_interrupt_flag_get(TIMER3, TIMER_INT_FLAG_UP))
{
 timer_interrupt_flag_clear(TIMER3, TIMER_INT_FLAG_UP);
 if(g_round_state==ROUND_NONE)
 {
	 if(track_cnt==1)
	 {
		 if(has_parked==false&&has_roundabout ==true)
		 {
			 track_PID.P = 50.0f;
			 base_pwm=5500;
		 }
		 else
		 {
				track_PID.P = 50.0f;
			 base_pwm=6000;
		 }
		  car_rgb_set_both(black);
			adc_getdata ();
			real_error = cal_err ();
			positional_PD_track(real_error, 0, &track_PID);
	 }
	 else if (g_bridge_state == BRIDGE_KEEP && avoid_cnt == 0)
	 {
		  track_PID.P = 20.0f;
		  car_rgb_set_both(white);
			adc_getdata ();
			real_error = cal_err ();
			positional_PD_track (real_error, bridge_target_error, &track_PID);
	 }
	  else if(avoid_cnt==1)
	 {
		 car_rgb_set_both(blue);
		 adc_value[0]=0;
		 adc_value[1]=0;
		 adc_value[2]=1500;
		 adc_value[3]=0;
		 adc_value[4]=1300;
		 if(g_millis>520)
		 {
			 avoid_cnt = 2;
			 g_millis = 0;
		 }
		 real_error = cal_err ();
		 positional_PD_track(real_error, 0, &track_PID);
	 }
	  else if(avoid_cnt == 2)
	 {
			car_rgb_set_both(yellow);
			adc_value[0]=3500;
		  adc_value[1]=0;
		  adc_value[2]=1500;
		  adc_value[3]=0;
		  adc_value[4]=0;
		 if(g_millis >= 750)
		 {
			 avoid_cnt = 3;
			 g_millis = 0;
		 }
		 real_error = cal_err ();
		 positional_PD_track(real_error, 0, &track_PID);
	 }
	 else if(avoid_cnt==3)
	 {
			car_rgb_set_both(red);
			adc_value[0]=0;
			adc_value[1]=0;
			adc_value[2]=1500;
			adc_value[3]=0;
			adc_value[4]=2500;
			 if(g_millis>500)
			 {
				 avoid_cnt = 0;
				 track_cnt = 1;
				 g_millis = 0;
				 is_checking_obs=0;
				 has_avoided_obs=true;
			 }
		 real_error = cal_err ();
		 positional_PD_track(real_error, 0, &track_PID);
	 }
	 
	 else if(parking_cnt==1)
	 {
		 car_rgb_set_both(blue);
		 track_PID.P = 40.0f;
		 base_pwm=5500;
		 adc_value[0]=2500;
		 adc_value[1]=0;
		 adc_value[2]=0;
		 adc_value[3]=0;
		 adc_value[4]=0;
		 if(g_millis>700)
		 {
			 parking_cnt = 2;
			 g_millis = 0;
		 }
		 real_error = cal_err ();
		 positional_PD_track(real_error, 0, &track_PID);
	 }
	  else if(parking_cnt == 2)
	 {
			car_rgb_set_both(cyan);
		  base_pwm=5500;
			adc_value[0]=0;
		  adc_value[1]=0;
		  adc_value[2]=0;
		  adc_value[3]=0;
		  adc_value[4]=2400;
		 if(g_millis > 300)
		 {
				 parking_cnt = 3;
				 g_millis = 0;
		 }
		 real_error = cal_err ();
		 positional_PD_track(real_error, 0, &track_PID);
	 }
	 else if(parking_cnt==3)
	 {
	  car_rgb_set_both(red);
		track_PID.output_pwm = 0;
		base_pwm = 0;
	 }
	 else if(parking_cnt==4)
	 {
		  car_rgb_set_both(green);
			base_pwm = 6000;
			adc_value[0]=0;
			adc_value[1]=0;
			adc_value[2]=0;
			adc_value[3]=0;
			adc_value[4]=800;
		 if(g_millis>280)
		 {
			 parking_cnt = 5;
			 g_millis = 0;
		 }
		 real_error = cal_err ();
		 positional_PD_track(real_error, 0, &track_PID);
	 }
	 else if(parking_cnt == 5)
	 {
			car_rgb_set_both(red);
			adc_value[0]=2500;
			adc_value[1]=0;
			adc_value[2]=1500;
			adc_value[3]=0;
			adc_value[4]=0;
			 if(g_millis>400)
			 {
				 parking_cnt = 0;
				 has_parked=true;
				 track_cnt = 1;
				 g_millis = 0;
				 is_checking_parking=0;
				 g_parking_state = PARKING_IN;
				 g_stop_count=g_millis;
			 }
		 real_error = cal_err ();
		 positional_PD_track(real_error, 0, &track_PID);
	 }
 }
	 else if(g_round_state==ROUND_KEEP&&has_round_flag == 0)
	{
		car_rgb_set_both(black);
    adc_getdata();
		real_error = cal_err ();
		positional_PD_track (real_error, 0, &track_PID);
	}
	else if(g_round_state == ROUND_IN&&has_round_flag ==0)
	{
		car_rgb_set_both(green);
		adc_value[0]=0;
		adc_value[1]=0;
		adc_value[2]=0;
		adc_value[3]=0;
		adc_value[4]=2500;
	  real_error = cal_err ();
		positional_PD_track(real_error, 0, &track_PID);
	}
	if(g_round_state==ROUND_OUT&&has_round_flag == 0)
		{
			motor_forward(right, 8000);
			motor_forward(left, 8000);
			car_rgb_set_both(red);
		}
		else 
		{
			left_pwm = (int32_t)(base_pwm - track_PID.output_pwm);
			right_pwm = (int32_t)(base_pwm + track_PID.output_pwm);
			if(left_pwm < 0) left_pwm = 0;
			if(right_pwm < 0) right_pwm = 0;
			motor_set_pwm(left_pwm,right_pwm);
		}
	}
}
void check_obstacle_trigger(void)
{
    if((start_avoid_flag == 1) && (avoid_cnt == 0))
    {
        is_checking_obs = 1;
    }
    else
    {
        is_checking_obs = 0;
    }
}
void check_parking_trigger(void)
{
    if (avoid_cnt == 0 && g_mag_cnt == 3 && g_flag_cnt[3] == 0)
    {
			g_millis = 0;
			track_cnt = 0;
			g_flag_cnt[3] = 1;
			is_checking_parking = 1;
			car_rgb_set_both(yellow);
			delay_1ms(390);
			parking_cnt=1;
			car_rgb_set_both(red);
    }
}
void check_bridge_trigger(void)
{
	if (avoid_cnt == 0 && g_mag_cnt == 2 && g_flag_cnt[2] == 0)
	{
		g_millis = 0;
	  car_rgb_set_both(yellow);
		track_cnt = 0;
		g_bridge_state = BRIDGE_KEEP;
		g_flag_cnt[2] = 1;
	}
}
void check_roundabout_trigger(void)
{
    if (start_round == 0)
    {
        if (adc_value[2] > 2800 )
        {
					car_rgb_set_both(blue);
					delay_1ms(200);
					start_round = 1;
					g_millis = 0;
        }
    }
   
}
void TIMER4_IRQHandler(void)
{
    if(SET == timer_interrupt_flag_get(TIMER4, TIMER_INT_FLAG_UP))
    {
        timer_interrupt_flag_clear(TIMER4, TIMER_INT_FLAG_UP);

        if(start_avoid_flag == 1)
        {
            check_dist_flag = true;
        }
    }
}
//void EXTI5_9_IRQHandler(void)
//{
//    if(RESET != exti_interrupt_flag_get(EXTI_5))
//    {
//        exti_interrupt_flag_clear(EXTI_5);
//			current_mag_cnt=g_millis ;
//        if((reed_last_state == 0) && reed_check())
//        {
//					if(g_millis-current_mag_cnt >150&&reed_check())
//					{
//            reed_last_state = 1;
//            g_mag_cnt++;
//            exti_interrupt_disable(EXTI_5);
//          }
//        }
//    }
//}
void EXTI5_9_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(EXTI_5))
    {
        exti_interrupt_flag_clear(EXTI_5);

        if((reed_last_state == 0) && (reed_pending == 0))
        {
            reed_trigger_time = g_millis;
            reed_pending = 1;
            exti_interrupt_disable(EXTI_5);
        }
    }
}
void cross_bridge(void)
{
	switch (g_bridge_state)
	{
		case BRIDGE_KEEP:
			if (g_millis > 850)
			{
				g_millis = 0;
				g_bridge_state = BRIDGE_NONE;
				track_cnt = 1;
				is_checking_bridge = 0;
				has_bridge=true;
			}
			break;
			case BRIDGE_NONE:
			break;
	}
}
void avoid_obs(void)
{
	if(avoid_cnt != 0)
	{
		return;
	}
	if (check_dist_flag == true)
	{
			check_dist_flag = false;
			g_total_distance = Get_Distance_TOF400C();
//			if (g_total_distance  < 650 && g_total_distance  > 0)
//			{
//					obs_confirm_cnt++;
//			}
//			else
//			{
//					obs_confirm_cnt = 0;
//			}
//			if (has_avoided_obs == false && obs_confirm_cnt >= 3)
//			{
//					strcpy(send_buffer, "0x02");
//					// timer_pit_close(TIMER4);
//					g_millis = 0;
//					track_cnt = 0;
//					avoid_cnt = 1;
//          obs_confirm_cnt = 0;
//			}
			if (g_total_distance  < 850 && g_total_distance  > 0&&has_avoided_obs == false)
			{
					// timer_pit_close(TIMER4);
					g_millis = 0;
					track_cnt = 0;
					avoid_cnt = 1;
			}
	}
}

void handle_roundabout(void)
{
    if(has_round_flag != 0)
    {
        return;
    }

    if(g_round_state == ROUND_NONE)
    {
        g_millis = 0;
        g_round_state = ROUND_IN;
    }
    else if((g_round_state == ROUND_IN) && (g_millis > 400))
    {
        g_round_state = ROUND_KEEP;
        g_millis = 0;
    }
}