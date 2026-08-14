/*
 * flash.c
 *
 * Created on: 2026年4月15日
 * Description: 智能车掉电保存系统
 */

#include "flash.h"
#include "hf_flash.h"
#include "control.h"
#include <string.h>

extern volatile PID_struct track_PID;
extern uint8_t g_car_role;

int set_off_count = 60;
int d_park_3 = 1;
uint16_t d_avoid = 100, p_avoid_1 = 5000, p_avoid_2 = 6500, p_avoid_3 = 3000;
uint16_t a_avoid_1 = 800, a_avoid_2 = 3000, a_avoid_3 = 800;
uint16_t d_round_in = 300, p_round_in = 4200, a_round_in = 3500, p_round_out = 3000;
uint16_t a_park_1 = 1300, p_park_1 = 6100, p_park_2 = 1400, p_park_4 = 600;

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
    track_PID.D = 10.0f;
    g_car_role = 0;
    set_off_count = 60;
    d_park_3 = 1;
    d_avoid = 100;
    p_avoid_1 = 5000;
    p_avoid_2 = 6500;
    p_avoid_3 = 3000;
    a_avoid_1 = 800;
    a_avoid_2 = 3000;
    a_avoid_3 = 800;
    d_round_in = 300;
    p_round_in = 4200;
    a_round_in = 3500;
    p_round_out = 3000;
    a_park_1 = 1300;
    p_park_1 = 6100;
    p_park_2 = 1400;
    p_park_4 = 600;
}

void flash_init_and_load(void)
{
    memset(&flash_box, 0, sizeof(CarParams_t));
    flash_ReadPage_uint32_t(PARAM_FLASH_PAGE, PARAM_FLASH_OFFSET, (uint32_t *)&flash_box, PARAM_FLASH_WORDS);

    if (flash_box.magic_code == FLASH_MAGIC_FLAG && flash_box.checksum == calculate_checksum(&flash_box))
    {
        track_PID.P = flash_box.p;
        track_PID.D = flash_box.d;
        g_car_role = (flash_box.car_role <= 1U) ? (uint8_t)flash_box.car_role : 0U;
        set_off_count = flash_box.set_off_count;
        d_park_3 = flash_box.d_park_3;
        d_avoid = flash_box.d_avoid;
        p_avoid_1 = flash_box.p_avoid_1;
        p_avoid_2 = flash_box.p_avoid_2;
        p_avoid_3 = flash_box.p_avoid_3;
        a_avoid_1 = flash_box.a_avoid_1;
        a_avoid_2 = flash_box.a_avoid_2;
        a_avoid_3 = flash_box.a_avoid_3;
        d_round_in = flash_box.d_round_in;
        p_round_in = flash_box.p_round_in;
        a_round_in = flash_box.a_round_in;
        p_round_out = flash_box.p_round_out;
        a_park_1 = flash_box.a_park_1;
        p_park_1 = flash_box.p_park_1;
        p_park_2 = flash_box.p_park_2;
        p_park_4 = flash_box.p_park_4;
    }
    else
    {
        flash_load_defaults();
        (void)flash_save_params();
    }
}

uint8_t flash_save_params(void)
{
    CarParams_t verify_box;

    flash_box.magic_code = FLASH_MAGIC_FLAG;
    flash_box.p = track_PID.P;
    flash_box.d = track_PID.D;
    flash_box.set_off_count = set_off_count;
    flash_box.d_park_3 = d_park_3;
    flash_box.d_avoid = d_avoid;
    flash_box.p_avoid_1 = p_avoid_1;
    flash_box.p_avoid_2 = p_avoid_2;
    flash_box.p_avoid_3 = p_avoid_3;
    flash_box.a_avoid_1 = a_avoid_1;
    flash_box.a_avoid_2 = a_avoid_2;
    flash_box.a_avoid_3 = a_avoid_3;
    flash_box.d_round_in = d_round_in;
    flash_box.p_round_in = p_round_in;
    flash_box.a_round_in = a_round_in;
    flash_box.p_round_out = p_round_out;
    flash_box.a_park_1 = a_park_1;
    flash_box.p_park_1 = p_park_1;
    flash_box.p_park_2 = p_park_2;
    flash_box.p_park_4 = p_park_4;
    flash_box.car_role = g_car_role;
    flash_box.reserved2[0] = 0;
    flash_box.reserved2[1] = 0;
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
