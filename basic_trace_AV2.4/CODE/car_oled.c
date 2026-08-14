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
// @brief        ????OLED???
// @param        void
// @return       void
// Sample usage:         oled_display_on();
                        //????OLED???
//-------------------------------------------------------------------------------------------------------------------
void oled_display_on(void)
{
    oled_write_byte(0X8D,OLED_CMD);  //SET DCDC????
    oled_write_byte(0X14,OLED_CMD);  //DCDC ON
    oled_write_byte(0XAF,OLED_CMD);  //DISPLAY ON
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        ???OLED??? 
// @param        void
// @return       void
// Sample usage:         oled_display_off();
                        //???OLED??? 
//-------------------------------------------------------------------------------------------------------------------
void oled_display_off(void)
{
    oled_write_byte(0X8D,OLED_CMD);  //SET DCDC????
    oled_write_byte(0X10,OLED_CMD);  //DCDC OFF
    oled_write_byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   	

//-------------------------------------------------------------------------------------------------------------------
// @brief        OLED???????
// @param        void
// @return       void
// Sample usage:         oled_refresh_gram();
                        //OLED???????
//------------------------------------------------------------------------------------------------------------------- 
void oled_refresh_gram(void)
{
    uint8_t i,n;		    
    for(i=0;i<8;i++)  
    {  
        oled_write_byte (0xb0+i,OLED_CMD);    //??????????0~7??
        oled_write_byte (0x00,OLED_CMD);      //?????????á??????
        oled_write_byte (0x10,OLED_CMD);      //?????????á??????   
					
        for(n=0;n<128;n++)
        {
            oled_write_byte(OLED_GRAM[n][i],OLED_DATA);
        }
    }   
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        OLED??L???
// @param        void
// @return       void
// Sample usage:         oled_clear();
                        //OLED??L???
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
    oled_refresh_gram();   //???????
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        Invert selected OLED pixel area
// @param        x0           start x: 0~127
// @param        y0           start y: 0~63
// @param        x1           end x: 0~127
// @param        y1           end y: 0~63
// @return       void
// Sample usage:         oled_invert_area(0,0,127,15);
//-------------------------------------------------------------------------------------------------------------------
void oled_invert_area(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    uint8_t x;
    uint8_t page;
    uint8_t start_page;
    uint8_t end_page;

    if (x0 > x1) {
        uint8_t tmp = x0;
        x0 = x1;
        x1 = tmp;
    }
    if (y0 > y1) {
        uint8_t tmp = y0;
        y0 = y1;
        y1 = tmp;
    }

    if (x1 >= Max_Column) x1 = Max_Column - 1;
    if (y1 >= Max_Row) y1 = Max_Row - 1;

    start_page = y0 / 8U;
    end_page = y1 / 8U;

    for (page = start_page; page <= end_page; page++) {
        uint8_t bit_start = (page == start_page) ? (uint8_t)(y0 & 0x07U) : 0U;
        uint8_t bit_end = (page == end_page) ? (uint8_t)(y1 & 0x07U) : 7U;
        uint8_t mask = (uint8_t)(((uint16_t)0xFFU << bit_start) & ((uint16_t)0xFFU >> (7U - bit_end)));

        for (x = x0; x <= x1; x++) {
            OLED_GRAM[x][page] ^= mask;
        }
    }

    oled_refresh_gram();
}
//-------------------------------------------------------------------------------------------------------------------
// @brief        OLED??????
// @param        x            ??L??x????0~127
// @param        y            ??L??y????0~7
// @param        chr          ????????oledfont.h???????ö??????
// @param        size         ????????12/16
// @return       void
// Sample usage:         oled_show_char(0,2,'!',16);
                        //??????0??2????????????????16
//-------------------------------------------------------------------------------------------------------------------
void oled_show_char(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size)
{      	
    unsigned char c=0,i=0;	
    c=chr-' ';           //?õ?t?????			
	
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
// @brief        OLED???????
// @param        x          ??L??x????0~127
// @param        y          ??L??y????0~7
// @param        num        ???:0~4294967295
// @param        len        ????????
// @param        size       ????????12/16
// @return       void
// Sample usage:         oled_show_number(48,2,9,1,16);;
                         //???48??9?????????9??????????1?????????16
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
// @brief        OLED????????
// @param        x            ??L??x????0~127
// @param        y            ??L??y????0~7
// @param        *chr         ?????
// @param        size         ????????12/16
// @return       void
// Sample usage:         oled_show_string(0,2,"i love you\0",16);
                         //??????????0??2????????????i love you?????????16
//-------------------------------------------------------------------------------------------------------------------
void oled_show_string(uint8_t x,uint8_t y,uint8_t *chr,uint8_t size)
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
// @brief        OLED???????
// @param        x            ??L??x????0~127
// @param        y            ??L??y????0~7
// @param        Chinese      ???????t????oledfont.h??Hzk[][32]???û???'?????g??????????l???
// @return       void   
// Sample usage:         oled_show_Chinese(16,0,1);
                        //??????16??0???????????¡?
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


