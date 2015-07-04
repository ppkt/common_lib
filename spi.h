#ifndef __SPI_H___
#define __SPI_H___

#include "stm32f10x_dma.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

void spi_init(void);
void spi_send(u8* tx, u8* rx, u8 size);

#endif // __SPI_H__
