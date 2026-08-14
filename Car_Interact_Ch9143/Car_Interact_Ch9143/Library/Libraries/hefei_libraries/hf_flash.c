/*********************************************************************************************************************
* @file            hf_flash.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.11, V1.0
* @modify          none
********************************************************************************************************************/

#include "hf_flash.h"

//-------------------------------------------------------------------------------------------------------------------
//@brief        写 uint32_t 数据
//@param        pageName             页数：0~127
//@param        offset               偏移地址
//@param        *writeData           要写入的数据
//@param        length               要写入的数据长度
//@return       返回0为失败，1为成功
//Sample usage：        flash_WritePage_uint32_t(120, 0, writeData, 5);
                        //向第120页写入数组writeData[i]中的5个32位的数据，偏移地址为0
//-------------------------------------------------------------------------------------------------------------------
uint8_t flash_WritePage_uint32_t(uint32_t pageName,
    uint32_t offset,
    uint32_t* writeData,
    uint16_t length)
{
    uint32_t i,page_start_address, page_end_address;
	
    //解锁FLASH擦除控制单元
    fmc_unlock();
	

    //清空标志位
    fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);
			
    /* 计算首地址和末地址*/
    page_start_address = offset + (FMC_START_ADDRESS + pageName * SIZE_2KB);
    page_end_address = page_start_address + 4 * length;

    //避免值过大导致程序卡死
    if ((page_end_address - page_start_address) >= SIZE_2KB)
    {
        return 0;
    }

    //以页为单位擦除
    while (FMC_READY != fmc_page_erase(page_start_address))
    {
        ;
    }
	
    //写入32位数据
    for (i = 0; i < length; i++) 
    {
        fmc_word_program(page_start_address, writeData[i]);
		    delay_1ms(1);	
        page_start_address = page_start_address + 4;
    }
	
		
    //锁上FLASH擦除控制单元
    fmc_lock();
    return 1;
}

//-------------------------------------------------------------------------------------------------------------------
//@brief        读 uint32_t 数据
//@param        pageName             页数：0~127
//@param        offset               偏移地址
//@param        *readData            要读取的数据
//@param        length               要读取的数据长度
//@return       返回0为失败，1为成功
//Sample usage：        flash_ReadPage_uint32_t(120, 0, readData, 5);
                        //从第120页读取5个数据存入数readData[i]，偏移地址为0
//-------------------------------------------------------------------------------------------------------------------
uint8_t flash_ReadPage_uint32_t(uint32_t pageName,
    uint32_t  offset,
    uint32_t* readData,
    uint16_t  length)
{
    uint32_t i, page_start_address, page_end_address;

    /* 计算首地址和末地址*/
    page_start_address = offset + (FMC_START_ADDRESS + pageName * SIZE_2KB);
    page_end_address = page_start_address + 4 * length;

    /* 检查FMC忙碌标志位 */
    while(fmc_flag_get(FMC_FLAG_BANK0_BUSY))
    {
        ;
    }

    //避免值过大导致程序卡死
    if ((page_end_address - page_start_address) >= SIZE_2KB)
    {
        return 0;
    }

    for (i=0; i<length; i++) 
    {
        readData[i] = *(__IO int32_t*)page_start_address;
        page_start_address = page_start_address + 4;
    }

    return 1;
}

