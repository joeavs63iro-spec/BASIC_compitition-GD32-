/*********************************************************************************************************************
* @file            main.h
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.10.08, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef _MAIN_H
#define _MAIN_H

//说明：该文件用于存放用户在main函数中用到的变量定义、宏定义、函数声明等内容

#include "headfile.h"
#include "stdbool.h"
#include "string.h"
#include "ui.h"
#include "flash.h"
typedef enum {
    BRIDGE_NONE = 0,
    BRIDGE_KEEP,
} BridgeState_t;

typedef enum {
    PARKING_NONE = 0,
    PARKING_WAIT,
    PARKING_IN,
    PARKING_KEEP,
    PARKING_OUT,
} ParkingState_t;

typedef enum
{
    ROUND_NONE = 0,
    ROUND_IN,
    ROUND_KEEP,
    ROUND_OUT
} RoundState_t;

void EXTI5_9_IRQHandler(void);
void TIMER3_IRQHandler(void);
void control_task(void);
void control_delay_ms(uint32_t ms);
void avoid_obs(void);
void park(void);
void cross_bridge(void);
void handle_roundabout(void);
void check_obstacle_trigger(void);
void check_roundabout_trigger(void);
void check_bridge_trigger(void);
void check_parking_trigger(void);
extern volatile PID_struct track_PID;
#endif
