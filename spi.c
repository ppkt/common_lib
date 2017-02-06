#include "spi.h"

void rcc_configuration(SPI_TypeDef *spi) {
    if (spi == SPI1) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
    } else if (spi == SPI2) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    }
}

void spi_gpio_configuration(SPI_TypeDef *spi) {
    GPIO_InitTypeDef GPIO_InitStructure;

    if (spi == SPI1) {
        // Configure SPI1 pins: SCK, MISO and MOSI
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    } else if (spi == SPI2) {
        // Configure SPI2 pins: SCK, MISO and MOSI
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
}

void spi_init(SPI_TypeDef *spi) {
    /* System clocks configuration ---------------------------------------------*/
    rcc_configuration(spi);

    /* GPIO configuration ------------------------------------------------------*/
    spi_gpio_configuration(spi);

    /* SPI Config -------------------------------------------------------------*/
    SPI_I2S_DeInit(spi);
    SPI_InitTypeDef   SPI_InitStructure;
    SPI_InitStructure.SPI_Direction = SPI_Direction_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(spi, &SPI_InitStructure);

    /* Enable SPI */
    SPI_Cmd(spi, ENABLE);
}

void spi_send(SPI_TypeDef *spi, u8* tx, u8* rx, u8 size) {
    u8 index;
    for (index = 0; index < size; ++index) {
        while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET);
        SPI_I2S_SendData(spi, tx[index]);

        while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == RESET);
        rx[index] = SPI_I2S_ReceiveData(spi);
    }
    // Wait for end of transmission
    while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET);
}

void spi_nss(SPI_TypeDef *spi, bool new_state) {
    BitAction action = Bit_RESET;
    if (new_state)
        action = Bit_SET;

    if (spi == SPI1) {
        GPIO_WriteBit(GPIOA, GPIO_Pin_4, action);
    } else if (spi == SPI2) {
        GPIO_WriteBit(GPIOB, GPIO_Pin_12, action);
    }
}
