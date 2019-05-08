#pragma once

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>

#include "utils.h"

#define GPIO_BANK_SPI1 GPIOA
#ifdef STM32F0
#define GPIO_SPI1_SCK GPIO5
#define GPIO_SPI1_MISO GPIO6
#define GPIO_SPI1_MOSI GPIO7
#endif

#define SPI_WAIT_FOR_TX_EMPTY(spi) while (!(SPI_SR(spi) & SPI_SR_TXE))
#define SPI_WAIT_FOR_RX_NOT_EMPTY(spi) while (!(SPI_SR(spi) & SPI_SR_RXNE))
#define SPI_WAIT_FOR_BUSY_CLEAR(spi) while ((SPI_SR(spi) & SPI_SR_BSY))

typedef struct spi_device {
  uint32_t spi;
} spi_device;

/**
 * Initialize SPI peripheral in master mode (including RCC and GPIOs).
 * Note: This function does not initialize NSS GPIO.
 */
void spi_init(uint32_t spi);

/**
 * Send / receive data to SPI slave in bulk.
 * WARNING! Because it's possible that there could be more than one device on
 * SPI bus, it's required to manually pull NSS pin down before calling this
 * function (and pulling it up afterwards).
 */
void spi_send_transaction(uint32_t spi, const uint8_t *tx, uint8_t *rx,
                          uint8_t size);
