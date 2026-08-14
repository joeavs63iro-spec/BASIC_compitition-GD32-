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
    PAGE_MAIN_MENU,
    PAGE_PID_SPEED,
    PAGE_PARK_TIME,
    PAGE_PARK_ADC,
    PAGE_AVOID_TIME,
    PAGE_AVOID_ADC,
    PAGE_ROUND_BRIDGE
} UIPage_t;

#define UI_VISIBLE_ROWS 4U
#define UI_MENU_COUNT   6U

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

static uint8_t ui_page_count(UIPage_t page)
{
    switch(page)
    {
        case PAGE_MAIN_MENU:    return UI_MENU_COUNT;
        case PAGE_PID_SPEED:    return 4U;
        case PAGE_PARK_TIME:    return 5U;
        case PAGE_PARK_ADC:     return 4U;
        case PAGE_AVOID_TIME:   return 3U;
        case PAGE_AVOID_ADC:    return 3U;
        case PAGE_ROUND_BRIDGE: return 2U;
        default:                return 0U;
    }
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

static void ui_enter_page(UIPage_t page)
{
    ui_page = page;
    ui_cursor = 0;
    scroll_top = 0;
    need_render = 1;
}

static void ui_adjust_current(int8_t dir)
{
    switch(ui_page)
    {
        case PAGE_PID_SPEED:
            if     (ui_cursor == 0U) track_PID.P = ui_adjust_float(track_PID.P, (float)dir * 1.0f, 0.0f, 300.0f);
            else if(ui_cursor == 1U) track_PID.D = ui_adjust_float(track_PID.D, (float)dir * 0.1f, 0.0f, 300.0f);
            else if(ui_cursor == 2U) used_speed_duty = ui_adjust_u32(used_speed_duty, dir * 100, 1200U, 10000U);
            else if(ui_cursor == 3U) target_speed_duty = ui_adjust_u32(target_speed_duty, dir * 100, 1200U, 10000U);
            break;

        case PAGE_PARK_TIME:
            if     (ui_cursor == 0U) t_parking_1 = ui_adjust_u16(t_parking_1, dir * 10, 0, 5000);
            else if(ui_cursor == 1U) t_parking_2 = ui_adjust_u16(t_parking_2, dir * 10, 0, 5000);
            else if(ui_cursor == 2U) t_parking_3 = ui_adjust_u16(t_parking_3, dir * 10, 0, 5000);
            else if(ui_cursor == 3U) t_parking_4 = ui_adjust_u16(t_parking_4, dir * 10, 0, 5000);
            else if(ui_cursor == 4U) t_parking_5 = ui_adjust_u16(t_parking_5, dir * 10, 0, 5000);
            break;

        case PAGE_PARK_ADC:
            if     (ui_cursor == 0U) a_parking_1 = ui_adjust_u16(a_parking_1, dir * 50, 0, 4095);
            else if(ui_cursor == 1U) a_parking_2 = ui_adjust_u16(a_parking_2, dir * 50, 0, 4095);
            else if(ui_cursor == 2U) a_parking_4 = ui_adjust_u16(a_parking_4, dir * 50, 0, 4095);
            else if(ui_cursor == 3U) a_parking_5 = ui_adjust_u16(a_parking_5, dir * 50, 0, 4095);
            break;

        case PAGE_AVOID_TIME:
            if     (ui_cursor == 0U) t_avoid_1 = ui_adjust_u16(t_avoid_1, dir * 10, 0, 5000);
            else if(ui_cursor == 1U) t_avoid_2 = ui_adjust_u16(t_avoid_2, dir * 10, 0, 5000);
            else if(ui_cursor == 2U) t_avoid_3 = ui_adjust_u16(t_avoid_3, dir * 10, 0, 5000);
            break;

        case PAGE_AVOID_ADC:
            if     (ui_cursor == 0U) a_avoid_1 = ui_adjust_u16(a_avoid_1, dir * 50, 0, 4095);
            else if(ui_cursor == 1U) a_avoid_2 = ui_adjust_u16(a_avoid_2, dir * 50, 0, 4095);
            else if(ui_cursor == 2U) a_avoid_3 = ui_adjust_u16(a_avoid_3, dir * 50, 0, 4095);
            break;

        case PAGE_ROUND_BRIDGE:
            if     (ui_cursor == 0U) t_round = ui_adjust_u16(t_round, dir * 10, 0, 5000);
            else if(ui_cursor == 1U) bridge_target_error = ui_adjust_int(bridge_target_error, dir, -300, 300);
            break;

        default:
            break;
    }
}

static void ui_format_item(UIPage_t page, uint8_t index, char *buf)
{
    switch(page)
    {
        case PAGE_MAIN_MENU:
            if     (index == 0U) sprintf(buf, "1.PID/Speed");
            else if(index == 1U) sprintf(buf, "2.Park Time");
            else if(index == 2U) sprintf(buf, "3.Park ADC");
            else if(index == 3U) sprintf(buf, "4.Avoid Time");
            else if(index == 4U) sprintf(buf, "5.Avoid ADC");
            else if(index == 5U) sprintf(buf, "6.Round/Bridge");
            break;

        case PAGE_PID_SPEED:
            if     (index == 0U) sprintf(buf, "Trk_P:%7.1f", track_PID.P);
            else if(index == 1U) sprintf(buf, "Trk_D:%7.1f", track_PID.D);
            else if(index == 2U) sprintf(buf, "Used:%7lu", (unsigned long)used_speed_duty);
            else if(index == 3U) sprintf(buf, "Tgt:%8lu", (unsigned long)target_speed_duty);
            break;

        case PAGE_PARK_TIME:
            if     (index == 0U) sprintf(buf, "T_Pk1:%6u", t_parking_1);
            else if(index == 1U) sprintf(buf, "T_Pk2:%6u", t_parking_2);
            else if(index == 2U) sprintf(buf, "T_Pk3:%6u", t_parking_3);
            else if(index == 3U) sprintf(buf, "T_Pk4:%6u", t_parking_4);
            else if(index == 4U) sprintf(buf, "T_Pk5:%6u", t_parking_5);
            break;

        case PAGE_PARK_ADC:
            if     (index == 0U) sprintf(buf, "A_Pk1:%6u", a_parking_1);
            else if(index == 1U) sprintf(buf, "A_Pk2:%6u", a_parking_2);
            else if(index == 2U) sprintf(buf, "A_Pk4:%6u", a_parking_4);
            else if(index == 3U) sprintf(buf, "A_Pk5:%6u", a_parking_5);
            break;

        case PAGE_AVOID_TIME:
            if     (index == 0U) sprintf(buf, "T_Av1:%6u", t_avoid_1);
            else if(index == 1U) sprintf(buf, "T_Av2:%6u", t_avoid_2);
            else if(index == 2U) sprintf(buf, "T_Av3:%6u", t_avoid_3);
            break;

        case PAGE_AVOID_ADC:
            if     (index == 0U) sprintf(buf, "A_Av1:%6u", a_avoid_1);
            else if(index == 1U) sprintf(buf, "A_Av2:%6u", a_avoid_2);
            else if(index == 2U) sprintf(buf, "A_Av3:%6u", a_avoid_3);
            break;

        case PAGE_ROUND_BRIDGE:
            if     (index == 0U) sprintf(buf, "T_Rnd:%6u", t_round);
            else if(index == 1U) sprintf(buf, "BrErr:%6d", bridge_target_error);
            break;

        default:
            sprintf(buf, "");
            break;
    }
}

static void ui_save_and_menu(void)
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
    ui_enter_page(PAGE_MAIN_MENU);
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
            ui_enter_page(PAGE_MAIN_MENU);
        }
        else if(ui_page == PAGE_MAIN_MENU)
        {
            if(key == KEY1_PRESS)
            {
                ui_cursor = (ui_cursor > 0U) ? (uint8_t)(ui_cursor - 1U) : (uint8_t)(UI_MENU_COUNT - 1U);
                ui_fix_scroll();
                need_render = 1;
            }
            else if(key == KEY2_PRESS)
            {
                ui_cursor = (ui_cursor < (UI_MENU_COUNT - 1U)) ? (uint8_t)(ui_cursor + 1U) : 0U;
                if(ui_cursor == 0U) scroll_top = 0U;
                ui_fix_scroll();
                need_render = 1;
            }
            else if(key == KEY3_PRESS || key == KEY5_PRESS)
            {
                ui_enter_page((UIPage_t)(PAGE_PID_SPEED + ui_cursor));
            }
            else if(key == KEY4_PRESS || key == KEY6_PRESS)
            {
                ui_enter_page(PAGE_HOME);
            }
        }
        else
        {
            uint8_t total = ui_page_count(ui_page);

            if(key == KEY1_PRESS)
            {
                ui_cursor = (ui_cursor > 0U) ? (uint8_t)(ui_cursor - 1U) : (uint8_t)(total - 1U);
                ui_fix_scroll();
                need_render = 1;
            }
            else if(key == KEY2_PRESS)
            {
                ui_cursor = (ui_cursor < (total - 1U)) ? (uint8_t)(ui_cursor + 1U) : 0U;
                if(ui_cursor == 0U) scroll_top = 0U;
                ui_fix_scroll();
                need_render = 1;
            }
            else if(key == KEY3_PRESS || key == KEY4_PRESS)
            {
                ui_adjust_current((key == KEY3_PRESS) ? 1 : -1);
                need_render = 1;
            }
            else if(key == KEY5_PRESS)
            {
                ui_save_and_menu();
            }
            else if(key == KEY6_PRESS)
            {
                ui_enter_page(PAGE_MAIN_MENU);
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
        ui_show_string(16, 3, "PARAM MENU", 16);
        ui_show_string(24, 6, "[PUSH MENU]", 8);
    }
    else
    {
        uint8_t total = ui_page_count(ui_page);
        uint8_t i;

        for(i = 0; i < UI_VISIBLE_ROWS; i++)
        {
            uint8_t item_idx = (uint8_t)(scroll_top + i);
            ui_show_string(0, (uint8_t)(i * 2U), "                ", 16);
            if(item_idx >= total)
            {
                continue;
            }

            ui_format_item(ui_page, item_idx, buf);
            ui_show_string(0, (uint8_t)(i * 2U), (item_idx == ui_cursor) ? ">" : " ", 16);
            ui_show_string(8, (uint8_t)(i * 2U), buf, 16);
        }
    }

    need_render = 0;
}
