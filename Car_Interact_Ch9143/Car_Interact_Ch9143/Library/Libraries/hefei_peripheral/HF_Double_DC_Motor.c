/*********************************************************************************************************************
* @file            HF_Double_DC_Motor.c
* @author          
* @Target core     GD32F303RCT6
* @revisions       2023.4.2, V2.0
* @modify          none
********************************************************************************************************************/

#include "HF_Double_DC_Motor.h"

//-------------------------------------------------------------------------------------------------------------------
// @brief        PWM初始化
// @param        object            控制对象
// @return       void
// Sample usage:        motor_init(left); 
                        //初始化左电机PWM
//-------------------------------------------------------------------------------------------------------------------
void motor_init(left_or_right object)
{
    if(object == left)
    {
        gpio_pin_init(L_PWM1_PORT, L_PWM1_MODE, L_PWM1_PIN);
        gpio_pin_init(L_PWM2_PORT, L_PWM2_MODE, L_PWM2_PIN);

        /* 复位外设TIMER */
        timer_deinit(TIMER1);
        /*
         * Motor PWM frequency is set here:
         * 120MHz / MOTOR_PWM_FREQ_DIV / MOTOR_PWM_PERIOD = 10kHz.
         */
        pwm_init(L_PWM1_TIMER, L_PWM1_TIMER_CH, MOTOR_PWM_FREQ_DIV, MOTOR_PWM_PERIOD, 0);
        pwm_init(L_PWM2_TIMER, L_PWM2_TIMER_CH, MOTOR_PWM_FREQ_DIV, MOTOR_PWM_PERIOD, 0);
    }
    else if(object == right)
    {
        gpio_pin_init(R_PWM1_PORT, R_PWM1_MODE, R_PWM1_PIN);
        gpio_pin_init(R_PWM2_PORT, R_PWM2_MODE, R_PWM2_PIN);

        /* 复位外设TIMER */
        timer_deinit(TIMER2);
        /*
         * Keep both motor sides on the same 10kHz PWM base.
         */
        pwm_init(R_PWM1_TIMER, R_PWM1_TIMER_CH, MOTOR_PWM_FREQ_DIV, MOTOR_PWM_PERIOD, 0);
        pwm_init(R_PWM2_TIMER, R_PWM2_TIMER_CH, MOTOR_PWM_FREQ_DIV, MOTOR_PWM_PERIOD, 0);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        控制电机正转
// @param        object            控制对象
// @param        value             PWM占空比(0-10000)
// @return       void
// Sample usage:        motor_forward(right, 10000);
                        //右电机满速正转(10000/10000)
//-------------------------------------------------------------------------------------------------------------------
void motor_forward(left_or_right object, uint16_t value)
{
    value = (value > MOTOR_MAX_SPEED) ? MOTOR_MAX_SPEED : value;
    if(object == left)
    {
        pwm_duty(L_PWM2_TIMER, L_PWM2_TIMER_CH, 0);
        pwm_duty(L_PWM1_TIMER, L_PWM1_TIMER_CH, value);
    }
    else if(object == right)
    {
        pwm_duty(R_PWM1_TIMER, R_PWM1_TIMER_CH, 0);
        pwm_duty(R_PWM2_TIMER, R_PWM2_TIMER_CH, value);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief        控制电机反转
// @param        object            控制对象
// @param        value             PWM占空比(0-10000)
// @return       void
// Sample usage:        motor_backward(left, 10000)；
                        //左电机满速反转(10000/10000)
//-------------------------------------------------------------------------------------------------------------------
void motor_backward(left_or_right object, uint16_t value)
{
    value = (value > MOTOR_MAX_SPEED) ? MOTOR_MAX_SPEED : value;
    if(object == left)
    {
        pwm_duty(L_PWM2_TIMER, L_PWM2_TIMER_CH, value);
        pwm_duty(L_PWM1_TIMER, L_PWM1_TIMER_CH, 0);
    }
    else if(object == right)
    {
        pwm_duty(R_PWM1_TIMER, R_PWM1_TIMER_CH, value);
        pwm_duty(R_PWM2_TIMER, R_PWM2_TIMER_CH, 0);
    }
}

void motor_set(uint8_t motor_num, Motor_Dir dir, int speed) {
    // 速度限幅处理
    if(speed < 0)
    {
        speed = 0;
    }
    else if(speed > MOTOR_MAX_SPEED)
    {
        speed = MOTOR_MAX_SPEED;
    }
    uint16_t duty = (uint16_t)((speed * MOTOR_PWM_RES) / MOTOR_MAX_SPEED);
    
    switch(motor_num) {
        case 1: // 电机1控制
            switch(dir) {
							case MOTOR_STOP:
								   motor_forward(left,0);
									 motor_backward(left,0);
                    break;
                case MOTOR_CW:
										motor_forward(left,duty);
                    break;
                case MOTOR_CCW:
										motor_backward(left,duty);
                    break;
                default:
                   motor_forward(left,0);
									 motor_backward(left,0);
            }
            break;
            
        case 2: // 电机2控制
            switch(dir) {
							case MOTOR_STOP:
								   motor_forward(right,0);
									 motor_backward(right,0);
                    break;
                case MOTOR_CW:
										motor_forward(right,duty);
                    break;
                case MOTOR_CCW:
										motor_backward(right,duty);
                    break;
                default:
                   motor_forward(right,0);
									 motor_backward(right,0);
            }
            break;
    }
}

void motor_set_pwm(int16_t left_pwm, int16_t right_pwm)
{
    uint16_t max_duty =10000 ;
    if (left_pwm > 0)
    {
        if(left_pwm>max_duty)
        {
            left_pwm=max_duty;
        }
        motor_forward(left, left_pwm);
    }
    else if (left_pwm < 0)
    {
        if(left_pwm<-max_duty)
        {
            left_pwm=-max_duty;
        }
        motor_backward(left, left_pwm);
    }
    else
    {
        motor_forward(left, 0);
			  motor_forward(right, 0);
    }

    if (right_pwm > 0)
        {
            if(right_pwm>max_duty)
            {
                right_pwm=max_duty;
            }
            motor_forward(right, right_pwm);
        }
        else if (right_pwm < 0)
        {
            if(right_pwm<-max_duty)
            {
                right_pwm=-max_duty;
            }
            motor_backward(right, right_pwm);
        }
        else
        {
					motor_forward(left, 0);
					motor_forward(right, 0);
        }
}

