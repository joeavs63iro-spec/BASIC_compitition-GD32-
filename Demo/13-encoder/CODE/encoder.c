/*

 *  Created on: 2025年6月12日
 *      Author: 张桂杰
 */
#include "gd32f30x_timer.h"
#include "gd32f30x.h"
#include <stdio.h>
#include <stdlib.h>  // C标准库abs()声明
#include <math.h>    // 部分平台需要同时包含
#include "car_motion.h"
#include "hf_systick.h"
#include "car_uart.h"
#include "HF_Double_DC_Motor.h"
#include "encoder.h"
// 电机控制引脚定义

// 硬件配置

#define ENCODER_PULSE_PER 1 //倍数设置，设置成转一度或者是一圈的倍数54，也可以设置成1。
//电机圈数线为13电机减速比为48/120所以电机转一圈的编码器数是624或者1560，
PIDController pid_left, pid_right;
// 全局变量
volatile int32_t motor1_count = 0;//编码器1的计数值
volatile int32_t motor2_count = 0;//编码器2的计数值
volatile uint8_t motor1_dir = 0;//编码器1的方向值
volatile uint8_t motor2_dir = 0;//编码器2的方向值
static uint16_t last_cnt1 = 0, last_cnt2 = 0;

//static volatile uint8_t pwm_counter = 0;
//static volatile uint8_t motor1_duty_a = 0;
//static volatile uint8_t motor1_duty_b = 0;
//static volatile uint8_t motor2_duty_a = 0;
//static volatile uint8_t motor2_duty_b = 0;


void encoder_timer_config(void) {
    // 通用定时器参数配置
    timer_parameter_struct timer_init_struct;
    timer_struct_para_init(&timer_init_struct);
    timer_init_struct.prescaler = 0;
    timer_init_struct.alignedmode = TIMER_COUNTER_EDGE;
    timer_init_struct.counterdirection = TIMER_COUNTER_UP;
    timer_init_struct.period = 65535;
    timer_init_struct.clockdivision = TIMER_CKDIV_DIV1;

    // GPIO配置初始化
       rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, 
             GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_0 | GPIO_PIN_1);

    // TIM1配置（电机1编码器接口 PA0-CH0/PA1-CH1）
    rcu_periph_clock_enable(RCU_TIMER1);
    timer_init(TIMER1, &timer_init_struct);
    timer_quadrature_decoder_mode_config(TIMER1,
        TIMER_ENCODER_MODE2,
        TIMER_IC_POLARITY_RISING,
        TIMER_IC_POLARITY_RISING);
    
    timer_ic_parameter_struct ic_init_struct = {
        .icpolarity = TIMER_IC_POLARITY_RISING,
        .icselection = TIMER_IC_SELECTION_DIRECTTI,
        .icprescaler = TIMER_IC_PSC_DIV1,
        .icfilter = 0x0F
    };
    timer_input_capture_config(TIMER1, TIMER_CH_0, &ic_init_struct);
    timer_input_capture_config(TIMER1, TIMER_CH_1, &ic_init_struct);
    timer_enable(TIMER1);

    // TIM2配置（电机2编码器接口 PA6-CH0/PA7-CH1）
    rcu_periph_clock_enable(RCU_TIMER2);
    timer_init(TIMER2, &timer_init_struct);
    timer_quadrature_decoder_mode_config(TIMER2,
        TIMER_ENCODER_MODE2,
        TIMER_IC_POLARITY_RISING,
        TIMER_IC_POLARITY_RISING);
    
    timer_input_capture_config(TIMER2, TIMER_CH_0, &ic_init_struct);
    timer_input_capture_config(TIMER2, TIMER_CH_1, &ic_init_struct);
    timer_enable(TIMER2);
}


//-------------------------------------------------------------------------------------------------------------------
/*PID初始化
输入参数：
pid：指向PID控制器结构体的指针
Kp/Ki/Kd：比例、积分、微分系数
limit：输出限幅值*/
//-----------------------------------------------------------------------------------------------------------------
void PID_Init(PIDController* pid, float Kp, float Ki, float Kd, float limit) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0;// 清零积分累积值
    pid->prev_error = 0;// 重置上一次误差记录
    pid->output_limit = limit;// 设置输出限幅阈值
}

//-------------------------------------------------------------------------------------------------------------------
/*位置式PID控制器更新函数
pid：指向PID控制器结构体的指针;
设定值(setpoint);
实际测量值(measurement)
*/
//-----------------------------------------------------------------------------------------------------------------
float PID_Update(PIDController* pid, float setpoint, float measurement) {
    float error = setpoint - measurement;//计算设定值(setpoint)与实际测量值(measurement)的偏差。
	
    pid->integral += error;//累积误差
    if(pid->integral > pid->output_limit) pid->integral = pid->output_limit;
    else if(pid->integral < -pid->output_limit) pid->integral = -pid->output_limit;//实现积分作用对积分值进行抗饱和处理
    
    float derivative = error - pid->prev_error;//通过当前误差与上次误差的差值(近似微分)
    pid->prev_error = error;//保存当前误差供下次计算使用
    
    float output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;
	//PID输出Kp*error：比例项（即时响应）；Ki*integral：积分项（消除静差）；Kd*derivative：微分项（抑制振荡）
    
    if(output > pid->output_limit) output = pid->output_limit;
    else if(output < -pid->output_limit) output = -pid->output_limit;//确保最终输出不超过执行器可接受范围
    
    return output;//输出，用于PWM函数控制速度
}
//-------------------------------------------------------------------------------------------------------------------
static void reset_encoder_counts(void)
{
    timer_counter_value_config(TIMER1, 0);
    timer_counter_value_config(TIMER2, 0);
    last_cnt1 = 0;
    last_cnt2 = 0;
    motor1_count = 0;
    motor2_count = 0;
}

static void reset_motion_pid(void)
{
    pid_left.integral = 0;
    pid_left.prev_error = 0;
    pid_right.integral = 0;
    pid_right.prev_error = 0;
}

//增量式编码器数据读取与处理函数，用于编码器计数
//-----------------------------------------------------------------------------------------------------------------
void update_encoder_values(void) {
    uint16_t curr_cnt1 = TIMER_CNT(TIMER1);//通过定时器捕获当前编码器计数值
    uint16_t curr_cnt2 = TIMER_CNT(TIMER2);
    
    int16_t diff1 = (int16_t)(curr_cnt1 - last_cnt1);
    int16_t diff2 = (int16_t)(last_cnt2 - curr_cnt2);
    
    if(diff1 >= 32767) diff1 -= 65536;
    else if(diff1 <= -32768) diff1 += 65536;
    
    if(diff2 >= 32767) diff2 -= 65536;
    else if(diff2 <= -32768) diff2 += 65536;
	/*计算两次采样的计数值差（转为有符号16位）
处理定时器溢出情况（当计数值超过65535后归零）
通过±32767阈值判断是否发生溢出跳变*/
    
    motor1_count += diff1;//将差值累加到全局位置计数器
    motor2_count += diff2;
    
    motor1_dir = (TIMER_CTL0(TIMER1) & TIMER_CTL0_DIR) ? 0 : 1;
    motor2_dir = (TIMER_CTL0(TIMER2) & TIMER_CTL0_DIR) ? 1 : 0;//通过定时器控制寄存器判断电机旋转方向
    
    last_cnt1 = curr_cnt1;
    last_cnt2 = curr_cnt2;//存储当前计数值供下次计算使用
}

//两个电机PID初始化
void Motor_PID_Init(void) {
    PID_Init(&pid_left, 0.8, 0.2, 0.1, MOTOR_MAX_SPEED);
    PID_Init(&pid_right, 0.8, 0.2, 0.1, MOTOR_MAX_SPEED);
}

// 单个电机移动指定距离
void Move_Single_Motor(uint8_t motor_num, int32_t target_mm) {
    int32_t target_pulse = target_mm * ENCODER_PULSE_PER;//输入的数值*倍数得到目标值
    volatile int32_t* current_count = (motor_num == 1) ? &motor1_count : &motor2_count;//指针指向设置电机编码器计数值的地址
    PIDController* pid = (motor_num == 1) ? &pid_left : &pid_right;//判断是哪一个电机，因为只有两个电机如果不是电机1就是电机2
    
    reset_encoder_counts();
    reset_motion_pid();
    int i=0;
	  float outputy=0;
    while(1) {
        update_encoder_values(); // 更新编码器值
        
        float output = PID_Update(pid, target_pulse, *current_count);//使用位置式PID控制器更新函数
//        if( fabs(output)<300&&fabs(output)>20){outputy=300;}
//				if( fabs(output)<300){outputy=300;}//300数值可改，寻找轮子最小转动值
//				 else{outputy=fabs(output);}//防止输出速度过小轮子无法转动
				 outputy = constrain(fabs(output),MOTOR_MIN_SPEED, MOTOR_MAX_SPEED);
				 Motor_Dir dir = (*current_count < target_pulse) ? MOTOR_CW : MOTOR_CCW;// 增加超限反转逻辑超限强制反转
					  if((dir==MOTOR_CW && *current_count>target_pulse) || 
               (dir==MOTOR_CCW && *current_count<target_pulse)) {
                dir = (dir == MOTOR_CW) ? MOTOR_CCW : MOTOR_CW;//如果编码器计数值大于实际设置值电机反转
            }
        motor_set(motor_num,dir, outputy);// 设置电机方向和速度
				 delay_1ms(10);
        				i++;
				if(i>10){
					i=0;//串口打印延迟函数每10Ms（根据delay_1ms(10)）*10打印一次
				printf("Encoder: Count=%d, Dir=%d\r\n", *current_count, dir);}//串口打印可以注释
        // 到达目标位置判断
//        if(20*abs(*current_count - target_pulse) <abs(target_pulse) )  //可以使用串口通信延迟方法实现到一定时间后强制退出
					if(abs(*current_count - target_pulse) <5 ) {
					printf("Encoderwc\r\n");//串口打印可以注释
            motor_set(motor_num, MOTOR_STOP, 0);//电机暂停
            break;//退出循环
        }
					
    }
}
// 双电机移动指定距离，距离相等，原理与单电机控制一样
void Move_Distance(int32_t target_mm) {
    int32_t target_pulse = target_mm * ENCODER_PULSE_PER;
    reset_encoder_counts();
    reset_motion_pid();
    float sync_error = 0;
    uint8_t overflag1 = 0, overflag2 = 0; // 超限标志位
		int i=0;
    while(1) {
        update_encoder_values();
        
        // 超限检测（新增核心逻辑）
        if(motor1_count > target_pulse + 10) { // 10脉冲容差带
            overflag1 = 1;
        } else if(motor1_count < target_pulse - 10) {
            overflag1 = 0;
        }
        
        if(motor2_count > target_pulse + 10) {
            overflag2 = 1;
        } else if(motor2_count < target_pulse - 10) {
            overflag2 = 0;
        }

        // 同步误差计算
        float current_sync = motor1_count - motor2_count;
        sync_error += current_sync * 0.001f;

        // PID控制（增加超限补偿）
        float out_left = PID_Update(&pid_left, target_pulse, motor1_count) 
                        - 0.3f * current_sync - 0.1f * sync_error;
        float out_right = PID_Update(&pid_right, target_pulse, motor2_count) 
                         + 0.3f * current_sync + 0.1f * sync_error;

        // 超限反转处理
        if(overflag1) out_left = -fabs(out_left) * 1.2; // 超限时增强反向输出
        if(overflag2) out_right = -fabs(out_right) * 1.2;

				
        // 输出限幅
        out_left = constrain(fabs(out_left), MOTOR_MIN_SPEED, MOTOR_MAX_SPEED);
        out_right = constrain(fabs(out_right), MOTOR_MIN_SPEED, MOTOR_MAX_SPEED);

        // 电机驱动（根据超限状态调整方向）
        motor_set(1, (overflag1 ? MOTOR_CCW : MOTOR_CW), out_left);
        motor_set(2, (overflag2 ? MOTOR_CCW : MOTOR_CW), out_right);
								
								 delay_1ms(10);
        				i++;
				if(i>10){
					i=0;//串口打印延迟函数每10Ms（根据delay_1ms(10)）*10打印一次
				printf("Encoder1: Count=%d, Dir=%d\r\n", motor1_count, motor1_dir);}//串口打印可以注释
				printf("Encoder2: Count=%d, Dir=%d\r\n", motor2_count , motor2_dir);
        // 精确停止条件
        if(abs(motor1_count - target_pulse) < 5 && 
           abs(motor2_count - target_pulse) < 5) {
            delay_1ms(20);
            if(abs(motor1_count - target_pulse) < 5 && 
               abs(motor2_count - target_pulse) < 5) {
								motor_set(1, MOTOR_STOP, 0);
                motor_set(2, MOTOR_STOP, 0);
                break;
            }
        }
        delay_1ms(10);
    }
}

// 两个电机分别移动指定距离，原理与单电机控制一样
void Move_ASYtance(int32_t target1_mm, int32_t target2_mm) {
    int32_t target1_pulse = target1_mm * ENCODER_PULSE_PER;
    int32_t target2_pulse = target2_mm * ENCODER_PULSE_PER;//输入的数值*倍数得到目标值
    reset_encoder_counts();
    reset_motion_pid();
    int i=0,j=0;
    int motor1_stop = (target1_mm == 0);
    int motor2_stop = (target2_mm == 0);
    float out_lefty =0, out_righty =0;//分别储存PID控制器更新函数的值
    while(1) {
        update_encoder_values();//开始启动编码器
          
         
        if((motor1_stop==0)&&(target1_mm != 0)) //当电机1的状态为0与电机1的目标数不等于0时开始执行，以下代码原理与单个电机驱动时一致
					{
						float out_left = PID_Update(&pid_left, target1_pulse, motor1_count);
						//			  if( fabs(out_left)<300){out_lefty=300;}
//			  	else{out_lefty=fabs(out_left);}
						        out_lefty = constrain(fabs(out_left), MOTOR_MIN_SPEED, MOTOR_MAX_SPEED);
       

					 // 增加超限反转逻辑超限强制反转
					  Motor_Dir dir1 = (motor1_count < target1_pulse) ? MOTOR_CW : MOTOR_CCW;
					  if((dir1==MOTOR_CW && motor1_count>target1_pulse) || 
               (dir1==MOTOR_CCW && motor1_count<target1_pulse)) {
                dir1 = (dir1 == MOTOR_CW) ? MOTOR_CCW : MOTOR_CW;
            }
            motor_set(1, dir1, out_lefty);

				 delay_1ms(10);
        	i++;
				if(i>10){
					i=0;
				 printf("Encoder1: Count=%d, Dir=%d\r\n", motor1_count , motor1_dir);
					}
        // 到达目标位置判断
//        if(20*abs(*current_count - target_pulse) <abs(target_pulse) )  
					if(abs(motor1_count  - target1_pulse) <5 ){
						delay_1ms(20);
					if(abs(motor1_count  - target1_pulse) <5 ) {
						motor1_stop=1;
					printf("Encoder1wc\r\n");
            motor_set(1, MOTOR_STOP, 0);}
					else{motor1_stop=0;}
					}
        }
        if((motor2_stop==0)&&(target2_mm != 0)) //当电机2的状态为0与电机2的目标数不等于0时开始执行，以下代码原理与单个电机驱动时一致
					{
            float out_right = PID_Update(&pid_right, target2_pulse, motor2_count);
						 out_righty = constrain(fabs(out_right), MOTOR_MIN_SPEED, MOTOR_MAX_SPEED);
//					        if( fabs(out_right)<300){out_righty=300;}
//									else{out_righty=fabs(out_right);}
							Motor_Dir dir2 = (motor2_count < target2_pulse) ? MOTOR_CW : MOTOR_CCW;
							if((dir2==MOTOR_CW && motor2_count>target2_pulse) || 
               (dir2==MOTOR_CCW && motor2_count<target2_pulse)) {
                dir2 = (dir2 == MOTOR_CW) ? MOTOR_CCW : MOTOR_CW;
            }
            motor_set(2, dir2, out_righty);
									delay_1ms(10);
						j++;
				if(j>10){
					j=0;
				 printf("Encoder2: Count=%d, Dir=%d\r\n", motor2_count , motor2_dir);}
        // 到达目标位置判断
					if(abs(motor2_count  - target2_pulse) <5 ) {
						delay_1ms(20);
						if(abs(motor2_count  - target2_pulse) <5 ) {
						motor2_stop=1;
					printf("Encoder2wc\r\n");
            motor_set(2, MOTOR_STOP, 0);}
					else{motor2_stop=0;}
			}
		}
        if(target1_mm == 0) {
            motor_set(1, MOTOR_STOP, 0);
        }
        
        if(target2_mm == 0) {
            motor_set(2, MOTOR_STOP, 0);
        }
        if(motor1_stop && motor2_stop)	{break;}//两个电机的状态都是停止结束循环
        delay_1ms(10);
    }
}


//int main(void) {
//    systick_config();
//    motor_init(left);
//	motor_init(right);
//    encoder_timer_config();  // 编码器初始化
//	
//    Motor_PID_Init();
//	uart0_init(UART0);
//    while(1) {
////			update_encoder_values();

////        delay_1ms(1000);
//			
//        // 适当延时避免输出过快
////      motor_forward(left, 6000);
////			motor_forward(right, 6000);
////			delay_1ms(1000);
//        Move_Distance(100);  // 电机一起前进
//        delay_1ms(1000);
////        motor_set(1, MOTOR_CW, 300);//电机1前进
////			  delay_1ms(1000);
////			   motor_set(1, MOTOR_STOP, 0);//电机1停止
////			  delay_1ms(1000);
//						 
////			  Move_ASYtance(90,-90);//电机1前进90*倍数，电机2反转90*倍数
////        delay_1ms(1000);
////			  Move_ASYtance(-90,90);  
////        delay_1ms(1000);
//        
////        Move_Single_Motor(1, 90);   // 电机一前进90*倍数
////        delay_1ms(1000);
////			  Move_Single_Motor(1,-90);   // 电机一反转90*倍数
////        delay_1ms(1000);

//			  break;//测试函数完成后跳出循环
//    }
//		printf("Encoderwc\r\n");
//			 printf("Encoder1: Count=%d, Dir=%d\r\n", motor1_count, motor1_dir);
//       printf("Encoder2: Count=%d, Dir=%d\r\n\r\n", motor2_count, motor2_dir);
//		motor_set(1, MOTOR_STOP, 0);
//		motor_set(2, MOTOR_STOP, 0);
//}



