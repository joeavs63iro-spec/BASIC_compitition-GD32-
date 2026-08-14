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

//???


//????
/* ?????,speed ?? 0~MOTOR_MAX_SPEED */
void car_forward(uint16_t speed);
/* ?????,speed ?? 0~MOTOR_MAX_SPEED */
void car_backward(uint16_t speed);
/* ????? */
void car_stop(void);
/* ?????,inner_speed ?????,outer_speed ????? */
void car_turnleft(uint16_t inner_speed, uint16_t outer_speed);
/* ?????,inner_speed ?????,outer_speed ????? */
void car_turnright(uint16_t inner_speed, uint16_t outer_speed);


//??????

#endif
