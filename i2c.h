#pragma once
#include <stdint-gcc.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>

typedef struct i2c_device {
    uint32_t i2c;
    uint8_t address;
} i2c_device;

void i2c1_init(enum i2c_speeds speed);

bool i2c_master_transaction_write_read(
        uint32_t i2c, uint8_t slave_address, uint8_t *tx_buffer,
        uint32_t bytes_to_write, uint8_t *rx_buffer, uint32_t bytes_to_read);

bool i2c_master_write(uint32_t iwc, uint8_t slave_address, uint8_t *tx_buffer,
                      uint32_t bytes_to_write);

void i2c_scan_bus(uint32_t i2c);
bool i2c_check_presence(uint32_t i2c, uint8_t addr);
