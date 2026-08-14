/*********************************************************************************************************************
* @file            encoder.c
* @brief           智能车电机编码器例程
* @author          
* @Target core     GD32F303RCT6
* @revisions       2025.6.13, V1.0
* @modify          none
********************************************************************************************************************/

/********************************************详细说明********************************************

* 1. 如果需要从外部调用库函数，只需将所用到的库函数放在CODE文件中，在工程中添加即可成功编译
* 2. 对于用户而言，只需要更改main函数的内容即可实现想要的功能
* 3. 如果需要声明变量、宏定义、函数等内容，可以放在main.h文件中
* 4. 如果需要用到中断，中断服务函数可以在gd32f30x_it.c文件中找到，只需要添加内容即可
* 5. 如果需要写一些说明性的文档，可以放在doc文件夹的readme.txt文件中

* 例程说明：
Move_Distance(100);
Move_ASYtance(-90,90); 
Move_Single_Motor(1, 90)
-此例程基于GD32F303学习板与扩展板, 展示编码器部分功能的使用。

************************************************************************************************/
#include "main.h"
#include "headfile.h"





int main(void)
{
    /* 此处声明需要用到的局部变量 */
    unsigned int adc_value[5];

    unsigned char ascii_value_array[5];

    /* 智能车初始化 */
    car_init();
	
    
    while(1)
    {
        /* 此处编写需要循环运行的代码 */
			        
        adc_value[0] = adc_get(ADC0, ADC_CH_10);             
        adc_value[1] = adc_get(ADC0, ADC_CH_11);
        adc_value[2] = adc_get(ADC0, ADC_CH_12);
        adc_value[3] = adc_get(ADC0, ADC_CH_13);
        adc_value[4] = adc_get(ADC0, ADC_CH_15); //采样电位器模拟量后，转换为数字量
        /*
         * Use len=4 and 12px font so 0~4095 ADC values fit on the OLED.
         * The old commented sample used len=3, which clipped four-digit readings.
         */
        oled_show_string(0, 0, (uint8_t *)"ADC", 12);
        oled_show_string(0, 1, (uint8_t *)"A0", 12);
				oled_show_number(24,1,adc_value[0],4,12);
        oled_show_string(0, 2, (uint8_t *)"A1", 12);
				oled_show_number(24,2,adc_value[1],4,12);
        oled_show_string(0, 3, (uint8_t *)"A2", 12);
				oled_show_number(24,3,adc_value[2],4,12);
        oled_show_string(0, 4, (uint8_t *)"A3", 12);
				oled_show_number(24,4,adc_value[3],4,12);
        oled_show_string(0, 5, (uint8_t *)"A4", 12);
				oled_show_number(24,5,adc_value[4],4,12);
//			  for(int i=0;i<5;i++)
//			  {
//					printf("adc_value[%d]=%d\n",i,adc_value[i]);
//					delay_1ms(500);
//				}
			
        int_to_ascii(adc_value[0], ascii_value_array);   //将数字量转换为ascii码
        uart_send_ascii(UART0, ascii_value_array);              //发送5位ascii码
		
        int_to_ascii(adc_value[1], ascii_value_array); 
        uart_send_ascii(UART0, ascii_value_array);
		
        int_to_ascii(adc_value[2], ascii_value_array); 
        uart_send_ascii(UART0, ascii_value_array);
					
        int_to_ascii(adc_value[3], ascii_value_array); 
        uart_send_ascii(UART0, ascii_value_array);
					
        int_to_ascii(adc_value[4], ascii_value_array); 
        uart_send_ascii(UART0, ascii_value_array);

        uart_send_char(UART0, 0x0A);  

        delay_1ms(500);     
    }
}

