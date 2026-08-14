/*********************************************************************************************************************
* @file            car_motion.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.20, V1.0
* @modify          none
********************************************************************************************************************/

#include "car_motion.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief        ????
// @param        speed       ???????,?? 0~MOTOR_MAX_SPEED
// @return       void
// Sample usage:        car_forward(2500); //????
//-----------------------------------------------------------------------------------------------------------------
void car_forward(uint16_t speed)
{
    motor_forward(left, speed);
    motor_forward(right, speed);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        ????
// @param        speed       ???????,?? 0~MOTOR_MAX_SPEED
// @return       void
// Sample usage:        car_backward(2500); //????
//-----------------------------------------------------------------------------------------------------------------
void car_backward(uint16_t speed)
{
    motor_backward(left, speed);
    motor_backward(right, speed);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        ????
// @param        void    
// @return       void
// Sample usage:        car_stop(); //????
//-----------------------------------------------------------------------------------------------------------------
void car_stop(void)
{
    motor_forward(left, 0);
    motor_forward(right, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        ????
// @param        inner_speed       ??????,?? 0~MOTOR_MAX_SPEED
// @param        outer_speed       ??????,?? 0~MOTOR_MAX_SPEED
// @return       void
// Sample usage:        car_turnleft(1500, 3500); //????
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
// @brief        ????
// @param        inner_speed       ??????,?? 0~MOTOR_MAX_SPEED
// @param        outer_speed       ??????,?? 0~MOTOR_MAX_SPEED
// @return       void
// Sample usage:        car_turnright(1500, 3500); //????
//-----------------------------------------------------------------------------------------------------------------
void car_turnright(uint16_t inner_speed, uint16_t outer_speed)
{
    motor_forward(left, outer_speed);
    motor_forward(right, inner_speed);
}
