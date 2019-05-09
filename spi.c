#include "spi.h"

void spi_init(uint32_t spi) {
  if (spi == SPI1) {
    rcc_periph_clock_enable(RCC_SPI1);
    rcc_periph_clock_enable(RCC_GPIOA);

    // Configure GPIOs: SCK=PA5, MISO=PA6 and MOSI=PA7
#ifdef STM32F0
    gpio_mode_setup(GPIO_BANK_SPI1, GPIO_MODE_AF, GPIO_PUPD_NONE,
                    GPIO_SPI1_SCK | GPIO_SPI1_MOSI);
    gpio_mode_setup(GPIO_BANK_SPI1, GPIO_MODE_AF, GPIO_PUPD_PULLUP,
                    GPIO_SPI1_MISO);
    gpio_set_af(GPIO_BANK_SPI1, GPIO_AF0,
                GPIO_SPI1_SCK | GPIO_SPI1_MOSI | GPIO_SPI1_MISO);

    gpio_set_output_options(GPIO_BANK_SPI1, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH,
                            GPIO_SPI1_SCK | GPIO_SPI1_MOSI);
#elif STM32F1
    gpio_set_mode(GPIO_BANK_SPI1, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                  GPIO_SPI1_SCK | GPIO_SPI1_MOSI);
    gpio_set_mode(GPIO_BANK_SPI1, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
                  GPIO_SPI1_MISO);
  } else if (spi == SPI2) {
    rcc_periph_clock_enable(RCC_SPI2);
    rcc_periph_clock_enable(RCC_GPIOB);
    // Configure GPIOs: SCK=PB13, MISO=PB14 and MOSI=PB15
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                  GPIO_SPI2_SCK | GPIO_SPI2_MOSI);
    gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_SPI2_MISO);
#endif
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
#ifdef STM32F0
  spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_64,
                  SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                  SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_MSBFIRST);
  spi_set_data_size(SPI1, SPI_CR2_DS_8BIT);
  spi_fifo_reception_threshold_8bit(SPI1);
#elif STM32F1
  spi_init_master(
      spi, SPI_CR1_BAUDRATE_FPCLK_DIV_64, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
      SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
#endif

  spi_enable(spi);
}

void spi_send_recv(uint32_t spi, const uint8_t *tx, uint8_t *rx, uint8_t size) {
  for (uint8_t i = 0; i < size; ++i) {
    spi_send(spi, tx[i]);
    SPI_WAIT_FOR_TX_EMPTY(spi);
    SPI_WAIT_FOR_RX_NOT_EMPTY(spi);
    SPI_WAIT_FOR_BUSY_CLEAR(spi);
    rx[i] = spi_read(spi);
  }
}
