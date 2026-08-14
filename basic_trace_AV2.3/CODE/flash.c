/*
 * flash.c
 *
 * Created on: 2026-04-15
 * Description: smart car parameter flash storage
 */

#include "flash.h"
#include "hf_flash.h"
#include "control.h"
#include <string.h>

extern volatile PID_struct track_PID;
extern int bridge_target_error;

uint16_t a_avoid_1 = 1000, a_avoid_2 = 3500, a_avoid_3 = 2800;
uint16_t t_parking_1 = 380, t_parking_2 = 270, t_parking_3 = 270, t_parking_4 = 200, t_parking_5 = 400;
uint16_t a_parking_1 = 2400, a_parking_2 = 2500, a_parking_4 = 2300, a_parking_5 = 2600;
uint16_t t_avoid_1 = 450, t_avoid_2 = 580, t_avoid_3 = 330;
uint16_t t_round = 250;
uint16_t round_check_delay_ms = 2000;
uint16_t ble_avoid_distance_mm = 800;
uint16_t round_adc_threshold = 3000;
uint16_t bridge_keep_time_ms = 960;
uint32_t used_speed_duty = 5500;
uint32_t target_speed_duty = 6000;

static CarParams_t flash_box;

#define PARAM_FLASH_PAGE    (((uint32_t)FMC_SIZE * 1024U / SIZE_2KB) - 1U)
#define PARAM_FLASH_OFFSET  0U
#define PARAM_FLASH_WORDS   ((uint16_t)(sizeof(CarParams_t) / sizeof(uint32_t)))
typedef char car_params_must_fit_one_flash_page[(sizeof(CarParams_t) <= SIZE_2KB) ? 1 : -1];
typedef char car_params_must_be_word_aligned[((sizeof(CarParams_t) % sizeof(uint32_t)) == 0U) ? 1 : -1];

static uint32_t calculate_checksum(CarParams_t *params)
{
    uint32_t sum = 0;
    uint32_t *ptr = (uint32_t *)params;
    uint32_t count = (sizeof(CarParams_t) / sizeof(uint32_t)) - 1U;
    uint32_t i;

    for (i = 0; i < count; i++) {
        sum += ptr[i];
    }

    return sum;
}

static void flash_load_defaults(void)
{
    track_PID.P = 60.0f;
    track_PID.D = 0.0f;
    a_avoid_1 = 1000;
    a_avoid_2 = 3300;
    a_avoid_3 = 2800;
    t_parking_1 = 390;
    t_parking_2 = 260;
    t_parking_3 = 260;
    t_parking_4 = 200;
    t_parking_5 = 400;
    a_parking_1 = 2100;
    a_parking_2 = 2600;
    a_parking_4 = 1950;
    a_parking_5 = 2500;
    t_avoid_1 = 450;
    t_avoid_2 = 560;
    t_avoid_3 = 350;
    t_round = 300;
    round_check_delay_ms = 2000;
    ble_avoid_distance_mm = 800;
    round_adc_threshold = 3000;
    bridge_keep_time_ms = 960;
    bridge_target_error = 63;
    used_speed_duty = 5500;
    target_speed_duty = 6000;
}

void flash_init_and_load(void)
{
    memset(&flash_box, 0, sizeof(CarParams_t));
    flash_ReadPage_uint32_t(PARAM_FLASH_PAGE, PARAM_FLASH_OFFSET, (uint32_t *)&flash_box, PARAM_FLASH_WORDS);

    if (flash_box.magic_code == FLASH_MAGIC_FLAG &&
        flash_box.param_version == FLASH_PARAM_VERSION &&
        flash_box.checksum == calculate_checksum(&flash_box))
    {
        track_PID.P = flash_box.track_p;
        track_PID.D = flash_box.track_d;
        used_speed_duty = flash_box.used_speed_duty;
        target_speed_duty = flash_box.target_speed_duty;

        t_avoid_1 = flash_box.t_avoid_1;
        t_avoid_2 = flash_box.t_avoid_2;
        t_avoid_3 = flash_box.t_avoid_3;
        a_avoid_1 = flash_box.a_avoid_1;
        a_avoid_2 = flash_box.a_avoid_2;
        a_avoid_3 = flash_box.a_avoid_3;
        ble_avoid_distance_mm = flash_box.ble_avoid_distance_mm;
        t_round = flash_box.t_round;
        round_check_delay_ms = flash_box.round_check_delay_ms;
        if((round_check_delay_ms < 100U) || (round_check_delay_ms > 10000U))
        {
            round_check_delay_ms = 2000U;
        }

        bridge_target_error = flash_box.bridge_target_error;
        round_adc_threshold = flash_box.round_adc_threshold;
        bridge_keep_time_ms = flash_box.bridge_keep_time_ms;

        t_parking_1 = flash_box.t_parking_1;
        t_parking_2 = flash_box.t_parking_2;
        t_parking_3 = flash_box.t_parking_3;
        t_parking_4 = flash_box.t_parking_4;
        t_parking_5 = flash_box.t_parking_5;
        a_parking_1 = flash_box.a_parking_1;
        a_parking_2 = flash_box.a_parking_2;
        a_parking_4 = flash_box.a_parking_4;
        a_parking_5 = flash_box.a_parking_5;
    }
    else
    {
        flash_load_defaults();
    }
}

uint8_t flash_save_params(void)
{
    CarParams_t verify_box;

    memset(&flash_box, 0, sizeof(CarParams_t));
    flash_box.magic_code = FLASH_MAGIC_FLAG;
    flash_box.param_version = FLASH_PARAM_VERSION;
    flash_box.track_p = track_PID.P;
    flash_box.track_d = track_PID.D;
    flash_box.used_speed_duty = used_speed_duty;
    flash_box.target_speed_duty = target_speed_duty;

    flash_box.t_avoid_1 = t_avoid_1;
    flash_box.t_avoid_2 = t_avoid_2;
    flash_box.t_avoid_3 = t_avoid_3;
    flash_box.a_avoid_1 = a_avoid_1;
    flash_box.a_avoid_2 = a_avoid_2;
    flash_box.a_avoid_3 = a_avoid_3;
    flash_box.ble_avoid_distance_mm = ble_avoid_distance_mm;
    flash_box.t_round = t_round;
    flash_box.round_check_delay_ms = round_check_delay_ms;

    flash_box.bridge_target_error = bridge_target_error;
    flash_box.round_adc_threshold = round_adc_threshold;
    flash_box.bridge_keep_time_ms = bridge_keep_time_ms;

    flash_box.t_parking_1 = t_parking_1;
    flash_box.t_parking_2 = t_parking_2;
    flash_box.t_parking_3 = t_parking_3;
    flash_box.t_parking_4 = t_parking_4;
    flash_box.t_parking_5 = t_parking_5;
    flash_box.a_parking_1 = a_parking_1;
    flash_box.a_parking_2 = a_parking_2;
    flash_box.a_parking_4 = a_parking_4;
    flash_box.a_parking_5 = a_parking_5;
    flash_box.checksum = calculate_checksum(&flash_box);

    if (0 == flash_WritePage_uint32_t(PARAM_FLASH_PAGE, PARAM_FLASH_OFFSET, (uint32_t *)&flash_box, PARAM_FLASH_WORDS))
    {
        return 0;
    }

    if (0 == flash_ReadPage_uint32_t(PARAM_FLASH_PAGE, PARAM_FLASH_OFFSET, (uint32_t *)&verify_box, PARAM_FLASH_WORDS))
    {
        return 0;
    }

    if (0 != memcmp(&flash_box, &verify_box, sizeof(CarParams_t)))
    {
        return 0;
    }

    return 1;
}
