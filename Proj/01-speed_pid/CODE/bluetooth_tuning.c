#include "headfile.h"
#include "control.h"
#include "bluetooth_tuning.h"
#include <stdlib.h>
#include <stdio.h>

#define BT_CMD_BUFFER_SIZE             48U
#define BT_RX_BUFFER_SIZE              96U
#define BT_CMD_TIMEOUT_MS              200U

static uint8_t waveform_enabled = 1;
static uint8_t ack_enabled = 0;
static uint8_t bt_cmd_len = 0;
static uint64_t bt_last_rx_time_us = 0;
static char bt_cmd_buffer[BT_CMD_BUFFER_SIZE];
static volatile uint8_t bt_rx_head = 0;
static volatile uint8_t bt_rx_tail = 0;
static volatile uint8_t bt_rx_buffer[BT_RX_BUFFER_SIZE];

static void bluetooth_feed_rx_char(char rx_char);
static uint8_t bluetooth_read_rx_byte(char *rx_char);
static void bluetooth_push_rx_byte(uint8_t rx_data);
static void bluetooth_process_buffer(void);
static uint8_t bluetooth_buffer_ready(void);
static void bluetooth_send_str(const char *text);
static void bluetooth_send_fixed_2(const char *label, float value);
static void bluetooth_send_help(void);
static void bluetooth_send_status(void);
static void bluetooth_handle_command(char *command);
static void bluetooth_send_ack_target(const char *name, int16_t target);
static char bluetooth_to_upper(char ch);
static uint8_t bluetooth_starts_with(const char *text, const char *prefix);
static const char *bluetooth_trim_left(const char *text);
static const char *bluetooth_skip_value(const char *text);
static int16_t bluetooth_parse_i16(const char *text);
static int16_t bluetooth_limit_speed_target(int16_t target);
static uint16_t bluetooth_parse_u16(const char *text);
static float bluetooth_parse_float(const char *text);
static void bluetooth_parse_sine(const char *text);

void BluetoothTune_Init(void)
{
    bt_cmd_len = 0;
    bt_rx_head = 0;
    bt_rx_tail = 0;
    bt_last_rx_time_us = get_current_time_us();
    uart_interrupt_init(UART1, USART_INT_RBNE, 1, USART1_IRQn);
}

void BluetoothTune_Service(void)
{
    uint16_t rx_data;
    char rx_char;
    uint64_t now_us;

    while(bluetooth_read_rx_byte(&rx_char) != 0)
    {
        bluetooth_feed_rx_char(rx_char);
    }

    while(uart_query(UART1, &rx_data) != 0)
    {
        bluetooth_feed_rx_char((char)(rx_data & 0xFFU));
    }

    now_us = get_current_time_us();
    if((bt_cmd_len != 0) &&
       (bluetooth_buffer_ready() != 0) &&
       ((now_us - bt_last_rx_time_us) >= ((uint64_t)BT_CMD_TIMEOUT_MS * 1000ULL)))
    {
        bluetooth_process_buffer();
    }
}

void BluetoothTune_RxIrqHandler(void)
{
    while(usart_flag_get(UART1, USART_FLAG_RBNE) != RESET)
    {
        bluetooth_push_rx_byte((uint8_t)(usart_data_receive(UART1) & 0xFFU));
    }

    if(usart_flag_get(UART1, USART_FLAG_ORERR) != RESET)
    {
        (void)usart_data_receive(UART1);
    }
}

void BluetoothTune_SendWaveform(void)
{
    ControlData data;
    char text[96];

    if(waveform_enabled == 0)
    {
        return;
    }

    Control_GetData(&data);
    sprintf(text,
            "%d,%ld,%ld,%d,%ld,%ld\r\n",
            (int)data.target_left,
            (long)data.actual_left,
            (long)data.pwm_left,
            (int)data.target_right,
            (long)data.actual_right,
            (long)data.pwm_right);
    bluetooth_send_str(text);
}

static void bluetooth_feed_rx_char(char rx_char)
{
    uint8_t rx_value = (uint8_t)rx_char;

    bt_last_rx_time_us = get_current_time_us();

    if((rx_char == '\r') || (rx_char == '\n') || (rx_char == ';'))
    {
        bluetooth_process_buffer();
    }
    else if((rx_value < 0x20U) || (rx_value > 0x7EU))
    {
        bt_cmd_len = 0;
    }
    else if(bt_cmd_len < (BT_CMD_BUFFER_SIZE - 1U))
    {
        bt_cmd_buffer[bt_cmd_len] = rx_char;
        bt_cmd_len++;
    }
    else
    {
        bt_cmd_len = 0;
    }
}

static uint8_t bluetooth_read_rx_byte(char *rx_char)
{
    uint8_t tail = bt_rx_tail;

    if(tail == bt_rx_head)
    {
        return 0;
    }

    *rx_char = (char)bt_rx_buffer[tail];
    tail++;
    if(tail >= BT_RX_BUFFER_SIZE)
    {
        tail = 0;
    }
    bt_rx_tail = tail;
    return 1;
}

static void bluetooth_push_rx_byte(uint8_t rx_data)
{
    uint8_t next_head = bt_rx_head + 1U;

    if(next_head >= BT_RX_BUFFER_SIZE)
    {
        next_head = 0;
    }

    if(next_head == bt_rx_tail)
    {
        bt_rx_tail++;
        if(bt_rx_tail >= BT_RX_BUFFER_SIZE)
        {
            bt_rx_tail = 0;
        }
    }

    bt_rx_buffer[bt_rx_head] = rx_data;
    bt_rx_head = next_head;
}

static void bluetooth_process_buffer(void)
{
    if(bt_cmd_len == 0)
    {
        return;
    }

    bt_cmd_buffer[bt_cmd_len] = '\0';
    bluetooth_handle_command(bt_cmd_buffer);
    bt_cmd_len = 0;
}

static uint8_t bluetooth_buffer_ready(void)
{
    uint8_t i;

    for(i = 0; i < bt_cmd_len; i++)
    {
        if(((bt_cmd_buffer[i] >= '0') && (bt_cmd_buffer[i] <= '9')) ||
           (bt_cmd_buffer[i] == '-') ||
           (bt_cmd_buffer[i] == '+') ||
           (bt_cmd_buffer[i] == '?'))
        {
            return 1;
        }
    }

    if((bt_cmd_len >= 4) &&
       (bluetooth_to_upper(bt_cmd_buffer[0]) == 'S') &&
       (bluetooth_to_upper(bt_cmd_buffer[1]) == 'T') &&
       (bluetooth_to_upper(bt_cmd_buffer[2]) == 'O') &&
       (bluetooth_to_upper(bt_cmd_buffer[3]) == 'P'))
    {
        return 1;
    }

    if((bt_cmd_len >= 4) &&
       (bluetooth_to_upper(bt_cmd_buffer[0]) == 'H') &&
       (bluetooth_to_upper(bt_cmd_buffer[1]) == 'E') &&
       (bluetooth_to_upper(bt_cmd_buffer[2]) == 'L') &&
       (bluetooth_to_upper(bt_cmd_buffer[3]) == 'P'))
    {
        return 1;
    }

    return 0;
}

static void bluetooth_send_help(void)
{
    bluetooth_send_str("CMD:P=45 I=2 D=8 S=20 L=20 R=20 SIN=20,10,2000 WAVE=1 STOP ?\r\n");
    bluetooth_send_str("CSV:LT,LV,LPWM,RT,RV,RPWM\r\n");
    bluetooth_send_status();
}

static void bluetooth_send_status(void)
{
    char text[112];
    ControlPidParam pid;
    ControlData data;

    Control_GetPid(&pid);
    Control_GetData(&data);

    bluetooth_send_fixed_2("P=", pid.kp);
    bluetooth_send_str(",");
    bluetooth_send_fixed_2("I=", pid.ki);
    bluetooth_send_str(",");
    bluetooth_send_fixed_2("D=", pid.kd);
    bluetooth_send_str(",");
    sprintf(text,
            "TL=%d,TR=%d,VL=%ld,VR=%ld,PL=%ld,PR=%ld,RUN=%u,SIN=%u,WAVE=%u\r\n",
            (int)data.target_left,
            (int)data.target_right,
            (long)data.actual_left,
            (long)data.actual_right,
            (long)data.pwm_left,
            (long)data.pwm_right,
            (unsigned int)data.enabled,
            (unsigned int)data.sine_enabled,
            (unsigned int)waveform_enabled);
    bluetooth_send_str(text);
}

static void bluetooth_handle_command(char *command)
{
    const char *cmd = bluetooth_trim_left(command);
    ControlPidParam pid;

    if(*cmd == '\0')
    {
        return;
    }

    if((*cmd == '?') || (bluetooth_starts_with(cmd, "HELP") != 0))
    {
        bluetooth_send_help();
    }
    else if(bluetooth_starts_with(cmd, "STOP") != 0)
    {
        Control_Stop();
    }
    else if(bluetooth_starts_with(cmd, "KP") != 0)
    {
        Control_GetPid(&pid);
        Control_SetPid(bluetooth_parse_float(cmd + 2), pid.ki, pid.kd);
    }
    else if(bluetooth_starts_with(cmd, "KI") != 0)
    {
        Control_GetPid(&pid);
        Control_SetPid(pid.kp, bluetooth_parse_float(cmd + 2), pid.kd);
    }
    else if(bluetooth_starts_with(cmd, "KD") != 0)
    {
        Control_GetPid(&pid);
        Control_SetPid(pid.kp, pid.ki, bluetooth_parse_float(cmd + 2));
    }
    else if(bluetooth_starts_with(cmd, "SIN") != 0)
    {
        bluetooth_parse_sine(cmd + 3);
    }
    else if(bluetooth_starts_with(cmd, "WAVE") != 0)
    {
        waveform_enabled = (bluetooth_parse_i16(cmd + 4) != 0) ? 1U : 0U;
    }
    else if(bluetooth_starts_with(cmd, "ACK") != 0)
    {
        ack_enabled = (bluetooth_parse_i16(cmd + 3) != 0) ? 1U : 0U;
        if(ack_enabled != 0)
        {
            bluetooth_send_str("ACK,ON\r\n");
        }
    }
    else if(bluetooth_to_upper(cmd[0]) == 'P')
    {
        Control_GetPid(&pid);
        Control_SetPid(bluetooth_parse_float(cmd + 1), pid.ki, pid.kd);
    }
    else if(bluetooth_to_upper(cmd[0]) == 'I')
    {
        Control_GetPid(&pid);
        Control_SetPid(pid.kp, bluetooth_parse_float(cmd + 1), pid.kd);
    }
    else if(bluetooth_to_upper(cmd[0]) == 'D')
    {
        Control_GetPid(&pid);
        Control_SetPid(pid.kp, pid.ki, bluetooth_parse_float(cmd + 1));
    }
    else if(bluetooth_to_upper(cmd[0]) == 'S')
    {
        int16_t target = bluetooth_limit_speed_target(bluetooth_parse_i16(cmd + 1));
        Control_SetSpeedTarget(target, target);
        bluetooth_send_ack_target("S", target);
    }
    else if(bluetooth_to_upper(cmd[0]) == 'L')
    {
        int16_t target = bluetooth_limit_speed_target(bluetooth_parse_i16(cmd + 1));
        Control_SetLeftTarget(target);
        bluetooth_send_ack_target("L", target);
    }
    else if(bluetooth_to_upper(cmd[0]) == 'R')
    {
        int16_t target = bluetooth_limit_speed_target(bluetooth_parse_i16(cmd + 1));
        Control_SetRightTarget(target);
        bluetooth_send_ack_target("R", target);
    }
    else if(((cmd[0] >= '0') && (cmd[0] <= '9')) || (cmd[0] == '-') || (cmd[0] == '+'))
    {
        int16_t target = bluetooth_limit_speed_target(bluetooth_parse_i16(cmd));
        Control_SetSpeedTarget(target, target);
        bluetooth_send_ack_target("S", target);
    }
}

static void bluetooth_send_ack_target(const char *name, int16_t target)
{
    char text[32];

    if(ack_enabled == 0)
    {
        return;
    }

    sprintf(text, "ACK,%s,%d\r\n", name, (int)target);
    bluetooth_send_str(text);
}

static void bluetooth_send_str(const char *text)
{
    while(*text != '\0')
    {
        uart_send_char(UART1, (unsigned char)*text);
        text++;
    }
}

static void bluetooth_send_fixed_2(const char *label, float value)
{
    char text[24];
    long scaled;
    long whole;
    long frac;

    scaled = (long)((value * 100.0f) + ((value >= 0.0f) ? 0.5f : -0.5f));
    whole = scaled / 100L;
    frac = scaled % 100L;
    if(frac < 0)
    {
        frac = -frac;
    }

    sprintf(text, "%s%ld.%02ld", label, whole, frac);
    bluetooth_send_str(text);
}

static char bluetooth_to_upper(char ch)
{
    if((ch >= 'a') && (ch <= 'z'))
    {
        return (char)(ch - ('a' - 'A'));
    }

    return ch;
}

static uint8_t bluetooth_starts_with(const char *text, const char *prefix)
{
    while(*prefix != '\0')
    {
        if(bluetooth_to_upper(*text) != *prefix)
        {
            return 0;
        }

        text++;
        prefix++;
    }

    return 1;
}

static const char *bluetooth_trim_left(const char *text)
{
    while((*text == ' ') || (*text == '\t'))
    {
        text++;
    }

    return text;
}

static const char *bluetooth_skip_value(const char *text)
{
    while((*text != '\0') &&
          !((*text >= '0') && (*text <= '9')) &&
          (*text != '-') &&
          (*text != '+') &&
          (*text != '.'))
    {
        text++;
    }

    return text;
}

static int16_t bluetooth_parse_i16(const char *text)
{
    long value = atol(bluetooth_skip_value(text));

    if(value > 32767L)
    {
        value = 32767L;
    }
    else if(value < -32768L)
    {
        value = -32768L;
    }

    return (int16_t)value;
}

static int16_t bluetooth_limit_speed_target(int16_t target)
{
    if(target > 300)
    {
        return 300;
    }

    if(target < -300)
    {
        return -300;
    }

    return target;
}

static uint16_t bluetooth_parse_u16(const char *text)
{
    long value = atol(bluetooth_skip_value(text));

    if(value < 0L)
    {
        value = 0L;
    }
    else if(value > 65535L)
    {
        value = 65535L;
    }

    return (uint16_t)value;
}

static float bluetooth_parse_float(const char *text)
{
    return (float)atof(bluetooth_skip_value(text));
}

static void bluetooth_parse_sine(const char *text)
{
    const char *amp_text;
    const char *period_text;
    int16_t base;
    int16_t amplitude;
    uint16_t period_ms;

    text = bluetooth_skip_value(text);
    base = bluetooth_parse_i16(text);

    amp_text = text;
    while((*amp_text != '\0') && (*amp_text != ','))
    {
        amp_text++;
    }
    if(*amp_text == ',')
    {
        amp_text++;
    }

    amplitude = bluetooth_parse_i16(amp_text);

    period_text = amp_text;
    while((*period_text != '\0') && (*period_text != ','))
    {
        period_text++;
    }
    if(*period_text == ',')
    {
        period_text++;
    }

    period_ms = bluetooth_parse_u16(period_text);
    if(period_ms == 0U)
    {
        period_ms = 2000U;
    }

    Control_SetSineTarget(base, amplitude, period_ms);
}
