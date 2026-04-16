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
 * @version 1.0
 */

#ifndef __SOFT_PWM_H
#define __SOFT_PWM_H

/* ================= INCLUDE ================= */
/* ========== Chon dong STM32 ban dang dung ===========*/
#include "stm32f103xb.h"

#define SOFT_PWM_MAX_CH  8
#define HSE_CLK 8000000

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
    uint16_t duty;      // 0 -> period
} SoftPWM_Channel;

void SoftPWM_Init(uint16_t period);
uint8_t SoftPWM_AddChannel(GPIO_TypeDef *port, uint16_t pin);
void SoftPWM_SetDuty(uint8_t ch, uint16_t duty);

#endif
