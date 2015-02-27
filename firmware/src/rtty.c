/*
 * Bit-bangs RTTY
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

#include <string.h>

#include "samd20.h"
#include "rtty.h"
#include "hw_config.h"
#include "system/port.h"

/**
 * Interface to the physical world.
 */
#define RTTY_ACTIVATE()
#define RTTY_DEACTIVATE()
#define RTTY_SET(b)	    port_pin_set_output_level(SI406X_GPIO1_PIN, !b);
#define RTTY_NEXT()

/**
 * Formatting 8N2
 */
#define ASCII_BITS	8
#define BITS_PER_CHAR	11

/**
 * Current output data
 */
uint8_t rtty_data;
/**
 * Where we currently are in the rtty output byte
 *
 * 0 = Start Bit
 * 1-8 = Data Bit
 * 10 = Stop Bit
 * 11 = Stop Bit
 */
uint8_t rtty_phase = 0xFE;

void rtty_start(uint8_t data) {
  /* Start transmission */
  rtty_phase = 0;
  rtty_data = data;
}

/**
 * Called at the baud rate, outputs bits of rtty
 */
uint8_t rtty_tick(void) {

  if (rtty_phase == 0) {			/* *** Start *** */
    RTTY_SET(0);

  } else if (rtty_phase < ASCII_BITS + 1) {	/* *** Data *** */
    if ((rtty_data >> (rtty_phase - 1)) & 1) {
      RTTY_SET(1);
    } else {
      RTTY_SET(0);
    }

  } else if (rtty_phase < BITS_PER_CHAR) {	/* *** Stop *** */
    RTTY_SET(1);

  } else {					/* *** Not running *** */
    return 0;
  }

  rtty_phase++;

  if (rtty_phase < BITS_PER_CHAR) {
    return 1;
  }

  /* if (rtty_phase >= BITS_PER_CHAR) { // Next character */
  /*   rtty_phase = 0; rtty_index++; RTTY_NEXT(); */

  /*   if (rtty_index >= rtty_string_length) { // All done, deactivate */
  /*     rtty_string_length = 0; // Deactivate */
  /*   } */
  /* } */

  return 0;
}
