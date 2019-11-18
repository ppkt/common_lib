#include "i2c.h"

#ifdef STM32F1
#define _IF_SB(i2c) ((I2C_SR1(i2c) & I2C_SR1_SB) == 0)
#define _IF_BTF(i2c) ((I2C_SR1(i2c) & I2C_SR1_BTF) == 0)
#define _IF_ADDR(i2c) ((I2C_SR1(i2c) & I2C_SR1_ADDR) == 0)
#define _IF_TxE(i2c) (I2C_SR1(i2c) & I2C_SR1_TxE) == 0

error_t i2c_wait_for_start(uint32_t i2c);
error_t i2c_wait_for_address(uint32_t i2c);

void i2c1_init(enum i2c_speeds speed) {
  // Enable I2C clock
  rcc_periph_clock_enable(RCC_I2C1);

  // Enable GPIO clock
  rcc_periph_clock_enable(RCC_GPIOB);

  // Use PB6 and PB7
  gpio_set_mode(GPIO_BANK_I2C1_SCL | GPIO_BANK_I2C1_SDA,
                GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
                GPIO_I2C1_SCL | GPIO_I2C1_SDA);

  // Shut down peripheral for configuration
  i2c_reset(I2C1);
  i2c_peripheral_disable(I2C1);

  if (speed == i2c_speed_sm_100k) {
    i2c_set_standard_mode(I2C1);
  } else {
    i2c_set_fast_mode(I2C1);
  }

  // APB1 is running at 36MHz.
  i2c_set_speed(I2C1, speed, I2C_CR2_FREQ_36MHZ);
  i2c_peripheral_enable(I2C1);
}

error_t i2c_wait_for_address(uint32_t i2c) {
  uint16_t timeout = 0;
  while (!(I2C_SR1(i2c) & I2C_SR1_ADDR)) {
    timeout++;
    if (timeout == 0x08FF) {
      return E_I2C_TIMEOUT;
    }
  }
  return E_SUCCESS;
}

error_t i2c_wait_for_start(uint32_t i2c) {
  uint32_t i = 0;
  while (!((I2C_SR1(i2c) & I2C_SR1_SB) &
           (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY)))) {
    ++i;
    if (i == 0xFFFF) {
      return E_I2C_TIMEOUT;
    }
  }
  return E_SUCCESS;
}

void i2c_scan_bus(uint32_t i2c) {
  usart1_print("Begin scanning!\r\n");
  for (uint8_t addr = 0x01; addr < (0xFFu >> 1u); ++addr) {
    if (i2c_check_presence(i2c, addr)) {
      usart1_printf("Device found at addr 0x%.2x\r\n", addr);
    }
  }
  usart1_print("Scan completed!\r\n");
}

error_t i2c_master_transaction_write_read(uint32_t i2c, uint8_t slave_address,
                                          uint8_t *tx_buffer,
                                          uint32_t bytes_to_write,
                                          uint8_t *rx_buffer,
                                          uint32_t bytes_to_read) {
  error_t e = i2c_check_arguments(i2c, slave_address);
  i2c_transfer7(i2c, slave_address, tx_buffer, bytes_to_write, rx_buffer,
                bytes_to_read);
  return e;
}

error_t i2c_master_write(uint32_t i2c, uint8_t slave_address,
                         uint8_t *tx_buffer, uint32_t bytes_to_write) {
  error_t e = i2c_check_arguments(i2c, slave_address);
  i2c_transfer7(i2c, slave_address, tx_buffer, bytes_to_write, 0, 0);
  return e;
}

// Checks if device with provided address is present (sends ACK for address)
// returns `true` if device is present, `false` otherwise
bool i2c_check_presence(uint32_t i2c, uint8_t addr) {
  i2c_send_start(i2c);
  if (i2c_wait_for_start(i2c) == E_I2C_TIMEOUT) { return false; }
  i2c_send_7bit_address(i2c, addr, I2C_WRITE);
  error_t error = i2c_wait_for_address(i2c);
  i2c_send_stop(i2c);
  return error == E_SUCCESS;
}

error_t i2c_check_arguments(uint32_t i2c, uint8_t device_address) {
  if ((i2c != I2C1 && i2c != I2C2)) {
    return E_VALUE_INVALID;
  }
  else if (device_address >= 0x7F) {
    return E_VALUE_INVALID;
  }
  return E_SUCCESS;
}
#endif
