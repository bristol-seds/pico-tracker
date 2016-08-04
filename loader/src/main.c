/*
 * Bristol SEDS pico-tracker
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

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "samd20.h"
#include "hw_config.h"
#include "init.h"
#include "watchdog.h"
#include "analogue.h"
#include "xosc.h"
#include "loader.h"

/**
 * MAIN
 * =============================================================================
 */
int main(void)
{
  /* Init */
  init(INIT_NORMAL);

  /* Stay in low power mode until power/temperature are high enough */
  do {
    led_off();
    idle(IDLE_LOADER);
    led_on();

    /* Read sensors */
    start_adc_sequence();
    while (is_adc_sequence_done() == 0) {
      idle(IDLE_LOADER);
    }

    /* Check battery */
  } while (get_battery() < 4.0);


  /* Check and repair memory */


  /* Transfer control to application */
  transfer_to_application();

  /* Should never get here */
  while (1) {
    idle(IDLE_LOADER);
  }
}
