/*********************************************************************************************************************
* @file            main.c
* @brief           智能车电磁巡线例程
* @author          
* @Target core     GD32F303RCT6
* @revisions       2025.9.15, V1.0
* @modify          none
********************************************************************************************************************/

#include "headfile.h"




unsigned int st_speed= 2500;//初始速度
float kp= 1.9,kd= 2.8;//上一次，和这一次权重比较
float adc_value[5];//未处理的ADC值  
float ll_max=3000,lm_max=3000,mm_max=3000,rm_max=3000,rr_max=3000; //最开始设置5个通道的ADC最大值
float ll,lm,mm,rm,rr;     //储存归一化并且滤波的ADC值                               
float error[2];//储存上一次，和这一次的变化率


void all_adc_get()  //电感采集
{
	adc_value[0] = adc_get(ADC0,ADC_CH_10);                         
	adc_value[1] = adc_get(ADC0,ADC_CH_11);
	adc_value[2] = adc_get(ADC0,ADC_CH_12);
	adc_value[3] = adc_get(ADC0,ADC_CH_13);
	adc_value[4] = adc_get(ADC0,ADC_CH_15);
	update_max();
}
/*seven_filter(float channel) 滤波说明
通过adc_get()函数连续采集7次ADC0通道10的模拟量值
采用双重循环对7个采样值进行升序排序
去除首尾极值后，取中间5个数值计算算术平均*/
// 通用滤波函数
float seven_filter(float channel) 
{
    float samples[7];
    float sum = 0.0f;
    
    // 采样
    for(int i = 0; i < 7; i++) {
        samples[i] = adc_get(ADC0, channel);
    }
    
    // 冒泡排序
    for(int i = 0; i < 6; i++) {
        for(int j = 0; j < 6-i; j++) {
            if(samples[j] > samples[j+1]) {
                float temp = samples[j];
                samples[j] = samples[j+1];
                samples[j+1] = temp;
            }
        }
    }
    
    // 计算中值平均
    for(int i = 1; i <= 5; i++) {
        sum += samples[i];
    }
    
    return sum / 5.0f;
}
void five_unification()  //归一化
{
	ll = (seven_filter(ADC_CH_10)/ll_max)*100;  
	lm = (seven_filter(ADC_CH_11)/lm_max)*100;
	mm = (seven_filter(ADC_CH_12)/mm_max)*100;
	rm = (seven_filter(ADC_CH_13)/rm_max)*100;
	rr = (seven_filter(ADC_CH_15)/rr_max)*100;
}
void diancixunxian()//调试电磁巡线核心代码、没有加入环岛等元素，需要自己添加
{
	unsigned int left_motor_duty, right_motor_duty;
	long int er_speed;
	
	if((adc_value[1] + adc_value[3]) >=500)   //巡转弯弧度较大的线
	{
				error[0] = 100*(rm - lm)/(lm + rm);
		if(error[0]>0){
			er_speed =error[0]*kp*7 + (error[0]*7 - error[1]*7)*kd;  
			left_motor_duty = st_speed + er_speed;
			right_motor_duty =0;

		}
		else {			
			er_speed =error[0]*kp*7 + (error[0]*7 - error[1]*7)*kd;  
			right_motor_duty = st_speed - er_speed;
			left_motor_duty=0;

		}
		oled_show_string(10,7,"Lm1",12);
		 
	}else    //巡转弯弧度不大的线
	{
		error[0] = 100*(rr-ll)/(ll + rr); 
		oled_show_string(10,7,"LL1",12);
		er_speed =error[0]*kp*7 + (error[0]*7 - error[1]*7)*kd;   

	if(adc_value[2]>=50)
	{	
		left_motor_duty = st_speed + er_speed;
		right_motor_duty = st_speed - er_speed;
		if(left_motor_duty > 7000) 
	{
	    left_motor_duty = 7000;
	}
	else if(left_motor_duty < 1400)    
	{
	    left_motor_duty = 0;
	}
	if(right_motor_duty > 7000)
	{
	    right_motor_duty = 7000;
	}
	else if(right_motor_duty < 1400)
	{
	    right_motor_duty = 0;
	}

}else
		{ left_motor_duty =0;
      right_motor_duty = 0;
			car_stop();
}
	}
	error[1] = error[0];
	motor_forward(left,left_motor_duty);
	motor_forward(right,right_motor_duty);
	oled_show_number(10,6,left_motor_duty,4,12);
	oled_show_number(70,7,right_motor_duty,4,12);

}
void update_max()  //更新最大值
{
			if(adc_value[0] > ll_max)               
			{
				ll_max = adc_value[0];
			}
			if(adc_value[1] > lm_max)
			{
				lm_max = adc_value[1];
			}
			if(adc_value[2] > mm_max)
			{
				mm_max = adc_value[2];
			}
			if(adc_value[3] > rm_max)
			{
				rm_max = adc_value[3];
			}
			if(adc_value[4] > rr_max)
			{
				rr_max = adc_value[4];
			}
}
void oled_show_adc()  //屏幕显示未处理的ADC值 
{
			oled_show_number(30,2,adc_value[0],4,12);
			oled_show_number(30,3,adc_value[1],4,12);
			oled_show_number(30,4,adc_value[2],4,12);
			oled_show_number(30,5,adc_value[3],4,12);
			oled_show_number(30,6,adc_value[4],4,12);
}
void oled_show_gyh() //屏幕显示归一化并且滤波的ADC值  
{
			oled_show_number(5,2,ll,4,12);
			oled_show_number(5,3,lm,4,12);
			oled_show_number(5,4,mm,4,12);
			oled_show_number(5,5,rm,4,12);
			oled_show_number(5,6,rr,4,12);
}
void xunxian() //巡线
{
				
			all_adc_get();  //获得电感值
			update_max();  //更新电感最大值
			five_unification();  //归一化
			diancixunxian();    //电磁巡线
}

