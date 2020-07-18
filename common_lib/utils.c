#include "utils.h"

// void rtc_setup(void) {
//    // Based on http://www.stm32.eu/node/97 and AN2821
//
//    // Enable PWR and BKP clocks
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
//    // Enable access to BKP
//    PWR_BackupAccessCmd(ENABLE);
//    // BKP de-init
//    BKP_DeInit();
//    // Start editing RTC configuration
//    RTC_EnterConfigMode();
//    // Enable LSE
//    RCC_LSEConfig(RCC_LSE_ON);  // RCC_LSICmd(ENABLE);
//    // Wait for LSE
//    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {}  //
//    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) {}
//    // RTCCLK = LSE = 32.768 kHz
//    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);  //
//    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
//    // Turn on RTC clock
//    RCC_RTCCLKCmd(ENABLE);
//    // Wait for synchronization
//    RTC_WaitForSynchro();
//    // Wait until operation finish
//    RTC_WaitForLastTask();
//    // Enable RTC second
//    RTC_ITConfig(RTC_IT_SEC, ENABLE);
//    RTC_WaitForLastTask();
//    // Set Prescaler to 32768 ticks
//    // RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)
//    RTC_SetPrescaler(32767);
//    RTC_WaitForLastTask();
//    // Exit configuration editing mode
//    RTC_ExitConfigMode();
//    // Set RTC counter (time) to 12:00:00
//    RTC_SetCounter(12 * 60 * 60);
//
//    // Disable default Tamper Pin
//    BKP_TamperPinCmd(DISABLE);
//
//    // Enable calibration data on Tamper Pin
////    BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
//
//    // Enable seconds indicator on Tamper Pin
//    BKP_RTCOutputConfig(BKP_RTCOutputSource_Second);
//
//    // Enable RTC interrupts
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
//    NVIC_InitTypeDef NVIC_InitStructure;
//    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//
//    // Interrupt every 1 second
//    RTC_ITConfig(RTC_IT_SEC, ENABLE);
//    RTC_WaitForLastTask();
//}

#ifndef LED_PORT
#warning "LED port not defined, using default value"
#define LED_PORT GPIOA
#endif

#ifndef LED_PIN
#warning "LED pin not defined, using default value"
#define LED_PIN GPIO1
#endif

void led_init(void) {

  // Enable GPIO clock
  if (LED_PORT == GPIOA) {
    rcc_periph_clock_enable(RCC_GPIOA);
  } else if (LED_PORT == GPIOB) {
    rcc_periph_clock_enable(RCC_GPIOB);
  } else if (LED_PORT == GPIOC) {
    rcc_periph_clock_enable(RCC_GPIOC);
  }

#ifdef STM32F1
  gpio_set_mode(LED_PORT, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                LED_PIN);
#elif STM32F0
  gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
#endif

  gpio_clear(LED_PORT, LED_PIN);
}

void led_toggle() {
  // Toggles LED state on dev board
  gpio_toggle(LED_PORT, LED_PIN);
}

void led_set(bool new_state) {
  if (new_state) {
    gpio_set(LED_PORT, LED_PIN);
  } else {
    gpio_clear(LED_PORT, LED_PIN);
  }
}

void hacf(void) {
  led_set(0);
  while (1) {
    delay_ms(30);
    led_toggle();
    delay_ms(300);
    led_toggle();
  }
}

// Saturated add functions for 8 / 16 / 32 unsigned integers
inline uint8_t sadd8(uint8_t a, uint8_t b) {
  return (a > 0xFF - b) ? 0xFF : a + b;
}

inline uint16_t sadd16(uint16_t a, uint16_t b) {
  return (a > 0xFFFF - b) ? 0xFFFF : a + b;
}

inline uint32_t sadd32(uint32_t a, uint32_t b) {
  return (a > 0xFFFFFFFF - b) ? 0xFFFFFFFF : a + b;
}

inline uint8_t check_bit(uint32_t variable, uint8_t pos) {
  return (uint8_t)((variable >> pos) & 1u);
}

inline uint32_t toggle_bit(uint32_t variable, uint8_t pos) {
  return variable ^ (1u << pos);
}

volatile uint32_t system_millis;

uint8_t system_precision;

void systick_setup(uint8_t precision) {
  system_precision = precision;
  // clock rate / 100 to get 10 ms interrupt rate
  systick_set_reload(
      (uint32_t)(rcc_ahb_frequency / fast_int_pow(10, (1 + precision))));
  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
  systick_counter_enable();
  systick_interrupt_enable();
}

// Due to some technical reasons, this function is rounded to 10 ms
void delay_ms(uint32_t time) {
  uint32_t wake = system_millis + time;
  while (wake > system_millis)
    ;
}

inline uint32_t get_system_millis(void) { return system_millis; }

// For quick calculation of power when base and exponent are integers
// https://stackoverflow.com/a/101613
int32_t fast_int_pow(int32_t base, uint32_t exponent) {
  int32_t result = 1;
  while (1) {
    if (exponent & 1) {
      result *= base;
    }
    exponent >>= 1;
    if (!exponent) {
      break;
    }
    base *= base;
  }
  return result;
}

void sys_tick_handler(void) {
  system_millis += fast_int_pow(10, 2u - system_precision);
}

void setup_delay_timer(uint32_t timer) {
  enum rcc_periph_clken rcc_tim;
  switch (timer) {
  case TIM1:
    rcc_tim = RCC_TIM1;
    break;
  case TIM2:
    rcc_tim = RCC_TIM2;
    break;
  case TIM3:
    rcc_tim = RCC_TIM3;
    break;
  default:
    hacf();
    return;
  }

  rcc_periph_clock_enable(rcc_tim);
  // microsecond counter
  timer_set_prescaler(timer, (rcc_ahb_frequency / 2) / 1e6 - 1);
  timer_set_period(timer, 0xffff);
  timer_one_shot_mode(timer);
}

void delay_us(uint32_t timer, uint16_t delay) {
  TIM_ARR(timer) = ssub(delay - 1, 3) + 1;
  TIM_EGR(timer) = TIM_EGR_UG;
  TIM_CR1(timer) |= TIM_CR1_CEN;
  // timer_enable_counter;
  while (TIM_CR1(timer) & TIM_CR1_CEN)
    ;
}

void trace_init(void) {
  // enable the use DWT
  if (!dwt_enable_cycle_counter()) {
    hacf();
  }
}

// Initialize structure used for debouncing
error_t debounce_init(debounce *config, const pin *_pin, uint16_t threshold) {
  if (!config || !_pin) {
    return E_NULL_PTR;
  }

  config->cnt = 0;
  config->pin = *_pin;
  config->state = 0;
  config->known_state = 0;
  config->threshold = threshold;

  return E_SUCCESS;
}

// When this function is called, value from pin is read and, if threshold is
// reached, state of structure is updated.
error_t debounce_get_state(debounce *config) {
  if (!config) {
    return E_NULL_PTR;
  }

  bool new_state = gpio_get(config->pin.port, config->pin.gpio);
  if (new_state != config->state) {
    config->cnt++;

    if (config->cnt > config->threshold) {
      config->state = new_state;
      config->cnt = 0;
    }
  } else {
    config->cnt = 0;
  }

  return E_SUCCESS;
}

uint32_t heap_size(void) {
  char stack_variable;
  char *heap_variable;
  heap_variable = malloc(4);
  uint32_t result = &stack_variable - heap_variable;
  free(heap_variable);
  return result;
}

// Return RCC clock associated with GPIO port
enum rcc_periph_clken gpio2rcc(uint32_t gpio) {
  switch (gpio) {
  case GPIOA:
    return RCC_GPIOA;
  case GPIOB:
    return RCC_GPIOB;
  case GPIOC:
    return RCC_GPIOC;
  case GPIOD:
    return RCC_GPIOD;
  case GPIOE:
    return RCC_GPIOE;
  case GPIOF:
    return RCC_GPIOF;
  case GPIOG:
    return RCC_GPIOG;
  default:
    hacf();
  }
}
