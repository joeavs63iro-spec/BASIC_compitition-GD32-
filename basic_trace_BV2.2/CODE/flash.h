/*
 * flash.h
 *
 * Created on: 2026年4月15日
 * Description: 智能车掉电保存系统 (产业级安全版)
 */

#ifndef CODE_FLASH_H_
#define CODE_FLASH_H_

#include "headfile.h"


/* Change this value whenever the on-flash layout or parameter semantics change. */
#define FLASH_MAGIC_FLAG 0x42563202UL

typedef struct
{
    uint32_t magic_code;

    float p;
    float d;

    int set_off_count;

    uint16_t car_role;

    uint16_t t_avoid_1;
    uint16_t t_avoid_2;
    uint16_t t_avoid_3;
    uint16_t t_avoid_4;
    uint16_t t_avoid_5;

    uint16_t a_avoid_1;
    uint16_t a_avoid_2;
    uint16_t a_avoid_4;
    uint16_t a_avoid_5;

    uint16_t t_round;

    uint32_t used_speed_duty;
    uint32_t target_speed_duty;

    uint32_t reserved2[3];

    uint32_t checksum;
} CarParams_t;

void flash_init_and_load(void);
uint8_t flash_save_params(void);

#endif /* CODE_FLASH_H_ */
