#include "usart.h"

void usart_init(uint32_t usart, uint32_t speed, uint32_t mode) {
  // Get I2C clock, pins and ports
  enum rcc_periph_clken usart_clock;
  uint32_t tx_port, rx_port;
  uint16_t tx_pin, rx_pin;

  switch (usart) {
  case USART1:
    usart_clock = RCC_USART1;
    tx_port = GPIO_BANK_USART1_TX;
    tx_pin = GPIO_USART1_TX;
    rx_port = GPIO_BANK_USART1_RX;
    rx_pin = GPIO_USART1_RX;
    break;
  case USART2:
    usart_clock = RCC_USART2;
    tx_port = GPIO_BANK_USART2_TX;
    tx_pin = GPIO_USART2_TX;
    rx_port = GPIO_BANK_USART2_RX;
    rx_pin = GPIO_USART2_RX;
    break;
  case USART3:
    usart_clock = RCC_USART3;
    tx_port = GPIO_BANK_USART3_TX;
    tx_pin = GPIO_USART3_TX;
    rx_port = GPIO_BANK_USART3_RX;
    rx_pin = GPIO_USART3_RX;
    break;
  default:
    hacf();
  }
  // Enable UART clock
  rcc_periph_clock_enable(usart_clock);

  // Enable GPIO clocks
  rcc_periph_clock_enable(gpio2rcc(tx_port));
  rcc_periph_clock_enable(gpio2rcc(rx_port));

  // Set GPIO mode
  gpio_set_mode(tx_port, GPIO_MODE_OUTPUT_50_MHZ,
                GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, tx_pin);
  gpio_set_mode(rx_port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_INPUT_FLOAT, rx_pin);

  usart_disable(usart);
  usart_set_baudrate(usart, speed);
  usart_set_mode(usart, mode);
  usart_enable(usart);
}

void usart1_init(uint32_t speed) {
  usart_init(USART1, speed, USART_MODE_TX);
}

void usart2_init(uint32_t speed) {
  usart_init(USART2, speed, USART_MODE_TX);
}

#ifndef DISABLE_UART
void usart_print(uint32_t usart, const char *string) {
  uint8_t a = 0;
  while (string[a]) {
    usart_send_blocking(usart, string[a++]);
  }
}
void usart_printf(uint32_t usart, const char *format, ...) {
  va_list args;
  static char buffer[512];
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  usart_print(usart, buffer);
}
#else
void usart_print(uint32_t usart, const char *string) {
  (void)usart;
  (void)string;
}
void usart_printf(uint32_t usart, const char *format, ...) {
  (void)usart;
  (void)format;
}
#endif

void cm3_assert_failed_verbose(const char *file, int line, const char *func,
                               const char *assert_expr) {
  usart1_printf("Assert failed in file %s:%d in function %s:\r\n%s\r\n", file,
                line, func, assert_expr);

  hacf();
}

void cm3_assert_failed(void) { hacf(); }
