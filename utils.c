#include "utils.h"

//void rtc_setup(void) {
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
//    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {}  // while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) {}
//    // RTCCLK = LSE = 32.768 kHz
//    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);  // RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
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


void led_init(void) {
    uint32_t rcc_clock = 0;
    uint16_t pin = 0;
    uint32_t gpio = 0;

    switch (LED_INDICATOR) {
        case 1:
            // PC13
            rcc_clock = RCC_GPIOC;
            pin = GPIO13;
            gpio = GPIOC;
            break;
        case 2:
            // PA1
            rcc_clock = RCC_GPIOA;
            pin = GPIO1;
            gpio = GPIOA;
            break;
        case 3:
            // PA8 PA9;
            rcc_clock = RCC_GPIOC;
            pin = GPIO8 | GPIO9;
            gpio = GPIOA;
            break;
        case 4:
            // PB12
            rcc_clock = RCC_GPIOB;
            pin = GPIO12;
            gpio = GPIOB;
            break;
        default:
            hacf();
            break;
    }

    // Enable GPIO clock
    rcc_periph_clock_enable(rcc_clock);

    gpio_set_mode(gpio, GPIO_MODE_OUTPUT_2_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL, pin);

    gpio_set(gpio, pin);
}

void led_toggle() {
    uint32_t gpio = 0;
    uint16_t pin = 0;

    switch (LED_INDICATOR) {
        case 1:
            gpio = GPIOC;
            pin = GPIO13;
            break;
        case 2:
            gpio = GPIOA;
            pin = GPIO1;
            break;
        case 4:
            gpio = GPIOB;
            pin = GPIO12;
            break;
        default:
            hacf();
    }

    // Toggles LED state on dev board
    gpio_toggle(gpio, pin);
}

void hacf(void) {
    while (1) {
        led_toggle();
        delay_ms(30);
        led_toggle();
        delay_ms(300);
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
    return (uint8_t) ((variable >> pos) & 1);
}

inline uint32_t toggle_bit(uint32_t variable, uint8_t pos) {
    return variable ^ (1 << pos);
}

volatile uint32_t system_millis;

uint8_t system_precision;

void systick_setup(uint8_t precision) {
    system_precision = precision;
    // clock rate / 100 to get 10 ms interrupt rate
    systick_set_reload((uint32_t) (rcc_ahb_frequency /
                                   fast_int_pow(10, (1 + precision))));
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_counter_enable();
    systick_interrupt_enable();
}

// Due to some technical reasons, this function is rounded to 10 ms
void delay_ms(uint32_t time) {
    uint32_t wake = system_millis + time;
    while (wake > system_millis);
}

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
