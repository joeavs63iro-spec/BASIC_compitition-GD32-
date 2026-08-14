/*********************************************************************************************************************
* @file            car_uart.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.16, V1.0
* @modify          none
********************************************************************************************************************/

#include "car_uart.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief        串口0初始化   
// @return       void
// Sample usage:        uart0_init(UART0);//串口0初始化
//-------------------------------------------------------------------------------------------------------------------
void uart0_init(unsigned int uart_periph)
{
    uart_init(UART0_TXD_PORT, UART0_TXD_PIN, UART0_RXD_PORT, UART0_RXD_PIN, uart_periph, UART0_BAUDRATE); 
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        串口1初始化   
// @return       void
// Sample usage:        uart1_init(UART1);//串口1初始化
//-------------------------------------------------------------------------------------------------------------------
void uart1_init(unsigned int uart_periph)
{
    uart_init(UART1_TXD_PORT, UART1_TXD_PIN, UART1_RXD_PORT, UART1_RXD_PIN, uart_periph, UART1_BAUDRATE); 
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        串口接收数据
// @param        uart_periph            串口模块号：USARTx(x = 0,1)
// @return       rec_dat
// Sample usage:        rx_char = uart_receive(UART0);//串口0接收数
//-------------------------------------------------------------------------------------------------------------------
unsigned char uart_receive(unsigned int uart_periph)
{
    unsigned char rec_dat;

    rec_dat = uart_getchar(uart_periph);    

    return rec_dat;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        串口发送一个字节
// @param        uart_periph              串口模块号：USARTx(x = 0,1)
// @param        dat                      需要发送的字节
// @return       void        
// Sample usage:        uart_send_char(USART0,0xA5);// 串口USART0发送0xA5
//-------------------------------------------------------------------------------------------------------------------
void uart_send_char(unsigned int uart_periph, unsigned char dat)
{
    uart_putchar(uart_periph, dat);  
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        串口发送ascii码
// @param        uart_periph              串口模块号：USARTx(x = 0,1)
// @param        ascii_value              ASCII码数组
// @return       void        
// Sample usage:        uart_send_ascii(USART0,ascii_value);
//-------------------------------------------------------------------------------------------------------------------
void uart_send_ascii(unsigned int uart_periph, unsigned char *ascii_value)
{
    unsigned char i;

    for(i=0; i<5; i++)                   
    {
        uart_send_char(uart_periph, ascii_value[i]);
    }                                    //从高位到低位发送ascii码
      
    uart_send_char(uart_periph, 0x20);        //换行
}
