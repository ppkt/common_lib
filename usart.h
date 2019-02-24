#pragma once

#include <libopencm3/cm3/assert.h>
#include <libopencm3/stm32/usart.h>

#include <stdio.h>
#include <stdarg.h>

#include "utils.h"

void usart1_init(uint32_t speed);

void usart2_init(uint32_t speed);

void usart_print(uint32_t usart, const char *string);

void usart_printf(uint32_t usart, const char *format, ...);

#define usart1_print(string) usart_print(USART1,string)
#define usart1_printf(format, ...) usart_printf(USART1,format,__VA_ARGS__)
#define print_variable_int(variable) usart_printf(USART1,"%s = %d\r\n",#variable,variable)
#define print_variable_hex(variable) usart_printf(USART1,"%s = 0x%.2x\r\n",#variable,variable)
#define print_variable_float(variable, precision) usart_printf(USART1,"%s = %.*f\r\n",#variable,precision,variable)
