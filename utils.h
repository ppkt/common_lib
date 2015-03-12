#ifndef __UTILS_H__
#define __UTILS_H__
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>

void delay(__IO uint32_t nTime /*ms*/);
void delay_decrement(void);
void delay_us(TIM_TypeDef *timer, unsigned int time);
void LED_Init();
void BTN_Init();
#endif
