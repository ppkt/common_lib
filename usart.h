#pragma once

#include <libopencm3/cm3/assert.h>
#include <libopencm3/stm32/usart.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

void usart1_init(uint32_t speed);

#ifdef STM32F1
void usart2_init(uint32_t speed);
#endif

void usart_print(uint32_t usart, const char *string);
void usart_printf(uint32_t usart, const char *format, ...);

#define usart1_print(string) usart_print(USART1, string)
#define usart1_printf(format, ...) usart_printf(USART1, format, __VA_ARGS__)
#define print_variable_int(variable)                                           \
  usart_printf(USART1, "%s = %d\r\n", #variable, variable)
#define print_variable_hex(variable)                                           \
  usart_printf(USART1, "%s = 0x%.2x\r\n", #variable, variable)
#define print_variable_float(variable, precision)                              \
  usart_printf(USART1, "%s = %.*f\r\n", #variable, precision, variable)

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
      usart1_printf(__fmt__ "%s", __FILENAME__, __LINE__, __FUNCTION__,        \
                    string);                                                   \
  } while (0)
#define debug_printf(fmt, ...)                                                 \
  do {                                                                         \
    if (DEBUG)                                                                 \
      usart1_printf(__fmt__ fmt, __FILENAME__, __LINE__, __FUNCTION__,         \
                    __VA_ARGS__);                                              \
  } while (0)

#define hello_world(app_name)                                                  \
  do {                                                                         \
    if (DEBUG)                                                                 \
      usart1_printf("[%s]\r\nCompiled: %s %s\r\n", app_name, __DATE__,         \
                    __TIME__);                                                 \
  } while (0)
