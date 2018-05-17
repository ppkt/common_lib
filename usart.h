#ifndef __USART_H__
#define __USART_H__
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_rcc.h>
#include <misc.h>

#include "utils.h"

#include <stdio.h>
#include <stdarg.h>

void USART1_IRQHandler(void);

void usart1_init(unsigned int speed);
void usart2_init(unsigned int speed);

void usart_print(USART_TypeDef* usart, char* string);
void usart_printf(USART_TypeDef* usart, const char *format, ...);

#define usart1_print(string) usart_print(USART1,string)
#define usart1_printf(format,...) usart_printf(USART1,format,__VA_ARGS__)

void usart_string_append(char c);
void usart_clear_string();

#endif
