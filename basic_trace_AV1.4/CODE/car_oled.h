/*********************************************************************************************************************
* @file            car_oled.h
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.10.15, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef __CAR_OLED_H
#define __CAR_OLED_H

#include "headfile.h"

//????

//????????
/* ????OLED??? */
void oled_display_on(void);
/* ???OLED??? */
void oled_display_off(void);
/* OLED??????? */
void oled_refresh_gram(void);
/* OLED??L??? */
void oled_clear(void);
/* OLED invert selected pixel area */
void oled_invert_area(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
/* OLED?????? */
void oled_show_char(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size);
/* OLED??????? */
void oled_show_number(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
/* OLED???????? */
void oled_show_string(uint8_t x,uint8_t y,uint8_t *chr,uint8_t size);
/* OLED??????? */
void oled_show_Chinese(uint8_t x, uint8_t y, uint8_t Chinese);


//??????????

#endif
