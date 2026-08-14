/*********************************************************************************************************************
* @file            car_motion.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.20, V1.0
* @modify          none
********************************************************************************************************************/

#include "car_motion.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief        小车前进
// @param        speed       左右轮前进速度，范围 0~MOTOR_MAX_SPEED
// @return       void
// Sample usage:        car_forward(2500); //小车前进
//-----------------------------------------------------------------------------------------------------------------
void car_forward(uint16_t speed)
{
    motor_forward(left, speed);
    motor_forward(right, speed);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        小车后退
// @param        speed       左右轮后退速度，范围 0~MOTOR_MAX_SPEED
// @return       void
// Sample usage:        car_backward(2500); //小车后退
//-----------------------------------------------------------------------------------------------------------------
void car_backward(uint16_t speed)
{
    motor_backward(left, speed);
    motor_backward(right, speed);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        小车停止
// @param        void    
// @return       void
// Sample usage:        car_stop(); //小车停止
//-----------------------------------------------------------------------------------------------------------------
void car_stop(void)
{
    motor_forward(left, 0);
    motor_forward(right, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        小车左转
// @param        inner_speed       左轮内侧速度，范围 0~MOTOR_MAX_SPEED
// @param        outer_speed       右轮外侧速度，范围 0~MOTOR_MAX_SPEED
// @return       void
// Sample usage:        car_turnleft(1500, 3500); //小车左转
//-----------------------------------------------------------------------------------------------------------------
void car_turnleft(uint16_t inner_speed, uint16_t outer_speed)
{
    /*
     * Parameterized turn speed replaces old fixed turn values,
     * making turn radius adjustable during province debug.
     */
    motor_forward(left, inner_speed);
    motor_forward(right, outer_speed);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        小车右转
// @param        inner_speed       右轮内侧速度，范围 0~MOTOR_MAX_SPEED
// @param        outer_speed       左轮外侧速度，范围 0~MOTOR_MAX_SPEED
// @return       void
// Sample usage:        car_turnright(1500, 3500); //小车右转
//-----------------------------------------------------------------------------------------------------------------
void car_turnright(uint16_t inner_speed, uint16_t outer_speed)
{
    motor_forward(left, outer_speed);
    motor_forward(right, inner_speed);
}
