/*
 * Collects data from sensors etc into a struct
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

#ifndef DATA_H
#define DATA_H

#include "samd20.h"
#include "cron.h"

/**
 * Structure for all the information in each datapoint.
 *
 * Size is approx 40 bytes
 */
typedef struct tracker_datapoint {
  /* Time */
  struct tracker_time time;

  /* Position */
  uint32_t latitude;            /* 100 nanodeg */
  uint32_t longitude;           /* 100 nanodeg */
  uint32_t altitude;            /* mm */
  uint8_t satillite_count;      /*  */

  /* Sensors */
  float battery;                /* Volts */
  float solar;                  /* Volts */
  float temperature;            /* ºC */
  uint32_t xosc_error;          /* Hertz */

} tracker_datapoint;


void collect_data_async(void);
struct tracker_datapoint* collect_data(void);

#endif /* DATA_H */
