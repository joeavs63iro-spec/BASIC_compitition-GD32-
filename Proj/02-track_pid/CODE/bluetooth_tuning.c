#include "headfile.h"
#include "control.h"
#include "bluetooth_tuning.h"
#include <stdlib.h>
#include <stdio.h>

#define BT_CMD_BUFFER_SIZE  48U
#define BT_RX_BUFFER_SIZE   96U
#define BT_CMD_TIMEOUT_MS   200U

static uint8_t bt_cmd_len = 0U;
static uint64_t bt_last_rx_time_us = 0U;
static char bt_cmd_buffer[BT_CMD_BUFFER_SIZE];
static volatile uint8_t bt_rx_head = 0U;
static volatile uint8_t bt_rx_tail = 0U;
static volatile uint8_t bt_rx_buffer[BT_RX_BUFFER_SIZE];

static void bluetooth_feed_rx_char(char rx_char);
static uint8_t bluetooth_read_rx_byte(char *rx_char);
static void bluetooth_push_rx_byte(uint8_t rx_data);
static void bluetooth_process_buffer(void);
static uint8_t bluetooth_buffer_ready(void);
static void bluetooth_send_str(const char *text);
static void bluetooth_send_fixed_2(const char *label, float value);
static void bluetooth_send_status(void);
static void bluetooth_send_help(void);
static void bluetooth_handle_command(char *command);
static char bluetooth_to_upper(char ch);
static uint8_t bluetooth_starts_with(const char *text, const char *prefix);
static const char *bluetooth_trim_left(const char *text);
static const char *bluetooth_skip_value(const char *text);
static float bluetooth_parse_float(const char *text);

void BluetoothTune_Init(void)
{
    bt_cmd_len = 0U;
    bt_rx_head = 0U;
    bt_rx_tail = 0U;
    bt_last_rx_time_us = get_current_time_us();
    uart_interrupt_init(UART1, USART_INT_RBNE, 1, USART1_IRQn);
}

void BluetoothTune_Service(void)
{
    char rx_char;
    uint64_t now_us;

    while(bluetooth_read_rx_byte(&rx_char) != 0U)
    {
        bluetooth_feed_rx_char(rx_char);
    }

    now_us = get_current_time_us();
    if((bt_cmd_len != 0U) &&
       (bluetooth_buffer_ready() != 0U) &&
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
        bt_cmd_len = 0U;
    }
    else if(bt_cmd_len < (BT_CMD_BUFFER_SIZE - 1U))
    {
        bt_cmd_buffer[bt_cmd_len] = rx_char;
        bt_cmd_len++;
    }
    else
    {
        bt_cmd_len = 0U;
    }
}

static uint8_t bluetooth_read_rx_byte(char *rx_char)
{
    uint8_t tail = bt_rx_tail;

    if(tail == bt_rx_head)
    {
        return 0U;
    }

    *rx_char = (char)bt_rx_buffer[tail];
    tail++;
    if(tail >= BT_RX_BUFFER_SIZE)
    {
        tail = 0U;
    }
    bt_rx_tail = tail;
    return 1U;
}

static void bluetooth_push_rx_byte(uint8_t rx_data)
{
    uint8_t next_head = bt_rx_head + 1U;

    if(next_head >= BT_RX_BUFFER_SIZE)
    {
        next_head = 0U;
    }

    if(next_head == bt_rx_tail)
    {
        bt_rx_tail++;
        if(bt_rx_tail >= BT_RX_BUFFER_SIZE)
        {
            bt_rx_tail = 0U;
        }
    }

    bt_rx_buffer[bt_rx_head] = rx_data;
    bt_rx_head = next_head;
}

static void bluetooth_process_buffer(void)
{
    if(bt_cmd_len == 0U)
    {
        return;
    }

    bt_cmd_buffer[bt_cmd_len] = '\0';
    bluetooth_handle_command(bt_cmd_buffer);
    bt_cmd_len = 0U;
}

static uint8_t bluetooth_buffer_ready(void)
{
    uint8_t i;

    for(i = 0U; i < bt_cmd_len; i++)
    {
        if(((bt_cmd_buffer[i] >= '0') && (bt_cmd_buffer[i] <= '9')) ||
           (bt_cmd_buffer[i] == '-') ||
           (bt_cmd_buffer[i] == '+') ||
           (bt_cmd_buffer[i] == '?'))
        {
            return 1U;
        }
    }

    if((bt_cmd_len >= 4U) &&
       (bluetooth_to_upper(bt_cmd_buffer[0]) == 'H') &&
       (bluetooth_to_upper(bt_cmd_buffer[1]) == 'E') &&
       (bluetooth_to_upper(bt_cmd_buffer[2]) == 'L') &&
       (bluetooth_to_upper(bt_cmd_buffer[3]) == 'P'))
    {
        return 1U;
    }

    if((bt_cmd_len >= 3U) &&
       (bluetooth_to_upper(bt_cmd_buffer[0]) == 'S') &&
       (bluetooth_to_upper(bt_cmd_buffer[1]) == 'T') &&
       (bluetooth_to_upper(bt_cmd_buffer[2]) == 'A'))
    {
        return 1U;
    }

    return 0U;
}

static void bluetooth_send_help(void)
{
    bluetooth_send_str("CMD:P=1.90 D=2.80 KP=1.90 KD=2.80 ?\r\n");
    bluetooth_send_status();
}

static void bluetooth_send_status(void)
{
    char text[128];

    bluetooth_send_fixed_2("P=", g_track_control.kp);
    bluetooth_send_str(",");
    bluetooth_send_fixed_2("D=", g_track_control.kd);
    bluetooth_send_str(",");
    sprintf(text,
            "DUTY=%u,L=%u,R=%u,ERR=%ld,OUT=%ld,RUN=%u,LOST=%u\r\n",
            (unsigned int)g_track_control.base_speed,
            (unsigned int)g_track_control.left_pwm,
            (unsigned int)g_track_control.right_pwm,
            (long)g_track_control.error,
            (long)g_track_control.output,
            (unsigned int)g_track_control.run_enable,
            (unsigned int)g_track_control.lost_flag);
    bluetooth_send_str(text);
}

static void bluetooth_handle_command(char *command)
{
    const char *cmd = bluetooth_trim_left(command);

    if(*cmd == '\0')
    {
        return;
    }

    if((*cmd == '?') || (bluetooth_starts_with(cmd, "HELP") != 0) || (bluetooth_starts_with(cmd, "STATUS") != 0))
    {
        bluetooth_send_help();
    }
    else if(bluetooth_starts_with(cmd, "KP") != 0)
    {
        Track_SetPD(bluetooth_parse_float(cmd + 2), g_track_control.kd);
        bluetooth_send_status();
    }
    else if(bluetooth_starts_with(cmd, "KD") != 0)
    {
        Track_SetPD(g_track_control.kp, bluetooth_parse_float(cmd + 2));
        bluetooth_send_status();
    }
    else if(bluetooth_to_upper(cmd[0]) == 'P')
    {
        Track_SetPD(bluetooth_parse_float(cmd + 1), g_track_control.kd);
        bluetooth_send_status();
    }
    else if(bluetooth_to_upper(cmd[0]) == 'D')
    {
        Track_SetPD(g_track_control.kp, bluetooth_parse_float(cmd + 1));
        bluetooth_send_status();
    }
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
    if(frac < 0L)
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
            return 0U;
        }

        text++;
        prefix++;
    }

    return 1U;
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

static float bluetooth_parse_float(const char *text)
{
    return (float)atof(bluetooth_skip_value(text));
}
