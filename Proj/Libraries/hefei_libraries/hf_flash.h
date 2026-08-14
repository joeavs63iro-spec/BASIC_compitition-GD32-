/*********************************************************************************************************************
* @file            hf_flash.h
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.11, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef _HF_FLASH_H
#define _HF_FLASH_H

#include "headfile.h"

/* 宏定义声明 */


/* 定义2KB的大小 */
#define SIZE_2KB ((uint32_t)0x00000800U) /*!< size of 2KB*/

/* FLASH起始地址 */
#define FMC_START_ADDRESS FLASH_BASE /*!< FLASH_BANK0 start address */
#define FMC_END_ADDRESS ((uint32_t)0x0807FFFFU)  /*!< FLASH_BANK0 end address */


/* 函数声明 */

/* 写 uint32_t 数据 */
uint8_t flash_WritePage_uint32_t(uint32_t pageName,
    uint32_t offset,
    uint32_t* writeData,
    uint16_t length);
/* 读 uint32_t 数据 */
uint8_t flash_ReadPage_uint32_t(uint32_t pageName,
    uint32_t offset,
    uint32_t* readData,
    uint16_t length);

#endif
