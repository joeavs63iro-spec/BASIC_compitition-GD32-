/*********************************************************************************************************************
* @file            main.c
* @brief           ���ܳ����Ѳ������
* @author          
* @Target core     GD32F303RCT6
* @revisions       2025.9.15, V1.0
* @modify          none
********************************************************************************************************************/

#include "headfile.h"




unsigned int st_speed= 2500;//��ʼ�ٶ�
float kp= 1.9,kd= 2.8;//��һ�Σ�����һ��Ȩ�رȽ�
float adc_value[5];//δ������ADCֵ  
float ll_max=3000,lm_max=3000,mm_max=3000,rm_max=3000,rr_max=3000; //�ʼ����5��ͨ����ADC���ֵ
float ll,lm,mm,rm,rr;     //�����һ�������˲���ADCֵ                               
float error[2];//������һ�Σ�����һ�εı仯��


void all_adc_get()  //��вɼ�
{
	adc_value[0] = adc_get(ADC0,ADC_CH_10);    
	adc_value[1] = adc_get(ADC0,ADC_CH_11);
	adc_value[2] = adc_get(ADC0,ADC_CH_12);
	adc_value[3] = adc_get(ADC0,ADC_CH_13);
	adc_value[4] = adc_get(ADC0,ADC_CH_15);
	update_max();
}
/*seven_filter(float channel) �˲�˵��
ͨ��adc_get()���������ɼ�7��ADC0ͨ��10��ģ����ֵ
����˫��ѭ����7������ֵ������������
ȥ����β��ֵ��ȡ�м�5����ֵ��������ƽ��*/
// ͨ���˲�����
float seven_filter(float channel) 
{
    float samples[7];
    float sum = 0.0f;
    
    // ����
    for(int i = 0; i < 7; i++) {
        samples[i] = adc_get(ADC0, channel);
    }
    
    // ð������
    for(int i = 0; i < 6; i++) {
        for(int j = 0; j < 6-i; j++) {
            if(samples[j] > samples[j+1]) {
                float temp = samples[j];
                samples[j] = samples[j+1];
                samples[j+1] = temp;
            }
        }
    }
    
    // ������ֵƽ��
    for(int i = 1; i <= 5; i++) {
        sum += samples[i];
    }
    
    return sum / 5.0f;
}
void five_unification()  //��һ��
{
	ll = (seven_filter(ADC_CH_10)/ll_max)*100;  
	lm = (seven_filter(ADC_CH_11)/lm_max)*100;
	mm = (seven_filter(ADC_CH_12)/mm_max)*100;
	rm = (seven_filter(ADC_CH_13)/rm_max)*100;
	rr = (seven_filter(ADC_CH_15)/rr_max)*100;
}
void diancixunxian()//���Ե��Ѳ�ߺ��Ĵ��롢û�м��뻷����Ԫ�أ���Ҫ�Լ�����
{
	unsigned int left_motor_duty, right_motor_duty;
	long int er_speed;
	
	if((adc_value[1] + adc_value[3]) >=500)   //Ѳת�仡�Ƚϴ����
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
		 
	}else    //Ѳת�仡�Ȳ������
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
void update_max()  //�������ֵ
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
void oled_show_adc()  //��Ļ��ʾδ������ADCֵ 
{
			oled_show_number(30,2,adc_value[0],4,12);
			oled_show_number(30,3,adc_value[1],4,12);
			oled_show_number(30,4,adc_value[2],4,12);
			oled_show_number(30,5,adc_value[3],4,12);
			oled_show_number(30,6,adc_value[4],4,12);
}
void oled_show_gyh() //��Ļ��ʾ��һ�������˲���ADCֵ  
{
			oled_show_number(5,2,ll,4,12);
			oled_show_number(5,3,lm,4,12);
			oled_show_number(5,4,mm,4,12);
			oled_show_number(5,5,rm,4,12);
			oled_show_number(5,6,rr,4,12);
}
void xunxian() //Ѳ��
{
				
			all_adc_get();  //��õ��ֵ
			update_max();  //���µ�����ֵ
			five_unification();  //��һ��
			diancixunxian();    //���Ѳ��
}

