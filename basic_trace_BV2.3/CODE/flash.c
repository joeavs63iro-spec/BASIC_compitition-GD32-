/*
 * flash.c
 *
 * Created on: 2026�?�?5�?
 * Description: 智能车掉电保存系�?
 */

#include "flash.h"
#include "hf_flash.h"
#include "control.h"
#include <string.h>

extern volatile PID_struct track_PID;
extern volatile int base_pwm;
extern uint16_t t_avoid_1;
extern uint16_t t_avoid_2;
extern uint16_t t_avoid_3;
extern uint16_t t_avoid_4;
extern uint16_t t_avoid_5;
extern uint16_t a_avoid_1;
extern uint16_t a_avoid_2;
extern uint16_t a_avoid_4;
extern uint16_t a_avoid_5;
extern uint16_t t_round;
extern uint16_t round_adc_threshold;
extern uint16_t round_trigger_debounce_ms;
extern uint32_t used_speed_duty;
extern uint32_t target_speed_duty;

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

static uint8_t flash_params_are_valid(const CarParams_t *params)
{
    if (!(params->p >= 0.0f && params->p <= 300.0f) ||
        !(params->d >= 0.0f && params->d <= 300.0f)) return 0U;
    if (params->t_avoid_1 > 10000U || params->t_avoid_2 > 10000U ||
        params->t_avoid_3 > 10000U || params->t_avoid_4 > 10000U ||
        params->t_avoid_5 > 10000U || params->t_round > 10000U) return 0U;
    if (params->a_avoid_1 > 4095U || params->a_avoid_2 > 4095U ||
        params->a_avoid_4 > 4095U || params->a_avoid_5 > 4095U) return 0U;
    if (params->round_adc_threshold > 4095U) return 0U;
    if (params->round_trigger_debounce_ms > 10000U) return 0U;
    if (params->used_speed_duty > 10000UL ||
        params->target_speed_duty > 10000UL) return 0U;
    return 1U;
}

static void flash_load_defaults(void)
{
    track_PID.P = 35.0f;
    track_PID.D = 0.0f;
    t_avoid_1 = 330;
    t_avoid_2 = 350;
    t_avoid_3 = 2800;
    t_avoid_4 = 480;
    t_avoid_5 = 300;
    a_avoid_1 = 3500;
    a_avoid_2 = 2000;
    a_avoid_4 = 3000;
    a_avoid_5 = 1500;
    t_round = 1000;
    round_adc_threshold = 3000;
    round_trigger_debounce_ms = 200;
    used_speed_duty = 5500;
    target_speed_duty = 5500;
    base_pwm = (int)target_speed_duty;
}

void flash_init_and_load(void)
{
    memset(&flash_box, 0, sizeof(CarParams_t));
    flash_ReadPage_uint32_t(PARAM_FLASH_PAGE, PARAM_FLASH_OFFSET, (uint32_t *)&flash_box, PARAM_FLASH_WORDS);

    if (flash_box.magic_code == FLASH_MAGIC_FLAG &&
        flash_box.checksum == calculate_checksum(&flash_box) &&
        flash_params_are_valid(&flash_box))
    {
        track_PID.P = flash_box.p;
        track_PID.D = flash_box.d;
        t_avoid_1 = flash_box.t_avoid_1;
        t_avoid_2 = flash_box.t_avoid_2;
        t_avoid_3 = flash_box.t_avoid_3;
        t_avoid_4 = flash_box.t_avoid_4;
        t_avoid_5 = flash_box.t_avoid_5;
        a_avoid_1 = flash_box.a_avoid_1;
        a_avoid_2 = flash_box.a_avoid_2;
        a_avoid_4 = flash_box.a_avoid_4;
        a_avoid_5 = flash_box.a_avoid_5;
        t_round = flash_box.t_round;
        round_adc_threshold = flash_box.round_adc_threshold;
        round_trigger_debounce_ms = flash_box.round_trigger_debounce_ms;
        used_speed_duty = flash_box.used_speed_duty;
        target_speed_duty = flash_box.target_speed_duty;
        base_pwm = (int)target_speed_duty;
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

    /* Clear padding/reserved bytes too, so checksum and read-back are deterministic. */
    memset(&flash_box, 0, sizeof(flash_box));
    flash_box.magic_code = FLASH_MAGIC_FLAG;
    flash_box.p = track_PID.P;
    flash_box.d = track_PID.D;
    flash_box.t_avoid_1 = t_avoid_1;
    flash_box.t_avoid_2 = t_avoid_2;
    flash_box.t_avoid_3 = t_avoid_3;
    flash_box.t_avoid_4 = t_avoid_4;
    flash_box.t_avoid_5 = t_avoid_5;
    flash_box.a_avoid_1 = a_avoid_1;
    flash_box.a_avoid_2 = a_avoid_2;
    flash_box.a_avoid_4 = a_avoid_4;
    flash_box.a_avoid_5 = a_avoid_5;
    flash_box.t_round = t_round;
    flash_box.round_adc_threshold = round_adc_threshold;
    flash_box.round_trigger_debounce_ms = round_trigger_debounce_ms;
    flash_box.used_speed_duty = used_speed_duty;
    flash_box.target_speed_duty = target_speed_duty;
    flash_box.reserved1[0] = 0;
    flash_box.reserved1[1] = 0;
    flash_box.reserved2[0] = 0;
    flash_box.reserved2[1] = 0;
    flash_box.reserved2[2] = 0;
    if (0U == flash_params_are_valid(&flash_box))
    {
        return 0;
    }
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
