/*********************************************************************************************************************
* @file            car_init.h
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.19, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef __ENCODER_H
#define __ENCODER_H

#include "headfile.h"

//宏定义
// 声明全局变量（不分配内存）
extern volatile int32_t motor1_count;
extern volatile int32_t motor2_count;
extern volatile uint8_t motor1_dir;
extern volatile uint8_t motor2_dir;
#define MOTOR_MIN_SPEED 300

#define constrain(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))

//-------------------------------------------------------------------------------------------------------------------
/*使用PID闭环设置
比例（P）：根据当前误差大小直接调整控制量，响应快但可能残留稳态误差。
积分（I）：累积历史误差，消除比例控制后的稳态偏差，但可能引起超调或振荡。
微分（D）：预测未来误差趋势（基于变化率），抑制系统振荡，提高稳定性*/
//-----------------------------------------------------------------------------------------------------------------
typedef struct {
    float Kp, Ki, Kd;
    float integral;
    float prev_error;
    float output_limit;
} PIDController;
//PIDController pid_motor1, pid_motor2;


//函数声明
void encoder_timer_config(void);
void PID_Init(PIDController* pid, float Kp, float Ki, float Kd, float limit);
float PID_Update(PIDController* pid, float setpoint, float measurement);
void update_encoder_values(void);
void Motor_PID_Init(void);
void Move_Single_Motor(uint8_t motor_num, int32_t target_mm);
void Move_Distance(int32_t target_mm);
void Move_ASYtance( int32_t target2_mm,int32_t target1_mm);

//全局变量声明

#endif
