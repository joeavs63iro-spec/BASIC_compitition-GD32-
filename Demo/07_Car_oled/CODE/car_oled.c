/*********************************************************************************************************************
* @file            car_oled.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.10.16, V1.0
* @modify          none
********************************************************************************************************************/

#include "car_oled.h"
#include "oledfont.h"  

unsigned char OLED_GRAM[128][8];

//-------------------------------------------------------------------------------------------------------------------
// @brief        开启OLED显示
// @param        void
// @return       void
// Sample usage:         oled_display_on();
                        //开启OLED显示
//-------------------------------------------------------------------------------------------------------------------
void oled_display_on(void)
{
    oled_write_byte(0X8D,OLED_CMD);  //SET DCDC命令
    oled_write_byte(0X14,OLED_CMD);  //DCDC ON
    oled_write_byte(0XAF,OLED_CMD);  //DISPLAY ON
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        关闭OLED显示 
// @param        void
// @return       void
// Sample usage:         oled_display_off();
                        //关闭OLED显示 
//-------------------------------------------------------------------------------------------------------------------
void oled_display_off(void)
{
    oled_write_byte(0X8D,OLED_CMD);  //SET DCDC命令
    oled_write_byte(0X10,OLED_CMD);  //DCDC OFF
    oled_write_byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   	

//-------------------------------------------------------------------------------------------------------------------
// @brief        OLED更新显存
// @param        void
// @return       void
// Sample usage:         oled_refresh_gram();
                        //OLED更新显存
//------------------------------------------------------------------------------------------------------------------- 
void oled_refresh_gram(void)
{
    uint8_t i,n;		    
    for(i=0;i<8;i++)  
    {  
        oled_write_byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
        oled_write_byte (0x00,OLED_CMD);      //设置显示位置—列低地址
        oled_write_byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
					
        for(n=0;n<128;n++)
        {
            oled_write_byte(OLED_GRAM[n][i],OLED_DATA);
        }
    }   
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        OLED屏幕清除
// @param        void
// @return       void
// Sample usage:         oled_clear();
                        //OLED屏幕清除
//-------------------------------------------------------------------------------------------------------------------
void oled_clear(void)  
{  
    uint8_t i,n;  
    for(i=0;i<8;i++)
    {
        for(n=0;n<128;n++)
        {
            OLED_GRAM[n][i]=0X00;
        }									
    }
    oled_refresh_gram();   //更新显示
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        OLED显示字符
// @param        x            屏幕的x坐标：0~127
// @param        y            屏幕的y坐标：0~7
// @param        chr          字符：参考oledfont.h中，点阵枚举的字符
// @param        size         字体大小：12/16
// @return       void
// Sample usage:         oled_show_char(0,2,'!',16);
                        //在坐标（0，2）显示！，字体大小为16
//-------------------------------------------------------------------------------------------------------------------
void oled_show_char(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size)
{      	
    unsigned char c=0,i=0;	
    c=chr-' ';           //得到偏移后的值			
	
    if(x>Max_Column-1)
    {
        x = 0;
        y = y+2;
    }
				
    if(Char_Size == 16)
    {
        oled_set_pos(x,y);	
        for(i=0;i<8;i++)
        {
            oled_write_byte(F8X16[c*16+i],OLED_DATA);
        }
        oled_set_pos(x,y+1);
								
        for(i=0;i<8;i++)
        {
            oled_write_byte(F8X16[c*16+i+8],OLED_DATA);
        }
    }
    else 
    {	
        oled_set_pos(x,y);
        for(i=0;i<6;i++)
        {
            oled_write_byte(F6x8[c][i],OLED_DATA);
        }
				
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        OLED显示数字
// @param        x          屏幕的x坐标：0~127
// @param        y          屏幕的y坐标：0~7
// @param        num        数值:0~4294967295
// @param        len        数字的位数
// @param        size       字体大小：12/16
// @return       void
// Sample usage:         oled_show_number(48,2,9,1,16);;
                         //在点（48，9）显示数字9，数字位数为1，字体大小为16
//------------------------------------------------------------------------------------------------------------------- 		  
void oled_show_number(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size)
{         	
    uint8_t t,temp;
    uint8_t enshow=0;						   
    for(t=0;t<len;t++)
    {
        temp=(num/mypow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                oled_show_char(x+(size/2)*t,y,' ',size);
                continue;
            }else enshow=1; 
		 	 
         }
         oled_show_char(x+(size/2)*t,y,temp+'0',size); 
   	}
} 

//-------------------------------------------------------------------------------------------------------------------
// @brief        OLED显示字符串
// @param        x            屏幕的x坐标：0~127
// @param        y            屏幕的y坐标：0~7
// @param        *chr         字符串
// @param        size         字体大小：12/16
// @return       void
// Sample usage:         oled_show_string(0,2,"i love you\0",16);
                         //起点坐标为（0，2），显示字符串i love you，字体大小为16
//-------------------------------------------------------------------------------------------------------------------
void oled_show_string(uint8_t x,uint8_t y,char *chr,uint8_t size)
{
    unsigned char j=0;
    while (chr[j]!='\0')
    {	
        oled_show_char(x,y,chr[j],size);
        x += 8;
        if(x>120)
        {
            x = 0;
            y += 2;
        }
        j++;
    } 
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        OLED显示汉字
// @param        x            屏幕的x坐标：0~127
// @param        y            屏幕的y坐标：0~7
// @param        Chinese      汉字的序号：参考oledfont.h的Hzk[][32]，用户可使用取字模软件自行修改汉字
// @return       void   
// Sample usage:         oled_show_Chinese(16,0,1);
                        //在坐标（16，0）显示汉字“月”
//-------------------------------------------------------------------------------------------------------------------
void oled_show_Chinese(uint8_t x, uint8_t y, uint8_t Chinese)
{
    uint8_t t,adder = 0;
    oled_set_pos(x, y);
    for(t=0; t<16; t++)
    {
        oled_write_byte(Hzk[2*Chinese][t], OLED_DATA);
        adder += 1;
    }
    oled_set_pos(x, y+1);
    for(t=0; t<16; t++)
    {
        oled_write_byte(Hzk[2*Chinese+1][t], OLED_DATA);
        adder += 1;
    }
}


