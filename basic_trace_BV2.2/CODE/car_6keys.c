/*********************************************************************************************************************
* @file            car_6keys.c
* @brief           Non-blocking 6-key scanner for the debug demo
* @Target core     GD32F303RCT6
********************************************************************************************************************/

#include "car_6keys.h"

typedef struct
{
    unsigned char key_num;
    unsigned char key_value;
    uint32_t port;
    uint32_t pin;
    uint8_t stable_level;
    uint8_t last_raw_level;
    uint8_t debounce_ticks;
    uint8_t reported;
} key_scan_t;

static key_scan_t key_table[] =
{
    {K1, 0x01, K1_PORT, K1_PIN, KEYUP, KEYUP, 0, 0},
    {K2, 0x02, K2_PORT, K2_PIN, KEYUP, KEYUP, 0, 0},
    {K3, 0x03, K3_PORT, K3_PIN, KEYUP, KEYUP, 0, 0},
    {K4, 0x04, K4_PORT, K4_PIN, KEYUP, KEYUP, 0, 0},
    {K5, 0x05, K5_PORT, K5_PIN, KEYUP, KEYUP, 0, 0},
    {K6, 0x06, K6_PORT, K6_PIN, KEYUP, KEYUP, 0, 0},
};

static uint8_t key_read_level(uint32_t port, uint32_t pin)
{
    return (KEYDOWN == gpio_get_input(port, pin)) ? KEYDOWN : KEYUP;
}

void key_init(unsigned char key_num)
{
    unsigned int i;

    for(i = 0; i < (sizeof(key_table) / sizeof(key_table[0])); i++)
    {
        if(key_table[i].key_num == key_num)
        {
            gpio_pin_init(key_table[i].port, IN_PULLUP, key_table[i].pin);
            key_table[i].stable_level = KEYUP;
            key_table[i].last_raw_level = KEYUP;
            key_table[i].debounce_ticks = 0;
            key_table[i].reported = 0;
            break;
        }
    }
}

unsigned char key_check(void)
{
    unsigned int i;

    for(i = 0; i < (sizeof(key_table) / sizeof(key_table[0])); i++)
    {
        key_scan_t *key = &key_table[i];
        uint8_t raw_level = key_read_level(key->port, key->pin);

        if(raw_level != key->last_raw_level)
        {
            key->last_raw_level = raw_level;
            key->debounce_ticks = 0;
            continue;
        }

        if(key->debounce_ticks < KEY_DEBOUNCE_TICKS)
        {
            key->debounce_ticks++;
            continue;
        }

        if(raw_level != key->stable_level)
        {
            key->stable_level = raw_level;

            if(raw_level == KEYUP)
            {
                key->reported = 0;
                continue;
            }
        }

        if((key->stable_level == KEYDOWN) && (key->reported == 0))
        {
            key->reported = 1;
            return key->key_value;
        }
    }

    return 0xFF;
}
