# 🔧 STM32 Software PWM (Bare-Metal)

A lightweight Software PWM (Pulse Width Modulation) library for STM32 microcontrollers using direct register access (no HAL required).

## 📌 Overview

This library implements PWM signals on any GPIO pins using a hardware timer interrupt. It is designed for flexibility and portability across multiple STM32 families such as F1, F2, and F4.

Unlike hardware PWM, this approach allows generating PWM signals on pins that do not support timer channels.

## ✨ Features

- ✅ Multi-channel PWM (configurable number of channels)
- ✅ Works on any GPIO pin
- ✅ Adjustable duty cycle per channel
- ✅ No HAL / LL libraries required (bare-metal)
- ✅ Simple and easy to integrate

## ⚠️ Limitations

- ❗ Higher CPU usage due to frequent timer interrupts
- ❗ Lower accuracy and stability compared to hardware PWM
- ❗ Not suitable for high-frequency applications (>100 Hz)
- ❗ Requires dedicating a hardware timer (e.g., TIM2), meaning its interrupt vector cannot be reused for other tasks

## ⚙️ How It Works

- A hardware timer generates periodic interrupts (e.g., every 10µs)
- A software counter increments inside the ISR
- Each channel compares its duty cycle with the counter
- GPIO pins are toggled accordingly to generate PWM signals

## Actual Results
<img width="1457" height="971" alt="RESULT1" src="https://github.com/user-attachments/assets/19679d9f-213a-404f-b93a-a99d8f0a2962" />
<img width="1487" height="993" alt="RESULT2" src="https://github.com/user-attachments/assets/23ef616d-cc26-4ab0-b272-c0190fc34a5c" />

## 📊 Example

```c
#include "Soft_PWM.h"
int main()
{
SoftPWM_Init(100);        // Set PWM resolution

SoftPWM_AddChannel(GPIOA, GPIO_PIN_0);
SoftPWM_AddChannel(GPIOA, GPIO_PIN_1);

while(1)
{
SoftPWM_SetDuty(0, 50);   // (50/perior)*100 (%)
SoftPWM_SetDuty(1, 75);   // (75/perior)*100 (%)
}
}
```
