/*********************************************************************************************************************
* @file            hf_i2c_software.h
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.9.7, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef _HF_I2C_SOFTWARE_H
#define _HF_I2C_SOFTWARE_H

#include "headfile.h"

/* 宏定义声明 */
#define I2C_PORT    GPIOA

#define SCL_PIN     GPIO_PIN_11
#define SDA_PIN     GPIO_PIN_12

#define SCL_Get()   gpio_get_output(I2C_PORT, SCL_PIN)
#define SCL_Clr()   gpio_bit_output(I2C_PORT, SCL_PIN, 0)//OLED_SCL
#define SCL_Set()   gpio_bit_output(I2C_PORT, SCL_PIN, 1)

#define SDA_Get()   gpio_get_output(I2C_PORT, SDA_PIN)
#define SDA_Clr()   gpio_bit_output(I2C_PORT, SDA_PIN, 0)//OLED_SDA
#define SDA_Set()   gpio_bit_output(I2C_PORT, SDA_PIN, 1)

#define CMD         0x40
#define DAT         0x00


/* 函数声明 */

/* 模拟I2C发送单字节 */
void i2c_sim_send_byte(uint8_t data);
/* 模拟I2C读取单字节 */
uint8_t i2c_sim_read_byte(void);
/* 模拟I2C向寄存器写入单字节 */
void i2c_sim_write_reg_byte(uint8_t addr, uint8_t reg, uint8_t dat);
/* 模拟I2C向寄存器写入多字节数据 */
void i2c_sim_write_reg_bytes(uint8_t addr, uint8_t reg, uint8_t *dat, uint8_t length);
/* 模拟I2C从寄存器读取多字节 */
void i2c_sim_read_reg_bytes(uint8_t addr, uint8_t reg, uint8_t *dat, uint8_t length);

//内部调用，用户无需关注
/* 模拟I2C延时 时间设置 */
void i2c_sim_delay(uint16_t time);
/* 模拟I2C开始信号 */
void i2c_sim_start(void);
/* 模拟I2C终止信号 */
void i2c_sim_stop(void);
/* 模拟I2C等待应答信号 */
void i2c_sim_waitack(void);
/* 模拟I2C应答信号 */
void i2c_sim_ack(void);
/* 模拟I2C非应答信号 */
void i2c_sim_nack(void);

#endif
