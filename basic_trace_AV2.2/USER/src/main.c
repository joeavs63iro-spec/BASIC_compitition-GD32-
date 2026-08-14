/*********************************************************************************************************************
* @file            encoder.c
* @brief           鏅鸿兘杞︾數鏈虹紪鐮佸櫒渚嬬▼
* @author          
* @Target core     GD32F303RCT6
* @revisions       2025.6.13, V1.0
* @modify          none
********************************************************************************************************************/

/********************************************璇︾粏璇存槑********************************************

* 1. 濡傛灉闇€瑕佷粠澶栭儴璋冪敤搴撳嚱鏁帮紝鍙渶灏嗘墍鐢ㄥ埌鐨勫簱鍑芥暟鏀惧湪CODE鏂囦欢涓紝鍦ㄥ伐绋嬩腑娣诲姞鍗冲彲鎴愬姛缂栬瘧
* 2. 瀵逛簬鐢ㄦ埛鑰岃█锛屽彧闇€瑕佹洿鏀筸ain鍑芥暟鐨勫唴瀹瑰嵆鍙疄鐜版兂瑕佺殑鍔熻兘
* 3. 濡傛灉闇€瑕佸０鏄庡彉閲忋€佸畯瀹氫箟銆佸嚱鏁扮瓑鍐呭锛屽彲浠ユ斁鍦╩ain.h鏂囦欢涓?
* 4. 濡傛灉闇€瑕佺敤鍒颁腑鏂紝涓柇鏈嶅姟鍑芥暟鍙互鍦╣d32f30x_it.c鏂囦欢涓壘鍒帮紝鍙渶瑕佹坊鍔犲唴瀹瑰嵆鍙?
* 5. 濡傛灉闇€瑕佸啓涓€浜涜鏄庢€х殑鏂囨。锛屽彲浠ユ斁鍦╠oc鏂囦欢澶圭殑readme.txt鏂囦欢涓?

* 渚嬬▼璇存槑锛?
Move_Distance(100);
Move_ASYtance(-90,90); 
Move_Single_Motor(1, 90)
-姝や緥绋嬪熀浜嶨D32F303瀛︿範鏉夸笌鎵╁睍鏉? 灞曠ず缂栫爜鍣ㄩ儴鍒嗗姛鑳界殑浣跨敤銆?

************************************************************************************************/
#include "main.h"
#include "headfile.h"
#include "stdbool.h"
#include "string.h"

#define CH9143_UART             USART1
#define CH9143_UART_IRQ         USART1_IRQn
#define CH9143_UART_BAUD        115200
#define CH9143_UART_TX_PORT     GPIOA
#define CH9143_UART_TX_PIN      GPIO_PIN_2
#define CH9143_UART_RX_PORT     GPIOA
#define CH9143_UART_RX_PIN      GPIO_PIN_3

#define BLE_BUF_SIZE            64
#define BLE_NO_DATA             0xFFFFU
#define BLE_AVOID_DISTANCE_MM   800U
#define TRACK_MIN_DRIVE_PWM     1200
#define TRACK_MAX_DUTY          10000
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
volatile uint8_t tof_data_ready = 0;
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
int bridge_target_error = 60;
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
char send_buffer[BLE_BUF_SIZE] = {0};
char recv_buffer[BLE_BUF_SIZE] = {0};
uint16_t recv_data_flag = BLE_NO_DATA;
bool flag=0;
uint8_t set_off=0;
volatile uint8_t has_round_flag = 0;
static volatile char ch9143_rx_buffer[BLE_BUF_SIZE] = {0};
static volatile uint8_t ch9143_rx_len = 0;
static volatile uint8_t ch9143_packet_ready = 0;
static volatile uint8_t ble_avoid_armed = 0;
static volatile uint8_t ble_parking_cmd_pending = 0;
static volatile uint8_t control_10ms_pending = 0;
static uint32_t round_out_wait_start = 0;
static void control_10ms_task(void);
static void track_pid_reset_output(void);
static void motor_apply_track_pwm(void);
static void recover_invalid_motion_state(void);
static void positional_PD_track_with_p(int real_error_value, int target_error_value, float p_value);

static void ch9143_ble_init(void)
{
	uart_init(CH9143_UART_TX_PORT, CH9143_UART_TX_PIN,
	          CH9143_UART_RX_PORT, CH9143_UART_RX_PIN,
	          CH9143_UART, CH9143_UART_BAUD);
	uart_interrupt_init(CH9143_UART, USART_INT_RBNE, 1, CH9143_UART_IRQ);
}

static void track_pid_reset_output(void)
{
	track_PID.error = 0;
	track_PID.last_error = 0;
	track_PID.output_pwm = 0;
}

static void motor_apply_track_pwm(void)
{
	int16_t turn_pwm = track_PID.output_pwm;
	int16_t max_turn_pwm;

	if(base_pwm <= 0)
	{
		left_pwm = 0;
		right_pwm = 0;
		motor_set_pwm(0, 0);
		return;
	}

	if(base_pwm > TRACK_MAX_DUTY)
	{
		base_pwm = TRACK_MAX_DUTY;
	}

	if(base_pwm > TRACK_MIN_DRIVE_PWM)
	{
		max_turn_pwm = (int16_t)(base_pwm - TRACK_MIN_DRIVE_PWM);
	}
	else
	{
		max_turn_pwm = 0;
	}

	if(turn_pwm > max_turn_pwm) turn_pwm = max_turn_pwm;
	if(turn_pwm < -max_turn_pwm) turn_pwm = -max_turn_pwm;

	left_pwm = (int16_t)(base_pwm - turn_pwm);
	right_pwm = (int16_t)(base_pwm + turn_pwm);

	if(left_pwm < 0) left_pwm = 0;
	if(right_pwm < 0) right_pwm = 0;
	if(left_pwm > TRACK_MAX_DUTY) left_pwm = TRACK_MAX_DUTY;
	if(right_pwm > TRACK_MAX_DUTY) right_pwm = TRACK_MAX_DUTY;

	motor_set_pwm(left_pwm, right_pwm);
}

static void positional_PD_track_with_p(int real_error_value, int target_error_value, float p_value)
{
	float keep_p = track_PID.P;

	track_PID.P = p_value;
	positional_PD_track(real_error_value, target_error_value, &track_PID);
	track_PID.P = keep_p;
}

static void recover_invalid_motion_state(void)
{
	if((g_round_state == ROUND_NONE) && (start_round != 0))
	{
		start_round = 0;
		round_out_wait_start = 0;
	}

	if((g_round_state == ROUND_IN) || (g_round_state == ROUND_KEEP))
	{
		start_round = 0;
		track_cnt = 0;
		round_out_wait_start = g_millis;
		g_round_state = ROUND_OUT;
		track_pid_reset_output();
	}

	if((g_round_state == ROUND_OUT) && ((has_round_flag != 0) ||
	   (avoid_cnt != 0) || (parking_cnt != 0) || (g_bridge_state != BRIDGE_NONE)))
	{
		start_round = 0;
		round_out_wait_start = 0;
		g_round_state = ROUND_NONE;
		track_pid_reset_output();
	}

	if((track_cnt == 0) && (avoid_cnt == 0) && (parking_cnt == 0) &&
	   (g_bridge_state == BRIDGE_NONE) && (g_round_state == ROUND_NONE) &&
	   (g_mag_cnt != 4))
	{
		track_cnt = 1;
		track_pid_reset_output();
	}
}

void ai_wb2_01s_ch9143_rx_byte(uint8_t data)
{
	if(ch9143_packet_ready)
	{
		return;
	}

	if((data == 0x01U) || (data == 0x02U))
	{
		memset((void *)ch9143_rx_buffer, 0, BLE_BUF_SIZE);
		ch9143_rx_buffer[0] = (char)data;
		ch9143_rx_len = 1;
		ch9143_packet_ready = 1;
		return;
	}

	if((data == '\r') || (data == '\n'))
	{
		if(ch9143_rx_len != 0U)
		{
			ch9143_packet_ready = 1;
		}
		return;
	}

	if(ch9143_rx_len >= (BLE_BUF_SIZE - 1U))
	{
		ch9143_rx_len = 0;
		memset((void *)ch9143_rx_buffer, 0, BLE_BUF_SIZE);
	}

	ch9143_rx_buffer[ch9143_rx_len++] = (char)data;
	ch9143_rx_buffer[ch9143_rx_len] = '\0';

	if((ch9143_rx_len >= 4U) ||
	   ((ch9143_rx_len >= 2U) && (ch9143_rx_buffer[0] == '0') &&
	    ((ch9143_rx_buffer[1] == '1') || (ch9143_rx_buffer[1] == '2'))))
	{
		ch9143_packet_ready = 1;
	}
}

static uint16_t ai_wb2_01s_ble_recv_data(char *out_buffer)
{
	uint8_t i;
	uint8_t len;

	if(!ch9143_packet_ready)
	{
		return BLE_NO_DATA;
	}

	len = ch9143_rx_len;
	if(len >= BLE_BUF_SIZE)
	{
		len = BLE_BUF_SIZE - 1U;
	}

	for(i = 0; i < len; i++)
	{
		out_buffer[i] = (char)ch9143_rx_buffer[i];
	}
	out_buffer[len] = '\0';

	ch9143_rx_len = 0;
	ch9143_packet_ready = 0;
	memset((void *)ch9143_rx_buffer, 0, BLE_BUF_SIZE);

	return len;
}

static uint8_t ble_recv_is_avoid_cmd(const char *buffer, uint16_t len)
{
	if((len == 1U) && (((uint8_t)buffer[0]) == 0x01U))
	{
		return 1U;
	}

	if((len == 2U) && (buffer[0] == '0') && (buffer[1] == '1'))
	{
		return 1U;
	}

	if((len == 4U) && (buffer[0] == '0') &&
	   ((buffer[1] == 'x') || (buffer[1] == 'X')) &&
	   (buffer[2] == '0') && (buffer[3] == '1'))
	{
		return 1U;
	}

	return 0U;
}

static uint8_t ble_recv_is_parking_cmd(const char *buffer, uint16_t len)
{
	if((len == 1U) && (((uint8_t)buffer[0]) == 0x02U))
	{
		return 1U;
	}

	if((len == 2U) && (buffer[0] == '0') && (buffer[1] == '2'))
	{
		return 1U;
	}

	if((len == 4U) && (buffer[0] == '0') &&
	   ((buffer[1] == 'x') || (buffer[1] == 'X')) &&
	   (buffer[2] == '0') && (buffer[3] == '2'))
	{
		return 1U;
	}

	return 0U;
}

static void ble_recv_task(void)
{
	recv_data_flag = ai_wb2_01s_ble_recv_data(recv_buffer);

	if((recv_data_flag == BLE_NO_DATA) || (recv_data_flag == 0U))
	{
		return;
	}

	if(ble_recv_is_parking_cmd(recv_buffer, recv_data_flag))
	{
		if((is_checking_parking == 1) && (parking_cnt == 3U))
		{
			ble_parking_cmd_pending = 1U;
		}
		memset(recv_buffer, 0, sizeof(recv_buffer));
		return;
	}

	if(ble_recv_is_avoid_cmd(recv_buffer, recv_data_flag))
	{
		ble_avoid_armed = 1;
		start_avoid_flag = 1;
	}

	memset(recv_buffer, 0, sizeof(recv_buffer));
}

int main(void)
{
	car_init();
	ch9143_ble_init();
		flash_init_and_load();
	ui_init();
//	car_rgb_set_both(red);
//	delay_1ms(1000);
//	car_rgb_set_both(green);
//	delay_1ms(1000);
	track_cnt = 1;
	timer_pit_interrupt_ms(TIMER3,10);
	nvic_irq_enable(TIMER3_IRQn, 15, 0); //TIMER3鏈€浣庝紭鍏堢骇
  while(1)
	{

		if(control_10ms_pending != 0U)
		{
			control_10ms_pending = 0U;
			control_10ms_task();
		}
		ui_task();
		ble_recv_task();

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
		check_obstacle_trigger();
		check_bridge_trigger();
		check_parking_trigger();
		check_roundabout_trigger();

		if((g_round_state == ROUND_OUT) && (has_round_flag == 0) &&
		   (avoid_cnt == 0) && (parking_cnt == 0) && (g_bridge_state == BRIDGE_NONE))
		{
			if(round_out_wait_start == 0)
			{
				round_out_wait_start = g_millis;
			}

			if((uint32_t)(g_millis - round_out_wait_start) > (uint32_t)t_round)
			{
				start_round = 0;
				track_cnt = 1;
				has_round_flag = 1;
				g_round_state = ROUND_NONE;
				round_out_wait_start = 0;
				track_pid_reset_output();
			}
		}
		else
		{
			if(g_round_state == ROUND_OUT)
			{
				g_round_state = ROUND_NONE;
				track_pid_reset_output();
			}
			start_round = 0;
			round_out_wait_start = 0;
		}
		// Consume each interrupt-driven TOF sample so GPIO1/INT can release and
		// the next measurement can generate a fresh falling edge.
		avoid_obs();
		
//		if (avoid_cnt == 0 && g_bridge_state != BRIDGE_NONE && is_checking_bridge == 1)
//		{
//			cross_bridge();
//		}

		// 銆愪慨鏀?锛氶潪闃诲鐨勫仠杞﹂樁娈?4000ms 绛夊緟銆?
		if(parking_cnt == 3 && is_checking_parking == 1)
		{
			if(ble_parking_cmd_pending != 0U)
			{
				ble_parking_cmd_pending = 0U;
				track_pid_reset_output();
				parking_cnt = 4;
			}
		}
		else
		{
			if(avoid_cnt == 0 && g_parking_state == PARKING_IN)
			{
				start_avoid_flag = 1;
			}
		}
		if (g_mag_cnt == 4)
		{
			delay_1ms(450); // 鏁翠釜姣旇禌缁撴潫鍋滆溅鍓嶇殑鏈€鍚庡欢鏃讹紝涓嶅奖鍝嶈繍琛岄€昏緫锛屼繚鐣欏嵆鍙?
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
		recover_invalid_motion_state();

		
		// 锛侊紒锛佹柊澧烇細鐢ㄤ簬鑷姩杩借釜鐘舵€佸紑濮嬫椂闂寸殑鍙橀噺 锛侊紒锛?
		static uint32_t avoid_start_ms = 0;
		static uint8_t last_avoid_cnt = 0;
		static uint32_t park_start_ms = 0;
		static uint8_t last_parking_cnt = 0;
		static uint32_t bridge_start_ms = 0;
		static BridgeState_t last_bridge_state = BRIDGE_NONE;
		static uint32_t round_start_ms = 0;
		static RoundState_t last_round_state = ROUND_NONE;

		// 鑷姩鏇存柊鐘舵€佹椂闂存埑
		if(avoid_cnt != last_avoid_cnt) { avoid_start_ms = g_millis; last_avoid_cnt = avoid_cnt; track_pid_reset_output(); }
		if(parking_cnt != last_parking_cnt) { park_start_ms = g_millis; last_parking_cnt = parking_cnt; track_pid_reset_output(); }
		if(g_bridge_state != last_bridge_state) { bridge_start_ms = g_millis; last_bridge_state = g_bridge_state; track_pid_reset_output(); }
		if(g_round_state != last_round_state) { round_start_ms = g_millis; last_round_state = g_round_state; track_pid_reset_output(); }

        if(g_round_state == ROUND_NONE)
        {
            if(track_cnt == 1)
            {
                if(has_parked == false && has_bridge == true) 
								{
									base_pwm = (int)used_speed_duty;
								}
                else 
								{								
									base_pwm = (int)target_speed_duty;
								}
                car_rgb_set_both(black);
                adc_getdata();
                real_error = cal_err();
                positional_PD_track(real_error, 0, &track_PID);
            }
            else if (g_bridge_state == BRIDGE_KEEP && avoid_cnt == 0)
            {
                car_rgb_set_both(white);
                adc_getdata();
                real_error = cal_err();
                positional_PD_track_with_p(real_error, bridge_target_error, 16.0f);
				
				// 妗ョ殑鏃堕棿鍒ゅ畾鏁村悎鍒拌繖閲岋紙鏇夸唬鍘熸潵鐨?cross_bridge锛?
				if(g_millis - bridge_start_ms > 960)
				{
					g_bridge_state = BRIDGE_NONE;
					track_cnt = 1;
					is_checking_bridge = 0;
					has_bridge = true;
				}
            }
            else if(avoid_cnt == 1)
            {
                car_rgb_set_both(blue);
                adc_value[0]=0; adc_value[1]=0; adc_value[2]=1500; adc_value[3]=0; adc_value[4]=1000;
                if(g_millis - avoid_start_ms > t_avoid_1) avoid_cnt = 2; // 涓嶅啀娓呴浂 g_millis锛?
                real_error = cal_err();
                positional_PD_track(real_error, 0, &track_PID);
            }
            else if(avoid_cnt == 2)
            {
                car_rgb_set_both(yellow);
                adc_value[0]=3300; adc_value[1]=0; adc_value[2]=1500; adc_value[3]=0; adc_value[4]=0;
                if(g_millis - avoid_start_ms > t_avoid_2) avoid_cnt = 3;
                real_error = cal_err();
                positional_PD_track(real_error, 0, &track_PID);
            }
            else if(avoid_cnt == 3)
            {
                car_rgb_set_both(red);
                adc_value[0]=0; adc_value[1]=0; adc_value[2]=1500; adc_value[3]=0; adc_value[4]=2800;
                if(g_millis - avoid_start_ms > t_avoid_3)
                {
                    avoid_cnt = 0;
                    track_cnt = 1;
                    is_checking_obs = 0;
                    has_avoided_obs = true;
                }
                real_error = cal_err();
                positional_PD_track(real_error, 0, &track_PID);
            }
            else if(parking_cnt == 1)
            {
                car_rgb_set_both(blue);
                base_pwm = (int)used_speed_duty;
                adc_value[0]=a_parking_1; adc_value[1]=0; adc_value[2]=0; adc_value[3]=0; adc_value[4]=0;
                if(g_millis - park_start_ms > t_parking_2) parking_cnt = 2;
                real_error = cal_err();
                positional_PD_track_with_p(real_error, 0, 40.0f);
            }
            else if(parking_cnt == 2)
            {
                car_rgb_set_both(cyan);
                base_pwm = (int)used_speed_duty;
                adc_value[0]=0; adc_value[1]=0; adc_value[2]=0; adc_value[3]=0; adc_value[4]=a_parking_2;
                if(g_millis - park_start_ms > t_parking_3) parking_cnt = 3;
                real_error = cal_err();
                positional_PD_track(real_error, 0, &track_PID);
            }
            else if(parking_cnt == 3)
            {
                car_rgb_set_both(red);
                track_PID.output_pwm = 0;
                base_pwm = 0;
            }
            else if(parking_cnt == 4)
            {
                car_rgb_set_both(green);
                base_pwm = (int)target_speed_duty;
                adc_value[0]=0; adc_value[1]=0; adc_value[2]=0; adc_value[3]=0; adc_value[4]=a_parking_4;
                if(g_millis - park_start_ms > t_parking_4) parking_cnt = 5;
                real_error = cal_err();
                positional_PD_track(real_error, 0, &track_PID);
            }
            else if(parking_cnt == 5)
            {
                car_rgb_set_both(red);
                adc_value[0]=a_parking_5; adc_value[1]=0; adc_value[2]=1500; adc_value[3]=0; adc_value[4]=0;
                if(g_millis - park_start_ms > t_parking_5)
                {
                    parking_cnt = 0;
                    ble_parking_cmd_pending = 0U;
                    has_parked = true;
                    track_cnt = 1;
                    is_checking_parking = 0;
                    g_parking_state = PARKING_IN;
                    g_stop_count = g_millis; // 璁板綍鍋滆溅瀹屾垚鏃堕棿
                }
                real_error = cal_err();
                positional_PD_track(real_error, 0, &track_PID);
            }
            else
            {
                base_pwm = 0;
                track_pid_reset_output();
            }
        }
        else if(g_round_state == ROUND_KEEP && has_round_flag == 0)
        {
            car_rgb_set_both(black);
            adc_getdata();
            real_error = cal_err();
            positional_PD_track(real_error, 0, &track_PID);
        }
        else if(g_round_state == ROUND_IN && has_round_flag == 0)
        {
            car_rgb_set_both(green);
            adc_value[0]=0; adc_value[1]=0; adc_value[2]=0; adc_value[3]=0; adc_value[4]=2500;
            real_error = cal_err();
            positional_PD_track(real_error, 0, &track_PID);
			
			// 杩涚幆宀涚殑鏃堕棿鍒ゅ畾鏁村悎鍒拌繖閲岋紙鏇夸唬鍘熸潵鐨?handle_roundabout锛?
			if(g_millis - round_start_ms > t_round)
			{
				g_round_state = ROUND_KEEP;
			}
        }

        if(g_round_state == ROUND_OUT && has_round_flag == 0)
        {
            motor_forward(right, 8000);
            motor_forward(left, 8000);
            car_rgb_set_both(red);
        }
        else
        {
            motor_apply_track_pwm();
        }
}
void check_obstacle_trigger(void)
{
    if((start_avoid_flag == 1) && (ble_avoid_armed != 0) && (avoid_cnt == 0) &&
       (parking_cnt == 0) && (start_round == 0) &&
       (g_bridge_state == BRIDGE_NONE) && (g_round_state == ROUND_NONE))
    {
        is_checking_obs = 1;
    }
    else
    {
        is_checking_obs = 0;
    }
}
// 銆愪慨鏀?锛氶潪闃诲鐨勫仠杞﹁Е鍙?390ms 绛夊緟銆?
void check_parking_trigger(void)
{
	static uint32_t park_trig_start = 0;
	static bool is_parking_triggering = false;

    if (avoid_cnt == 0 && parking_cnt == 0 && start_round == 0 &&
        g_bridge_state == BRIDGE_NONE && g_round_state == ROUND_NONE &&
        g_mag_cnt == 3 && g_flag_cnt[3] == 0 && !is_parking_triggering)
    {
		track_cnt = 1;
		is_checking_parking = 1;
		car_rgb_set_both(yellow);
	
		park_trig_start = g_millis;
		is_parking_triggering = true;
    }
	if(is_parking_triggering && (g_millis - park_trig_start > t_parking_1))
	{
		track_cnt = 0;
		track_pid_reset_output();
		parking_cnt = 1;
		ble_parking_cmd_pending = 0U;
		car_rgb_set_both(red);
		g_flag_cnt[3] = 1;
		is_parking_triggering = false;
	}
}

// 銆愪慨鏀?锛氶潪闃诲鐨勭幆宀涜Е鍙?200ms 绛夊緟銆?
void check_roundabout_trigger(void)
{
	static uint32_t round_trig_start = 0;
	static bool is_round_triggering = false;

	if ((is_checking_round == 0) || (start_round != 0) || (has_round_flag != 0) ||
	    (avoid_cnt != 0) || (parking_cnt != 0) || (track_cnt == 0) ||
	    (g_bridge_state != BRIDGE_NONE) || (g_round_state != ROUND_NONE))
	{
		is_round_triggering = false;
		return;
	}

	if (adc_value[2] > 3000)
	{
		if (!is_round_triggering)
		{
			car_rgb_set_both(blue);
			round_trig_start = g_millis;
			is_round_triggering = true;
		}

		if ((uint32_t)(g_millis - round_trig_start) > 20U)
		{
			start_round = 0;
			track_cnt = 0;
			track_pid_reset_output();
			g_round_state = ROUND_OUT;
			round_out_wait_start = g_millis;
			is_checking_round = 0;
			is_round_triggering = false;
		}
	}
	else
	{
		is_round_triggering = false;
	}
}
void check_bridge_trigger(void)
{
	if (avoid_cnt == 0 && parking_cnt == 0 && g_bridge_state == BRIDGE_NONE &&
	    g_mag_cnt == 2 && g_flag_cnt[2] == 0)
	{
		car_rgb_set_both(yellow);
		start_round = 0;
		round_out_wait_start = 0;
		g_round_state = ROUND_NONE;
		track_pid_reset_output();
		track_cnt = 0;
		has_round_flag = 1;
		g_bridge_state = BRIDGE_KEEP;
		g_flag_cnt[2] = 1;
	}
}
void avoid_obs(void)
{
	if(avoid_cnt != 0) return;
	
	if (tof_data_ready != 0)
	{
		tof_data_ready = 0;
		g_total_distance = Get_Distance_TOF400C();

		if (is_checking_obs && (ble_avoid_armed != 0) && g_total_distance <= BLE_AVOID_DISTANCE_MM && g_total_distance > 0 && has_avoided_obs == false)
		{
			// 鍘绘帀 g_millis = 0;
			ble_avoid_armed = 0;
			start_avoid_flag = 0;
			track_cnt = 0;
			track_pid_reset_output();
			avoid_cnt = 1;
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
	static uint32_t bridge_start_ms = 0;

	switch (g_bridge_state)
	{
		case BRIDGE_KEEP:
			if(bridge_start_ms == 0)
			{
				bridge_start_ms = g_millis;
			}
			if ((uint32_t)(g_millis - bridge_start_ms) > 850)
			{
				g_bridge_state = BRIDGE_NONE;
				track_cnt = 1;
				is_checking_bridge = 0;
				has_bridge=true;
				bridge_start_ms = 0;
			}
			break;
			case BRIDGE_NONE:
			bridge_start_ms = 0;
			break;
	}
}
void handle_roundabout(void)
{
    static uint32_t round_start_ms = 0;

    if(has_round_flag != 0)
    {
        round_start_ms = 0;
        return;
    }

    if(g_round_state == ROUND_NONE)
    {
        round_start_ms = g_millis;
        g_round_state = ROUND_IN;
    }
    else if((g_round_state == ROUND_IN) && ((uint32_t)(g_millis - round_start_ms) > 400))
    {
        g_round_state = ROUND_KEEP;
        round_start_ms = 0;
    }
}
