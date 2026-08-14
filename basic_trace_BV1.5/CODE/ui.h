/*
 * interact.h
 *
 *  Created on: 2026年4月14日
 *      Author: A0728
 */

#ifndef CODE_UI_H_
#define CODE_UI_H_

#include "control.h"
#include "headfile.h"


/* ---------------- 全局身份变量 ---------------- */
extern uint8_t g_car_role;
extern volatile uint8_t g_mag_cnt;
extern volatile PID_struct track_PID;

extern int set_off_count;
extern uint16_t d_avoid,p_avoid_1, p_avoid_2, p_avoid_3;
extern uint16_t a_avoid_1, a_avoid_2, a_avoid_3;
extern uint16_t d_round_in, p_round_in, a_round_in, p_round_out;
extern uint16_t a_park_1, p_park_1, p_park_2, p_park_4;
extern int d_park_3;
/* ---------------- UI 接口函数 ---------------- */
void ui_init(void);
void ui_task(void);

#endif /* CODE_UI_H_ */

