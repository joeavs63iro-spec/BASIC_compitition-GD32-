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

//宏定义

//函数声明
/* 开启OLED显示 */
void oled_display_on(void);
/* 关闭OLED显示 */
void oled_display_off(void);
/* OLED更新显存 */
void oled_refresh_gram(void);
/* OLED屏幕清除 */
void oled_clear(void);
/* OLED显示字符 */
void oled_show_char(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size);
/* OLED显示数字 */
void oled_show_number(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
/* OLED显示字符串 */
void oled_show_string(uint8_t x,uint8_t y,char *chr,uint8_t size);
/* OLED显示汉字 */
void oled_show_Chinese(uint8_t x, uint8_t y, uint8_t Chinese);


//全局变量声明

#endif
