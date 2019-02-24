#include "spi.h"

void
spi_init(uint32_t spi) {
    if (spi == SPI1) {
        rcc_periph_clock_enable(RCC_SPI1);
        rcc_periph_clock_enable(RCC_GPIOA);

        // Configure GPIOs: SS=PA4, SCK=PA5, MISO=PA6 and MOSI=PA7
        gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                      GPIO_SPI1_SCK | GPIO_SPI1_MOSI);
        gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                      GPIO_CNF_OUTPUT_PUSHPULL,
                      GPIO_SPI1_NSS);
        gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                      GPIO_CNF_INPUT_FLOAT,
                      GPIO_SPI1_MISO);
    } else if (spi == SPI2) {
        rcc_periph_clock_enable(RCC_SPI2);
        rcc_periph_clock_enable(RCC_GPIOB);

        // Configure GPIOs: SS=PB12, SCK=PB13, MISO=PB14 and MOSI=PB15
        gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                      GPIO_SPI2_SCK | GPIO_SPI2_MOSI);
        gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                      GPIO_CNF_OUTPUT_PUSHPULL,
                      GPIO_SPI2_NSS);
        gpio_set_mode(GPIOB, GPIO_MODE_INPUT,
                      GPIO_CNF_INPUT_FLOAT,
                      GPIO_SPI2_MISO);
    } else {
        hacf();
    }

    // reset and shut down peripheral
    spi_reset(spi);

    // Set up SPI in Master mode with:
    // - Clock baud rate: 1/64 of peripheral clock frequency
    // - Clock polarity: Idle Low
    // - Clock phase: Data valid on leading edge
    // - Data frame format: 8-bit
    // - Frame format: MSB First
    spi_init_master(spi,
                    SPI_CR1_BAUDRATE_FPCLK_DIV_64,
                    SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_1,
                    SPI_CR1_DFF_8BIT,
                    SPI_CR1_MSBFIRST
    );

    spi_enable_software_slave_management(spi);
    spi_set_nss_high(spi);

    spi_enable(spi);
    spi_nss(spi, true);
}

void
spi_nss(uint32_t spi, bool new_state) {
    void (*ptr)(uint32_t, uint16_t);

    if (new_state) {
        ptr = &gpio_set;
    } else {
        ptr = &gpio_clear;
    }

    // TODO(ppkt): SS pin / bank should be bound to device, not peripheral!
    if (spi == SPI1) {
        ptr(GPIO_BANK_SPI1_NSS, GPIO_SPI1_NSS);
    } else if (spi == SPI2) {
        ptr(GPIO_BANK_SPI2_NSS, GPIO_SPI2_NSS);
    } else {
        hacf();
    }
}

void
spi_send_transaction(uint32_t spi, uint8_t *tx, uint8_t *rx, uint8_t size) {
    // TODO(ppkt): Add receiving data from SPI!
    (void)rx;
    spi_nss(spi, false);
    for (uint8_t i = 0; i < size; ++i) {
        spi_send(spi, tx[i]);
        SPI_WAIT_FOR_TX_EMPTY(spi);
        SPI_WAIT_FOR_BUSY_CLEAR(spi);
    }
    spi_nss(spi, true);

}
