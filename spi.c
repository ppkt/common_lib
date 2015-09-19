#include "spi.h"

void rcc_configuration() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
}

void spi_gpio_configuration() {
    GPIO_InitTypeDef GPIO_InitStructure;

    // Configure SPI1 pins: SCK, MISO and MOSI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void spi_init(SPI_TypeDef *spi) {
    /* System clocks configuration ---------------------------------------------*/
    rcc_configuration();

    /* GPIO configuration ------------------------------------------------------*/
    spi_gpio_configuration();

    /* SPI1 Config -------------------------------------------------------------*/
    SPI_I2S_DeInit(spi);
    SPI_InitTypeDef   SPI_InitStructure;
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(spi, &SPI_InitStructure);

    /* Enable SPI1 */
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
