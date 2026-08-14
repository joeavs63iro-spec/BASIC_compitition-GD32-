/*********************************************************************************************************************
* @file            hf_spi.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.08, V1.0
* @modify          none
********************************************************************************************************************/

#include "hf_spi.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief        spi初始化
// @param        spi_periph            初始化spix(x = 0,1,2)		
// @return       void
// Sample usage:            spi_periph_init(SPI0);
//                          //初始化SPI0
//-------------------------------------------------------------------------------------------------------------------	
void spi_periph_init(uint32_t spi_periph)
{
    rcu_periph_clock_enable((rcu_periph_enum)spi_periph);

    spi_i2s_deinit(spi_periph);
    spi_parameter_struct spi_init_struct;
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_16;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;

    spi_init(spi_periph, &spi_init_struct);

    spi_nss_output_enable(spi_periph);
    spi_enable(spi_periph);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        spi读写一个字节
// @param        TxData             spi发送值		
// @return       uint8_t            返回接收值
// Sample usage:            uint8_t data; data = SPI_ReadWrite_Byte(0xff);
//                           //发送0xff，返回值存入data
//-------------------------------------------------------------------------------------------------------------------	
uint8_t spi_ReadWrite_Byte(uint32_t spi_periph, uint8_t TxData)
{    
    while (spi_i2s_flag_get(spi_periph, SPI_FLAG_TBE) == RESET)
    {
        ;
    }//等待发送区空 
    spi_i2s_data_transmit(spi_periph, TxData); //通过外设SPIx发送一个byte数据
			
    while (spi_i2s_flag_get(spi_periph, SPI_FLAG_RBNE) == RESET)
    {
        ;
    } //等待接收完一个byte  
    return spi_i2s_data_receive(spi_periph); //返回通过SPIx最近接收的数据		
}
