/*********************************************************************************************************************
* @file            main.c
* @brief           Speed loop bluetooth tuning project
* @Target core     GD32F303RCT6
********************************************************************************************************************/

#include "main.h"
#include "headfile.h"
#include "control.h"
#include "bluetooth_tuning.h"

#define MAIN_LOOP_DELAY_MS             1U

static uint64_t control_last_time_us = 0;

static uint8_t period_elapsed(uint64_t *last_time_us, uint32_t period_ms);

int main(void)
{
    car_init();
    buzzer_duty(0);
    car_both_rgb_off();
    Control_Stop();
    BluetoothTune_Init();

    while(1)
    {
        BluetoothTune_Service();

        if(period_elapsed(&control_last_time_us, CONTROL_PERIOD_MS) != 0)
        {
            Control_Task();
            BluetoothTune_SendWaveform();
        }

        delay_1ms(MAIN_LOOP_DELAY_MS);
    }
}

static uint8_t period_elapsed(uint64_t *last_time_us, uint32_t period_ms)
{
    uint64_t now_us = get_current_time_us();
    uint64_t period_us = (uint64_t)period_ms * 1000ULL;

    if((now_us - *last_time_us) >= period_us)
    {
        *last_time_us = now_us;
        return 1;
    }

    return 0;
}
