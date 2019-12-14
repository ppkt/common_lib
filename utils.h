#pragma once
#include <stdlib.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#ifdef STM32F1
#define U_ID_PTR (0x1FFFF7E8)
#define U_ID_0 (*(uint32_t *)(U_ID_PTR))
#define U_ID_1 (*(uint32_t *)(U_ID_PTR + 4))
#define U_ID_2 (*(uint32_t *)(U_ID_PTR + 8))
#else
#define U_ID_0 0xFFFFFFFF
#define U_ID_1 0xFFFFFFFF
#define U_ID_2 0xFFFFFFFF
#endif

#ifndef LED_INDICATOR
#define LED_INDICATOR 1
#endif

typedef struct {
  uint32_t port;
  uint16_t gpio;
} pin;

typedef struct {
  // pin to monitor
  pin pin;
  // internal counter
  uint16_t cnt;
  // threshold - when `cnt` will reach this value, new state will be updated
  uint16_t threshold;
  // current state
  bool state;
} debounce;

typedef enum {
  E_SUCCESS = 0,
  // returned when a pointer was expected but it was empty
  E_NULL_PTR,
  // returned when provided argument has incorrect value
  E_VALUE_INVALID,
  // problem with I2C communication
  E_I2C_TIMEOUT,
  // returned when value is too big
  E_VALUE_TOO_BIG,
  // other error, unspecified
  E_UNSPECIFIED = 255,
} error_t;

// Set precision for systick (low = 100 ms, medium = 10 ms, high = 1 ms)
#define PRECISION_LOW 0
#define PRECISION_MEDIUM 1
#define PRECISION_HIGH 2

#define sizeof_a(a) (sizeof(a) / sizeof(a[0]))

void hacf(void) __attribute__((__noreturn__));
// void rtc_setup(void);
void systick_setup(uint8_t precision);
void delay_ms(uint32_t time);
void led_init(void);
void led_toggle(void);
void led_set(bool new_state);

void setup_delay_timer(uint32_t timer);
void delay_us(uint32_t timer, uint16_t us);

error_t debounce_init(debounce *config, const pin *pin, uint16_t threshold);
error_t debounce_get_state(debounce *config);

uint8_t sadd8(uint8_t a, uint8_t b);
uint16_t sadd16(uint16_t a, uint16_t b);
uint32_t sadd32(uint32_t a, uint32_t b);

/**
 * Saturated add and sub
 */
#define sadd(a, b)                                                             \
  _Generic((a), uint8_t : sadd8, uint16_t : sadd16, uint32_t : sadd32)(a, b)

#define ssub(a, b)                                                             \
  ({                                                                           \
    __typeof__(a) _c = a > b ? a - b : 0;                                      \
    _c;                                                                        \
  })

uint8_t check_bit(uint32_t variable, uint8_t pos);

int32_t fast_int_pow(int32_t base, uint32_t exponent);

#define fast_abs(x)                                                            \
  _Generic((x), int8_t                                                         \
           : fast_abs8, int16_t                                                \
           : fast_abs16, int32_t                                               \
           : fast_abs32, int                                                   \
           : fast_abs32)(x)

inline uint8_t fast_abs8(int8_t i) {
  if (i < 0)
    i = -i;
  return (uint8_t)i;
}

inline uint16_t fast_abs16(int16_t i) {
  if (i < 0)
    i = -i;
  return (uint16_t)i;
}

inline uint32_t fast_abs32(int32_t i) {
  if (i < 0)
    i = -i;
  return (uint32_t)i;
}

#define check_error(e) if (e != E_SUCCESS) return e
#define halt_on_error(e) if (e != E_SUCCESS) hacf()

#define max(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a);                                                    \
    __typeof__(b) _b = (b);                                                    \
    _a > _b ? _a : _b;                                                         \
  })

#define min(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a);                                                    \
    __typeof__(b) _b = (b);                                                    \
    _a < _b ? _a : _b;                                                         \
  })
