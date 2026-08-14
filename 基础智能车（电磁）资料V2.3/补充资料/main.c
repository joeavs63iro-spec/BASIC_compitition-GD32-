/*********************************************************************************************************************
* @file            main.c
* @brief           智能车超声波测距例程
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.10.08, V1.0
* @modify          none
********************************************************************************************************************/

/********************************************详细说明********************************************

* 1. 如果需要从外部调用库函数，只需将所用到的库函数放在CODE文件中，在工程中添加即可成功编译
* 2. 对于用户而言，只需要更改main函数的内容即可实现想要的功能
* 3. 如果需要声明变量、宏定义、函数等内容，可以放在main.h文件中
* 4. 如果需要用到中断，中断服务函数可以在gd32f30x_it.c文件中找到，只需要添加内容即可
* 5. 如果需要写一些说明性的文档，可以放在doc文件夹的readme.txt文件中

* 例程说明：

-此例程基于GD32F303学习板与扩展板, 控制小车超声波模块测障碍物距离，并将测得距离用串口发送。

************************************************************************************************/

#include "main.h"
#include "headfile.h"

  unsigned int get_dis;
  unsigned int adc_value[5];		
  unsigned char key_value;//按键使用
	int ADC_value[5];				
	int LADC_value[5]={0};
	int car_error;					
	int car_error_old=0;				
	int adc_value_min=0;		
	int adc_value_max=3400;		
	int valueL,valueR;		

void show_number();	
void Motor_OUT(int valueL,int valueR);
void Motor_OUT_B(int valueL,int valueR);
void car_color(int key);

//	void car_ws28_zy(u8 ws28z, u8 ws28y);
//	u8 car_wsz28(unsigned int lmd);
//	u8 car_wsy28(unsigned int lmd); //lmd灵敏度值需要>300



int main(void)
{
    car_init();							//智能车初始化
	  oled_init();          //OLED初始化
	
    oled_display_on();  // 开启OLED显示 
	
	  Init_TOF400C();     //tof模块初始化
	
	TCS34725_Init();
  TCS34725_2_Init();
  ws2812b_init();
	u8 ws28z=0,ws28y=0;
	
	 /* 此处声明需要用到的局部变量 */
		unsigned int  distance_value;
    unsigned char distance_ascii[6];
	  int out2=0,out1=0,out3=0,out4=0,out5=0,out6=0,out7=0,out12=0;
		int outc1=0,outc2=0,outc3=0,outc4=0;
 	  int led=0;//灯光闪烁要求代码使用
    int outb1=0,outb2=0,outb3=0;
	  int outw1=0,outw2=0,outw3=0,outw4=0,outw5=0,outw6=0,outw7=0,wan=1;
	  int key1=0,key2=0,key3=0,key4=0,key5=0,key6=0;
	  
    while(1)
    {
						oled_show_number(70,0,0,3,10);		
						ws28z=car_wsz28(650);
						ws28y=car_wsy28(720);
						car_ws28_zy(ws28z,ws28y);
			      oled_show_number(50,0,1,3,10);	
			      key_value = key_check();
			      if(key_value == 0x01)
            {
							   key1=1;
                 car_both_rgb_on(yellow, 50);  //按下按键1，尾灯显示黄色
							   board_led_off(LED1);
            }
            else if(key_value == 0x02)
            {    key1=1;
                car_both_rgb_on(red, 50);     //按下按键2，尾灯显示红色
            }
            else if(key_value == 0x03)
            {
							  key3=1;
                car_both_rgb_on(blue, 50);    //按下按键3，尾灯显示蓝色
            }
            else if(key_value == 0x04)
            {
						    key4++;
                car_both_rgb_on(purple, 50);   //按下按键4，尾灯显示紫色
            }
            else if(key_value == 0x05)
            {
                car_both_rgb_on(green, 50);   //按下按键5，尾灯显示绿色
            }
            else if(key_value == 0x06)
            {
                car_both_rgb_on(cyan, 50);   //按下按键6，尾灯显示青色
            }
				  	oled_show_number(70,4,key4,3,10);		
//2500速度  key1,2
	while((key1==1)||(key2==1))
	{
								if(led==0)//灯光闪烁要求
			{
			car_both_rgb_on(red,300);
			car_both_rgb_on(green,300);
			car_both_rgb_off();//尾灯熄灭
			led++;
			}
			show_number();
			
			car_error=ADC_value[0]-ADC_value[4];
			 distance_value = ultra_get_distance();              //超声波模块测距  
			 int_to_ascii(distance_value, distance_ascii);       //将测得距离转换为ascii码
			  oled_show_char(0, 6, distance_ascii[0], 10);   
				oled_show_char(10, 6, distance_ascii[1], 10);   
				oled_show_char(20, 6, distance_ascii[2], 10);
				oled_show_char(30, 6, distance_ascii[3], 10);
				oled_show_char(40, 6, '.', 10);
				oled_show_char(50, 6, distance_ascii[4], 10);  
			 //uart_send_distance_ascii(distance_ascii);           //串口发送测得的距离////在助手中展示
        get_dis = Get_Distance_TOF400C();// 获取TOF400C模块测量距离
				oled_show_number(70,6,get_dis,3,10);		

   if((RESET == gpio_get_input(REED_PORT, REED_PIN))&&outc1==0)//一号磁标（已经注释，后续磁标无法使用）
		{
						Motor_OUT(3000,3500);
            delay_1ms(400);			
						car_both_rgb_on(green, 50);                        //智能车左右尾灯亮绿灯

			outc1=1;
		}
		if((RESET == gpio_get_input(REED_PORT, REED_PIN))&&outc1==1&&outc2==0)//二号磁标：入库   //入库完后要停一下，留给颜色识别                               //要测
		{
						//	car_both_rgb_on(red, 50);                       
            Motor_OUT(3000,3000);   //库口就位
            delay_1ms(600);
							//Motor_OUT(0,0);                   //会顿一下（测完之后把所有00删掉）
            //delay_1ms(700);
						Motor_OUT(6500,3500);   //入库
            delay_1ms(550);
							//Motor_OUT(0,0);
            //delay_1ms(700);
            Motor_OUT(3000,3000);   //加深位置
            delay_1ms(500);
						Motor_OUT(0,0);   
            delay_1ms(700);
			      Motor_OUT_B(3000,3000); //倒到库口                    //带B为倒退函数 
			      delay_1ms(500);
							//Motor_OUT(0,0);
            //delay_1ms(700);
						Motor_OUT_B(6500,3500);  //倒车出库
            delay_1ms(450);
							//Motor_OUT(0,0);
            //delay_1ms(700);
						Motor_OUT_B(3000,3000); //倒车
            delay_1ms(400);	

//						Motor_OUT_B(3000,3000);
//            delay_1ms(400);	
//						Motor_OUT(0,0);
//            delay_1ms(700);
//						Motor_OUT(0,0);
//            delay_1ms(700);
//						car_color(key4);
//						Motor_OUT(0,0);
//            delay_1ms(700);					
//						Motor_OUT(0,0);
//            delay_1ms(700);			
//						Motor_OUT(0,0);
//            delay_1ms(700);
						
			outc2=1;
		}
		
		if(	(RESET == gpio_get_input(REED_PORT, REED_PIN))&&outc2==1&&outc3==0)//三号磁标：单边桥        （）         //要测
		{
			  oled_show_number(70,0,3,3,10);		
				car_both_rgb_on(yellow, 50); 
	      
				//省略单边桥的执行代码
//				Motor_OUT(3000,3000);
//        delay_1ms(500);
//		  	Motor_OUT(2000,2000);
//   		  delay_1ms(300);

				//单边桥代码
					Motor_OUT(0,0);
					delay_1ms(500);
				Motor_OUT(2000,0);      //向右转向
				delay_1ms(350);
					Motor_OUT(0,0);
					delay_1ms(500);
				Motor_OUT(2000,2000);   //斜着前进
				delay_1ms(350);   //380
					Motor_OUT(0,0);
					delay_1ms(500);
				Motor_OUT(0,2000);   //矫正方向，对准单边桥方向 
				delay_1ms(400);   //360
					Motor_OUT(0,0);
					delay_1ms(500);
				Motor_OUT(3000,3000);   //前进
				delay_1ms(600);
					Motor_OUT(0,0);
					delay_1ms(500);
				Motor_OUT(4500,4000);
				delay_1ms(800);         //过单边桥
					Motor_OUT(0,0);
					delay_1ms(500);
				Motor_OUT(3000,4500);   //回到中线（方向是斜的，剩下靠PID调整方向，是否人工调整看后续情况）
				delay_1ms(100);
				
				
				outc3=1;
		}   
		if(	(RESET == gpio_get_input(REED_PORT, REED_PIN))&&outc3==1&&outc4==0)//四号磁标                                 
		{
			Motor_OUT(4000,4300);
      delay_1ms(700);	
      			
				while(1)
					{
				 	car_both_rgb_on(red, 50); 

					  Motor_OUT(0,0);
            delay_1ms(200);
					}
			outc4=1;
		}   
		
////环岛
////		if(ADC_value[0]<=10&&ADC_value[1]<=10&&ADC_value[2]>=85&&ADC_value[3]>=0&&ADC_value[3]<=20&&ADC_value[4]<=60&&ADC_value[4]>=40&&out1==0)//一号
////{
////		Motor_OUT(9000,9700);
////	  delay_1ms(300);
////		out1=1;
////}   
//if(ADC_value[0]<=10&&ADC_value[1]<=10&&ADC_value[2]>=80&&ADC_value[3]>=90&&ADC_value[4]>=40&&out2==0)//二号
//{
//		Motor_OUT(9700,4500);
//	  delay_1ms(700);
//		out2=1;
//}   
////if(ADC_value[0]<=15&&ADC_value[1]>=60&&ADC_value[2]>=80&&ADC_value[3]>=90&&ADC_value[4]>=30&&out2==1&&out3==0)//三号
////{
////		
////    Motor_OUT(0,0);
////   delay_1ms(700);
////	Motor_OUT(0,0);
////   delay_1ms(700);
////	 Motor_OUT(0,0);
////	Motor_OUT(9000,4000);
////	  delay_1ms(450);
////		out3=1;
////}   
//if(ADC_value[0]<=10&&ADC_value[1]<=10&&ADC_value[2]>=90&&ADC_value[3]<=10&&ADC_value[4]<=10&&out2==1&&out12==0)//1，2号之间地点
//{
////	 Motor_OUT(0,0);
////   delay_1ms(700);
////	 Motor_OUT(0,0);
////   delay_1ms(700);
////	 Motor_OUT(0,0);
////   delay_1ms(700);
//		oled_show_number(30,1,2,3,10);

//		out12=1;
//}   
//if(ADC_value[0]<=10&&ADC_value[1]<=10&&ADC_value[2]>=80&&ADC_value[3]>=90&&ADC_value[4]>=30&&out12==1&&out4==0)//4号////若不用out3而用out1,2号4号判断语句可能连续直接运行
//{
//		Motor_OUT(8700,9800);
//	  delay_1ms(700);
//		out4=1;
//}   


//避障代码//3：51&&distance_ascii[2]==51    //向右拐
			if(distance_ascii[1]==52&&distance_ascii[0]==48&&distance_ascii[2]<=53&&get_dis<=730&& get_dis>=500&&out7==0)
			{
				Motor_OUT(4500,3000);
				delay_1ms(500);	
					//Motor_OUT(0,0);
					//delay_1ms(900);
				Motor_OUT(3000,3000);
				delay_1ms(300);	
					//Motor_OUT(0,0);
					//delay_1ms(900);
				Motor_OUT(3000,6000);
				delay_1ms(500);	
					//Motor_OUT(0,0);
					//delay_1ms(900);
				Motor_OUT(3000,3000);
				delay_1ms(600);	
					//Motor_OUT(0,0);
					//delay_1ms(900);
				Motor_OUT(5000,3000);
				delay_1ms(400);	
					//Motor_OUT(0,0);
					//delay_1ms(900);

				out7=1;
			 }
			
			 if(ADC_value[0]<10&&ADC_value[1]<10&&ADC_value[2]<10&&ADC_value[3]<10&&ADC_value[4]<10)//出赛道，后退
       {
           Motor_OUT_B(3000,3000);
           delay_1ms(800);
       }
}
//4500速度：   key3
while(key3==1)
{
			if(led==0)//灯光闪烁要求
			{
			car_both_rgb_on(red,300);
			car_both_rgb_on(green,300);
			car_both_rgb_off();//尾灯熄灭
			led++;
			}
			show_number();
			
			car_error=ADC_value[0]-ADC_value[4];
			int Kp=18; 
			int Kd=60;
			valueL=car_error*(-Kp)+(car_error-car_error_old)*(-Kd)+4500;
			valueR=car_error*Kp+(car_error-car_error_old)*Kd+4500;
			Motor_OUT(valueL,valueR);
			car_error_old=car_error;			
				 
			 distance_value = ultra_get_distance();              //超声波模块测距  
			 int_to_ascii(distance_value, distance_ascii);       //将测得距离转换为ascii码
			  oled_show_char(0, 6, distance_ascii[0], 10);   
				oled_show_char(10, 6, distance_ascii[1], 10);   
				oled_show_char(20, 6, distance_ascii[2], 10);
				oled_show_char(30, 6, distance_ascii[3], 10);
				oled_show_char(40, 6, '.', 10);
				oled_show_char(50, 6, distance_ascii[4], 10);  
			 //uart_send_distance_ascii(distance_ascii);           //串口发送测得的距离////在助手中展示
        get_dis = Get_Distance_TOF400C();// 获取TOF400C模块测量距离
				oled_show_number(70,6,get_dis,3,10);		

   if((RESET == gpio_get_input(REED_PORT, REED_PIN))&&outc1==0)//一号磁标
		{
						Motor_OUT(4000,4500);
            delay_1ms(300);			
							car_both_rgb_on(green, 50);                        //智能车左右尾灯亮绿灯

			outc1=1;
		}
		
		if((RESET == gpio_get_input(REED_PORT, REED_PIN))&&outc1==1&&outc2==0)//二号磁标：入库   //入库完后要停一下，留给颜色识别      //新版   ()                      //要测
		{
						//	car_both_rgb_on(red, 50);                       
            Motor_OUT(3000,3000);   //库口就位
            delay_1ms(480);
							//Motor_OUT(0,0);                   //会顿一下（测完之后把所有00删掉）
            //delay_1ms(700);
						Motor_OUT(6500,3500);   //入库
            delay_1ms(550);
							//Motor_OUT(0,0);
            //delay_1ms(700);
            Motor_OUT(3000,3000);   //加深位置
            delay_1ms(500);
						Motor_OUT(0,0);   
            delay_1ms(700);
			      Motor_OUT_B(3000,3000); //倒到库口                    //带B为倒退函数 
			      delay_1ms(500);
							//Motor_OUT(0,0);
            //delay_1ms(700);
						Motor_OUT_B(6500,3500);  //倒车出库
            delay_1ms(450);
							//Motor_OUT(0,0);
            //delay_1ms(700);
						Motor_OUT_B(3000,3000); //倒车
            delay_1ms(400);	
						outc2=1;
		}		
		
//		if((RESET == gpio_get_input(REED_PORT, REED_PIN))&&outc1==1&&outc2==0)//二号磁标：入库 //旧版
//		{
//						//	car_both_rgb_on(red, 50);                       
//            Motor_OUT(3000,3000);
//            delay_1ms(700);
//			Motor_OUT(0,0);
//            delay_1ms(700);
//						Motor_OUT(6500,3500);
//            delay_1ms(600);
//			Motor_OUT(0,0);
//            delay_1ms(700);
//            Motor_OUT(3000,3000);
//            delay_1ms(350);
//		        Motor_OUT(0,0);
//            delay_1ms(700);
//			      Motor_OUT_B(3000,3000);
//			      delay_1ms(350);
//			Motor_OUT(0,0);
//            delay_1ms(700);
//						Motor_OUT_B(6000,4000);
//            delay_1ms(550);
//			Motor_OUT(0,0);
//            delay_1ms(700);
//						Motor_OUT_B(3000,3000);
//            delay_1ms(400);	

////						Motor_OUT_B(3000,3000);
////            delay_1ms(400);	
////						Motor_OUT(0,0);
////            delay_1ms(700);
////						Motor_OUT(0,0);
////            delay_1ms(700);
////						car_color(key4);
////						Motor_OUT(0,0);
////            delay_1ms(700);					
////						Motor_OUT(0,0);
////            delay_1ms(700);			
////						Motor_OUT(0,0);
////            delay_1ms(700);

		
		if(	(RESET == gpio_get_input(REED_PORT, REED_PIN))&&outc2==1&&outc3==0)//三号磁标：单边桥        （）         //要测
		{
			  oled_show_number(70,0,3,3,10);		
				car_both_rgb_on(yellow, 50); 
	      
				//省略单边桥的执行代码
//				Motor_OUT(3000,3000);
//        delay_1ms(500);
//		  	Motor_OUT(2000,2000);
//   		  delay_1ms(300);

				//单边桥代码
					Motor_OUT(0,0);
					delay_1ms(500);
				Motor_OUT(2000,0);      //向右转向
				delay_1ms(350);
					Motor_OUT(0,0);
					delay_1ms(500);
				Motor_OUT(2000,2000);   //斜着前进
				delay_1ms(200);
					Motor_OUT(0,0);
					delay_1ms(500);
				Motor_OUT(0,2000);   //矫正方向，对准单边桥方向 
				delay_1ms(400);
					Motor_OUT(0,0);
					delay_1ms(500);
				Motor_OUT(3000,3000);   //前进
				delay_1ms(300);
					Motor_OUT(0,0);
					delay_1ms(500);
				Motor_OUT(4500,4000);
				delay_1ms(800);         //过单边桥
					Motor_OUT(0,0);
					delay_1ms(500);
				Motor_OUT(3000,4500);   //回到中线（方向是斜的，剩下靠PID调整方向，是否人工调整看后续情况）
				delay_1ms(100);
				
				
				outc3=1;
		}   
		
		if(	(RESET == gpio_get_input(REED_PORT, REED_PIN))&&outc3==1&&outc4==0)//四号磁标
		{
			Motor_OUT(5000,5300);
      delay_1ms(700);	
					
				while(1)
					{
				 	car_both_rgb_on(red, 50); 

					  Motor_OUT(0,0);
            delay_1ms(200);
					}
			outc4=1;
		}   
		//环岛
//		if(ADC_value[0]<=10&&ADC_value[1]<=10&&ADC_value[2]>=90&&ADC_value[3]>=0&&ADC_value[3]<=20&&ADC_value[4]<=40&&ADC_value[4]>=10&&out1==0)//一号
//		{
//		  	oled_show_number(50,0,1,3,10);		

//				Motor_OUT(0,0);
//				delay_1ms(700);
//				Motor_OUT(0,0);
//				delay_1ms(700);
//				Motor_OUT(0,0);
//				delay_1ms(700);
//				Motor_OUT(0,0);
//				delay_1ms(700);
//				Motor_OUT(5000,5000);
//				delay_1ms(300);
//				out1=1;
//		}   
//		if(ADC_value[0]<=10&&ADC_value[1]<=10&&ADC_value[2]>=80&&ADC_value[3]>=80&&ADC_value[4]>=30&&out2==0)//二号
//		{
//			oled_show_number(50,2,2,3,10);		

//				Motor_OUT(0,0);
//				delay_1ms(700);
//				Motor_OUT(0,0);
//				delay_1ms(700);
//				Motor_OUT(0,0);
//				delay_1ms(700);
//				Motor_OUT(0,0);
//				delay_1ms(700);
//				Motor_OUT(5000,3000);
//				delay_1ms(600);
//			out2=1;
//		}   
//		if(ADC_value[0]<=15&&ADC_value[1]>=60&&ADC_value[2]>=80&&ADC_value[3]>=90&&ADC_value[4]>=30&&out2==1&&out3==0)//三号
//		{
//						  	oled_show_number(50,3,3,3,10);		

//				Motor_OUT(0,0);
//			 delay_1ms(700);
//			Motor_OUT(0,0);
//			 delay_1ms(700);
//			 Motor_OUT(0,0);
//			 delay_1ms(700);
//			 Motor_OUT(0,0);
//			 delay_1ms(700);
//			//Motor_OUT(9000,4000);
//				delay_1ms(450);
//				out3=1;
//		}   
//		if(ADC_value[0]<=10&&ADC_value[1]<=10&&ADC_value[2]>=90&&ADC_value[3]<=10&&ADC_value[4]<=10&&out2==1&&out12==0)//1，2号之间地点
//		{
//				 Motor_OUT(0,0);
//				 delay_1ms(700);
//				 Motor_OUT(0,0);
//				 delay_1ms(700);
//				 Motor_OUT(0,0);
//				 delay_1ms(700);
//			oled_show_number(30,1,2,3,10);

//			out12=1;
//		}   

//		if(ADC_value[0]<=10&&ADC_value[1]<=10&&ADC_value[2]>=80&&ADC_value[3]>=90&&ADC_value[4]>=30&&out12==1&&out4==0)//4号////若不用out3而用out1,2号4号判断语句可能连续直接运行
//		{
//			Motor_OUT(8700,9800);
//			delay_1ms(700);
//			out4=1;
//		}   


		//避障代码//3：51&&distance_ascii[2]==51
			if(distance_ascii[0]==48&&distance_ascii[1]==52&&distance_ascii[2]<=57&&get_dis<=730&& get_dis>=450&&out7==0)
			{
				Motor_OUT(4500,3000);
				delay_1ms(500);	
					//Motor_OUT(0,0);
					//delay_1ms(900);
				Motor_OUT(3000,3000);
				delay_1ms(300);	
					//Motor_OUT(0,0);
					//delay_1ms(900);
				Motor_OUT(3000,6000);
				delay_1ms(500);	
					//Motor_OUT(0,0);
					//delay_1ms(900);
				Motor_OUT(3000,3000);
				delay_1ms(600);	
					//Motor_OUT(0,0);
					//delay_1ms(900);
				Motor_OUT(5000,3000);
				delay_1ms(400);	
					//Motor_OUT(0,0);
					//delay_1ms(900);

				out7=1;
			 }
				
				
//				if(distance_ascii[1]==52&&distance_ascii[0]==48&&distance_ascii[2]<=53&&get_dis<=730&& get_dis>=500&&out7==0)
//				{
//					Motor_OUT(3000,4500);
//					delay_1ms(500);	
//					Motor_OUT(0,0);
//						delay_1ms(900);
//					Motor_OUT(3000,3000);
//					delay_1ms(300);	
//					Motor_OUT(0,0);
//						delay_1ms(900);
//					Motor_OUT(6000,3000);
//					delay_1ms(700);	
//					Motor_OUT(0,0);
//						delay_1ms(900);
//					Motor_OUT(3000,3000);
//					delay_1ms(400);	
//					Motor_OUT(0,0);
//						delay_1ms(900);
//						Motor_OUT(3000,4500);
//					delay_1ms(500);	
//					Motor_OUT(0,0);
//						delay_1ms(900);

//					out7=1;
//				}
				
				if(ADC_value[0]<10&&ADC_value[1]<10&&ADC_value[2]<10&&ADC_value[3]<10&&ADC_value[4]<10)//出赛道，后退
       {
           Motor_OUT_B(3000,3000);
           delay_1ms(800);
       }

}			


		}
}

//void uart_send_distance_ascii(unsigned char *ascii_value) //发送函数：串口发送测得的距离////在助手中展示
//			
//{
//    uart_send_char(UART0, ascii_value[1]);   
//    uart_send_char(UART0, ascii_value[2]);
//    uart_send_char(UART0, ascii_value[3]);
//    uart_send_char(UART0, '.');
//    uart_send_char(UART0, ascii_value[4]);
//    uart_send_char(UART0, 0x0D);
//    uart_send_char(UART0, 0x0A);                            //换行
//}

void Motor_OUT(int valueL,int valueR)
{
	if(valueL>=0&&valueR>=0)
	{
		motor_forward(left, valueR);
		motor_forward(right, valueL);
	}
}

void Motor_OUT_B(int valueL,int valueR)
{
	if(valueL>=0&&valueR>=0)
	{
  motor_backward(left, valueR);
	motor_backward(right,valueL);
	}
}
void car_color(int key)
{
				switch(key){
				case 1 :{
				car_left_rgb_flash(white,3,1000);
				car_right_rgb_flash(red,3,1000);
        car_both_rgb_off();
					break;
					}
				case 2 :ws2812b_write(rgb_green);	break;
				case 3 :ws2812b_write(rgb_blue);	break;
				case 4 :ws2812b_write(rgb_off);	break;
				case 5 :ws2812b_write(rgb_off);	break;
				case 6 :ws2812b_write(rgb_off);	break;

			  default :ws2812b_write(rgb_off);
			
			}

}
void show_number()
{
				adc_value[0] = adc_get(ADC0, ADC_CH_10);  	//??????LL           
				adc_value[1] = adc_get(ADC0, ADC_CH_11);		//??????L
				adc_value[2] = adc_get(ADC0, ADC_CH_12);		//??????M 
				adc_value[3] = adc_get(ADC0, ADC_CH_13);		//??????R 
				adc_value[4] = adc_get(ADC0, ADC_CH_15);		//??????RR
		//???????	
				if(adc_value[0]>adc_value_max)
					adc_value[0]=adc_value_max;		
				if(adc_value[4]>adc_value_max)
					adc_value[4]=adc_value_max;						
		//???
				ADC_value[0]=100*(adc_value[0]-adc_value_min)/(adc_value_max-adc_value_min);   
				ADC_value[1]=100*(adc_value[1]-adc_value_min)/(adc_value_max-adc_value_min);
				ADC_value[2]=100*(adc_value[2]-adc_value_min)/(adc_value_max-adc_value_min);
				ADC_value[3]=100*(adc_value[3]-adc_value_min)/(adc_value_max-adc_value_min);
				ADC_value[4]=100*(adc_value[4]-adc_value_min)/(adc_value_max-adc_value_min);
		//??????????)
				oled_show_number(0,0,ADC_value[0],3,10);		
				oled_show_number(0,1,ADC_value[1],3,10);
				oled_show_number(0,2,ADC_value[2],3,10);
				oled_show_number(0,3,ADC_value[3],3,10);
				oled_show_number(0,4,ADC_value[4],3,10);
}

//void set_string_uart(char *str)
//{
//  while(*str)
//    {
//			uart_putchar(CH9143_UART,*str++);//cH9143_UARt串口需要在main.h中定义
//    }
//}
