/*********************************************************************************************************************
* @file            hf_i2c_hardware.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.9.7, V1.0
* @modify          none
********************************************************************************************************************/

#include "hf_i2c_hardware.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief        I2C初始化
// @param        I2Cx(x=0,1)            选择I2C通道
// @param        bound                  通信时钟频率，不超过400KHz
// @param        address                I2C设备自身地址
// @return       void
// Sample usage:        i2c_init(I2C1, 0x00);
                        //配置I2C1,地址为0x00
//-------------------------------------------------------------------------------------------------------------------
void i2c_init(uint32_t i2c_periph, uint32_t i2c_address)
{
    /*开启复用功能时钟*/
    rcu_periph_clock_enable(RCU_AF);
	
    if (i2c_periph == I2C0)
    {
        /* 使能I2C时钟 */
        rcu_periph_clock_enable(RCU_I2C0);

        /* GPIO 初始化 */
        gpio_pin_init(GPIOB, OUT_AF_OD, GPIO_PIN_6 | GPIO_PIN_7);
    }

    else if (i2c_periph == I2C1)
    {
        /* 使能I2C时钟 */
        rcu_periph_clock_enable(RCU_I2C1);
					
        /* GPIO 初始化 */
        gpio_pin_init(GPIOB, OUT_AF_OD, GPIO_PIN_10 | GPIO_PIN_11);
    }

    /* 配置I2C时钟速率和占空比 */
    i2c_clock_config(i2c_periph, I2C_SPEED, I2C_DTCY_2);
				
    /* 配置I2C地址 */
    i2c_mode_addr_config(i2c_periph, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, i2c_address);
				
    /* 使能I2C */
    i2c_enable(i2c_periph);
				
    /* I2C允许应答 */
    i2c_ack_config(i2c_periph, I2C_ACK_ENABLE);
}
//-------------------------------------------------------------------------------------------------------------------
// @brief        I2C写入一个字节
// @param        i2c_periph            I2Cx(x=0,1)
// @param        slave_address         从机地址
// @param        data                  发送的字节
// @return        void
// Sample usage:        i2c_send_byte(I2C1, 0xD0, 0x21);
                        //I2C1开始通信,向地址为0xD0的从机发送数据0x21
//-------------------------------------------------------------------------------------------------------------------
void i2c_send_byte(uint32_t i2c_periph, uint32_t slave_address, uint8_t data)
{
    /* 等待I2C总线空闲 */
    while (i2c_flag_get(i2c_periph, I2C_FLAG_I2CBSY))
    {
        ;
    }

    /* 发送起始信号 */
    i2c_start_on_bus(i2c_periph);

    /* 等待起始信号发送完毕 */
    while (!i2c_flag_get(i2c_periph, I2C_FLAG_SBSEND))
    {
        ;	
    }
				
    /* 发送从机地址 */
    i2c_master_addressing(i2c_periph, slave_address, I2C_TRANSMITTER);

    /* 等待地址发送完毕 */
    while (!i2c_flag_get(i2c_periph, I2C_FLAG_ADDSEND))
    {
        ;
    }

    /* 地址发送标志位清除 */
    i2c_flag_clear(i2c_periph, I2C_FLAG_ADDSEND);

    /* 等待从机应答 */
    while (!i2c_flag_get(i2c_periph, I2C_FLAG_TBE))
    {
        ;
    }

    /* 发送字节 */
    i2c_data_transmit(i2c_periph, data);
				
    /* 等待发送完毕 */
    while (!i2c_flag_get(i2c_periph, I2C_FLAG_BTC))
    {
        ;
    }

    /* I2C放开总线 */
				
    i2c_stop_on_bus(i2c_periph);
    /* 等待总线释放 */
    while (I2C_CTL0(i2c_periph) & 0x0200)
    {
        ;
    }
}
//-------------------------------------------------------------------------------------------------------------------
// @brief        I2C写入多个字节
// @param        i2c_periph            I2Cx(x=0,1)
// @param        slave_address         从机地址
// @param        reg_address           寄存器地址
// @param        *p_buffer             写入的字节存放的数组地址
// @param        number_of_byte        写入的字节数量
// @return       void
// Sample usage:        i2c_send_bytes(I2C1, 0x65, 0x70, 0x12, 2);
//                      //I2C1开始通信,向从机地址为0x65的外设内部地址为0x70的寄存器写入2个字节数据，数据存放首地址为0x12
//-------------------------------------------------------------------------------------------------------------------
void i2c_send_bytes(uint32_t i2c_periph, uint32_t slave_address, uint8_t reg_address, uint8_t *p_buffer, uint8_t number_of_byte)
{
    /* 等待I2C总线空闲 */
    while (i2c_flag_get(i2c_periph, I2C_FLAG_I2CBSY))
    {
        ;
    }

    /* 发送起始信号 */
    i2c_start_on_bus(i2c_periph);

    /* 等待起始信号发送完毕 */
    while (!i2c_flag_get(i2c_periph, I2C_FLAG_SBSEND))
    {
        ;
    }

    /* 发送从机地址 */
    i2c_master_addressing(i2c_periph, slave_address, I2C_TRANSMITTER);

    /* 等待地址发送完毕 */
    while (!i2c_flag_get(i2c_periph, I2C_FLAG_ADDSEND))
    {
        ;
    }

    /* 地址发送标志位清除 */
    i2c_flag_clear(i2c_periph, I2C_FLAG_ADDSEND);

    /* 等待从机应答 */
    while (SET != i2c_flag_get(i2c_periph, I2C_FLAG_TBE))
    {
        ;
    }

    /* 发送字节 */
    i2c_data_transmit(i2c_periph, reg_address);

    /* 等待发送完毕 */
    while (!i2c_flag_get(i2c_periph, I2C_FLAG_BTC))
    {
        ;
    }

    /* 发送字节 */
    while (number_of_byte--)
    {
        i2c_data_transmit(i2c_periph, *p_buffer);
					
        /* 指向下一个要写的字节 */
        p_buffer++;
					
        /* 等待发送完毕 */
        while (!i2c_flag_get(i2c_periph, I2C_FLAG_BTC))
        {
            ;
        }
    }

    /* I2C放开总线 */
    i2c_stop_on_bus(i2c_periph);

    /* 等待总线释放 */
    while (I2C_CTL0(i2c_periph) & 0x0200)
    {
        ;
    }
}
//-------------------------------------------------------------------------------------------------------------------
// @brief        I2C向指定寄存器写入一个字节
// @param        i2c_periph            I2Cx(x=0,1)
// @param        slave_address         从机地址
// @param        reg_address           寄存器地址
// @param        data                  发送的字节
// @return       void
// Sample usage:        i2c_send_reg_byte(I2C1, 0xA0, 0x34, 0x21);
//                      //I2C1开始通信,向0xA0地址的器件发送寄存器地址0x34,发送数据0x21
//-------------------------------------------------------------------------------------------------------------------
void i2c_send_reg_byte(uint32_t i2c_periph, uint32_t slave_address, uint8_t reg_address, uint8_t data)
{
    /* 等待I2C总线空闲 */
    while (i2c_flag_get(i2c_periph, I2C_FLAG_I2CBSY))
    {
        ;
    }

    /* 发送起始信号 */
    i2c_start_on_bus(i2c_periph);

    /* 等待起始信号发送完毕 */
    while (!i2c_flag_get(i2c_periph, I2C_FLAG_SBSEND))
    {
        ;
    }

    /* 发送从机地址 */
    i2c_master_addressing(i2c_periph, slave_address, I2C_TRANSMITTER);

    /* 等待地址发送完毕 */
    while (!i2c_flag_get(i2c_periph, I2C_FLAG_ADDSEND))
    {
        ;
    }

    /* 地址发送标志位清除 */
    i2c_flag_clear(i2c_periph, I2C_FLAG_ADDSEND);

    /* 等待从机应答 */
    while (!i2c_flag_get(i2c_periph, I2C_FLAG_TBE))
    {
        ;
    }

    /* 发送寄存器地址 */
    i2c_data_transmit(i2c_periph, reg_address);

    /* 等待发送完毕 */
    while (!i2c_flag_get(i2c_periph, I2C_FLAG_BTC))
    {
        ;
    }

    /* 发送字节 */
    i2c_data_transmit(i2c_periph, data);

    /* 等待发送完毕 */
    while (!i2c_flag_get(i2c_periph, I2C_FLAG_BTC))
    {
        ;
    }

    /* I2C放开总线 */
    i2c_stop_on_bus(i2c_periph);

    /* 等待总线释放 */
    while (I2C_CTL0(i2c_periph) & 0x0200)
    {
        ;
    }
}

void i2c_send_reg_buffer(uint32_t i2c_periph, uint32_t slave_address, uint8_t reg_address, uint8_t *p_buffer, uint16_t number_of_byte)
{
    uint8_t number_of_page, number_of_single, address, count;
    number_of_page = 0;
    number_of_single = 0;
    address = 0;
    count = 0;

    address = reg_address % I2C_PAGE_SIZE;
    count = I2C_PAGE_SIZE - address;
    number_of_page = number_of_byte / I2C_PAGE_SIZE;
    number_of_single = number_of_byte % I2C_PAGE_SIZE;

    /* 如果写入地址与页大小对齐  */
    if (0 == address)
    {
        while (number_of_page--)
        {
            i2c_send_bytes(i2c_periph, slave_address, reg_address, p_buffer, I2C_PAGE_SIZE);
            slave_address += I2C_PAGE_SIZE;
            p_buffer += I2C_PAGE_SIZE;
        }
        if (0 != number_of_single)
        {
            i2c_send_bytes(i2c_periph, slave_address, reg_address, p_buffer, I2C_PAGE_SIZE);
        }
    }

    else
    {
        /* 如果写入地址与页大小不对齐 */
        if (number_of_byte < count)
        {
            i2c_send_bytes(i2c_periph, slave_address, reg_address, p_buffer, count);
        }

        else
        {
            number_of_byte -= count;
            number_of_page = number_of_byte / I2C_PAGE_SIZE;
            number_of_single = number_of_byte % I2C_PAGE_SIZE;

            if (0 != count)
            {
                i2c_send_bytes(i2c_periph, slave_address, reg_address, p_buffer, count);
                slave_address += count;
                p_buffer += count;
            }

            /* 写页面 */
            while (number_of_page--)
            {
                i2c_send_bytes(i2c_periph, slave_address, reg_address, p_buffer, count);
                slave_address += I2C_PAGE_SIZE;
                p_buffer += I2C_PAGE_SIZE;
            }

            /* 写单个字节 */
            if (0 != number_of_single)
            {
                i2c_send_bytes(i2c_periph, slave_address, reg_address, p_buffer, count);
            }
        }
    }
}

void i2c_read_reg_buffer(uint32_t i2c_periph, uint32_t slave_address, uint8_t reg_address, uint8_t *p_buffer, uint16_t number_of_byte)
{
    /* 等待I2C总线空闲 */
    while (i2c_flag_get(i2c_periph, I2C_FLAG_I2CBSY))
    {
        ;
    }

    if (2 == number_of_byte)
    {
        i2c_ackpos_config(i2c_periph, I2C_ACKPOS_NEXT);
    }

    /* 发送起始信号 */
    i2c_start_on_bus(i2c_periph);

    /* 等待起始信号发送完毕 */
    while (!i2c_flag_get(i2c_periph, I2C_FLAG_SBSEND))
    {
        ;
    }

    /* 发送从机地址 */
    i2c_master_addressing(i2c_periph, slave_address, I2C_TRANSMITTER);

    /* 等待地址发送完毕 */
    while (!i2c_flag_get(i2c_periph, I2C_FLAG_ADDSEND))
    {
        ;
    }

    /* 地址发送标志位清除 */
    i2c_flag_clear(i2c_periph, I2C_FLAG_ADDSEND);

    /* 等待从机应答 */
    while (SET != i2c_flag_get(i2c_periph, I2C_FLAG_TBE))
    {
        ;
    }

    /* 使能I2C */
    i2c_enable(i2c_periph);

    /* 发送寄存器地址 */
    i2c_data_transmit(i2c_periph, reg_address);

    /* 等待发送完毕 */
    while (!i2c_flag_get(i2c_periph, I2C_FLAG_BTC))
    {
        ;
    }

    /* 发送起始信号 */
    i2c_start_on_bus(i2c_periph);

    /* 等待数据发送完毕 */
    while (!i2c_flag_get(i2c_periph, I2C_FLAG_SBSEND))
    {
        ;
    }

    /* 发送从机地址 */
    i2c_master_addressing(i2c_periph, slave_address, I2C_RECEIVER);

    if (number_of_byte < 3)
    {
        /* 失能应答信号 */
        i2c_ack_config(i2c_periph, I2C_ACK_DISABLE);
    }

    /* 等待地址发送完毕 */
    while (!i2c_flag_get(i2c_periph, I2C_FLAG_ADDSEND))
    {
        ;
    }

    /* 地址发送标志位清除 */
    i2c_flag_clear(i2c_periph, I2C_FLAG_ADDSEND);

    if (1 == number_of_byte)
    {
        /* 发送停止信号 */
        i2c_stop_on_bus(i2c_periph);
    }

    /* 当有数据要读取时 */
    while (number_of_byte)
    {
        if (3 == number_of_byte)
        {
            /* 等待发送完毕 */
            while (!i2c_flag_get(i2c_periph, I2C_FLAG_BTC))
            {
                ;
            }

            /* 失能应答信号 */
            i2c_ack_config(i2c_periph, I2C_ACK_DISABLE);
        }
        if (2 == number_of_byte)
        {
            /* 等待发送完毕 */
            while (!i2c_flag_get(i2c_periph, I2C_FLAG_BTC))
            {
                ;
            }

            /* 发送停止信号 */
            i2c_stop_on_bus(i2c_periph);
        }

        /* 等待读取完毕并清除标志位 */
        if (i2c_flag_get(i2c_periph, I2C_FLAG_RBNE))
        {
            /* 读取一个字节 */
            *p_buffer = i2c_data_receive(i2c_periph);

            /* 指向将保存读取的字节的下一个位置 */
            p_buffer++;

            /* 减少读取字节计数器 */
            number_of_byte--;
        }
    }

    /* 等待停止条件完成 */
    while (I2C_CTL0(i2c_periph) & 0x0200)
    {
        ;
    }

    /* 使能应答信号 */
    i2c_ack_config(i2c_periph, I2C_ACK_ENABLE);

    i2c_ackpos_config(i2c_periph, I2C_ACKPOS_CURRENT);
}
