/*********************************************************************************************************************
* @file            main.c
* @brief           CH9143蓝牙模块收发测试例程
* @author          
* @Target core     GD32F303RCT6
* @revisions       2024.4.30, V1.0
* @modify          none
********************************************************************************************************************/

/********************************************详细说明********************************************

* 1. 如果需要从外部调用库函数，只需将所用到的库函数放在CODE文件中，在工程中添加即可成功编译
* 2. 对于用户而言，只需要更改main函数的内容即可实现想要的功能
* 3. 如果需要声明变量、宏定义、函数等内容，可以放在main.h文件中
* 4. 如果需要用到中断，中断服务函数可以在gd32f30x_it.c文件中找到，只需要添加内容即可
* 5. 如果需要写一些说明性的文档，可以放在doc文件夹的readme.txt文件中

************************************************************************************************/

/** ！！！！！！！！
* ch9143蓝牙模块采用串口通讯
* 相关文件在该项目下的 -> hefei_libraries -> hf_uart.c和hf_uart.h中
* 该例程为两个CH9143蓝牙模块通信例程
* 电脑端通过USB~TTL连接到CH9143模块
* 开发板端将CH9143模块插入主板预留的无线接口的位置
* 电脑端通过串口助手发送字符到主板,主板给电脑端返回接收到的数据
* 如：电脑端通过串口助手发送"DMX",随后收到回复"DMX"
* 注：本模块具有详细的说明文档,可在资料中查看
！！！！！！！！**/

// CH9143蓝牙模块串口号
#define CH9143_UART             USART1
// CH9143蓝牙模块串口中断
#define CH9143_UART_IRQ         USART1_IRQn
// CH9143蓝牙模块波特率
#define CH9143_UART_BAUD        115200
// CH9143蓝牙模块TXD端口
#define CH9143_UART_TX_PORT    	GPIOA
// CH9143蓝牙模块TXD引脚
#define CH9143_UART_TX_PIN      GPIO_PIN_2
// CH9143蓝牙模块RXD端口
#define CH9143_UART_RX_PORT     GPIOA
// CH9143蓝牙模块RXD引脚
#define CH9143_UART_RX_PIN      GPIO_PIN_3

#include "main.h"
#include "headfile.h"

/**
*
* @brief    CH9143_UART发送字符串函数
* @param    str         要发送的字符串首地址
* @return   void
* @notes    遇null停止发送,使用前请先初始化CH9143_UART
* Example:  set_string_uart("1234");
*
**/
void set_string_uart(char *str)
{
   while(*str)
    {
			uart_putchar(CH9143_UART,*str++);
    }
}

uint8_t i=0;

int main(void)
{
    /* 此处声明需要用到的局部变量 */
	unsigned char key_value;

     /* 智能车初始化 */
    car_init();

    car_both_rgb_on(green, 50);                    //智能车左右尾灯亮绿灯

    /* 此处编写单次运行的代码(例如：初始化代码等) */
//	  oled_show_string(0,2,"transmit\0",16);
	
	  // 初始化CH9143蓝牙模块
	  uart_init(CH9143_UART_TX_PORT, CH9143_UART_TX_PIN, CH9143_UART_RX_PORT, CH9143_UART_RX_PIN, CH9143_UART, CH9143_UART_BAUD);
    // 开启串口接收中断
	  uart_interrupt_init(CH9143_UART, USART_INT_RBNE, 1, CH9143_UART_IRQ);
    // 发送字符串
    set_string_uart("This is DMX CH9143 example!\r\n");
	
//	  oled_show_string(0,2,"hello world!\0",16);
    oled_show_number(4,2,i,2,16);
	
    while(1)
    {
        /* 此处编写需要循环运行的代码 */
			key_value = key_check();
			if (key_value != 0xFF)
			{
				if(key_value == 0x01)
				{
					i++;
					oled_clear();
					uart_putchar( USART1 , i);
					oled_show_number(4,2,i,2,16);
				}
			}
        // 该例程为UART收发测试,UART中断相关函数在该项目下的 -> user_c -> gd32f30x_it.c文件中

    }
     
     
}
