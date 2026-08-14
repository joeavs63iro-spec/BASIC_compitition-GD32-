/*********************************************************************************************************************
* @file            car_uart.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.16, V1.0
* @modify          none
********************************************************************************************************************/

#include "car_uart.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief        ????0??'??   
// @return       void
// Sample usage:        uart0_init(UART0);//????0??'??
//-------------------------------------------------------------------------------------------------------------------
void uart0_init(unsigned int uart_periph)
{
    uart_init(UART0_TXD_PORT, UART0_TXD_PIN, UART0_RXD_PORT, UART0_RXD_PIN, uart_periph, UART0_BAUDRATE); 
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        ????1??'??   
// @return       void
// Sample usage:        uart1_init(UART1);//????1??'??
//-------------------------------------------------------------------------------------------------------------------
void uart1_init(unsigned int uart_periph)
{
    uart_init(UART1_TXD_PORT, UART1_TXD_PIN, UART1_RXD_PORT, UART1_RXD_PIN, uart_periph, UART1_BAUDRATE); 
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        ???????????
// @param        uart_periph            ????g??t?USARTx(x = 0,1)
// @return       rec_dat
// Sample usage:        rx_char = uart_receive(UART0);//????0??????
//-------------------------------------------------------------------------------------------------------------------
unsigned char uart_receive(unsigned int uart_periph)
{
    unsigned char rec_dat;

    rec_dat = uart_getchar(uart_periph);    

    return rec_dat;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        ???????h?????
// @param        uart_periph              ????g??t?USARTx(x = 0,1)
// @param        dat                      ???????????
// @return       void        
// Sample usage:        uart_send_char(USART0,0xA5);// ????USART0????0xA5
//-------------------------------------------------------------------------------------------------------------------
void uart_send_char(unsigned int uart_periph, unsigned char dat)
{
    uart_putchar(uart_periph, dat);  
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        ???????ascii??
// @param        uart_periph              ????g??t?USARTx(x = 0,1)
// @param        ascii_value              ASCII??????
// @return       void        
// Sample usage:        uart_send_ascii(USART0,ascii_value);
//-------------------------------------------------------------------------------------------------------------------
void uart_send_ascii(unsigned int uart_periph, unsigned char *ascii_value)
{
    unsigned char i;

    for(i=0; i<5; i++)                   
    {
        uart_send_char(uart_periph, ascii_value[i]);
    }                                    //?????????????ascii??
      
    uart_send_char(uart_periph, 0x20);        //????
}
