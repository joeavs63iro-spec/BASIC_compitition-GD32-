/*********************************************************************************************************************
* @file            car_motion.h
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.20, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef __CAR_MOTION_H
#define __CAR_MOTION_H

#include "headfile.h"

//宏定义


//函数声明
/* 智能车前进，speed 范围 0~MOTOR_MAX_SPEED */
void car_forward(uint16_t speed);
/* 智能车后退，speed 范围 0~MOTOR_MAX_SPEED */
void car_backward(uint16_t speed);
/* 智能车停车 */
void car_stop(void);
/* 智能车左转，inner_speed 为左轮速度，outer_speed 为右轮速度 */
void car_turnleft(uint16_t inner_speed, uint16_t outer_speed);
/* 智能车右转，inner_speed 为右轮速度，outer_speed 为左轮速度 */
void car_turnright(uint16_t inner_speed, uint16_t outer_speed);


//全局变量声明

#endif
