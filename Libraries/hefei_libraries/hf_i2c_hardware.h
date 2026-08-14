/*********************************************************************************************************************
* @file            hf_i2c_hardware.h
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.9.7, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef _HF_I2C_HARDWARE_H_
#define _HF_I2C_HARDWARE_H_

#include "headfile.h"

/* 宏定义声明 */

#define I2C_SPEED            400000
#define I2C_PAGE_SIZE        8


/* 函数声明 */

/* I2C配置 */
void i2c_init(uint32_t i2c_periph, uint32_t i2c_address);
/* I2C写入一个字节 */
void i2c_send_byte(uint32_t i2c_periph, uint32_t slave_address, uint8_t data);
/* I2C写入多个字节 */
void i2c_send_bytes(uint32_t i2c_periph, uint32_t slave_address, uint8_t reg_address, uint8_t* p_buffer, uint8_t number_of_byte);
/* I2C向指定寄存器写入一个字节 */
void i2c_send_reg_byte(uint32_t i2c_periph, uint32_t slave_address, uint8_t reg_address, uint8_t data);
/* I2C向指定寄存器写入数组 */
void i2c_send_reg_buffer(uint32_t i2c_periph, uint32_t slave_address, uint8_t reg_address, uint8_t* p_buffer, uint16_t number_of_byte);
/* I2C从指定寄存器读取数组 */
void i2c_read_reg_buffer(uint32_t i2c_periph, uint32_t slave_address, uint8_t reg_address, uint8_t* p_buffer, uint16_t number_of_byte);

#endif
