#pragma once

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>

#include "utils.h"

#define SPI_WAIT_FOR_TX_EMPTY(spi) while (!(SPI_SR(spi) & SPI_SR_TXE))
#define SPI_WAIT_FOR_BUSY_CLEAR(spi) while ((SPI_SR(spi) & SPI_SR_BSY));

typedef struct spi_device {
    uint32_t spi;
} spi_device;


/**
 * Initialize SPI peripheral (including RCC and GPIOs)
 */
void
spi_init(uint32_t spi);

/**
 * Send / receive data to SPI slave using transaction (i.e. pull down SS before
 * 1st byte and pull up after all data is sent).
 */
void
spi_send_transaction(uint32_t spi, uint8_t *tx, uint8_t *rx, uint8_t size);

/**
 * Set new value for SS depending on selected peripheral
 */
void
spi_nss(uint32_t spi, bool new_state);

