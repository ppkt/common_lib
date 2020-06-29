#include "sd_card.h"

// NOTE: Code is based on samples from http://elm-chan.org/fsw/ff/ffsample.zip

/**
 * Wait up to `timeout_ms` milliseconds until card is ready. Return `true` or 1
 * if card is ready or `false` / 0 if timeout has been reached.
 */
static bool sd_wait_ready(const spi_device *dev, uint16_t timeout_ms) {
  uint32_t timeout = get_system_millis() + timeout_ms;
  uint8_t data;
  do {
    data = spi_xfer(dev->spi, 0xFF);
    if (data == 0xFF) {
      return true;
    }
  } while (get_system_millis() < timeout);
  return false;
}

/**
 * Deselect SD Card on SPI bus, additionally send dummy byte to "flush" D0
 */
void sd_deselect(const spi_device *dev) {
  // Set CS high
  spi_cs_deselect(dev);
  // Dummy clock to force D0 hi-z for multiple slave SPI
  spi_xfer(dev->spi, 0xFF);
}

/**
 * Select SD Card on SPI bus, additionally send dummy byte to enable DO.
 * Also check whether card is ready.
 */
bool sd_select(const spi_device *dev) {
  spi_cs_select(dev);       // Set CS low
  spi_xfer(dev->spi, 0xFF); // Dummy clock, force D0 enabled

  // Wait for card ready
  if (sd_wait_ready(dev, 500)) {
    return true; // card is ready
  }
  spi_cs_deselect(dev); // Card not ready, release SPI bus
  return false;
}

/**
 * Send Command to SD card, if it's ACMD, prepend it by CMD55. Return response
 */
uint8_t sd_cmd(const spi_device *dev, uint8_t cmd, uint32_t arg) {
  uint8_t res;
  if (cmd & 0x80u) {
    // Send CMD55 prior to ACMD<n>
    cmd &= 0x7Fu;
    res = sd_cmd(dev, CMD55, 0);
    if (res > 1) {
      return res;
    }
  }

  // Select the card and wait for ready except to stop multiple block read
  if (cmd != CMD12) {
    sd_deselect(dev);
    if (!sd_select(dev)) {
      return 0xFF;
    }
  }

  // Send command packet
  spi_xfer(dev->spi, 0x40u | cmd);           // Start + command index
  spi_xfer(dev->spi, (uint8_t)(arg >> 24u)); // Argument [31..24]
  spi_xfer(dev->spi, (uint8_t)(arg >> 16u)); // Argument [23..16]
  spi_xfer(dev->spi, (uint8_t)(arg >> 8u));  // Argument [15..8]
  spi_xfer(dev->spi, (uint8_t)arg);          // Argument [7..0]

  uint8_t crc = 0x01; // Dummy CRC + Stop bit
  if (cmd == CMD0) {
    crc = 0x95; // Valid CRC for CMD0(0)
  } else if (cmd == CMD8) {
    crc = 0x87; // Valid CRC for CMD8(0x1AA);
  }
  spi_xfer(dev->spi, crc);

  // Receive response
  if (cmd == CMD12) {
    spi_xfer(dev->spi, 0xFF); // Discard following byte for CMD12
  }

  // Wait for response, 10 bytes max
  uint8_t n = 10;
  do {
    res = spi_xfer(dev->spi, 0xFF);
  } while ((res & 0x80u) && --n);
  return res;
}

/**
 * Perform SD card initialization, currently only SDv2.0 protocol is supported.
 */
uint8_t sd_init(const spi_device *dev, uint8_t *card_type) {
  debug_print("Initializing SD Card\r\n");

  // Lower SPI speed
  spi_set_baudrate_prescaler(dev->spi, SPI_CR1_BR_FPCLK_DIV_128);

  // 74+ clocks when CS is high
  sd_deselect(dev);
  for (uint8_t i = 0; i < 10; ++i) {
    spi_xfer(dev->spi, 0xFF);
  }

  if (sd_cmd(dev, CMD0, 0) != 1) {
    debug_print("Problem during initialization. Card missing?\r\n");
    return 1;
  }
  uint32_t timeout = get_system_millis() + 1000;

  uint8_t ocr[4], ret;
  if (sd_cmd(dev, CMD8, 0x01AA) == 1) {
    // SDv2 Card
    ocr[0] = spi_xfer(dev->spi, 0xFF);
    ocr[1] = spi_xfer(dev->spi, 0xFF);
    ocr[2] = spi_xfer(dev->spi, 0xFF);
    ocr[3] = spi_xfer(dev->spi, 0xFF);

    // Does card support VCC of 2.7-3.6V?
    if (ocr[2] != 0x01 || ocr[3] != 0xAA) {
      debug_print("Voltage mismatch\r\n");
      return 2;
    }

    // Wait for end of initialization with ACMD41(HCS)
    do {
      ret = sd_cmd(dev, ACMD41, 1u << 30u);
    } while ((ret) && (timeout > get_system_millis()));
    debug_printf("Initialization done in %d ms.\r\n",
                 get_system_millis() - timeout + 1000);

    if (timeout <= get_system_millis()) {
      debug_print("Timeout reached!\r\n");
      return 3;
    }

    if (sd_cmd(dev, CMD58, 0) == 0) {
      ocr[0] = spi_xfer(dev->spi, 0xFF);
      ocr[1] = spi_xfer(dev->spi, 0xFF);
      ocr[2] = spi_xfer(dev->spi, 0xFF);
      ocr[3] = spi_xfer(dev->spi, 0xFF);
      *card_type = (ocr[0] & 0x40u) ? CT_SD2 | CT_BLOCK : CT_SD2;
      debug_printf("Card type: %d\r\n", *card_type);
    }

  } else {
    debug_print("Card is not supported\r\n");
    return 4;
  }
  sd_deselect(dev);

  // bump SPI speed
  spi_set_baudrate_prescaler(dev->spi, SPI_CR1_BR_FPCLK_DIV_2);
  return 0;
}

/**
 * Read single block from SD card
 */
uint8_t sd_read_block(const spi_device *dev, uint8_t *buffer, uint16_t length) {
  uint32_t timeout = get_system_millis() + 200; // 200 ms timeout
  uint8_t token;
  do {
    token = spi_xfer(dev->spi, 0xFF);
  } while ((token == 0xFF) && (timeout > get_system_millis()));
  if (token != 0xFE) {
    debug_print("TIMEOUT!!!\r\n");
    return 1;
  }

  // Using 16 bit mode is slightly faster
  spi_set_dff_16bit(dev->spi);
  uint16_t data;
  for (uint16_t cnt = 0; cnt < length; cnt += 2) {
    data = spi_xfer(dev->spi, 0xFFFF);
    buffer[cnt] = data >> 8u;
    buffer[cnt + 1] = (uint8_t)data;
  }

  // discard CRC
  spi_xfer(dev->spi, 0xFFFF);

  // return to 8 bit mode
  spi_set_dff_8bit(dev->spi);
  return 0;
}
