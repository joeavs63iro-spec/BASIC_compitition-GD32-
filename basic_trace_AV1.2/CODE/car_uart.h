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

//????
//UART0????????
#define UART0            USART0
#define UART0_TXD_PORT   GPIOA
#define UART0_TXD_PIN    GPIO_PIN_9
#define UART0_RXD_PORT   GPIOA
#define UART0_RXD_PIN    GPIO_PIN_10
#define UART0_BAUDRATE   115200

//UART1????????
#define UART1            USART1
#define UART1_TXD_PORT   GPIOA
#define UART1_TXD_PIN    GPIO_PIN_2
#define UART1_RXD_PORT   GPIOA
#define UART1_RXD_PIN    GPIO_PIN_3
#define UART1_BAUDRATE   115200


//????????
/* ????0??'?? */
void uart0_init(unsigned int uart_periph);
/* ????0??'?? */
void uart1_init(unsigned int uart_periph);
/* ???????h????? */
void uart_send_char(unsigned int uart_periph, unsigned char dat);
/* ??????????? */
unsigned char uart_receive(unsigned int uart_periph);
/*???????ascii??*/
void uart_send_ascii(unsigned int uart_periph, unsigned char *ascii_value);
//??????????

#endif
