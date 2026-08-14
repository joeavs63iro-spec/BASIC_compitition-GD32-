/*********************************************************************************************************************
* @file            main.c
* @brief           Electromagnetic track position PD tuning project
* @Target core     GD32F303RCT6
********************************************************************************************************************/

#include "main.h"
#include "headfile.h"
#include "bluetooth_tuning.h"

#define MAIN_LOOP_DELAY_MS  1U

int main(void)
{
    car_init();
    buzzer_duty(0);
    car_both_rgb_off();

    track_init();
    track_timer_init();
    BluetoothTune_Init();

    car_rgb_set_both(green);

    while(1)
    {
        BluetoothTune_Service();
        delay_1ms(MAIN_LOOP_DELAY_MS);
    }
}
