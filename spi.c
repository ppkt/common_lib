#include "spi.h"

#define BufferSize       32
u8 tmp[2] = {0x00, 0x00};
uint8_t SPI1_Buffer_Tx[BufferSize] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
                                      0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                                      0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C,
                                      0x1D, 0x1E, 0x1F, 0x20};
uint8_t SPI1_Buffer_Rx[BufferSize] = {0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
                                      0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E,
                                      0x5F, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65,
                                      0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C,
                                      0x6D, 0x6E, 0x6F, 0x70};

void rcc_configuration() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
}

void gpio_configuration() {
    GPIO_InitTypeDef GPIO_InitStructure;

    // Configure SPI1 pins: SCK, MISO and MOSI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOA, GPIO_Pin_2, Bit_SET);
    GPIO_WriteBit(GPIOA, GPIO_Pin_3, Bit_SET);
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);


}

void spi_init(void) {
    /* System clocks configuration ---------------------------------------------*/
    rcc_configuration();

    /* GPIO configuration ------------------------------------------------------*/
    gpio_configuration();

    /* SPI1 Config -------------------------------------------------------------*/
    SPI_I2S_DeInit(SPI1);
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
    SPI_Init(SPI1, &SPI_InitStructure);

    /* Enable SPIy */
    SPI_Cmd(SPI1, ENABLE);
}

void spi_send(u8* tx, u8* rx, u8 size) {
    u8 index;
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);
    for (index = 0; index < size; ++index) {
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
        SPI_I2S_SendData(SPI1, tx[index]);
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
        rx[index] = SPI_I2S_ReceiveData(SPI1);
    }
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
}
