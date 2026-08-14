/*********************************************************************************************************************
* @file            HF_OLED.h
* @author         
* @Target core     GD32F303RCT6
* @revisions       2022.09.24, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef __HF_OLED_H
#define __HF_OLED_H

#include "headfile.h"

//宏定义
#define OLED_MODE    0
#define SIZE         8
#define XLevelL      0x00
#define XLevelH      0x10
#define Max_Column   128
#define Max_Row      64
#define Brightness   0xFF 
#define X_WIDTH 	   128
#define Y_WIDTH 	   64	

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据


//函数声明
/* OLED初始化 */
void oled_init(void);
/* OLED写数据 */
void oled_write(uint8_t addr, uint8_t *data, uint32_t data_length);
/* 向OLED写入一个字节 */
void oled_write_byte(uint8_t dat,uint8_t cmd);
/* m^n函数 */
uint32_t mypow(uint8_t m,uint8_t n);
/* 设置OLED的坐标 */
void oled_set_pos(unsigned char x, unsigned char y);


//全局变量声明

#endif
