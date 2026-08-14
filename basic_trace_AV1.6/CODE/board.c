/*********************************************************************************************************************
* @file            board.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.19, V1.0
* @modify          none
********************************************************************************************************************/

#include "board.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief        ??LED???
// @param        led_num            ??LED:LEDx(x = 1.2)
// @return       void
// Sample usage:       board_led_init(LED1);//??LED1???
//-------------------------------------------------------------------------------------------------------------------
void board_led_init(unsigned char led_num)
{
    if(led_num == LED1)
    {
        gpio_pin_init(LED1_PORT, LED1_MODE, LED1_PIN);//??LED1???
        gpio_bit_output(LED1_PORT, LED1_PIN, OFF);//LED1??
    }
    if(led_num == LED2)
    {
        gpio_pin_init(LED2_PORT, LED2_MODE, LED2_PIN);//??LED2???
        gpio_bit_output(LED2_PORT, LED2_PIN, OFF);//LED2??
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        ??LED??
// @param        led_num            ??LED:LEDx(x = 1.2)
// @return       void
// Sample usage:       board_led_on(LED1);//????LED1
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
// @brief        ??LED??
// @param        led_num            ??LED:LEDx(x = 1.2)
// @return       void
// Sample usage:       board_led_off(LED1);//??LED1??
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
// @brief        ??LED????
// @param        led_num            ??LED:LEDx(x = 1.2)
// @return       void
// Sample usage:       board_led_toggle(LED1);//??LED1????
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
  * @brief  ??TCS34725??????SDA????????
  * @note   ????(MCU)????I2C?????????(?????ACK/NACK??)
  * @param  ?
  * @retval ?
  */
static void TCS_SDA_OUT()
{
	  gpio_pin_init(GPIOA, OUT_PP, GPIO_PIN_12); // ???GPIOA?PIN12?????
}
/**
  * @brief  ??TCS34725??????SDA????????
  * @note   ????(MCU)??SDA????,??(TCS34725)?????
  * @param  ?
  * @retval ?
  */
static void TCS_SDA_IN()
{
	  gpio_pin_init(GPIOA, IN_FLOATING, GPIO_PIN_12);// ???GPIOA?PIN12?????
}
 
/*************IIC????*********************/
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
/***************IIC????*********************/
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

/*** ??????:0???  1?????***/
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

/*** ? ? ? ?:?????????????:0????  1?????**SDA??,???????*/
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
/*****************?????**SDA??,?????????***********/
		    
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
/***********??????:dat??????**********************/
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
/*************IIC???*****************/
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
 * @brief TCS34725??????
 *
 * @return None
*******************************************************************************/
void TCS34725_SetIntegrationTime(u8 time)
{
	TCS34725_Write(TCS34725_ATIME, &time, 1);
}
/*******************************************************************************
 * @brief TCS34725????
 *
 * @return None
*******************************************************************************/
void TCS34725_SetGain(u8 gain)
{
	TCS34725_Write(TCS34725_CONTROL, &gain, 1);
}
/*******************************************************************************
 * @brief TCS34725??
 *
 * @return None
*******************************************************************************/
void TCS34725_Enable(void)
{
	u8 cmd = TCS34725_ENABLE_PON;
	
	TCS34725_Write(TCS34725_ENABLE, &cmd, 1);
	cmd = TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN;
	TCS34725_Write(TCS34725_ENABLE, &cmd, 1);
	 //delay_s(600000);//delay_ms(3);//????????AEN??
}
/*******************************************************************************
 * @brief TCS34725??
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
 * @brief TCS34725???
 *
 * @return ID - ID??????
*******************************************************************************/
u8 TCS34725_Init(void)
{
	u8 id=0;
	
	TCS34725_I2C_Init(); 
	TCS34725_Read(TCS34725_ID, &id, 1);//TCS34725 ? ID ? 0x44 ???????????????,0x4D?TCS34727;
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
 * @brief TCS34725????????
 *
 * @return data - ???????
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
 * @brief TCS34725????????
 *
 * @return 1 - ????,????
 *              0 - ?????,?????
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
//RGB?HSL
void RGBtoHSL(COLOR_RGBC *Rgb, COLOR_HSL *Hsl)
{
	u8 maxVal,minVal,difVal;
	u8 r = Rgb->r*100/Rgb->c;   //[0-100]
	u8 g = Rgb->g*100/Rgb->c;
	u8 b = Rgb->b*100/Rgb->c;
	
	maxVal = max3v(r,g,b);
	minVal = min3v(r,g,b);
	difVal = maxVal-minVal;
	
	//????
	Hsl->l = (maxVal+minVal)/2;   //[0-100]
	
	if(maxVal == minVal)//?r=g=b,??
	{
		Hsl->h = 0; 
		Hsl->s = 0;
	}
	else
	{
		//????
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
		
		//?????
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
    gpio_pin_init(TCS_SCL2_GPIO_PORT, OUT_PP, TCS_SCL2_GPIO_PIN);
	  gpio_pin_init(TCS_SDA2_GPIO_PORT, OUT_PP, TCS_SDA2_GPIO_PIN);
}

static void TCS_2_SDA_OUT()
{
	  gpio_pin_init(TCS_SDA2_GPIO_PORT, OUT_PP, TCS_SDA2_GPIO_PIN);
}
// ????I/O??????
static void TCS_2_SDA_IN()
{
	  gpio_pin_init(TCS_SDA2_GPIO_PORT, IN_FLOATING, TCS_SDA2_GPIO_PIN);
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
			  TCS34725_GetRawData(&rgb);  //????CSDN?????,??????	(a>b) ? a : b;
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

 

 
