#ifndef __UTILS_H__
#define __UTILS_H__
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>

#ifdef STM32F10X_MD
#define U_ID_0 (*(uint32_t*)0x1FFFF7E8)
#define U_ID_1 (*(uint32_t*)0x1FFFF7EC)
#define U_ID_2 (*(uint32_t*)0x1FFFF7F0)
#else
#define U_ID_0 0xFFFFFFFF
#define U_ID_1 0xFFFFFFFF
#define U_ID_2 0xFFFFFFFF
#endif

void delay(__IO uint32_t nTime /*ms*/);
void delay_decrement(void);
void delay_us(TIM_TypeDef *timer, unsigned int time);
void LED_Init();
void BTN_Init();
#endif
