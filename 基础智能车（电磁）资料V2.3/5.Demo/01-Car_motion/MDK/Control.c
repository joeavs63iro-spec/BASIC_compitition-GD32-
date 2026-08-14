#include "Control.h"

unsigned int Distance=0;
float ADC_Deviation_Sum=0;//总偏移量
uint8_t left_flag=0;
uint8_t left_strat=1;

void Lock_BlackLine(void)
{
//	printf("计算偏移值\n");
	unsigned int adc_value[5];
	uint16_t ADC_Grayscale_Cnt=0;//传感器数量
	int ADC_Deviation=0;//单个传感器偏移值
	
	
	adc_value[0] = adc_get(ADC0,ADC_CH_10);
	adc_value[1] = adc_get(ADC0,ADC_CH_11);
	adc_value[2] = adc_get(ADC0,ADC_CH_12);
	adc_value[3] = adc_get(ADC0,ADC_CH_13);
	adc_value[4] = adc_get(ADC0,ADC_CH_15);//最右侧传感器
	
//	printf("adc_value[0]=%d,",adc_value[0]);
//	printf("adc_value[1]=%d,",adc_value[1]);
//	printf("adc_value[2]=%d,",adc_value[2]);
//	printf("adc_value[3]=%d,",adc_value[3]);
//	printf("adc_value[4]=%d\n",adc_value[4]);
//  oled_show_string(0,2,"i\0",16);
	
//	oled_show_string(0,2,(uint8_t *)"1:",16);
//	oled_show_number(32,0,adc_value[0],4,8);
	
	if(adc_value[0] >= 300)  {ADC_Grayscale_Cnt++;ADC_Deviation += 6;}//300的是根据上面的printf查看的传感器的值进行判断的，每台车的值是不一样的，需要进行更改
	if(adc_value[1] >= 300)  {ADC_Grayscale_Cnt++;ADC_Deviation += 2;}
	if(adc_value[2] >= 300)  {ADC_Grayscale_Cnt++;ADC_Deviation += 0;}
	if(adc_value[3] >= 300)  {ADC_Grayscale_Cnt++;ADC_Deviation += -2;}
	if(adc_value[4] >= 300)  {ADC_Grayscale_Cnt++;ADC_Deviation += -6;}
	
	if(ADC_Grayscale_Cnt == 0)//丢线
	{
//		oled_clear();
//		oled_show_number(0,2,0,1,16);
//		printf("丢线\n");
	}
	else if(ADC_Grayscale_Cnt > 0)
	{
		if(ADC_Grayscale_Cnt > 5)
		{
			
		}
		else
		{
			if(left_strat == 1)
			{
				if(((adc_value[2] >= 300) && (adc_value[1] >= 300)) || ((adc_value[1] >= 300) && (adc_value[0] >= 300))) 
				{
					left_flag=1;
					left_strat=0;
				}
				else
				{
					ADC_Deviation_Sum=(ADC_Deviation/ADC_Grayscale_Cnt);//计算总偏移量
				}
			}
			else
			{
				ADC_Deviation_Sum=(ADC_Deviation/ADC_Grayscale_Cnt);//计算总偏移量
			}
//			oled_clear();
//			oled_show_number(4,2,ADC_Deviation_Sum,4,16);
//			printf("总偏移值：%f",ADC_Deviation_Sum);
		}
	}
	ADC_Grayscale_Cnt=0;//传感器计数清零
	ADC_Deviation=0;//偏移量清零
}

void Car_Control(void)
{
//	Distance = ultra_get_distance();
//	printf("超声波测距距离：%d\n",Distance);
//	delay_1ms(200);
	Lock_BlackLine();
	if(ADC_Deviation_Sum > 0)//当总偏移值大于0时，小车左轮加速，使小车中间传感器回到线上
	{
		
//		if(ADC_Deviation_Sum > 5)
//		{
//			motor_forward(left, 0);
//			motor_forward(right, 3000+ADC_Deviation_Sum*1000);//1200为系数，根据实际情况进行修改，速度也是一样
//		}
//		else
//		{
			motor_forward(left, 5000);
		  motor_forward(right, 5000+ADC_Deviation_Sum*1500);//1200为系数，根据实际情况进行修改，速度也是一样
//		}
	}
	else if(ADC_Deviation_Sum < 0)//当总偏移值小于0时，小车右轮加速，是小车中间传感器回到线上，此处小车左右轮根据实际情况进行修改，连接方式不同，代码里面的左右轮与实际情况的左右轮是不同的
	{
//		if(ADC_Deviation_Sum < -5)
//		{
//			motor_forward(left, 3000-ADC_Deviation_Sum*1000);
//		  motor_forward(right, 0);
//		}
//		else
//		{
			motor_forward(left, 5000-ADC_Deviation_Sum*1500);
			motor_forward(right, 5000);
//		}
	}
	else if(left_flag == 1)
	{
		left_flag=0;
		car_turnleft();
		delay_1ms(500);
	}
	else
	{
		motor_forward(left, 5000);
		motor_forward(right, 5000);
	}
//	delay_1ms(2);

}




