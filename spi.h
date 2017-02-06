#ifndef __SPI_H___
#define __SPI_H___

#include <stdbool.h>

#include "stm32f10x_dma.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

void spi_init(SPI_TypeDef *spi);
void spi_send(SPI_TypeDef *spi, u8* tx, u8* rx, u8 size);
void spi_nss(SPI_TypeDef *spi, bool new_state);

#endif // __SPI_H__
