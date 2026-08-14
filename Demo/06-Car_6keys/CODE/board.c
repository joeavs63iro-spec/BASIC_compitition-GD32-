/*********************************************************************************************************************
* @file            board.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.19, V1.0
* @modify          none
********************************************************************************************************************/

#include "board.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief        板载LED初始化   
// @param        led_num            选择LED：LEDx(x = 1.2)
// @return       void
// Sample usage:       board_led_init(LED1);//板载LED1初始化
//-------------------------------------------------------------------------------------------------------------------
void board_led_init(unsigned char led_num)
{
    if(led_num == LED1)
    {
        gpio_pin_init(LED1_PORT, LED1_MODE, LED1_PIN);//配置LED1的引脚
        gpio_bit_output(LED1_PORT, LED1_PIN, OFF);//LED1熄灭
    }
    if(led_num == LED2)
    {
        gpio_pin_init(LED2_PORT, LED2_MODE, LED2_PIN);//配置LED2的引脚
        gpio_bit_output(LED2_PORT, LED2_PIN, OFF);//LED2熄灭
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        板载LED点亮   
// @param        led_num            选择LED：LEDx(x = 1.2)
// @return       void
// Sample usage:       board_led_on(LED1);//点亮板载LED1
//-------------------------------------------------------------------------------------------------------------------
void board_led_on(unsigned char led_num)
{
    if(led_num == LED1)
    {
        gpio_bit_output(LED1_PORT, LED1_PIN, ON);
    }
    if(led_num == LED2)
    {
        gpio_bit_output(LED2_PORT, LED2_PIN, ON);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        板载LED熄灭
// @param        led_num            选择LED：LEDx(x = 1.2)
// @return       void
// Sample usage:       board_led_off(LED1);//板载LED1熄灭
//-------------------------------------------------------------------------------------------------------------------
void board_led_off(unsigned char led_num)
{
    if(led_num == LED1)
    {
        gpio_bit_output(LED1_PORT, LED1_PIN, OFF);
    }
    if(led_num == LED2)
    {
        gpio_bit_output(LED2_PORT, LED2_PIN, OFF);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        板载LED电平翻转
// @param        led_num            选择LED：LEDx(x = 1.2)
// @return       void
// Sample usage:       board_led_toggle(LED1);//板载LED1电平翻转
//-------------------------------------------------------------------------------------------------------------------
void board_led_toggle(unsigned char led_num)
{
    if(led_num == LED1)
    {
        gpio_toggle(LED1_PORT, LED1_PIN);
    }
    if(led_num == LED2)
    {
        gpio_toggle(LED2_PORT, LED2_PIN);
    }
}





 
COLOR_RGBC rgb;
COLOR_HSL  hsl;
 
 
void TCS34725_I2C_Init( void )
{
    gpio_pin_init(GPIOA, OUT_PP, GPIO_PIN_11);	
	  gpio_pin_init(GPIOA, OUT_PP, GPIO_PIN_12);	
}
/**
  * @brief  配置TCS34725颜色传感器的SDA线为推挽输出模式
  * @note   用于主机（MCU）主动控制I2C通信时的数据线电平（发送数据或ACK/NACK信号）
  * @param  无
  * @retval 无
  */
static void TCS_SDA_OUT()
{
	  gpio_pin_init(GPIOA, OUT_PP, GPIO_PIN_12); // 初始化GPIOA的PIN12为推挽输出
}
/**
  * @brief  配置TCS34725颜色传感器的SDA线为浮空输入模式
  * @note   用于主机（MCU）释放SDA线控制权，从机（TCS34725）可返回数据
  * @param  无
  * @retval 无
  */
static void TCS_SDA_IN()
{
	  gpio_pin_init(GPIOA, IN_FLOATING, GPIO_PIN_12);// 初始化GPIOA的PIN12为浮空输入
}
 
/*************IIC起始时序*********************/
void TCS34725_I2C_Start()
{
	TCS_SDA_OUT();
	TCS_SDA_H;
	TCS_SCL_H;
	delay_1us(4);//delay_us(4);
	TCS_SDA_L;
	delay_1us(4);//delay_us(4);
	TCS_SCL_L;
}
/***************IIC停止信号*********************/
void TCS34725_I2C_Stop()
{
	TCS_SDA_OUT();
	TCS_SCL_L;
	TCS_SDA_L;
	delay_1us(4);//delay_us(4);
	TCS_SCL_H;
	TCS_SDA_H;
	delay_1us(4);//delay_us(4);							   	
}

/*** 等待从机应答：0有应答  1超时无应答***/
u8 TCS34725_I2C_Wait_ACK()
{
	u32 t=0;
	
	TCS_SDA_IN();
	TCS_SDA_H; 
	delay_1us(1);//delay_us(1);
	TCS_SCL_H; 
	delay_1us(1);//delay_us(1);
	while(TCS_SDA_READ)
	{
		t++;
		if(t > 250)
		{
			TCS34725_I2C_Stop();
			return 1;
		}
	}
	TCS_SCL_L;
	return 0;	
}

/*** 函 数 说 明：主机发送应答或者非应答信号：0发送应答  1发送非应答**SDA拉低，表示继续通信。*/
void TCS34725_I2C_ACK()
{
	TCS_SCL_L;
	TCS_SDA_OUT();// Set SDA as output (host controls the line)
	TCS_SDA_L;
	delay_1us(2);//delay_us(2);
	TCS_SCL_H;
	delay_1us(2);//delay_us(2);
	TCS_SCL_L;
}
/*****************非应答信号**SDA拉高，表示停止接收或错误***********/
		    
void TCS34725_I2C_NACK()
{
	TCS_SCL_L;
	TCS_SDA_OUT();//sda???
	TCS_SDA_H;
	delay_1us(2);//delay_us(2);
	TCS_SCL_H;
	delay_1us(2);//delay_us(2);
	TCS_SCL_L;
}
/***********写入一个字节：dat要写入的数据**********************/  
void TCS34725_I2C_Send_Byte(u8 byte)
{
	u8 i;
	
	TCS_SDA_OUT();
	TCS_SCL_L;
	for(i = 0; i < 8; i++)
	{
		if(((byte&0x80)>>7)==1)
		{
			  TCS_SDA_H;
		}
		else
		{
			  TCS_SDA_L;
		}
		byte <<= 1;
		
		delay_1us(2);//delay_us(2);
		TCS_SCL_H;
		delay_1us(2);//delay_us(2);
		TCS_SCL_L;
		delay_1us(2);//delay_us(2);
	} 
}
/*************IIC读时序*****************/  
u8 TCS34725_I2C_Read_Byte(u8 ack)
{
	u8 i,receive = 0;
	
	TCS_SDA_IN();
	for(i = 0; i < 8; i++)
	{
		TCS_SCL_L;
		delay_1us(2);//delay_us(2);
		TCS_SCL_H;
		receive <<= 1;
		if(TCS_SDA_READ) receive++;
		delay_1us(1);//delay_us(1);
	}
	if (!ack) TCS34725_I2C_NACK();//??nACK
	else TCS34725_I2C_ACK(); //??ACK 
	
	return receive;
}
/*******************************************************************************
 * @brief Writes data to a slave device.
 *
 * @param slaveAddress - Adress of the slave device.
 * @param dataBuffer - Pointer to a buffer storing the transmission data.
 * @param bytesNumber - Number of bytes to write.
 * @param stopBit - Stop condition control.
 *                  Example: 0 - A stop condition will not be sent;
 *                           1 - A stop condition will be sent.
*******************************************************************************/
void TCS34725_I2C_Write(u8 slaveAddress, u8* dataBuffer,u8 bytesNumber, u8 stopBit)
{
	u8 i = 0;
	
	TCS34725_I2C_Start();
	TCS34725_I2C_Send_Byte((slaveAddress << 1) | 0x00);	   //?????????
	TCS34725_I2C_Wait_ACK();
	for(i = 0; i < bytesNumber; i++)
	{
		TCS34725_I2C_Send_Byte(*(dataBuffer + i));
		TCS34725_I2C_Wait_ACK();
	}
	if(stopBit == 1) TCS34725_I2C_Stop();
}
/*******************************************************************************
 * @brief Reads data from a slave device.
 *
 * @param slaveAddress - Adress of the slave device.
 * @param dataBuffer - Pointer to a buffer that will store the received data.
 * @param bytesNumber - Number of bytes to read.
 * @param stopBit - Stop condition control.
 *                  Example: 0 - A stop condition will not be sent;
 *                           1 - A stop condition will be sent.
*******************************************************************************/
void TCS34725_I2C_Read(u8 slaveAddress, u8* dataBuffer, u8 bytesNumber, u8 stopBit)
{
	u8 i = 0;
	
	TCS34725_I2C_Start();
	TCS34725_I2C_Send_Byte((slaveAddress << 1) | 0x01);	   //?????????
	TCS34725_I2C_Wait_ACK();
	for(i = 0; i < bytesNumber; i++)
	{
		if(i == bytesNumber - 1)
		{
			*(dataBuffer + i) = TCS34725_I2C_Read_Byte(0);//???????????NACK
		}
		else
		{
			*(dataBuffer + i) = TCS34725_I2C_Read_Byte(1);
		}
	}
	if(stopBit == 1) TCS34725_I2C_Stop();
}
/*******************************************************************************
 * @brief Writes data into TCS34725 registers, starting from the selected
 *        register address pointer.
 *
 * @param subAddr - The selected register address pointer.
 * @param dataBuffer - Pointer to a buffer storing the transmission data.
 * @param bytesNumber - Number of bytes that will be sent.
 *
 * @return None.
*******************************************************************************/
void TCS34725_Write(u8 subAddr, u8* dataBuffer, u8 bytesNumber)
{
    u8 sendBuffer[10] = {0, };
    u8 byte = 0;
    
    sendBuffer[0] = subAddr | TCS34725_COMMAND_BIT;
    for(byte = 1; byte <= bytesNumber; byte++)
    {
        sendBuffer[byte] = dataBuffer[byte - 1];
    }
	TCS34725_I2C_Write(TCS34725_ADDRESS, sendBuffer, bytesNumber + 1, 1);
}
/*******************************************************************************
 * @brief Reads data from TCS34725 registers, starting from the selected
 *        register address pointer.
 *
 * @param subAddr - The selected register address pointer.
 * @param dataBuffer - Pointer to a buffer that will store the received data.
 * @param bytesNumber - Number of bytes that will be read.
 *
 * @return None.
*******************************************************************************/
void TCS34725_Read(u8 subAddr, u8* dataBuffer, u8 bytesNumber)
{
	subAddr |= TCS34725_COMMAND_BIT;
	
	TCS34725_I2C_Write(TCS34725_ADDRESS, (u8*)&subAddr, 1, 0);
	TCS34725_I2C_Read(TCS34725_ADDRESS, dataBuffer, bytesNumber, 1);
}
/*******************************************************************************
 * @brief TCS34725设置积分时间
 *
 * @return None
*******************************************************************************/
void TCS34725_SetIntegrationTime(u8 time)
{
	TCS34725_Write(TCS34725_ATIME, &time, 1);
}
/*******************************************************************************
 * @brief TCS34725设置增益
 *
 * @return None
*******************************************************************************/
void TCS34725_SetGain(u8 gain)
{
	TCS34725_Write(TCS34725_CONTROL, &gain, 1);
}
/*******************************************************************************
 * @brief TCS34725使能
 *
 * @return None
*******************************************************************************/
void TCS34725_Enable(void)
{
	u8 cmd = TCS34725_ENABLE_PON;
	
	TCS34725_Write(TCS34725_ENABLE, &cmd, 1);
	cmd = TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN;
	TCS34725_Write(TCS34725_ENABLE, &cmd, 1);
	 //delay_s(600000);//delay_ms(3);//延时应该放在设置AEN之后
}
/*******************************************************************************
 * @brief TCS34725失能
 *
 * @return None
*******************************************************************************/
void TCS34725_Disable(void)
{
	u8 cmd = 0;
	
	TCS34725_Read(TCS34725_ENABLE, &cmd, 1);
	cmd = cmd & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
	TCS34725_Write(TCS34725_ENABLE, &cmd, 1);
}
/*******************************************************************************
 * @brief TCS34725初始化
 *
 * @return ID - ID寄存器中的值
*******************************************************************************/
u8 TCS34725_Init(void)
{
	u8 id=0;
	
	TCS34725_I2C_Init(); 
	TCS34725_Read(TCS34725_ID, &id, 1);//TCS34725 的 ID 是 0x44 可以根据这个来判断是否成功连接,0x4D是TCS34727;
	if(id==0x4D | id==0x44)
		{
			TCS34725_SetIntegrationTime(TCS34725_INTEGRATIONTIME_154MS);
			TCS34725_SetGain(TCS34725_GAIN_16X);
			TCS34725_Enable();
			return 1;
		}
	return 0;
}
/*******************************************************************************
 * @brief TCS34725获取单个通道数据
 *
 * @return data - 该通道的转换值
*******************************************************************************/
u16 TCS34725_GetChannelData(u8 reg)
{
	u8 tmp[2] = {0,0};
	u16 data;
	
	TCS34725_Read(reg, tmp, 2);
	data = (tmp[1] << 8) | tmp[0];
	
	return data;
}
/*******************************************************************************
 * @brief TCS34725获取各个通道数据
 *
 * @return 1 - 转换完成，数据可用
 *              0 - 转换未完成，数据不可用
*******************************************************************************/
u8 TCS34725_GetRawData(COLOR_RGBC *rgbc)
{
	u8 status = TCS34725_STATUS_AVALID;
	
	TCS34725_Read(TCS34725_STATUS, &status, 1);
	
	if(status & TCS34725_STATUS_AVALID)
	{
		rgbc->c = TCS34725_GetChannelData(TCS34725_CDATAL);	
		rgbc->r = TCS34725_GetChannelData(TCS34725_RDATAL);	
		rgbc->g = TCS34725_GetChannelData(TCS34725_GDATAL);	
		rgbc->b = TCS34725_GetChannelData(TCS34725_BDATAL);
		return 1;
	}
	return 0;
}
/******************************************************************************/
//RGB转HSL
void RGBtoHSL(COLOR_RGBC *Rgb, COLOR_HSL *Hsl)
{
	u8 maxVal,minVal,difVal;
	u8 r = Rgb->r*100/Rgb->c;   //[0-100]
	u8 g = Rgb->g*100/Rgb->c;
	u8 b = Rgb->b*100/Rgb->c;
	
	maxVal = max3v(r,g,b);
	minVal = min3v(r,g,b);
	difVal = maxVal-minVal;
	
	//计算亮度
	Hsl->l = (maxVal+minVal)/2;   //[0-100]
	
	if(maxVal == minVal)//若r=g=b,灰度
	{
		Hsl->h = 0; 
		Hsl->s = 0;
	}
	else
	{
		//计算色调
		if(maxVal==r)
		{
			if(g>=b)
				Hsl->h = 60*(g-b)/difVal;
			else
				Hsl->h = 60*(g-b)/difVal+360;
		}
		else
			{
				if(maxVal==g)Hsl->h = 60*(b-r)/difVal+120;
				else
					if(maxVal==b)Hsl->h = 60*(r-g)/difVal+240;
			}
		
		//计算饱和度
		if(Hsl->l<=50)Hsl->s=difVal*100/(maxVal+minVal);  //[0-100]
		else
			Hsl->s=difVal*100/(200-(maxVal+minVal));
	}
}
/******************************************************************************/
 
 /*********************************************************************************************************************
* @file            board.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.19, V1.0
* @modify          none
********************************************************************************************************************/

 
COLOR_RGBC rgb2;
COLOR_HSL  hsl2;
 
 
void TCS34725_2_I2C_Init( void )
{
    gpio_pin_init(GPIOA, OUT_PP, GPIO_PIN_5);	
	  gpio_pin_init(GPIOA, OUT_PP, GPIO_PIN_6);	
}

static void TCS_2_SDA_OUT()
{
	  gpio_pin_init(GPIOA, OUT_PP, GPIO_PIN_6);
}
// ????I/O??????
static void TCS_2_SDA_IN()
{
	  gpio_pin_init(GPIOA, IN_FLOATING, GPIO_PIN_6);
}
 
/*********************************************/
void TCS34725_2_I2C_Start()
{
	TCS_2_SDA_OUT();
	TCS_SDA2_H;
	TCS_SCL2_H;
	delay_1us(4);//delay_us(4);
	TCS_SDA2_L;
	delay_1us(4);//delay_us(4);
	TCS_SCL2_L;
}
/*********************************************/
void TCS34725_2_I2C_Stop()
{
	TCS_2_SDA_OUT();
	TCS_SCL2_L;
	TCS_SDA2_L;
	delay_1us(4);//delay_us(4);
	TCS_SCL2_H;
	TCS_SDA2_H;
	delay_1us(4);//delay_us(4);							   	
}
/*********************************************/
//???:1,??????
//        0,??????
u8 TCS34725_2_I2C_Wait_ACK()
{
	u32 t=0;
	
	TCS_2_SDA_IN();//SDA?????  
	TCS_SDA2_H; 
	delay_1us(1);//delay_us(1);
	TCS_SCL2_H; 
	delay_1us(1);//delay_us(1);
	while(TCS_SDA2_READ)
	{
		t++;
		if(t > 250)
		{
			TCS34725_2_I2C_Stop();
			return 1;
		}
	}
	TCS_SCL2_L;
	return 0;	
}
/*********************************************/
//??ACK??
void TCS34725_2_I2C_ACK()
{
	TCS_SCL2_L;
	TCS_2_SDA_OUT();//sda???
	TCS_SDA2_L;
	delay_1us(2);//delay_us(2);
	TCS_SCL2_H;
	delay_1us(2);//delay_us(2);
	TCS_SCL2_L;
}
/*********************************************/
//???ACK??		    
void TCS34725_2_I2C_NACK()
{
	TCS_SCL2_L;
	TCS_2_SDA_OUT();//sda???
	TCS_SDA2_H;
	delay_1us(2);//delay_us(2);
	TCS_SCL2_H;
	delay_1us(2);//delay_us(2);
	TCS_SCL2_L;
}
/*********************************************/
//I2C??????		  
void TCS34725_2_I2C_Send_Byte(u8 byte)
{
	u8 i;
	
	TCS_2_SDA_OUT();//sda???
	TCS_SCL2_L;//??????????
	for(i = 0; i < 8; i++)
	{
		if(((byte&0x80)>>7)==1)
		{
			  TCS_SDA2_H;
		}
		else
		{
			  TCS_SDA2_L;
		}
		byte <<= 1;
		
		delay_1us(2);//delay_us(2);
		TCS_SCL2_H;
		delay_1us(2);//delay_us(2);
		TCS_SCL2_L;
		delay_1us(2);//delay_us(2);
	} 
}
/*********************************************/
//?1???,ack=1?,??ACK,ack=0,??nACK   
u8 TCS34725_2_I2C_Read_Byte(u8 ack)
{
	u8 i,receive2 = 0;
	
	TCS_2_SDA_IN();
	for(i = 0; i < 8; i++)
	{
		TCS_SCL2_L;
		delay_1us(2);//delay_us(2);
		TCS_SCL2_H;
		receive2 <<= 1;
		if(TCS_SDA2_READ) receive2++;
		delay_1us(1);//delay_us(1);
	}
	if (!ack) TCS34725_2_I2C_NACK();//??nACK
	else TCS34725_2_I2C_ACK(); //??ACK 
	
	return receive2;
}
/*******************************************************************************
 * @brief Writes data to a slave device.
 *
 * @param slaveAddress - Adress of the slave device.
 * @param dataBuffer - Pointer to a buffer storing the transmission data.
 * @param bytesNumber - Number of bytes to write.
 * @param stopBit - Stop condition control.
 *                  Example: 0 - A stop condition will not be sent;
 *                           1 - A stop condition will be sent.
*******************************************************************************/
void TCS34725_2_I2C_Write(u8 slaveAddress, u8* dataBuffer,u8 bytesNumber, u8 stopBit)
{
	u8 i = 0;
	
	TCS34725_2_I2C_Start();
	TCS34725_2_I2C_Send_Byte((slaveAddress << 1) | 0x00);	   //?????????
	TCS34725_2_I2C_Wait_ACK();
	for(i = 0; i < bytesNumber; i++)
	{
		TCS34725_2_I2C_Send_Byte(*(dataBuffer + i));
		TCS34725_2_I2C_Wait_ACK();
	}
	if(stopBit == 1) TCS34725_2_I2C_Stop();
}
/*******************************************************************************
 * @brief Reads data from a slave device.
 *
 * @param slaveAddress - Adress of the slave device.
 * @param dataBuffer - Pointer to a buffer that will store the received data.
 * @param bytesNumber - Number of bytes to read.
 * @param stopBit - Stop condition control.
 *                  Example: 0 - A stop condition will not be sent;
 *                           1 - A stop condition will be sent.
*******************************************************************************/
void TCS34725_2_I2C_Read(u8 slaveAddress, u8* dataBuffer, u8 bytesNumber, u8 stopBit)
{
	u8 i = 0;
	
	TCS34725_2_I2C_Start();
	TCS34725_2_I2C_Send_Byte((slaveAddress << 1) | 0x01);	   //?????????
	TCS34725_2_I2C_Wait_ACK();
	for(i = 0; i < bytesNumber; i++)
	{
		if(i == bytesNumber - 1)
		{
			*(dataBuffer + i) = TCS34725_2_I2C_Read_Byte(0);//???????????NACK
		}
		else
		{
			*(dataBuffer + i) = TCS34725_2_I2C_Read_Byte(1);
		}
	}
	if(stopBit == 1) TCS34725_2_I2C_Stop();
}
/*******************************************************************************
 * @brief Writes data into TCS34725 registers, starting from the selected
 *        register address pointer.
 *
 * @param subAddr - The selected register address pointer.
 * @param dataBuffer - Pointer to a buffer storing the transmission data.
 * @param bytesNumber - Number of bytes that will be sent.
 *
 * @return None.
*******************************************************************************/
void TCS34725_2_Write(u8 subAddr, u8* dataBuffer, u8 bytesNumber)
{
    u8 sendBuffer[10] = {0, };
    u8 byte = 0;
    
    sendBuffer[0] = subAddr | TCS34725_COMMAND_BIT;
    for(byte = 1; byte <= bytesNumber; byte++)
    {
        sendBuffer[byte] = dataBuffer[byte - 1];
    }
	TCS34725_2_I2C_Write(TCS34725_ADDRESS, sendBuffer, bytesNumber + 1, 1);
}
/*******************************************************************************
 * @brief Reads data from TCS34725 registers, starting from the selected
 *        register address pointer.
 *
 * @param subAddr - The selected register address pointer.
 * @param dataBuffer - Pointer to a buffer that will store the received data.
 * @param bytesNumber - Number of bytes that will be read.
 *
 * @return None.
*******************************************************************************/
void TCS34725_2_Read(u8 subAddr, u8* dataBuffer, u8 bytesNumber)
{
	subAddr |= TCS34725_COMMAND_BIT;
	
	TCS34725_2_I2C_Write(TCS34725_ADDRESS, (u8*)&subAddr, 1, 0);
	TCS34725_2_I2C_Read(TCS34725_ADDRESS, dataBuffer, bytesNumber, 1);
}
/*******************************************************************************
 * @brief TCS34725??????
 *
 * @return None
*******************************************************************************/
void TCS34725_2_SetIntegrationTime(u8 time)
{
	TCS34725_2_Write(TCS34725_ATIME, &time, 1);
}
/*******************************************************************************
 * @brief TCS34725????
 *
 * @return None
*******************************************************************************/
void TCS34725_2_SetGain(u8 gain)
{
	TCS34725_2_Write(TCS34725_CONTROL, &gain, 1);
}
/*******************************************************************************
 * @brief TCS34725??
 *
 * @return None
*******************************************************************************/
void TCS34725_2_Enable(void)
{
	u8 cmd = TCS34725_ENABLE_PON;
	
	TCS34725_2_Write(TCS34725_ENABLE, &cmd, 1);
	cmd = TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN;
	TCS34725_2_Write(TCS34725_ENABLE, &cmd, 1);
	//delay_s(600000);//delay_ms(3);//????????AEN??
}
/*******************************************************************************
 * @brief TCS34725??
 *
 * @return None
*******************************************************************************/
void TCS34725_2_Disable(void)
{
	u8 cmd = 0;
	
	TCS34725_2_Read(TCS34725_ENABLE, &cmd, 1);
	cmd = cmd & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
	TCS34725_2_Write(TCS34725_ENABLE, &cmd, 1);
}
/*******************************************************************************
 * @brief TCS34725???
 *
 * @return ID - ID??????
*******************************************************************************/
u8 TCS34725_2_Init(void)
{
	u8 id=0;
	
	TCS34725_2_I2C_Init(); 
	TCS34725_2_Read(TCS34725_ID, &id, 1);  //TCS34725 ? ID ? 0x44 ???????????????,0x4D?TCS34727;
	if(id==0x4D | id==0x44)
		{
			TCS34725_2_SetIntegrationTime(TCS34725_INTEGRATIONTIME_154MS);
			TCS34725_2_SetGain(TCS34725_GAIN_16X);
			TCS34725_2_Enable();
			return 1;
		}
	return 0;
}
/*******************************************************************************
 * @brief TCS34725????????
 *
 * @return data - ???????
*******************************************************************************/
u16 TCS34725_2_GetChannelData(u8 reg)
{
	u8 tmp[2] = {0,0};
	u16 data2;
	
	TCS34725_2_Read(reg, tmp, 2);
	data2 = (tmp[1] << 8) | tmp[0];
	
	return data2;
}
/*******************************************************************************
 * @brief TCS34725????????
 *
 * @return 1 - ????,????
 *   	   0 - ?????,?????
*******************************************************************************/
u8 TCS34725_2_GetRawData(COLOR_RGBC *rgbc2)
{
	u8 status = TCS34725_STATUS_AVALID;
	
	TCS34725_2_Read(TCS34725_STATUS, &status, 1);
	
	if(status & TCS34725_STATUS_AVALID)
	{
		rgbc2->c = TCS34725_2_GetChannelData(TCS34725_CDATAL);	
		rgbc2->r = TCS34725_2_GetChannelData(TCS34725_RDATAL);	
		rgbc2->g = TCS34725_2_GetChannelData(TCS34725_GDATAL);	
		rgbc2->b = TCS34725_2_GetChannelData(TCS34725_BDATAL);
		return 1;
	}
	return 0;
}
/******************************************************************************/
//rgb2?hsl2
void RGB2toHSL2(COLOR_RGBC *Rgb2, COLOR_HSL *Hsl2)
{
	u8 maxVal,minVal,difVal;
	u8 r = Rgb2->r*100/Rgb2->c;   //[0-100]
	u8 g = Rgb2->g*100/Rgb2->c;
	u8 b = Rgb2->b*100/Rgb2->c;
	
	maxVal = max3v(r,g,b);
	minVal = min3v(r,g,b);
	difVal = maxVal-minVal;
	
	//????
	Hsl2->l = (maxVal+minVal)/2;   //[0-100]
	
	if(maxVal == minVal)//?r=g=b,??
	{
		Hsl2->h = 0; 
		Hsl2->s = 0;
	}
	else
	{
		//????
		if(maxVal==r)
		{
			if(g>=b)
				Hsl2->h = 60*(g-b)/difVal;
			else
				Hsl2->h = 60*(g-b)/difVal+360;
		}
		else
			{
				if(maxVal==g)Hsl2->h = 60*(b-r)/difVal+120;
				else
					if(maxVal==b)Hsl2->h = 60*(r-g)/difVal+240;
			}
		
		//?????
		if(Hsl2->l<=50)Hsl2->s=difVal*100/(maxVal+minVal);  //[0-100]
		else
			Hsl2->s=difVal*100/(200-(maxVal+minVal));
	}
}
/******************************************************************************/
 
 enum Wcolor { Wred = 1, Wgreen, Wblue ,Woff};
 
u8 car_wsz28(unsigned int lmd)
{
    	unsigned	int wza=0,wzb=0,wzc=0,wsz28;
	  u8 ws28;
			  TCS34725_GetRawData(&rgb);  //此处遵从CSDN教程的方法，重复读取两次	(a>b) ? a : b;
			  delay_1ms(100);
			  TCS34725_GetRawData(&rgb);
        wza=rgb.r*255/rgb.c;
			  wzb=rgb.g*255/rgb.c ;
			  wzc=rgb.b*255/rgb.c+10;
			wsz28=max3v(wza,wzb,wzc);
	printf("wza:%d,wzb:%d,wzc:%d\r\n",wza,wzb,wzc);
		 if(wsz28 >= lmd)
	{
			if(wsz28 == wza)
    {	 
        ws28=Wred;		
    }
		else if(wsz28 == wzb )
		 {	
			  ws28=Wgreen;
    }
		else if(wsz28 == wzc )
		 {	
			  ws28=Wblue;
    }
	}
	 else if(wsz28 < lmd)
     {	
			  ws28=Woff;
    }
		 
return ws28;
}
u8 car_wsy28(unsigned int lmd) 
{
    	unsigned	int wya=0,wyb=0,wyc=0,wsy28=0;
	  u8 ws28;


			  TCS34725_2_GetRawData(&rgb2);
			  delay_1ms(100);
			  TCS34725_2_GetRawData(&rgb2);
        wya=rgb2.r*255/rgb2.c;
			  wyb=rgb2.g*255/rgb2.c ;
			  wyc=rgb2.b*255/rgb2.c+10;

			wsy28=max3v(wya,wyb ,wyc);
	printf("wya:%d,wyb:%d,wyc:%d\r\n",wya,wyb,wyc);
				if(wsy28 >= lmd)
	{
			 if(wsy28 == wya)
    {	 
        ws28=Wred;		
    }
		 else if(wsy28 == wyb )
		 {	
			  ws28=Wgreen;
    }
		 else if(wsy28 == wyc )
		 {	
			  ws28=Wblue;
    }
	}
	  else if(wsy28 < lmd)
     {	
			  ws28=Woff;
    }
		 return ws28;
}
void car_ws28_zy(u8 ws28z, u8 ws28y)
{
				switch(ws28z){
				case Wred :ws2812b_write(rgb_red);	break;
				case Wgreen :ws2812b_write(rgb_green);	break;
				case Wblue :ws2812b_write(rgb_blue);	break;
				case Woff :ws2812b_write(rgb_off);	break;
			  default :ws2812b_write(rgb_off);
			
			}

	
			switch(ws28y){
				case Wred :ws2812b_write(rgb_red);	break;
				case Wgreen :ws2812b_write(rgb_green);	break;
				case Wblue :ws2812b_write(rgb_blue);	break;
				case Woff :ws2812b_write(rgb_off);	break;
			  default :ws2812b_write(rgb_off);
			
			}
}

 

 
