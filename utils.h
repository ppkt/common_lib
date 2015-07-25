#ifndef __UTILS_H__
#define __UTILS_H__
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>

#ifdef STM32F10X_MD
#define U_ID_PTR            (0x1FFFF7E8)
#define U_ID_0 (*(uint32_t*)(U_ID_PTR))
#define U_ID_1 (*(uint32_t*)(U_ID_PTR + 4))
#define U_ID_2 (*(uint32_t*)(U_ID_PTR + 8))
#else
#define U_ID_0 0xFFFFFFFF
#define U_ID_1 0xFFFFFFFF
#define U_ID_2 0xFFFFFFFF
#endif

void setup_delay_timer(TIM_TypeDef *timer);
void delay(__IO uint32_t nTime /*ms*/);
void delay_decrement(void);
void delay_us(TIM_TypeDef *timer, unsigned int time);
void delay_ms(TIM_TypeDef *timer, unsigned int time);
void LED_Init(void);
void BTN_Init(void);

void LED_Init1(void);
void LED_Init2(void);
void LED_Init3(void);
#endif
