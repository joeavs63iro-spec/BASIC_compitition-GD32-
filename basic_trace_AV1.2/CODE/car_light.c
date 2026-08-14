/*********************************************************************************************************************
* @file            car_light.c
* @brief           WS2812B tail light helpers
* @Target core     GD32F303RCT6
********************************************************************************************************************/

#include "car_light.h"

static uint8_t rgb_auto_off_enabled = 0;
static uint64_t rgb_auto_off_time_us = 0;

static unsigned char *car_rgb_color_data(rgb_color color)
{
    switch(color)
    {
        case red:
            return rgb_red;
        case yellow:
            return rgb_yellow;
        case white:
            return rgb_white;
        case green:
            return rgb_green;
        case blue:
            return rgb_blue;
        case cyan:
            return rgb_cyan;
        case purple:
            return rgb_purple;
				case black :
					return rgb_off;
        default:
            return rgb_user_define;
    }
}

static void car_rgb_write_pair(unsigned char *left_color, unsigned char *right_color)
{
    ws2812b_write(left_color);
    ws2812b_write(right_color);
}

void car_rgb_set_both(rgb_color color)
{
    rgb_auto_off_enabled = 0;
    car_rgb_write_pair(car_rgb_color_data(color), car_rgb_color_data(color));
}

void car_both_rgb_on(rgb_color color, unsigned int time)
{
    car_rgb_write_pair(car_rgb_color_data(color), car_rgb_color_data(color));

    /*
     * Keep the historical API, but avoid blocking delay_1ms(time).
     * main() must call car_rgb_task() periodically to finish the timed off action.
     */
    if(time > 0)
    {
        rgb_auto_off_enabled = 1;
        rgb_auto_off_time_us = get_current_time_us() + ((uint64_t)time * 1000ULL);
    }
    else
    {
        rgb_auto_off_enabled = 0;
    }
}

void car_both_rgb_off(void)
{
    rgb_auto_off_enabled = 0;
    car_rgb_write_pair(rgb_off, rgb_off);
}

void car_rgb_task(void)
{
    if((rgb_auto_off_enabled != 0) && (get_current_time_us() >= rgb_auto_off_time_us))
    {
        car_both_rgb_off();
    }
}

void car_left_rgb_flash(rgb_color color, unsigned int flash_count, unsigned int time)
{
    unsigned int i;
    unsigned char *color_data = car_rgb_color_data(color);

    rgb_auto_off_enabled = 0;
    for(i = 0; i < flash_count; i++)
    {
        car_rgb_write_pair(color_data, rgb_off);
        delay_1ms(time);
        car_rgb_write_pair(rgb_off, rgb_off);
        delay_1ms(time);
    }
}

void car_right_rgb_flash(rgb_color color, unsigned int flash_count, unsigned int time)
{
    unsigned int i;
    unsigned char *color_data = car_rgb_color_data(color);

    rgb_auto_off_enabled = 0;
    for(i = 0; i < flash_count; i++)
    {
        car_rgb_write_pair(rgb_off, color_data);
        delay_1ms(time);
        car_rgb_write_pair(rgb_off, rgb_off);
        delay_1ms(time);
    }
}

void car_both_rgb_flash(rgb_color color, unsigned int flash_count, unsigned int time)
{
    unsigned int i;
    unsigned char *color_data = car_rgb_color_data(color);

    rgb_auto_off_enabled = 0;
    for(i = 0; i < flash_count; i++)
    {
        car_rgb_write_pair(color_data, color_data);
        delay_1ms(time);
        car_rgb_write_pair(rgb_off, rgb_off);
        delay_1ms(time);
    }
}
