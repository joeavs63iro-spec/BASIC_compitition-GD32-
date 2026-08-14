#include "ui.h"
#include "flash.h"
#include <stdio.h>

typedef enum {
    NO_KEY_PRESS = 0xFF,
    KEY1_PRESS = 0x01,
    KEY2_PRESS = 0x02,
    KEY3_PRESS = 0x03,
    KEY4_PRESS = 0x04,
    KEY5_PRESS = 0x05,
    KEY6_PRESS = 0x06
} teKeyNum_t;

typedef enum {
    PAGE_HOME = 0,
    PAGE_PARAM_LIST
} UIPage_t;

#define UI_VISIBLE_ROWS 4U
#define UI_PARAM_COUNT  18U

uint8_t g_car_role = 0;

static UIPage_t ui_page = PAGE_HOME;
static uint8_t ui_cursor = 0;
static uint8_t scroll_top = 0;
static uint8_t need_render = 1;

static void ui_show_string(uint8_t x, uint8_t y, const char *str, uint8_t size)
{
    oled_show_string(x, y, (uint8_t *)str, size);
}

static teKeyNum_t ui_read_key_once(void)
{
    return (teKeyNum_t)key_check();
}

static uint16_t ui_adjust_u16(uint16_t value, int32_t delta, uint16_t min_value, uint16_t max_value)
{
    int32_t next = (int32_t)value + delta;
    if(next < (int32_t)min_value) next = (int32_t)min_value;
    if(next > (int32_t)max_value) next = (int32_t)max_value;
    return (uint16_t)next;
}

static uint32_t ui_adjust_u32(uint32_t value, int32_t delta, uint32_t min_value, uint32_t max_value)
{
    int32_t next = (int32_t)value + delta;
    if(next < (int32_t)min_value) next = (int32_t)min_value;
    if(next > (int32_t)max_value) next = (int32_t)max_value;
    return (uint32_t)next;
}

static int ui_adjust_int(int value, int32_t delta, int min_value, int max_value)
{
    int32_t next = (int32_t)value + delta;
    if(next < (int32_t)min_value) next = (int32_t)min_value;
    if(next > (int32_t)max_value) next = (int32_t)max_value;
    return (int)next;
}

static float ui_adjust_float(float value, float delta, float min_value, float max_value)
{
    float next = value + delta;
    if(next < min_value) next = min_value;
    if(next > max_value) next = max_value;
    return next;
}

static void ui_fix_scroll(void)
{
    if(ui_cursor < scroll_top)
    {
        scroll_top = ui_cursor;
    }
    else if(ui_cursor >= (uint8_t)(scroll_top + UI_VISIBLE_ROWS))
    {
        scroll_top = (uint8_t)(ui_cursor - (UI_VISIBLE_ROWS - 1U));
    }
}

static void ui_adjust_param(uint8_t index, int8_t dir)
{
    switch(index)
    {
        case 0:  track_PID.P = ui_adjust_float(track_PID.P, (float)dir * 1.0f, 0.0f, 300.0f); break;
        case 1:  track_PID.D = ui_adjust_float(track_PID.D, (float)dir * 0.1f, 0.0f, 300.0f); break;
        case 2:  t_parking_1 = ui_adjust_u16(t_parking_1, dir * 10, 0, 5000); break;
        case 3:  t_parking_2 = ui_adjust_u16(t_parking_2, dir * 10, 0, 5000); break;
        case 4:  t_parking_3 = ui_adjust_u16(t_parking_3, dir * 10, 0, 5000); break;
        case 5:  t_parking_4 = ui_adjust_u16(t_parking_4, dir * 10, 0, 5000); break;
        case 6:  t_parking_5 = ui_adjust_u16(t_parking_5, dir * 10, 0, 5000); break;
        case 7:  a_parking_1 = ui_adjust_u16(a_parking_1, dir * 50, 0, 4095); break;
        case 8:  a_parking_2 = ui_adjust_u16(a_parking_2, dir * 50, 0, 4095); break;
        case 9:  a_parking_4 = ui_adjust_u16(a_parking_4, dir * 50, 0, 4095); break;
        case 10: a_parking_5 = ui_adjust_u16(a_parking_5, dir * 50, 0, 4095); break;
        case 11: t_avoid_1 = ui_adjust_u16(t_avoid_1, dir * 10, 0, 5000); break;
        case 12: t_avoid_2 = ui_adjust_u16(t_avoid_2, dir * 10, 0, 5000); break;
        case 13: t_avoid_3 = ui_adjust_u16(t_avoid_3, dir * 10, 0, 5000); break;
        case 14: t_round = ui_adjust_u16(t_round, dir * 10, 0, 5000); break;
        case 15: bridge_target_error = ui_adjust_int(bridge_target_error, dir, -300, 300); break;
        case 16: used_speed_duty = ui_adjust_u32(used_speed_duty, dir * 100, 1200U, 10000U); break;
        case 17: target_speed_duty = ui_adjust_u32(target_speed_duty, dir * 100, 1200U, 10000U); break;
        default: break;
    }
}

static void ui_format_param(uint8_t index, char *buf)
{
    switch(index)
    {
        case 0:  sprintf(buf, "Trk_P:%7.1f", track_PID.P); break;
        case 1:  sprintf(buf, "Trk_D:%7.1f", track_PID.D); break;
        case 2:  sprintf(buf, "T_Pk1:%6u", t_parking_1); break;
        case 3:  sprintf(buf, "T_Pk2:%6u", t_parking_2); break;
        case 4:  sprintf(buf, "T_Pk3:%6u", t_parking_3); break;
        case 5:  sprintf(buf, "T_Pk4:%6u", t_parking_4); break;
        case 6:  sprintf(buf, "T_Pk5:%6u", t_parking_5); break;
        case 7:  sprintf(buf, "A_Pk1:%6u", a_parking_1); break;
        case 8:  sprintf(buf, "A_Pk2:%6u", a_parking_2); break;
        case 9:  sprintf(buf, "A_Pk4:%6u", a_parking_4); break;
        case 10: sprintf(buf, "A_Pk5:%6u", a_parking_5); break;
        case 11: sprintf(buf, "T_Av1:%6u", t_avoid_1); break;
        case 12: sprintf(buf, "T_Av2:%6u", t_avoid_2); break;
        case 13: sprintf(buf, "T_Av3:%6u", t_avoid_3); break;
        case 14: sprintf(buf, "T_Rnd:%6u", t_round); break;
        case 15: sprintf(buf, "BrErr:%6d", bridge_target_error); break;
        case 16: sprintf(buf, "Used:%7lu", (unsigned long)used_speed_duty); break;
        case 17: sprintf(buf, "Tgt:%8lu", (unsigned long)target_speed_duty); break;
        default: sprintf(buf, ""); break;
    }
}

static void ui_save_params(void)
{
    uint8_t saved = flash_save_params();

    oled_clear();
    if(saved)
    {
        ui_show_string(16, 2, "PARAM SAVED", 16);
    }
    else
    {
        ui_show_string(16, 2, "SAVE FAILED", 16);
    }
    delay_1ms(250);
    need_render = 1;
}

void ui_init(void)
{
    ui_page = PAGE_HOME;
    ui_cursor = 0;
    scroll_top = 0;
    need_render = 1;
    oled_clear();
}

void ui_task(void)
{
    char buf[32];
    teKeyNum_t key = ui_read_key_once();

    if(key != NO_KEY_PRESS)
    {
        if(ui_page == PAGE_HOME)
        {
            ui_page = PAGE_PARAM_LIST;
            ui_cursor = 0;
            scroll_top = 0;
            need_render = 1;
        }
        else
        {
            if(key == KEY1_PRESS)
            {
                ui_cursor = (ui_cursor > 0U) ? (uint8_t)(ui_cursor - 1U) : (uint8_t)(UI_PARAM_COUNT - 1U);
                ui_fix_scroll();
                need_render = 1;
            }
            else if(key == KEY2_PRESS)
            {
                ui_cursor = (ui_cursor < (UI_PARAM_COUNT - 1U)) ? (uint8_t)(ui_cursor + 1U) : 0U;
                if(ui_cursor == 0U) scroll_top = 0U;
                ui_fix_scroll();
                need_render = 1;
            }
            else if(key == KEY3_PRESS || key == KEY4_PRESS)
            {
                ui_adjust_param(ui_cursor, (key == KEY3_PRESS) ? 1 : -1);
                need_render = 1;
            }
            else if(key == KEY5_PRESS)
            {
                ui_save_params();
            }
            else if(key == KEY6_PRESS)
            {
                ui_page = PAGE_HOME;
                need_render = 1;
            }
        }
    }

    if(need_render == 0U)
    {
        return;
    }

    oled_clear();
    if(ui_page == PAGE_HOME)
    {
        ui_show_string(16, 0, "SMART CAR V2", 16);
        ui_show_string(16, 3, "PARAM TUNE", 16);
        ui_show_string(24, 6, "[PUSH MENU]", 8);
    }
    else
    {
        uint8_t i;
        for(i = 0; i < UI_VISIBLE_ROWS; i++)
        {
            uint8_t item_idx = (uint8_t)(scroll_top + i);
            ui_show_string(0, (uint8_t)(i * 2U), "                ", 16);
            if(item_idx >= UI_PARAM_COUNT)
            {
                continue;
            }

            ui_format_param(item_idx, buf);
            ui_show_string(0, (uint8_t)(i * 2U), (item_idx == ui_cursor) ? ">" : " ", 16);
            ui_show_string(8, (uint8_t)(i * 2U), buf, 16);
        }
    }

    need_render = 0;
}
