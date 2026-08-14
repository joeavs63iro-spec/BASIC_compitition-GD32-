/*
 * interact.h
 *
 *  Created on: 2026Äê4ÔÂ14ÈÕ
 *      Author: A0728
 */

#ifndef CODE_UI_H_
#define CODE_UI_H_

#include "control.h"
#include "headfile.h"


/* ---------------- UI parameter externs ---------------- */
extern volatile uint8_t g_mag_cnt;
extern volatile PID_struct track_PID;

extern uint16_t t_avoid_1, t_avoid_2, t_avoid_3, t_avoid_4, t_avoid_5;
extern uint16_t a_avoid_1, a_avoid_2, a_avoid_4, a_avoid_5;
extern uint16_t t_round;
extern uint16_t round_adc_threshold;
extern uint16_t round_trigger_debounce_ms;
extern uint32_t used_speed_duty, target_speed_duty;
/* ---------------- UI API ---------------- */
void ui_init(void);
void ui_task(void);

#endif /* CODE_UI_H_ */

