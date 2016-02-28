/*
 * Accumulator provides averaged datapoints. May overflow with more than 2^24 readings.
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

#include "samd20.h"
#include "data.h"

/* Accumulator values. Ensure storage type has at least 24 more bits than type in dp */
int64_t altitude_accumulator;
uint32_t satillite_count_accumulator;
uint32_t time_to_first_fix_accumulator;
uint64_t xosc_error_accumulator;

/* Count */
uint32_t accumulator_count = 0;

/**
 * Adds a datapoint to the accumulator.
 */
void accumulator_add(struct tracker_datapoint* dp)
{
  /* ensure reset of accumulator */
  if (accumulator_count == 0) {
    altitude_accumulator = 0LL;
    satillite_count_accumulator = 0;
    time_to_first_fix_accumulator = 0;
    xosc_error_accumulator = 0LL;
  }

  /* add values */
  altitude_accumulator += dp->altitude;
  satillite_count_accumulator += dp->satillite_count;
  time_to_first_fix_accumulator += dp->time_to_first_fix;
  xosc_error_accumulator += dp->xosc_error;

  /* increment count */
  accumulator_count++;
}
/**
 * Populates datapoint with averages from value submitted to the accumulator.
 * Also resets the accumulator.
 */
void accumulator_read(struct tracker_datapoint* dp)
{
  /* calculte average */
  if (accumulator_count > 0) {
    dp->altitude = (int32_t)(altitude_accumulator / accumulator_count);
    dp->satillite_count = (uint8_t)(satillite_count_accumulator / accumulator_count);
    dp->time_to_first_fix = (uint8_t)(time_to_first_fix_accumulator / accumulator_count);
    dp->xosc_error = (uint32_t)(xosc_error_accumulator / accumulator_count);
  }

  /* reset */
  accumulator_count = 0;
  altitude_accumulator = 0LL;
  satillite_count_accumulator = 0;
  time_to_first_fix_accumulator = 0;
  xosc_error_accumulator = 0LL;
}
