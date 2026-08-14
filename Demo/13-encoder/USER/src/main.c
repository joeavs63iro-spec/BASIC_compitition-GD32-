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

    /*智能车初始化*/
    car_init();
    /* 此处编写单次运行的代码(例如：初始化代码等) */
	
    while(1)
    {
        Move_ASYtance(4000, 4000);
        delay_1ms(500);
        Move_Single_Motor(1, 5000);
				car_both_rgb_on(cyan, 500);
        delay_1ms(500);
        Move_ASYtance(7000, 7000);
        delay_1ms(500);
        Move_Single_Motor(2, 5000);
        delay_1ms(500);
        Move_ASYtance(-4000, -4000);
        delay_1ms(500);
        Move_Single_Motor(2, 5000);
        delay_1ms(500);
        Move_ASYtance(7000, 7000);
        delay_1ms(500);
        Move_Single_Motor(1, 5000);
        delay_1ms(500);
        Move_ASYtance(14000, 14000);
        delay_1ms(500);
    		break;//测试函数完成后跳出循环    			

    }
     
     
}


//编码器测试
//			update_encoder_values();
//      delay_1ms(1000);
//			printf("Encoder1: Count=%d, Dir=%d\r\n", motor1_count, motor1_dir);
//      printf("Encoder2: Count=%d, Dir=%d\r\n\r\n", motor2_count, motor2_dir);
// 电机测试
//      motor_backward(left, 6000);
//			motor_forward(right, 6000);
//			delay_1ms(1000);
//        Move_Distance(1000);  // 电机一起前进
//        delay_1ms(1000);
//			  Move_ASYtance(-900,900);  
//        delay_1ms(1000);
//		motor_set(1, MOTOR_STOP, 0);
//		motor_set(2, MOTOR_STOP, 0);
//		delay_1ms(1000);
