/*********************************************************************************************************************
* @file            HF_OLED.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.10.14, V1.0
* @modify          none
********************************************************************************************************************/

#include "HF_OLED.h"
#include "stdlib.h"


/********************************************详细说明********************************************
OLED的显存
存放格式如下：
[0]0 1 2 3 ... 127	
[1]0 1 2 3 ... 127	
[2]0 1 2 3 ... 127	
[3]0 1 2 3 ... 127	
[4]0 1 2 3 ... 127	
[5]0 1 2 3 ... 127	
[6]0 1 2 3 ... 127	
[7]0 1 2 3 ... 127 	
************************************************************************************************/


//-------------------------------------------------------------------------------------------------------------------
// @brief        OLED初始化
// @param        void
// @return       void
// Sample usage:        oled_init(); 
                        //OLED初始化
//-------------------------------------------------------------------------------------------------------------------
void oled_init(void)
{
    //I2C1初始化
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_I2C1);
    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_10 | GPIO_PIN_11);

    i2c_clock_config(I2C1, 100000, I2C_DTCY_2);
    i2c_mode_addr_config(I2C1, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0x78);
    i2c_enable(I2C1);
    i2c_ack_config(I2C1, I2C_ACK_ENABLE);
	
	
    //初始化SSD1306				
    delay_1ms(800);
	
    oled_write_byte(0xAE,OLED_CMD);
    oled_write_byte(0x00,OLED_CMD);                //设置低列地址
    oled_write_byte(0x10,OLED_CMD);                //设置高列地址
    oled_write_byte(0x40,OLED_CMD);                //设置起始行地址
    oled_write_byte(0xB0,OLED_CMD);                //设置页面地址
    oled_write_byte(0x81,OLED_CMD);
    oled_write_byte(0xFF,OLED_CMD);   
    oled_write_byte(0xA1,OLED_CMD);                //设置段重映射
    oled_write_byte(0xA6,OLED_CMD);                //正向/反向
    oled_write_byte(0xA8,OLED_CMD);
    oled_write_byte(0x3F,OLED_CMD);
    oled_write_byte(0xC8,OLED_CMD);
    oled_write_byte(0xD3,OLED_CMD);
    oled_write_byte(0x00,OLED_CMD);
	
    oled_write_byte(0xD5,OLED_CMD);
    oled_write_byte(0x80,OLED_CMD);
	
    oled_write_byte(0xD8,OLED_CMD);
    oled_write_byte(0x05,OLED_CMD);
	
    oled_write_byte(0xD9,OLED_CMD);
    oled_write_byte(0xF1,OLED_CMD);
	
    oled_write_byte(0xDA,OLED_CMD);
    oled_write_byte(0x12,OLED_CMD);
	
    oled_write_byte(0xDB,OLED_CMD);
    oled_write_byte(0x30,OLED_CMD);
		 
    oled_clear();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        OLED写数据
// @param        void
// @return       void
// Sample usage:         oled_write(0x78, 5, 1);
                        //向从机地址0x78发送数据5，长度为1个字节
//-------------------------------------------------------------------------------------------------------------------
void oled_write(uint8_t addr, uint8_t *data, uint32_t data_length)
{
    uint8_t i;
	
    /* 等待总线空闲 */
    while(i2c_flag_get(I2C1, I2C_FLAG_I2CBSY));

    /* 发送start信号 */
    i2c_start_on_bus(I2C1);

    /* 等待SBSEND位置1 */
    while(!i2c_flag_get(I2C1, I2C_FLAG_SBSEND));

    /* 设置从机地址和执行发送操作*/
    i2c_master_addressing(I2C1, addr, I2C_TRANSMITTER);

    /* 等待ADDSEND置位*/
    while(!i2c_flag_get(I2C1, I2C_FLAG_ADDSEND));
	
    /* 清除ADDSEND位 */
    i2c_flag_clear(I2C1, I2C_FLAG_ADDSEND);
	
    for(i = 0; i < data_length; i++)
    {
        /* 发送数据 */
        i2c_data_transmit(I2C1, data[i]);
        /* 等待发送寄存器为空 */
        while(!i2c_flag_get(I2C1, I2C_FLAG_TBE));
    }
			
    i2c_stop_on_bus(I2C1);

    while(I2C_CTL0(I2C1) & 0x0200);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        向OLED写入一个字节
// @param        dat       要写入的数据/命令
// @param        cmd       数据/命令标志： OLED_CMD,表示命令;OLED_DATA,表示数据;
// @return       void
// Sample usage:         oled_write_byte(0x40,OLED_CMD);
                        //设置起始行地址为0x40
//-------------------------------------------------------------------------------------------------------------------
void oled_write_byte(uint8_t dat,uint8_t cmd)
{
    uint8_t byte[2];

    if(cmd)
    {
        byte[0] = 0x40;
        byte[1] = dat;
        oled_write(0x78, byte, 2);
    }
    else
    {
        byte[0] = 0x00;
        byte[1] = dat;
        oled_write(0x78, byte, 2);
    }
} 	

//-------------------------------------------------------------------------------------------------------------------
// @brief        m^n函数
// @param        m
// @param        n
// @return       result    计算结果
// Sample usage:         mypow(m,n);
                        //计算m的n次方
//-------------------------------------------------------------------------------------------------------------------
//m^n函数
uint32_t mypow(uint8_t m,uint8_t n)
{
    uint32_t result=1;	 
    while(n--)result*=m;    
    return result;
}	

//-------------------------------------------------------------------------------------------------------------------
// @brief        设置OLED的坐标
// @param        x       屏幕的x坐标：0~127
// @param        y       屏幕的y坐标：0~7
// @return       void
// Sample usage:         oled_set_pos(48,2);
                        //设置坐标为（48，2）
//-------------------------------------------------------------------------------------------------------------------
void oled_set_pos(unsigned char x, unsigned char y)
{
    oled_write_byte(0xb0+y, OLED_CMD);
    oled_write_byte(((x&0xf0)>>4)|0x10, OLED_CMD);
    oled_write_byte((x&0x0f), OLED_CMD);
}

    	    





