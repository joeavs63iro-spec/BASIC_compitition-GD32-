/*********************************************************************************************************************
* @file            hf_spi.h
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.08, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef _HF_SPI_H
#define _HF_SPI_H

#include "headfile.h"

/* 函数定义 */

// spi初始化
void spi_periph_init(uint32_t spi_periph);
// spi读写一个字节
uint8_t spi_ReadWrite_Byte(uint32_t spi_periph, uint8_t TxData);
 
#endif 

