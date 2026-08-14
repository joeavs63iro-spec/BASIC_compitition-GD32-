/*********************************************************************************************************************
* @file            car_value_process.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.22, V1.0
* @modify          none
********************************************************************************************************************/

#include "car_value_process.h"

//-------------------------------------------------------------------------------------------------------------------
//@brief        16位无符号整型转换为ASCII码
//@param        dat:            	  16位无符号整型
//@param        ascii_value:          ASCII码数组
//@return       void
//Sample usage:        int_to_ascii(adc_value, ascii_value_array);   //16位无符号整型转换为ASCII码
//-------------------------------------------------------------------------------------------------------------------
void int_to_ascii(unsigned int dat, unsigned char *ascii_value)
{
    ascii_value[4] = dat % 10 + 0x30;
    ascii_value[3] = dat / 10 % 10 + 0x30;
    ascii_value[2] = dat / 100 % 10 + 0x30;
    ascii_value[1] = dat / 1000 % 10 + 0x30;
    ascii_value[0] = dat / 10000 % 10 + 0x30;
}
