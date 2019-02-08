#include <libopencm3/stm32/usart.h>
#include "usart.h"

void usart1_init(uint32_t speed)
{
    // Enable GPIO clock
    rcc_periph_clock_enable(RCC_GPIOA);

    // Enable UART clock
    rcc_periph_clock_enable(RCC_USART1);

    // Use PA9 and PA10
    gpio_set_mode(GPIO_BANK_USART1_TX, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
    gpio_set_mode(GPIO_BANK_USART1_RX, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);

    usart_set_baudrate(USART1, speed);
    usart_set_mode(USART1, USART_MODE_TX);
    usart_enable(USART1);
}

void usart2_init(uint32_t speed)
{
    // Enable GPIO clock
    rcc_periph_clock_enable(RCC_GPIOA);

    // Enable UART clock
    rcc_periph_clock_enable(RCC_USART2);

    // Use PA2 and PA3
    gpio_set_mode(GPIO_BANK_USART2_TX, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);
    gpio_set_mode(GPIO_BANK_USART2_RX, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_INPUT_FLOAT, GPIO_USART2_RX);

    usart_set_baudrate(USART2, speed);
    usart_set_mode(USART2, USART_MODE_TX);
    usart_enable(USART2);
}

void usart_print(uint32_t usart, char* string)
{
    uint8_t a = 0;
    while (string[a]) {
        usart_send_blocking(usart, string[a++]);
    }
}

void usart_printf(uint32_t usart, const char *format, ...)
{
    va_list args;
    static char buffer[120];
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    usart_print(usart, buffer);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* msg, uint8_t* file, uint32_t line) {
      usart_printf(USART1, "Assert: %s failed: %s:%d\r\n", msg, file, (int)line);
      hacf();
}
#endif
