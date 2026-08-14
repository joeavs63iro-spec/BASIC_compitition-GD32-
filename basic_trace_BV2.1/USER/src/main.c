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
#include "ui.h"
#include "flash.h"

#define CH9143_UART             USART1
#define CH9143_UART_BAUD        115200
#define CH9143_UART_TX_PORT     GPIOA
#define CH9143_UART_TX_PIN      GPIO_PIN_2
#define CH9143_UART_RX_PORT     GPIOA
#define CH9143_UART_RX_PIN      GPIO_PIN_3

#define BLE_BUF_SIZE            64
#define BLE_AVOID_MSG           ((uint8_t)0x01U)
#define BLE_PARKING_MSG         ((uint8_t)0x02U)
#define BLE_SEND_PERIOD_MS      100U
#define BLE_PARKING_DELAY_MS    2500U

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
volatile uint8_t start_avoid_flag = 0;
extern volatile uint32_t g_millis ;
char recv_buffer[BLE_BUF_SIZE] = {0};
bool recv_data_flag = false;
bool flag=0;
uint8_t set_off=0;
volatile uint8_t has_round_flag = 0;
static volatile uint8_t ble_avoid_tx_active = 0;
static volatile uint8_t ble_parking_tx_active = 0;
static volatile uint8_t ble_parking_tx_started = 0;
static uint32_t ble_parking_send_timer = 0;
static volatile uint8_t control_10ms_pending = 0;

static void control_10ms_task(void);

static void ch9143_ble_init(void)
{
	uart_init(CH9143_UART_TX_PORT, CH9143_UART_TX_PIN,
	          CH9143_UART_RX_PORT, CH9143_UART_RX_PIN,
	          CH9143_UART, CH9143_UART_BAUD);
}

static void ch9143_ble_send_byte(uint8_t data)
{
	uart_putchar(CH9143_UART, data);
}

static void ble_avoid_tx_start(void)
{
	ble_avoid_tx_active = 1;
	ble_send_timer = g_millis;
	ch9143_ble_send_byte(BLE_AVOID_MSG);
}

static void ble_avoid_tx_stop(void)
{
	ble_avoid_tx_active = 0;
}

static void ble_avoid_tx_task(void)
{
	if((ble_avoid_tx_active != 0) && ((uint32_t)(g_millis - ble_send_timer) >= BLE_SEND_PERIOD_MS))
	{
		ch9143_ble_send_byte(BLE_AVOID_MSG);
		ble_send_timer = g_millis;
	}
}

static void ble_parking_tx_start(void)
{
	ble_parking_tx_active = 1;
	ble_parking_tx_started = 0;
	ble_parking_send_timer = g_millis;
}

static void ble_parking_tx_task(void)
{
	if(ble_parking_tx_active == 0)
	{
		return;
	}

	if((g_millis-g_stop_count)>10000)
	{
		ble_parking_tx_active = 0;
		ble_parking_tx_started = 0;
		return;
	}

	if((g_millis-g_stop_count)>BLE_PARKING_DELAY_MS)
	{
		if((ble_parking_tx_started == 0) || ((uint32_t)(g_millis-ble_parking_send_timer) >= BLE_SEND_PERIOD_MS))
		{
			ch9143_ble_send_byte(BLE_PARKING_MSG);
			ble_parking_send_timer = g_millis;
			ble_parking_tx_started = 1;
		}
	}
}

volatile uint32_t  round_out_ms = 0;
int main(void)
{
	car_init();
	ch9143_ble_init();
//    printf("boot after car_init\r\n");
	flash_init_and_load();
	ui_init();
//	car_rgb_set_both(red);
//	delay_1ms(500);
//	car_rgb_set_both(green);
//	delay_1ms(500);
	track_cnt = 1;
	timer_pit_interrupt_ms(TIMER3,10);
    nvic_irq_enable(TIMER3_IRQn, 15, 0); //TIMER3最低优先级
//    printf("enter while\r\n");
	while(1)
	{
		if(control_10ms_pending != 0U)
		{
			control_10ms_pending = 0U;
			control_10ms_task();
		}

		ui_task();
		ble_avoid_tx_task();
		ble_parking_tx_task();

		if(reed_pending &&((uint32_t)(g_millis - reed_trigger_time) >= 3))
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
			handle_roundabout();
			// 在 main 函数 while(1) 内部替换为：
			if(g_round_state == ROUND_KEEP)
			{
					static uint32_t keep_start_time = 0;
					if(keep_start_time == 0)
					{
							car_rgb_set_both(cyan);
							keep_start_time = g_millis; // 记录进入 KEEP 状态的时间
					}
					
					if(g_millis - keep_start_time > 1000) // 等待满 1 秒
					{
							g_round_state = ROUND_OUT;
							car_rgb_set_both(black);
							round_out_ms = g_millis;
							keep_start_time = 0; // 状态结束，清零计时器
					}
			}
		}
			// 在 main 函数 while(1) 内部替换为：
			if(flag == 1)
			{
					static uint32_t flag_start_time = 0;
					if(flag_start_time == 0) 
					{
							flag_start_time = g_millis;
					}
					
					if(g_millis - flag_start_time > 400) // 等待 400ms
					{
							flag = 0;
							start_round = 0;
							has_round_flag = 1;
							track_cnt = 1;
							track_PID.error = 0;
							track_PID.last_error = 0;
							track_PID.output_pwm = 0;
							g_round_state = ROUND_NONE;
						  has_roundabout=true;
							
							flag_start_time = 0; // 重置
					}
			}
		if(is_checking_obs)
		{
			avoid_obs();
		}
		if(avoid_cnt == 3)
		{
				static uint32_t avoid_wait_start = 0;
				if(avoid_wait_start == 0) 
				{
						avoid_wait_start = g_millis; // 记录开始等待的时间
				}
				
				if(g_millis - avoid_wait_start > 2600) // 时间到了 3 秒
				{
						g_millis = 0;
						avoid_cnt = 4;
						ble_avoid_tx_stop();
						avoid_wait_start = 0; // 清零标志，为下次准备
				}
		}
		if (avoid_cnt == 0 && g_bridge_state != BRIDGE_NONE && is_checking_bridge == 1)
		{
			cross_bridge ();
		}
		else if (avoid_cnt == 0 && (g_millis-g_stop_count)>700&&g_parking_state == PARKING_WAIT)//添加接收到蓝牙0x03消息判断条件
		 {
			 g_parking_state = PARKING_OUT;
			 track_PID.error = 0;
			 track_PID.last_error = 0;
			 track_PID.output_pwm = 0;
			 car_rgb_set_both(black);
			 g_park_pulse=0;
			 g_millis = 0;
			 nvic_irq_enable(TIMER3_IRQn, 15, 0);
			 timer_enable(TIMER3);
		 }
		 else if(avoid_cnt == 0&&g_parking_state == PARKING_IN&&(g_millis-g_stop_count)>10000)
		 {
				ble_parking_tx_active = 0;
				ble_parking_tx_started = 0;
				start_avoid_flag=1;
		 }
		if (g_mag_cnt == 4)
		{
			delay_1ms(250);
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
 control_10ms_pending = 1U;
}
}

static void control_10ms_task(void)
{
 if(g_round_state==ROUND_NONE)
 {
	 if(track_cnt==1||g_bridge_state == BRIDGE_KEEP)
	 {				
		 if(has_parked)
		 {
			 	track_PID.P=30.0f;
		 }
		 else
		 {
			 	track_PID.P=35.0f;
		 }
		  car_rgb_set_both(black);
			adc_getdata ();
			real_error = cal_err ();
			positional_PD_track(real_error, 0, &track_PID);
	 }
	  else if(avoid_cnt==1)
	 {
		 car_rgb_set_both(blue);
		 adc_value[0]=3500;
		 adc_value[1]=0;
		 adc_value[2]=1500;
		 adc_value[3]=0;
		 adc_value[4]=0;
		 if(g_millis>330)
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
			adc_value[0]=0;
		  adc_value[1]=0;
		  adc_value[2]=1500;
		  adc_value[3]=0;
		  adc_value[4]=2000;
		 if(g_millis >= 350)
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
		track_PID.output_pwm = 0;
		base_pwm = 0;
	 }
	 else if(avoid_cnt==4)
	 {
		  car_rgb_set_both(green);
			base_pwm = 6000;
			adc_value[0]=0;
			adc_value[1]=0;
			adc_value[2]=1500;
			adc_value[3]=0;
			adc_value[4]=3000;
		 if(g_millis>480)
		 {
			 avoid_cnt = 5;
			 g_millis = 0;
		 }
		 real_error = cal_err ();
		 positional_PD_track(real_error, 0, &track_PID);
	 }
	 else if(avoid_cnt == 5)
	 {
			car_rgb_set_both(red);
			adc_value[0]=1500;
			adc_value[1]=0;
			adc_value[2]=1500;
			adc_value[3]=0;
			adc_value[4]=0;
			 if(g_millis>300)
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
 }
	 else if(g_round_state==ROUND_KEEP&&has_round_flag == 0)
	{
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
	if(g_parking_state == PARKING_OUT)
	{
		motor_forward(right, 6000);
		motor_forward(left, 6000);
		car_rgb_set_both(green);
		if(g_millis>100)
		{
			  base_pwm=6000;
				track_PID.error = 0;
				track_PID.last_error = 0;
				track_PID.output_pwm = 0;
				track_cnt=1;
				g_parking_state = PARKING_IN;
				g_stop_count = g_millis;
				ble_parking_tx_start();
		  	has_parked=true;
			}
		}
		else if(g_round_state==ROUND_OUT&&has_round_flag == 0)
		{
			adc_getdata();
			if(adc_value [2]>2275)
			{
				flag=1;
			}
			if(flag)
			{
//				motor_forward(right, 5000);
//				motor_forward(left, 8000);
//				car_rgb_set_both(red);
				car_rgb_set_both(green);
				adc_value[0]=0;
				adc_value[1]=0;
				adc_value[2]=1500;
				adc_value[3]=0;
				adc_value[4]=1100;
				real_error = cal_err ();
				positional_PD_track(real_error, 0, &track_PID);
			}
			else
			{
				real_error = cal_err ();
				track_PID.P=20.0f;
				positional_PD_track (real_error, 95, &track_PID);
			}
				left_pwm = (int32_t)(base_pwm - track_PID.output_pwm);
				right_pwm = (int32_t)(base_pwm + track_PID.output_pwm);
				if(left_pwm < 0) left_pwm = 0;
				if(right_pwm < 0) right_pwm = 0;
				motor_set_pwm(left_pwm,right_pwm);
//			motor_forward(right, 8000);
//			motor_forward(left, 8000);
//			car_rgb_set_both(red);
		}
		else
		{
			if(has_roundabout==1&&has_parked==0)
			{
				base_pwm=5500;
			}
			left_pwm = (int32_t)(base_pwm - track_PID.output_pwm);
			right_pwm = (int32_t)(base_pwm + track_PID.output_pwm);
			if(left_pwm < 0) left_pwm = 0;
			if(right_pwm < 0) right_pwm = 0;
			motor_set_pwm(left_pwm,right_pwm);
		}
}
void check_obstacle_trigger(void)
{
    if((start_avoid_flag == 1) && (avoid_cnt == 0)&&is_checking_obs == 0)
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
			g_parking_state = PARKING_WAIT;
			g_flag_cnt[3] = 1;
			is_checking_parking = 1;
			g_stop_count=g_millis;
			motor_forward(left,0);
			motor_forward(right,0);
			track_PID.output_pwm = 0;
			base_pwm = 0;
			timer_disable(TIMER3);
			car_rgb_set_both(red);
    }
}
void check_bridge_trigger(void)
{
	if (avoid_cnt == 0 && g_mag_cnt == 2 && g_flag_cnt[2] == 0)
	{
		track_cnt = 0;
		car_rgb_set_both(yellow);
		g_bridge_state = BRIDGE_KEEP;
		g_flag_cnt[2] = 1;
	}
}
void check_roundabout_trigger(void)
{
    static uint32_t trigger_start_time = 0;
    static bool is_triggering = false;

    if (start_round == 0)
    {
        if (adc_value[2] > 3000 && !is_triggering)
        {
            car_rgb_set_both(blue);
            trigger_start_time = g_millis;
            is_triggering = true; // 开始倒计时
        }
        
        // 当处于触发倒计时中，并且过了200ms
        if (is_triggering && (g_millis - trigger_start_time > 200))
        {
            start_round = 1;
            g_millis = 0;
            is_triggering = false; // 触发完成
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
			if (g_pulse_init == 0)
			{
				g_pulse_init = 1;
			}
			if (g_pulse_init == 1&&g_millis > 4800)
			{
				g_millis = 0;
				g_bridge_state = BRIDGE_NONE;
				g_pulse_init = 0;
				track_cnt = 1;
				is_checking_bridge = 0;
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
			g_total_distance = ultra_get_distance();
			if (g_total_distance  < 60 && g_total_distance  > 0)
			{
					obs_confirm_cnt++;
			}
			else
			{
					obs_confirm_cnt = 0;
			}
			if (has_avoided_obs == false && obs_confirm_cnt >= 3)
			{
					// timer_pit_close(TIMER4);
					g_millis = 0;
					track_cnt = 0;
					avoid_cnt = 1;
					ble_avoid_tx_start();
          obs_confirm_cnt = 0;
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
    else if((g_round_state == ROUND_IN) && (g_millis > 280))
    {
        g_round_state = ROUND_KEEP;
        g_millis = 0;
    }
}
