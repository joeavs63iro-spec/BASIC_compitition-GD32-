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
#define FLASH_PARAM_VERSION 4U

typedef struct
{
    uint32_t magic_code;
    uint16_t param_version;
    uint16_t reserved0;

    float track_p;
    float track_d;

    uint32_t used_speed_duty;
    uint32_t target_speed_duty;

    uint16_t t_avoid_1;
    uint16_t t_avoid_2;
    uint16_t t_avoid_3;
    uint16_t a_avoid_1;
    uint16_t a_avoid_2;
    uint16_t a_avoid_3;
    uint16_t ble_avoid_distance_mm;
    uint16_t t_round;

    int bridge_target_error;
    uint16_t round_adc_threshold;
    uint16_t bridge_keep_time_ms;

    uint16_t t_parking_1;
    uint16_t t_parking_2;
    uint16_t t_parking_3;
    uint16_t t_parking_4;
    uint16_t t_parking_5;
    uint16_t a_parking_1;
    uint16_t a_parking_2;
    uint16_t a_parking_4;
    uint16_t a_parking_5;
    uint16_t round_check_delay_ms;

    uint32_t checksum;
} CarParams_t;

void flash_init_and_load(void);
uint8_t flash_save_params(void);

#endif /* CODE_FLASH_H_ */
