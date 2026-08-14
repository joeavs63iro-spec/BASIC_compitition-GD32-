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

//???
/* rgb???? */
static unsigned char rgb_off[3] = {0x00, 0x00, 0x00};
static unsigned char rgb_blue[3] = {0x00, 0x00, 0xFF};
static unsigned char rgb_green[3] = {0x00, 0xFF, 0x00};
static unsigned char rgb_cyan[3] = {0x00, 0xFF, 0xFF};
static unsigned char rgb_red[3] = {0xFF, 0x00, 0x00};
static unsigned char rgb_purple[3] = {0xFF, 0x00, 0xFF};
static unsigned char rgb_yellow[3] = {0xFF, 0xFF, 0x00};
static unsigned char rgb_white[3] = {0xFF, 0xFF, 0xFF};
static unsigned char rgb_user_define[3] = {0xA7, 0xC8, 0xE2};


/* ???? */
typedef enum
{
    red,
    yellow,
    white,
    green,
    blue,
	  black,
    cyan,
    purple,
}rgb_color;


//????(????,???????)
/* ????????? */
void car_both_rgb_on(rgb_color color, unsigned int time);
/* ???????,??????? */
void car_rgb_set_both(rgb_color color);
/* ?????????,????????? */
void car_rgb_task(void);
/* ??????? */
void car_both_rgb_off(void);
/* ????????? */
void car_left_rgb_flash(rgb_color color, unsigned int flash_count, unsigned int time);
/* ????????? */
void car_right_rgb_flash(rgb_color color, unsigned int flash_count, unsigned int time);
/* ????????? */
void car_both_rgb_flash(rgb_color color, unsigned int flash_count, unsigned int time);


//??????

#endif
