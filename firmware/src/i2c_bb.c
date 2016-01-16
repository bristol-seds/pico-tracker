/*
 * i2c bit-bang
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

#include "samd20.h"
#include "hw_config.h"
#include "system/port.h"
#include "i2c_bb.h"

#define SDA I2C_SERCOM_SDA_PIN
#define SCL I2C_SERCOM_SCL_PIN

/**
 * Helper functions for pin access
 */
static inline void i2c_bb_claim_pin(uint32_t pin, uint8_t value)
{
  port_pin_set_output_level(pin, value);
  port_pin_set_config(pin,
                      SYSTEM_PINMUX_PIN_DIR_OUTPUT_WITH_READBACK, /* Direction */
                      SYSTEM_PINMUX_PIN_PULL_NONE,  /* Pull */
                      false);                       /* Powersave */
}
static inline void i2c_bb_release_pin(uint32_t pin)
{
  port_pin_set_config(pin,
                      SYSTEM_PINMUX_PIN_DIR_INPUT, /* Direction */
                      SYSTEM_PINMUX_PIN_PULL_NONE, /* Pull */
                      false);                      /* Powersave */
}
#define i2c_bb_write_pin port_pin_set_output_level
#define i2c_bb_read_pin port_pin_get_input_level



/**
 * i2c start
 */
void i2c_bb_start(void)
{
  i2c_bb_claim_pin(SDA, 1);     /* force pins high */
  i2c_bb_claim_pin(SCL, 1);

  i2c_bb_write_pin(SDA, 0);     /* set SDA low */
  i2c_bb_write_pin(SCL, 0);     /* then SCL */

  i2c_bb_write_pin(SDA, 1);     /* return SDA high */
}
/**
 * i2c stop
 */
void i2c_bb_stop(void)
{
  i2c_bb_write_pin(SCL, 0);       /* set both lines low */
  i2c_bb_write_pin(SDA, 0);

  i2c_bb_release_pin(SCL);        /* release SCL */
  i2c_bb_release_pin(SDA);        /* then SDA */
}

/**
 * writes a single i2c byte
 */
void i2c_bb_put_byte(uint8_t b)
{
  uint8_t bit;

  for (uint8_t i = 0; i < 8; i++) {
    bit = (b & 0x80) ? 1:0;     /* get msb */
    b = b << 1;

    if (i == 0) {
      i2c_bb_claim_pin(SDA, bit);
    } else {
      i2c_bb_write_pin(SDA, bit);
    }

    i2c_bb_write_pin(SCL, 1);   /* clock out */
    if ((i == 7) && bit) { i2c_bb_release_pin(SDA); } /* release early if high */
    i2c_bb_write_pin(SCL, 0);
  }

  i2c_bb_release_pin(SDA);      /* release SDA pin */
}
uint8_t i2c_bb_get_byte(void)
{
  uint8_t bit, b = 0;

  i2c_bb_release_pin(SDA);      /* release SDA */

  for (uint8_t i = 0; i < 8; i++) {

    i2c_bb_write_pin(SCL, 1);   /* clock in */
    bit = i2c_bb_read_pin(SDA);
    i2c_bb_write_pin(SCL, 0);

    b |= (bit?1:0);             /* write msb first */
    if (i < 7) {                /* all except last bit */
      b = b << 1;
    }
  }

  return b;
}

/**
 * Issues i2c acknoledge
 */
void i2c_bb_ack(void)
{
  i2c_bb_claim_pin(SDA, 0);
  i2c_bb_write_pin(SCL, 1);     /* clock out */
  i2c_bb_write_pin(SCL, 0);
  i2c_bb_write_pin(SDA, 1);
}
/**
 * Issues i2c not acknoledge
 */
void i2c_bb_nack(void)
{
  i2c_bb_claim_pin(SDA, 1);
  i2c_bb_write_pin(SCL, 1);     /* clock out */
  i2c_bb_write_pin(SCL, 0);
}
/**
 * Waits to receive a slave ack. SDA is already released
 */
i2c_bb_result_t i2c_bb_get_ack(void)
{
  uint8_t i;

  i2c_bb_write_pin(SCL, 1);

  for (i = 0; i < 10; i++) {                  /* 10 attempts to */
    if (i2c_bb_read_pin(SDA) == 0) { break; } /* wait for SDA=0 */
  }

  i2c_bb_write_pin(SCL, 0);

  if (i == 10) {                /* fail */
    return I2C_BB_SLAVE_NO_ACK; /* no ack */
  }

  return I2C_BB_SUCCESS;
}

/**
 * I2C Read.
 *
 * address is the full read address like 0xEF
 */
i2c_bb_result_t i2c_bb_read(uint8_t address, uint8_t* data, uint8_t data_length)
{
  i2c_bb_result_t result;

  address |= 1;                 /* set read flag */

  i2c_bb_start();               /* start. claim both */
  i2c_bb_put_byte(address);     /* address. claim and relase SDA */
  if ((result = i2c_bb_get_ack()) != I2C_BB_SUCCESS) { /* slave acks */
    return result;
  }

  for (uint8_t n = 0; n < data_length; n++) {

    data[n] = i2c_bb_get_byte(); /* read byte. release SDA */
    if (n+1 < data_length) {     /* if not last byte */
      i2c_bb_ack();              /* ack. claim SDA */
    } else {
      i2c_bb_nack();            /* nack. claim SDA */
    }
  }

  i2c_bb_stop();                /* stop. release both */

  return I2C_BB_SUCCESS;
}
/**
 * I2C Write.
 *
 * address is the full write address like 0xEE
 */
i2c_bb_result_t i2c_bb_write(uint8_t address, uint8_t* data, uint8_t data_length)
{
  i2c_bb_result_t result;

  address &= ~1;                /* clear read flag */

  i2c_bb_start();               /* start. claim both */
  i2c_bb_put_byte(address);     /* address. claim and release SDA */
  if ((result = i2c_bb_get_ack()) != I2C_BB_SUCCESS) { /* slave acks */
    return result;
  }

  for (uint8_t n = 0; n < data_length; n++) {
    i2c_bb_put_byte(data[n]);   /* data. claim and release SDA */
    if ((result = i2c_bb_get_ack()) != I2C_BB_SUCCESS) { /* slave acks */
      return result;
    }
  }

  i2c_bb_claim_pin(SDA, 0);     /* claim SDA again */
  i2c_bb_stop();                /* stop */

  return I2C_BB_SUCCESS;
}



void i2c_bb_init(void)
{
  i2c_bb_claim_pin(SDA, 1);
  i2c_bb_claim_pin(SCL, 0);

  i2c_bb_stop();                /* stop. release both */
}
