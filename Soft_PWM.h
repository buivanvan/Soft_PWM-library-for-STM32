/**
 * @file    soft_pwm.h
 * @brief   Software PWM driver using timer interrupt (bare-metal, no HAL)
 *
 * @details
 * This library implements Software PWM on STM32 microcontrollers
 * using a hardware timer interrupt. It allows generating multiple
 * PWM channels on any GPIO pins without using hardware PWM peripherals.
 *
 * Features:
 *  - Multi-channel PWM (configurable)
 *  - Adjustable duty cycle
 *  - Portable across STM32 families (F1, F2, F4...)
 *  - No HAL required (direct register access)
 *
 * Limitations:
 *  - CPU overhead due to interrupt handling
 *  - Limited PWM frequency compared to hardware PWM
 *
 * @author  Bui Van Van
 * @contact
 * Email: vanvan6604@gmail.com
 * GitHub: https://github.com/buivanvan
 
 * @date    2026
 * @version 1.1
 */

#ifndef __SOFT_PWM_H
#define __SOFT_PWM_H

/* ================= INCLUDE ================= */
#include "stm32f103xb.h"

#define SOFT_PWM_MAX_CH  8
#define HSE_CLK 8000000

#define Channel0 0
#define Channel1 1
#define Channel2 2
#define Channel3 3
#define Channel4 4
#define Channel5 5
#define Channel6 6
#define Channel7 7

#define PWM_MODE1 1
#define PWM_MODE2 2

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
    uint16_t duty;      // 0 -> period
		uint8_t mode;
} SoftPWM_Channel;

void SoftPWM_Init(uint16_t period);
void SoftPWM_AddChannel(GPIO_TypeDef *port, uint16_t pin);
void SoftPWM_SetDuty(uint8_t channel, uint16_t duty);
void SoftPWM_ChangeMode(uint8_t channel, uint8_t mode);

#endif
