/*********************************************************************************************************************
* @file            car_light.h
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.17, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef __CAR_LIGHT_H
#define __CAR_LIGHT_H

#include "headfile.h"

//宏定义
/* rgb颜色定义 */
static unsigned char rgb_off[3] = {0x00, 0x00, 0x00};
static unsigned char rgb_blue[3] = {0x00, 0x00, 0xFF};
static unsigned char rgb_green[3] = {0x00, 0xFF, 0x00};
static unsigned char rgb_cyan[3] = {0x00, 0xFF, 0xFF};
static unsigned char rgb_red[3] = {0xFF, 0x00, 0x00};
static unsigned char rgb_purple[3] = {0xFF, 0x00, 0xFF};
static unsigned char rgb_yellow[3] = {0xFF, 0xFF, 0x00};
static unsigned char rgb_white[3] = {0xFF, 0xFF, 0xFF};
static unsigned char rgb_user_define[3] = {0xA7, 0xC8, 0xE2};


/* 类型定义 */
typedef enum
{
    red,
    yellow,
    white,
    green,
    blue,
    cyan,
    purple,
}rgb_color;


//函数声明(外部函数，用户可自行调用）
/* 智能车尾灯同时亮起 */
void car_both_rgb_on(rgb_color color, unsigned int time);
/* 智能车尾灯常亮，不设置自动熄灭 */
void car_rgb_set_both(rgb_color color);
/* 非阻塞尾灯定时服务，主循环需要周期调用 */
void car_rgb_task(void);
/* 智能车尾灯熄灭 */
void car_both_rgb_off(void);
/* 智能车尾灯左灯闪烁 */
void car_left_rgb_flash(rgb_color color, unsigned int flash_count, unsigned int time);
/* 智能车尾灯右灯闪烁 */
void car_right_rgb_flash(rgb_color color, unsigned int flash_count, unsigned int time);
/* 智能车尾灯同时闪烁 */
void car_both_rgb_flash(rgb_color color, unsigned int flash_count, unsigned int time);


//全局变量声明

#endif
