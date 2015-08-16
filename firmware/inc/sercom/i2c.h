/*
 * A wrapper around the samd20 i2c functions. Single master only
 * Copyright (C) 2015  Richard Meadows <richardeoin>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef I2C_H
#define I2C_H

#include "sercom/i2c_master.h"

/**
 * We just declare our single instance here because. Very naughty
 */
struct i2c_master_module i2c_master_instance;

/**
 * I2C Write.
 *
 * address is the full write address like 0xEE
 */
static void i2c_master_write(uint8_t address, uint8_t* data, uint16_t data_length)
{
  uint32_t timeout = 0;
  struct i2c_master_packet packet = {
    .address     = address >> 1,
    .data_length = data_length,
    .data        = data,
    .ten_bit_address = false,
    .high_speed      = false,
    .hs_master_code  = 0x0,
  };

  while (i2c_master_write_packet_wait(&i2c_master_instance, &packet) !=
         STATUS_OK) {
    /* Increment timeout counter and check if timed out. */
    if (timeout++ > 1000) {
      break;
    }
  }
}


/**
 * I2C Read.
 *
 * address is the full write address like 0xEE
 */
static void i2c_master_read(uint8_t address, uint8_t* data, uint16_t data_length)
{
  uint32_t timeout = 0;
  struct i2c_master_packet packet = {
    .address     = address >> 1,
    .data_length = data_length,
    .data        = data,
    .ten_bit_address = false,
    .high_speed      = false,
    .hs_master_code  = 0x0,
  };

  while (i2c_master_read_packet_wait(&i2c_master_instance, &packet) !=
         STATUS_OK) {
    /* Increment timeout counter and check if timed out. */
    if (timeout++ > 1000) {
      break;
    }
  }
}


/**
 * I2C bus master.
 */
static void i2c_init(void)
{
  struct i2c_master_config config_i2c_master;
  i2c_master_get_config_defaults(&config_i2c_master);

  /* Config */
  config_i2c_master.buffer_timeout = 10000;
  config_i2c_master.baud_rate = 10;

  /* Pinmux */
  config_i2c_master.pinmux_pad0 = PINMUX_PA04D_SERCOM0_PAD0;
  config_i2c_master.pinmux_pad1 = PINMUX_PA05D_SERCOM0_PAD1;

  /* Initialize and enable device with config. */
   i2c_master_init(&i2c_master_instance, SERCOM0, &config_i2c_master);
   i2c_master_enable(&i2c_master_instance);
}

#endif /* I2C_H */
