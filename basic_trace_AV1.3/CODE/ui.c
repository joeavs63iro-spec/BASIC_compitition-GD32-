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

static void ui_show_string(uint8_t x, uint8_t y, const char *str, uint8_t size)
{
    oled_show_string(x, y, (uint8_t *)str, size);
}
// ---------------- Page and state machine ----------------
typedef enum {
    PAGE_HOME = 0,
    PAGE_MAIN_MENU,
    PAGE_ROLE_SETUP,
    PAGE_PID_PWM,
    PAGE_SET_OFF,
    PAGE_AVOID,
    PAGE_ROUND,
    PAGE_PARK,
    PAGE_SENSOR_ALL
} UIPage_t;

static UIPage_t ui_page = PAGE_HOME;
static uint8_t ui_cursor = 0;
static uint8_t scroll_top = 0;
uint8_t g_car_role = 0;

typedef struct {
    uint8_t full_page;
    uint8_t list_items;
    uint8_t cursor_move;
    uint8_t values_only;
} RenderRequest_t;

static RenderRequest_t req = {1, 0, 0, 0};
static uint8_t last_screen_row = 255;

static teKeyNum_t ui_read_key_once(void) {
    return (teKeyNum_t)key_check();
}

static uint16_t ui_adjust_u16(uint16_t value, int32_t delta, uint16_t min_value, uint16_t max_value) {
    int32_t next = (int32_t)value + delta;
    if (next < (int32_t)min_value) next = (int32_t)min_value;
    if (next > (int32_t)max_value) next = (int32_t)max_value;
    return (uint16_t)next;
}

static int ui_adjust_int(int value, int32_t delta, int min_value, int max_value) {
    int32_t next = (int32_t)value + delta;
    if (next < (int32_t)min_value) next = (int32_t)min_value;
    if (next > (int32_t)max_value) next = (int32_t)max_value;
    return (int)next;
}

static float ui_adjust_float(float value, float delta, float min_value, float max_value) {
    float next = value + delta;
    if (next < min_value) next = min_value;
    if (next > max_value) next = max_value;
    return next;
}

static void ui_save_and_return(uint8_t menu_cursor) {
    uint8_t saved = flash_save_params();

    oled_clear();
    if (saved) {
        ui_show_string(16, 2, "PARAM SAVED", 16);
    } else {
        ui_show_string(16, 2, "SAVE FAILED", 16);
    }
    delay_1ms(250);

    ui_page = PAGE_MAIN_MENU;
    ui_cursor = menu_cursor;
    scroll_top = (ui_cursor > 3U) ? (uint8_t)(ui_cursor - 3U) : 0U;
    req.full_page = 1;
    req.cursor_move = 0;
    req.values_only = 0;
    req.list_items = 0;
    last_screen_row = 255;
}


void ui_init(void) {
    ui_page = PAGE_HOME;
    ui_cursor = 0;
    scroll_top = 0;
    req.full_page = 1;
    req.cursor_move = 0;
    req.values_only = 0;
    req.list_items = 0;
    last_screen_row = 255;
    oled_clear();
}

void ui_task(void) {
    char buf[32];
    teKeyNum_t key = ui_read_key_once();

    if (ui_page == PAGE_SENSOR_ALL)
    {
        static uint8_t sensor_tick = 0;
        if(++sensor_tick > 10)
        {
            req.values_only = 1;
            sensor_tick = 0;
        }
    }

    // ================== Controller logic ==================
    if (key != NO_KEY_PRESS) {
        switch (ui_page) {
            case PAGE_HOME:
                ui_page = PAGE_MAIN_MENU;
                ui_cursor = 0;
                req.full_page = 1;
                break;

            case PAGE_MAIN_MENU: {
                uint8_t total = 7;
                if (key == KEY1_PRESS) { ui_cursor = (ui_cursor > 0) ? ui_cursor - 1 : total - 1;
                if (ui_cursor < scroll_top) { scroll_top = ui_cursor; req.list_items = 1; }
                else req.cursor_move = 1; }
                if (key == KEY3_PRESS) {
                    ui_cursor = (ui_cursor < total - 1) ? ui_cursor + 1 : 0;
                    if (ui_cursor >= scroll_top + 4) {
                        scroll_top = ui_cursor - 3;
                        req.list_items = 1;
                    } else {
                        req.cursor_move = 1;
                    }
                    if (ui_cursor == 0) {
                        scroll_top = 0;
                        req.list_items = 1;
                    }
                }
                if (key == KEY4_PRESS) { ui_page = PAGE_HOME; req.full_page = 1; }
                if (key == KEY5_PRESS) {
                    if      (ui_cursor == 0) ui_page = PAGE_ROLE_SETUP;
                    else if (ui_cursor == 1) ui_page = PAGE_PID_PWM;
                    else if (ui_cursor == 2) ui_page = PAGE_SET_OFF;
                    else if (ui_cursor == 3) ui_page = PAGE_AVOID;
                    else if (ui_cursor == 4) ui_page = PAGE_ROUND;
                    else if (ui_cursor == 5) ui_page = PAGE_PARK;
                    else if (ui_cursor == 6) ui_page = PAGE_SENSOR_ALL;
                    ui_cursor = 0; scroll_top = 0; req.full_page = 1;
                }
                break;
            }

            case PAGE_ROLE_SETUP:
                if (key == KEY2_PRESS) { g_car_role = 1; req.cursor_move = 1; }
                if (key == KEY4_PRESS) { g_car_role = 0; req.cursor_move = 1; }
                if (key == KEY5_PRESS) { ui_save_and_return(0); }
                break;

            case PAGE_PID_PWM: {
                int8_t step_pid;

                if (key == KEY1_PRESS) { ui_cursor = (ui_cursor > 0) ? ui_cursor - 1 : 1; req.cursor_move = 1; }
                if (key == KEY3_PRESS) { ui_cursor = (ui_cursor < 1) ? ui_cursor + 1 : 0; req.cursor_move = 1; }
                step_pid = (key == KEY2_PRESS) ? 1 : (key == KEY4_PRESS ? -1 : 0);
                if (step_pid != 0) {
                    if      (ui_cursor == 0) track_PID.P = ui_adjust_float(track_PID.P, (float)step_pid * 1.0f, 0.0f, 300.0f);
                    else if (ui_cursor == 1) track_PID.D = ui_adjust_float(track_PID.D, (float)step_pid * 1.0f, 0.0f, 300.0f);
                    req.values_only = 1;
                }
                if (key == KEY5_PRESS) { ui_save_and_return(1); }
                break;
            }

            case PAGE_SET_OFF: {
                int8_t step_so = (key == KEY2_PRESS) ? 1 : (key == KEY4_PRESS ? -1 : 0);
                if (step_so != 0) { set_off_count = ui_adjust_int(set_off_count, step_so, 1, 300); req.values_only = 1; }
                if (key == KEY5_PRESS) { ui_save_and_return(2); }
                break;
            }

            case PAGE_AVOID:
            case PAGE_ROUND:
            case PAGE_PARK: {
                uint8_t total = (ui_page == PAGE_AVOID) ? 7 : (ui_page == PAGE_ROUND ? 4 : 5);
                int8_t step;
                if (key == KEY1_PRESS) { ui_cursor = (ui_cursor > 0) ? ui_cursor - 1 : total - 1; if (ui_cursor < scroll_top) { scroll_top = ui_cursor; req.list_items = 1; } else req.cursor_move = 1; }
                if (key == KEY3_PRESS) {
                    ui_cursor = (ui_cursor < total - 1) ? ui_cursor + 1 : 0;
                    if (ui_cursor >= scroll_top + 4) {
                        scroll_top = ui_cursor - 3;
                        req.list_items = 1;
                    } else {
                        req.cursor_move = 1;
                    }
                    if (ui_cursor == 0) {
                        scroll_top = 0;
                        req.list_items = 1;
                    }
                }

                step = (key == KEY2_PRESS) ? 1 : (key == KEY4_PRESS ? -1 : 0);
                if (step != 0) {
                    if (ui_page == PAGE_AVOID) {
                        if      (ui_cursor == 0) d_avoid   = ui_adjust_u16(d_avoid,   step * 100, 0, 5000);
                        else if (ui_cursor == 1) p_avoid_1 = ui_adjust_u16(p_avoid_1, step * 500, 0, 60000);
                        else if (ui_cursor == 2) p_avoid_2 = ui_adjust_u16(p_avoid_2, step * 500, 0, 60000);
                        else if (ui_cursor == 3) p_avoid_3 = ui_adjust_u16(p_avoid_3, step * 500, 0, 60000);
                        else if (ui_cursor == 4) a_avoid_1 = ui_adjust_u16(a_avoid_1, step * 50, 0, 4095);
                        else if (ui_cursor == 5) a_avoid_2 = ui_adjust_u16(a_avoid_2, step * 50, 0, 4095);
                        else if (ui_cursor == 6) a_avoid_3 = ui_adjust_u16(a_avoid_3, step * 50, 0, 4095);
                    } else if (ui_page == PAGE_ROUND) {
                        if      (ui_cursor == 0) d_round_in = ui_adjust_u16(d_round_in, step * 10, 0, 5000);
                        else if (ui_cursor == 1) p_round_in = ui_adjust_u16(p_round_in, step * 500, 0, 60000);
                        else if (ui_cursor == 2) a_round_in = ui_adjust_u16(a_round_in, step * 10, 0, 4095);
                        else if (ui_cursor == 3) p_round_out = ui_adjust_u16(p_round_out, step * 500, 0, 60000);
                    } else if (ui_page == PAGE_PARK) {
                        if      (ui_cursor == 0) a_park_1 = ui_adjust_u16(a_park_1, step * 10, 0, 4095);
                        else if (ui_cursor == 1) p_park_1 = ui_adjust_u16(p_park_1, step * 500, 0, 60000);
                        else if (ui_cursor == 2) p_park_2 = ui_adjust_u16(p_park_2, step * 50, 0, 60000);
                        else if (ui_cursor == 3) p_park_4 = ui_adjust_u16(p_park_4, step * 50, 0, 60000);
                        else if (ui_cursor == 4) d_park_3 = ui_adjust_int(d_park_3, step, 0, 20);
                    }
                    req.list_items = 1;
                }
                if (key == KEY5_PRESS) { ui_save_and_return((ui_page==PAGE_AVOID)?3:(ui_page==PAGE_ROUND?4:5)); }
                break;
            }

            case PAGE_SENSOR_ALL:
                if (key == KEY5_PRESS || key == KEY4_PRESS) { ui_page = PAGE_MAIN_MENU; ui_cursor = 6; scroll_top = 3; req.full_page = 1; }
                break;
        }
    }
    // ================== View rendering ==================
    if (!req.full_page && !req.cursor_move && !req.values_only && !req.list_items) return;
    if (req.full_page) {
        oled_clear();
        switch(ui_page) {
            case PAGE_HOME: ui_show_string(16, 0, "SMART CAR V2", 16); ui_show_string(24, 6, "[PUSH MENU]", 8); break;
            case PAGE_ROLE_SETUP: ui_show_string(0, 0, "SET IDENTITY:", 16); ui_show_string(20, 2, "CAR 1 (TX)", 16); ui_show_string(20, 4, "CAR 2 (RX)", 16); break;
            case PAGE_PID_PWM: ui_show_string(8, 0, "Trk_P:", 16); ui_show_string(8, 2, "Trk_D:", 16); break;
            case PAGE_SET_OFF: ui_show_string(8, 0, "SetOff Dly:", 16); break;
            default: break;
        }
        req.cursor_move = 1; req.values_only = 1; req.list_items = 1;
        last_screen_row = 255;
    }

    // --- List item render ---
    if (req.list_items && (ui_page == PAGE_MAIN_MENU || ui_page == PAGE_AVOID || ui_page == PAGE_ROUND || ui_page == PAGE_PARK)) {
        uint8_t total = (ui_page == PAGE_MAIN_MENU) ? 7 : (ui_page == PAGE_AVOID ? 7 : (ui_page == PAGE_ROUND ? 4 : 5));
        uint8_t i;
        for (i = 0; i < 4; i++) {
            uint8_t item_idx = scroll_top + i;
            ui_show_string(0, i * 2, "                ", 16);
            if (item_idx >= total) continue;

            if (ui_page == PAGE_MAIN_MENU) {
                if      (item_idx == 0) sprintf(buf, "1.Role Setup");
                else if (item_idx == 1) sprintf(buf, "2.PID Sets");
                else if (item_idx == 2) sprintf(buf, "3.SetOff(Go)");
                else if (item_idx == 3) sprintf(buf, "4.Avoid Obs");
                else if (item_idx == 4) sprintf(buf, "5.Roundabout");
                else if (item_idx == 5) sprintf(buf, "6.Park(Stop)");
                else if (item_idx == 6) sprintf(buf, "7.All Sensors");
            } else if (ui_page == PAGE_AVOID) {
                if      (item_idx == 0) sprintf(buf, "D_Avd: %-5u", d_avoid);
                else if (item_idx == 1) sprintf(buf, "P_Av1: %-5u", p_avoid_1);
                else if (item_idx == 2) sprintf(buf, "P_Av2: %-5u", p_avoid_2);
                else if (item_idx == 3) sprintf(buf, "P_Av3: %-5u", p_avoid_3);
                else if (item_idx == 4) sprintf(buf, "A_Av1: %-5u", a_avoid_1);
                else if (item_idx == 5) sprintf(buf, "A_Av2: %-5u", a_avoid_2);
                else if (item_idx == 6) sprintf(buf, "A_Av3: %-5u", a_avoid_3);
            } else if (ui_page == PAGE_ROUND) {
                if      (item_idx == 0) sprintf(buf, "D_RIn: %-5u", d_round_in);
                else if (item_idx == 1) sprintf(buf, "P_RIn: %-5u", p_round_in);
                else if (item_idx == 2) sprintf(buf, "A_RIn: %-5u", a_round_in);
                else if (item_idx == 3) sprintf(buf, "P_ROt: %-5u", p_round_out);
            } else if (ui_page == PAGE_PARK) {
                if      (item_idx == 0) sprintf(buf, "A_Pk1: %-5u", a_park_1);
                else if (item_idx == 1) sprintf(buf, "P_Pk1: %-5u", p_park_1);
                else if (item_idx == 2) sprintf(buf, "P_Pk2: %-5u", p_park_2);
                else if (item_idx == 3) sprintf(buf, "P_Pk4: %-5u", p_park_4);
                else if (item_idx == 4) sprintf(buf, "D_Pk3: %-5d", d_park_3);
            }
            ui_show_string(8, i * 2, buf, 16);
        }
        req.cursor_move = 1;
    }

    if (req.values_only || req.full_page) {
        if (ui_page == PAGE_HOME) { sprintf(buf, "ROLE: CAR %d", g_car_role + 1); ui_show_string(16, 3, buf, 16); }
        else if (ui_page == PAGE_PID_PWM) { sprintf(buf, "%-6.2f", track_PID.P); ui_show_string(64, 0, buf, 16); sprintf(buf, "%-6.2f", track_PID.D); ui_show_string(64, 2, buf, 16); }
        else if (ui_page == PAGE_SET_OFF) { sprintf(buf, "%-5d", set_off_count); ui_show_string(64, 2, buf, 16); }
        else if (ui_page == PAGE_SENSOR_ALL) {
            sprintf(buf, "Mag Cnt: %-5d", g_mag_cnt); ui_show_string(0, 0, buf, 16);
            sprintf(buf, "A0:%-4d A1:%-4d", adc_value[0], adc_value[1]); ui_show_string(0, 2, buf, 16);
            sprintf(buf, "A2:%-4d A3:%-4d", adc_value[2], adc_value[3]); ui_show_string(0, 4, buf, 16);
            sprintf(buf, "A4:%-4d     ", adc_value[4]); ui_show_string(0, 6, buf, 16);
        }
    }
    // --- Cursor marker render ---
    if (req.cursor_move || req.full_page || req.list_items) {
        uint8_t i;

        if (ui_page == PAGE_ROLE_SETUP) {
            ui_show_string(0, 2, " ", 16);
            ui_show_string(0, 4, " ", 16);
            ui_show_string(0, (uint8_t)((g_car_role + 1U) * 2U), ">", 16);
        } else if (ui_page == PAGE_PID_PWM) {
            ui_show_string(0, 0, " ", 16);
            ui_show_string(0, 2, " ", 16);
            ui_show_string(0, (uint8_t)(ui_cursor * 2U), ">", 16);
        } else if (ui_page == PAGE_SET_OFF) {
            ui_show_string(0, 0, ">", 16);
        } else if (ui_page == PAGE_MAIN_MENU || ui_page == PAGE_AVOID || ui_page == PAGE_ROUND || ui_page == PAGE_PARK) {
            for (i = 0; i < 4; i++) {
                uint8_t item_idx = scroll_top + i;
                ui_show_string(0, (uint8_t)(i * 2U), (item_idx == ui_cursor) ? ">" : " ", 16);
            }
        }
    }

    req.full_page = 0;
    req.list_items = 0;
    req.cursor_move = 0;
    req.values_only = 0;
}
