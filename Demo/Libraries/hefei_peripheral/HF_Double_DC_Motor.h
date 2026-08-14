/*********************************************************************************************************************
* @file            HF_Double_DC_Motor.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2023.4.2, V2.0
* @modify          none
********************************************************************************************************************/

#ifndef _HF_DOUBLE_DC_MOTOR_H
#define _HF_DOUBLE_DC_MOTOR_H



#include "headfile.h"

//#define MOTOR1_A   GPIO_PIN_6
//#define MOTOR1_B   GPIO_PIN_7
//#define MOTOR2_A   GPIO_PIN_8
//#define MOTOR2_B   GPIO_PIN_9
//宏定义

#define L_PWM1_PORT        GPIOC
#define L_PWM1_PIN         GPIO_PIN_9
#define L_PWM1_MODE        OUT_AF_PP
#define L_PWM1_TIMER       TIMER7
#define L_PWM1_TIMER_CH    TIMER_CH_3

#define L_PWM2_PORT        GPIOC
#define L_PWM2_PIN         GPIO_PIN_8
#define L_PWM2_MODE        OUT_AF_PP
#define L_PWM2_TIMER       TIMER7
#define L_PWM2_TIMER_CH    TIMER_CH_2

#define R_PWM1_PORT        GPIOC
#define R_PWM1_PIN         GPIO_PIN_7
#define R_PWM1_MODE        OUT_AF_PP
#define R_PWM1_TIMER       TIMER7
#define R_PWM1_TIMER_CH    TIMER_CH_1

#define R_PWM2_PORT        GPIOC
#define R_PWM2_PIN         GPIO_PIN_6
#define R_PWM2_MODE        OUT_AF_PP
#define R_PWM2_TIMER       TIMER7
#define R_PWM2_TIMER_CH    TIMER_CH_0

/* TIMER7 motor PWM: 120MHz / 12 / 1000 = 10kHz. */
#define MOTOR_PWM_TIMER_CLOCK_HZ    120000000U
#define MOTOR_PWM_TARGET_HZ         10000U
#define MOTOR_PWM_FREQ_DIV          12U
#define MOTOR_PWM_PERIOD            (MOTOR_PWM_TIMER_CLOCK_HZ / MOTOR_PWM_FREQ_DIV / MOTOR_PWM_TARGET_HZ)
#define MOTOR_MAX_SPEED             10000
#define MOTOR_PWM_RES               10000    // 与 motor_forward/motor_backward 的 0~10000 占空比单位一致

/* 类型定义 */
typedef enum
{
	left,
	right
}left_or_right;

typedef enum {
    MOTOR_STOP,
    MOTOR_CW,     // 顺时针
    MOTOR_CCW     // 逆时针
} Motor_Dir;

/* 函数声明 */
/* PWM初始化 */
void motor_init(left_or_right object);
/* 控制电机正转 */
void motor_forward(left_or_right object, uint16_t value);
/* 控制电机反转 */
void motor_backward(left_or_right object, uint16_t value);
void motor_set(uint8_t motor_num, Motor_Dir dir, int speed);

//全局变量声明

#endif
