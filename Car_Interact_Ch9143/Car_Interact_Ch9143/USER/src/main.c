/*********************************************************************************************************************
* @file            main.c
* @brief           CH9143 backend for old dual-car message logic
* @Target core     GD32F303RCT6
********************************************************************************************************************/

#include "main.h"
#include "headfile.h"
#include <string.h>

#define CH9143_UART             USART1
#define CH9143_UART_IRQ         USART1_IRQn
#define CH9143_UART_BAUD        115200
#define CH9143_UART_TX_PORT     GPIOA
#define CH9143_UART_TX_PIN      GPIO_PIN_2
#define CH9143_UART_RX_PORT     GPIOA
#define CH9143_UART_RX_PIN      GPIO_PIN_3

#define BLE_BUF_SIZE            64
#define BLE_NO_DATA             0xFFFFU

/* 1: same side as old new_racetrackV3.2, 2: same side as old basic_campus12. */
uint8_t g_car_role = 1;

char send_buffer[BLE_BUF_SIZE] = {0};
char recv_buffer[BLE_BUF_SIZE] = {0};
uint16_t recv_data_flag = 0;

static volatile char ch9143_rx_buffer[BLE_BUF_SIZE] = {0};
static volatile uint8_t ch9143_rx_len = 0;
static volatile uint8_t ch9143_packet_ready = 0;
static uint32_t ble_send_timer = 0;
static uint8_t last_key_value = 0xFF;

extern volatile uint32_t g_millis;

static void ble_show(const char *line1, const char *line2)
{
    oled_clear();
    oled_show_string(0, 0, (uint8_t *)line1, 16);
    oled_show_string(0, 2, (uint8_t *)line2, 16);
}

static void set_send_buffer(const char *msg)
{
    strncpy(send_buffer, msg, BLE_BUF_SIZE - 1U);
    send_buffer[BLE_BUF_SIZE - 1U] = '\0';
}

/* Old AI-WB2 send API, now backed by CH9143 UART transparent transmission. */
void ai_wb2_01s_ble_send_data(char *data, int len)
{
    if (data == 0 || data[0] == '\0')
    {
        return;
    }

    if (len < 0)
    {
        len = (int)strlen(data);
    }

    while (len-- > 0)
    {
        uart_putchar(CH9143_UART, (uint8_t)*data++);
    }
}

/* Called only by USART1_IRQHandler. A valid old message is 4 bytes: 0x01..0x04. */
void ai_wb2_01s_ch9143_rx_byte(uint8_t data)
{
    if (data == '\r' || data == '\n')
    {
        return;
    }

    if (ch9143_packet_ready)
    {
        return;
    }

    if (ch9143_rx_len >= (BLE_BUF_SIZE - 1U))
    {
        ch9143_rx_len = 0;
        memset((void *)ch9143_rx_buffer, 0, BLE_BUF_SIZE);
    }

    ch9143_rx_buffer[ch9143_rx_len++] = (char)data;
    ch9143_rx_buffer[ch9143_rx_len] = '\0';

    if (ch9143_rx_len >= 4U)
    {
        ch9143_packet_ready = 1;
    }
}

/* Old AI-WB2 recv API, now backed by the CH9143 UART RX buffer. */
uint16_t ai_wb2_01s_ble_recv_data(char *out_buffer)
{
    uint8_t i;
    uint8_t len;

    if (!ch9143_packet_ready)
    {
        return BLE_NO_DATA;
    }

    len = ch9143_rx_len;
    if (len >= BLE_BUF_SIZE)
    {
        len = BLE_BUF_SIZE - 1U;
    }

    for (i = 0; i < len; i++)
    {
        out_buffer[i] = (char)ch9143_rx_buffer[i];
    }
    out_buffer[len] = '\0';

    ch9143_rx_len = 0;
    ch9143_packet_ready = 0;
    memset((void *)ch9143_rx_buffer, 0, BLE_BUF_SIZE);

    return len;
}

static uint8_t recv_has(const char *long_token, const char *short_token)
{
    return (strstr(recv_buffer, long_token) != 0 || strstr(recv_buffer, short_token) != 0);
}

static void set_car_role(uint8_t role)
{
    g_car_role = role;

    if (g_car_role == 1U)
    {
        set_send_buffer("0x01");
        ble_show("ROLE", "CAR1");
    }
    else
    {
        g_car_role = 2U;
        set_send_buffer("0x03");
        ble_show("ROLE", "CAR2");
    }

    ble_send_timer = g_millis;
}

static void manual_set_message(uint8_t key_value)
{
    switch (key_value)
    {
        case 0x01:
            set_send_buffer("0x01");
            break;
        case 0x02:
            set_send_buffer("0x02");
            break;
        case 0x03:
            set_send_buffer("0x03");
            break;
        case 0x04:
            set_send_buffer("0x04");
            break;
        case 0x05:
            set_car_role(1U);
            return;
        case 0x06:
            set_car_role(2U);
            return;
        default:
            return;
    }

    ai_wb2_01s_ble_send_data(send_buffer, -1);
    ble_send_timer = g_millis;
    ble_show("TX", send_buffer);
}

static void old_logic_send_task(void)
{
    uint32_t period_ms;

    if (send_buffer[0] == '\0')
    {
        return;
    }

    /* Same periods as the old two projects: car1 500 ms, car2 150 ms. */
    period_ms = (g_car_role == 1U) ? 500U : 150U;

    if ((uint32_t)(g_millis - ble_send_timer) >= period_ms)
    {
        ai_wb2_01s_ble_send_data(send_buffer, -1);
        ble_send_timer = g_millis;
    }
}

static void old_logic_recv_task(void)
{
    recv_data_flag = ai_wb2_01s_ble_recv_data(recv_buffer);

    if ((recv_data_flag == BLE_NO_DATA) || (recv_data_flag == 0U))
    {
        return;
    }

    printf("%s\r\n", recv_buffer);

    if (g_car_role == 1U)
    {
        if (recv_has("0x01", "01"))
        {
            ble_show("RX CAR1", "0x01");
        }
        else if (recv_has("0x03", "03"))
        {
            ble_show("RX CAR1", "0x03");
        }
        else if (recv_has("0x04", "04"))
        {
            ble_show("RX CAR1", "0x04");
        }
    }
    else
    {
        if (recv_has("0x01", "01"))
        {
            ble_show("RX CAR2", "0x01");
        }
        else if (recv_has("0x02", "02"))
        {
            ble_show("RX CAR2", "0x02");
        }
        else if (recv_has("0x04", "04"))
        {
            ble_show("RX CAR2", "0x04");
        }
        else if (recv_has("0x03", "03"))
        {
            ble_show("RX CAR2", "0x03");
        }
    }

    memset(recv_buffer, 0, sizeof(recv_buffer));
}

int main(void)
{
    unsigned char key_value;

    car_init();
    car_both_rgb_on(green, 50);

    uart_init(CH9143_UART_TX_PORT, CH9143_UART_TX_PIN,
              CH9143_UART_RX_PORT, CH9143_UART_RX_PIN,
              CH9143_UART, CH9143_UART_BAUD);
    uart_interrupt_init(CH9143_UART, USART_INT_RBNE, 1, CH9143_UART_IRQ);

    set_car_role(1U);

    while (1)
    {
        key_value = key_check();
        if (key_value != 0xFF && key_value != last_key_value)
        {
            manual_set_message(key_value);
            last_key_value = key_value;
        }
        else if (key_value == 0xFF)
        {
            last_key_value = 0xFF;
        }

        old_logic_send_task();
        old_logic_recv_task();
    }
}
