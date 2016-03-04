/*
 * Monitors xosc drift while transmitting
 * Copyright (C) 2016  Richard Meadows <richardeoin>
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

#include <math.h>

#include "samd20.h"
#include "hw_config.h"
#include "si_trx.h"
#include "xosc.h"
#include "drift.h"

/**
 * Initial reference, in Hz
 */
uint32_t initial_xosc_frequency;
/**
 * Current offset, in Hz at the xosc output
 * +ve if xosc too fast, -ve if xosc too slow
 */
float current_frequency_offset;
/**
 * Current frequency configuration
 */
struct si_frequency_configuration* _fconfig = NULL;
/**
 * Current offset, in rf channels for the current frequency configuration
 * +ve if xosc too fast, -ve if xosc too slow
 */
volatile int16_t current_channel_offset;


/**
 * Moving average
 */
#define MOVING_AVERAGE_N	(8)
int32_t frequency_moving_average[MOVING_AVERAGE_N];
uint32_t frequency_moving_average_index = 0;


/**
 * The result is the xosc frequency in Hz
 */
void xosc_drift_measure_callback(uint32_t result)
{
  int32_t offset;

  if (initial_xosc_frequency == 0) { /* Initial reference */
    initial_xosc_frequency = result;
    offset = 0;
    frequency_moving_average[0] = 0;
    frequency_moving_average_index++;
  } else {
    offset = result - initial_xosc_frequency;

    /* add to moving average */
    frequency_moving_average[(frequency_moving_average_index % 8)] = offset;

    /* calculate average */
    uint32_t n = (frequency_moving_average_index > 8) ? 8: frequency_moving_average_index;
    float sum = 0;
    for (uint32_t i = 0; i < n; i++) {
      sum += frequency_moving_average[i];
    }

    /* record result */
    current_frequency_offset = sum / n;
    frequency_moving_average_index++;
  }

  if (_fconfig != NULL) {       /* if we seem to have a fconfig */
    float rfband_offset_hz = ((float)current_frequency_offset * _fconfig->frequency)
      / XOSC_FREQUENCY;

    float channel_offset_f = rfband_offset_hz / _fconfig->lsb_tuning_resolution;
    current_channel_offset = (int16_t)round(channel_offset_f);
  }
}

/**
 * Returns the current channel delta caused by drift.
 *
 * The subtract this from the intended channel to compensate.
 */
int16_t drift_get_channel_offset(void)
{
  return current_channel_offset;
}


/**
 * Set the current frequency configuration.
 *
 * drift_get_channel_offset will always return zero if this is not set
 */
void drift_set_fconfig(struct si_frequency_configuration* config)
{
  _fconfig = config;
  current_channel_offset = 0;
}
/**
 * Start xosc drift measurements
 */
void drift_measure_start(void)
{
  initial_xosc_frequency = 0;
  current_frequency_offset = 0;
  current_channel_offset = 0;
  frequency_moving_average_index = 0;

  /* start ontinuous measurement (oneshot=0) */
  measure_xosc(XOSC_MEASURE_LFTIMER, xosc_drift_measure_callback, 0);
}
/**
 * Stop xosc drift measurements
 */
void drift_measure_stop(void)
{
  /* clear offsets to zero while we're stopped */
  initial_xosc_frequency = 0;
  current_frequency_offset = 0;
  current_channel_offset = 0;

  /* stop measurement */
  measure_xosc_disable(XOSC_MEASURE_LFTIMER);
}
