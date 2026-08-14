/*********************************************************************************************************************
* @file            car_light.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2022.09.17, V1.0
* @modify          none
********************************************************************************************************************/

#include "car_light.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief        智能车尾灯同时亮起
// @param        color          尾灯颜色
// @param        time           延时时间(ms)
// @return       void
// Sample usage:        car_both_rgb_on(red, 1000);//尾灯同时显示红色，延时1s
//-------------------------------------------------------------------------------------------------------------------
void car_both_rgb_on(rgb_color color, unsigned int time)
{
    if(color == red)
    {	
        ws2812b_write(rgb_red);
        ws2812b_write(rgb_red);
        delay_1ms(time);
    }
    else if(color == yellow)
    {
        ws2812b_write(rgb_yellow);
        ws2812b_write(rgb_yellow);
        delay_1ms(time);
    }
    else if(color == white)
    {
        ws2812b_write(rgb_white);
        ws2812b_write(rgb_white);
        delay_1ms(time);
    }
    else if(color == blue)
    {
        ws2812b_write(rgb_blue);
        ws2812b_write(rgb_blue);
        delay_1ms(time);
    }
    else if(color == green)
    {
        ws2812b_write(rgb_green);
        ws2812b_write(rgb_green);
        delay_1ms(time);
    }
    else if(color == cyan)
    {
        ws2812b_write(rgb_cyan);
        ws2812b_write(rgb_cyan);
        delay_1ms(time);
    }
    else if(color == purple)
    {
        ws2812b_write(rgb_purple);
        ws2812b_write(rgb_purple);
        delay_1ms(time);
    }
    else 
    {
        ws2812b_write(rgb_user_define);
        ws2812b_write(rgb_user_define);
        delay_1ms(time);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        智能车尾灯左灯闪烁
// @param        color          尾灯颜色
// @param        flash_count    闪烁次数
// @param        time           间隔时间(ms)
// @return       void
// Sample usage:        car_left_rgb_flash(red, 3, 500);//左灯闪烁红色三次，间隔0.5s
//-------------------------------------------------------------------------------------------------------------------
void car_left_rgb_flash(rgb_color color, unsigned int flash_count, unsigned int time)
{
    unsigned int i;
	
    if(color == red)
    {				  
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_red);
            ws2812b_write(rgb_off);
		
            delay_1ms(time);

            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of if(color == red)
    else if(color == yellow)
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_yellow);
            ws2812b_write(rgb_off);
		
            delay_1ms(time);

            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of else if(color == yellow)
    else if(color == white)
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_white);
            ws2812b_write(rgb_off);
		
            delay_1ms(time);
					
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of else if(color == white)
    else if(color == green)
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_green);
            ws2812b_write(rgb_off);
		
            delay_1ms(time);
					
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of else if(color == green)
    else if(color == blue)
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_blue);
            ws2812b_write(rgb_off);
		
            delay_1ms(time);
					
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of else if(color == blue)
    else if(color == cyan)
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_cyan);
            ws2812b_write(rgb_off);
		
            delay_1ms(time);
					
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of else if(color == cyan)
    else if(color == purple)
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_purple);
            ws2812b_write(rgb_off);
		
            delay_1ms(time);
					
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of else if(color == purple)
    else 
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_user_define);
            ws2812b_write(rgb_off);
		
            delay_1ms(time);
					
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of else
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        智能车尾灯熄灭
// @param        void
// @return       void
// Sample usage:        car_both_rgb_off();//尾灯熄灭
//-------------------------------------------------------------------------------------------------------------------
void car_both_rgb_off(void)
{
    ws2812b_write(rgb_off);
    ws2812b_write(rgb_off);
    delay_1ms(50);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        智能车尾灯右灯闪烁
// @param        color          尾灯颜色
// @param        flash_count    闪烁次数
// @param        time           间隔时间(ms)
// @return       void
// Sample usage:        car_right_rgb_flash(red, 3, 500);//右灯闪烁红色三次，间隔0.5s
//-------------------------------------------------------------------------------------------------------------------
void car_right_rgb_flash(rgb_color color, unsigned int flash_count, unsigned int time)
{
    unsigned int i;
	
    if(color == red)
    {	
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_red);
		
            delay_1ms(time);
      
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of if(color == red)
    else if(color == yellow)
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_yellow);
		
            delay_1ms(time);
        
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of if(color == yellow)
    else if(color == white)
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_white);
		
            delay_1ms(time);
        
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of if(color == white)
    else if(color == green)
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_green);
		
            delay_1ms(time);
        
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of if(color == green)
    else if(color == blue)
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_blue);
		
            delay_1ms(time);
        
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of if(color == blue)
    else if(color == cyan)
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_cyan);
		
            delay_1ms(time);
        
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of if(color == cyan)
    else if(color == purple)
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_purple);
		
            delay_1ms(time);
        
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of if(color == purple)
    else 
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_user_define);
		
            delay_1ms(time);
        
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of else
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        智能车左右尾灯同时闪烁
// @param        color          尾灯颜色
// @param        flash_count    闪烁次数
// @param        time           间隔时间(ms)
// @return       void
// Sample usage:        car_both_rgb_flash(red, 3, 500);//尾灯同时闪烁红色三次，间隔0.5s
//-------------------------------------------------------------------------------------------------------------------
void car_both_rgb_flash(rgb_color color, unsigned int flash_count, unsigned int time)
{
    unsigned int i;
	
    if(color == red)
    {	
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_red);
            ws2812b_write(rgb_red);
		
            delay_1ms(time);
      
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of if(color == red)
    else if(color == yellow)
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_yellow);
            ws2812b_write(rgb_yellow);
		
            delay_1ms(time);
        
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of if(color == yellow)
    else if(color == white)
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_white);
            ws2812b_write(rgb_white);
		
            delay_1ms(time);
        
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of if(color == white)
    else if(color == green)
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_green);
            ws2812b_write(rgb_green);
		
            delay_1ms(time);
        
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of if(color == green)
    else if(color == blue)
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_blue);
            ws2812b_write(rgb_blue);
		
            delay_1ms(time);
        
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of if(color == blue)		
    else if(color == cyan)
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_cyan);
            ws2812b_write(rgb_cyan);
		
            delay_1ms(time);
        
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of if(color == cyan)		
    else if(color == purple)
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_purple);
            ws2812b_write(rgb_purple);
		
            delay_1ms(time);
        
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of if(color == purple)
    else 
    {
        for(i=0; i<flash_count; i++)
        {
            ws2812b_write(rgb_user_define);
            ws2812b_write(rgb_user_define);
		
            delay_1ms(time);
        
            ws2812b_write(rgb_off);
            ws2812b_write(rgb_off);
            delay_1ms(time);
        }//end of for(i=0; i<flash_count; i++)
    }//end of else
}
