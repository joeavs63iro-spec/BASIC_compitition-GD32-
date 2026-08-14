/*********************************************************************************************************************
* @file            car_uart.h
* @author         
* @Target core     GD32F303RCT6
* @revisions       2022.09.16, V1.0
* @modify          none
********************************************************************************************************************/

#ifndef __CAR_UART_H
#define __CAR_UART_H

#include "headfile.h"

//宏定义
//UART0为有线串口
#define UART0            USART0
#define UART0_TXD_PORT   GPIOA
#define UART0_TXD_PIN    GPIO_PIN_9
#define UART0_RXD_PORT   GPIOA
#define UART0_RXD_PIN    GPIO_PIN_10
#define UART0_BAUDRATE   115200

//UART1为无线串口
#define UART1            USART1
#define UART1_TXD_PORT   GPIOA
#define UART1_TXD_PIN    GPIO_PIN_2
#define UART1_RXD_PORT   GPIOA
#define UART1_RXD_PIN    GPIO_PIN_3
#define UART1_BAUDRATE   115200


//函数声明
/* 串口0初始化 */
void uart0_init(unsigned int uart_periph);
/* 串口0初始化 */
void uart1_init(unsigned int uart_periph);
/* 串口发送一个字节 */
void uart_send_char(unsigned int uart_periph, unsigned char dat);
/* 串口接收数据 */
unsigned char uart_receive(unsigned int uart_periph);
/*串口发送ascii码*/
void uart_send_ascii(unsigned int uart_periph, unsigned char *ascii_value);
//全局变量声明

#endif
