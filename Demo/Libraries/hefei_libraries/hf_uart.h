/*********************************************************************************************************************
* @file            hf_uart.h
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.03, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef _HF_UART_H
#define _HF_UART_H

#include "headfile.h"
#include "gd32f30x_usart.h"

/* 函数定义 */

/* 串口初始化 */
void uart_init(uint32_t gpio_tx_periph, uint32_t tx_pin, uint32_t gpio_rx_periph, uint32_t rx_pin, uint32_t uart_periph, uint32_t baud);
/* 串口发送字节 */
void uart_putchar(uint32_t uart_periph, uint32_t data);
/* 串口发送数组 */
void uart_putbuff(uint32_t uart_periph, uint32_t *buff, uint32_t len);
/* 串口发送字符串 */
void uart_putstr(uint32_t uart_periph, uint32_t *str);
/* 读取串口接收的字节（whlie等待） */
uint16_t uart_getchar(uint32_t uart_periph);
/* 读取串口接收的字符串（whlie等待） */
void uart_getstr(uint32_t uart_periph, uint16_t *dat);
/* 读取串口接收的数据（查询接收） */
uint8_t uart_query(uint32_t uart_periph, uint16_t *dat);
/* 串口中断设置 */
void uart_interrupt_init(uint32_t uart_periph, usart_interrupt_enum int_flag, uint8_t status, uint8_t uart_irq);

int fputc(int ch, FILE *f);
#endif


