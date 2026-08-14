/*
 * flash.h
 *
 * Created on: 2026年4月15日
 * Description: 智能车掉电保存系统 (产业级安全版)
 */

#ifndef CODE_FLASH_H_
#define CODE_FLASH_H_

#include "headfile.h"


#define FLASH_MAGIC_FLAG 0x5A5A5A5A
#define FLASH_PARAM_VERSION 2U

typedef struct
{
    uint32_t magic_code;

    float p;
    float d;

    int set_off_count;
    int d_park_3;

    uint16_t d_avoid;
    uint16_t p_avoid_1;
    uint16_t p_avoid_2;
    uint16_t p_avoid_3;
    uint16_t a_avoid_1;
    uint16_t a_avoid_2;
    uint16_t a_avoid_3;

    uint16_t d_round_in;
    uint16_t p_round_in;
    uint16_t a_round_in;
    uint16_t p_round_out;

    uint16_t a_park_1;
    uint16_t p_park_1;
    uint16_t p_park_2;
    uint16_t p_park_4;

    uint16_t car_role;
    uint16_t param_version;

    uint16_t t_parking_1;
    uint16_t t_parking_2;
    uint16_t t_parking_3;
    uint16_t t_parking_4;
    uint16_t t_parking_5;
    uint16_t a_parking_1;
    uint16_t a_parking_2;
    uint16_t a_parking_4;
    uint16_t a_parking_5;

    uint16_t t_avoid_1;
    uint16_t t_avoid_2;
    uint16_t t_avoid_3;
    uint16_t t_round;

    int bridge_target_error;
    uint32_t used_speed_duty;
    uint32_t target_speed_duty;

    uint32_t reserved2[2];

    uint32_t checksum;
} CarParams_t;

void flash_init_and_load(void);
uint8_t flash_save_params(void);

#endif /* CODE_FLASH_H_ */
