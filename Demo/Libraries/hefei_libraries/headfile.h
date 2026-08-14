/*********************************************************************************************************************
* @file            headfile.h
* @author          
* @Target core     GD32F303
* @revisions       2022.09.02, V1.0
* @modify          none
********************************************************************************************************************/
 
#ifndef _HEADFILE_H
#define _HEADFILE_H

#include "gd32f30x.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>

//------MCU外设驱动头文件
#include "hf_adc.h"
#include "hf_exti.h"
#include "hf_flash.h"
#include "hf_gpio.h"
#include "hf_pit.h"
#include "hf_pwm.h"
#include "hf_qtimer.h"
#include "hf_uart.h"
#include "hf_i2c_hardware.h"
#include "hf_i2c_software.h"
#include "hf_systick.h"
#include "hf_spi.h"
#include "car_oled.h"
#include "car_light.h"

//------外设驱动头文件
#include "HF_Double_DC_Motor.h"
#include "HF_WS2812b.h"
#include "HF_HC-SR04.h"
#include "HF_OLED.h"
#include "dmx_tof400c.h"
#include "encoder.h"

//------智能车驱动头文件
#include "board.h"
#include "car_init.h"
#include "car_motion.h"
#include "car_light.h"
#include "car_uart.h"
#include "car_buzzer.h"
#include "car_6keys.h"
#include "car_adc.h"
#include "car_reed.h"
#include "car_value_process.h"
#include "xunxian.h"
//自己写的函数头文件
//#include "Control.h"

#endif
