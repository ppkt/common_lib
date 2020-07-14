#pragma once

#include <libopencm3/cm3/assert.h>
#include <libopencm3/stm32/usart.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

#ifndef DEBUG
#define DEBUG 0
#endif

void usart1_init(uint32_t speed);

#ifdef STM32F1
void usart2_init(uint32_t speed);
#endif

void usart_print(uint32_t usart, const char *string);
void usart_printf(uint32_t usart, const char *format, ...);

#define usart1_print(string) usart_print(USART1, string)
#define usart1_printf(format, ...) usart_printf(USART1, format, __VA_ARGS__)

#define __fmt__ "%-5s:%-3d%10s(): "
#define __FILENAME__ strrchr("/" __FILE__, '/') + 1
#define trace()                                                                \
  do {                                                                         \
    if (DEBUG)                                                                 \
      usart1_printf(__fmt__ "TRACE\n\r", __FILENAME__, __LINE__,               \
                    __FUNCTION__);                                             \
  } while (0)
#define debug_print(string)                                                    \
  do {                                                                         \
    if (DEBUG)                                                                 \
      usart1_printf(__fmt__ "%s\r\n", __FILENAME__, __LINE__, __FUNCTION__,    \
                    string);                                                   \
  } while (0)
#define debug_printf(fmt, ...)                                                 \
  do {                                                                         \
    if (DEBUG)                                                                 \
      usart1_printf(__fmt__ fmt "\r\n", __FILENAME__, __LINE__, __FUNCTION__,  \
                    __VA_ARGS__);                                              \
  } while (0)

#define hello_world(app_name)                                                  \
  do {                                                                         \
    if (DEBUG)                                                                 \
      usart1_printf("[%s]\r\nCompiled: %s %s\r\n", app_name, __DATE__,         \
                    __TIME__);                                                 \
  } while (0)

#define print_variable_int(variable)                                           \
  debug_printf("%s = %d", #variable, variable)
#define print_variable_hex(variable)                                           \
  debug_printf("%s = 0x%.2x", #variable, variable)
#define print_variable_float(variable, precision)                              \
  debug_printf("%s = %.*f", #variable, precision, variable)
