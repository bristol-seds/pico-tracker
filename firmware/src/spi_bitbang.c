/*
 * SPI bit-banging! Yay
 * Copyright (C) 2014  Richard Meadows <richardeoin>
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
#include "system/port.h"


/** Should be at least 20ns or something. Why am I bit-banging this? */
#define BIT_DELAY	__NOP

#define BIT_TOGGLE(bit)	do {			\
    PORTA.OUTTGL.reg = bit; } while (0)
#define BIT_SET(bit)	do {			\
    PORTA.OUTSET.reg = bit; } while (0)
#define BIT_CLEAR(bit)	do {			\
    PORTA.OUTCLR.reg = bit; } while (0)
#define BIT_READ(bit)	(PORTA.IN.reg & bit)

uint32_t mosi_mask = 0;
uint32_t miso_mask = 0;
uint32_t sck_mask = 0;

void spi_bitbang_init(const uint8_t mosi,
		      const uint8_t miso,
		      const uint8_t sck)
{
  /* Set the masks */
  mosi_mask = (1 << mosi);
  miso_mask = (1 << miso);
  sck_mask = (1 << sck);

  /* Configure the output pins */
  if (mosi == miso) { /* Loopback */
    port_pin_set_config(mosi,
			PORT_PIN_DIR_OUTPUT_WTH_READBACK,/* Direction */
			PORT_PIN_PULL_NONE,	/* Pull */
			false);			/* Powersave */
  } else { /* No loopback */
    port_pin_set_config(mosi,
			PORT_PIN_DIR_OUTPUT,	/* Direction */
			PORT_PIN_PULL_NONE,	/* Pull */
			false);			/* Powersave */
    port_pin_set_config(miso,
			PORT_PIN_DIR_INPUT,	/* Direction */
			PORT_PIN_PULL_UP,	/* Pull */
			false);			/* Powersave */
  }
  port_pin_set_config(sck,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */

  /* Set output pins to default values */
  BIT_SET(mosi_mask);
  BIT_CLEAR(sck_mask);
}
uint8_t spi_bitbang_transfer(uint8_t byte)
{
  for (uint8_t counter = 0; counter < 8; counter++) {
    /* Set output data */
    if (byte & 0x80) {
      BIT_SET(mosi_mask);
    } else {
      BIT_CLEAR(mosi_mask);
    }
    byte <<= 1;
    BIT_DELAY();

    /* Latch Data into Slave */
    BIT_TOGGLE(sck_mask);
    BIT_DELAY();

    /* Read Data */
    if (BIT_READ(miso_mask)) {
      byte |= 0x1;
    }

    /* Slave shifts out next data bit */
    BIT_TOGGLE(sck_mask);
  }

  return byte;
}
