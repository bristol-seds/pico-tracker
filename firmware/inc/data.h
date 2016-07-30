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
 * Size is approx 52 bytes
 */
typedef struct tracker_datapoint {
  /* Time */
  struct tracker_time time;

  /* Position */
  int32_t latitude;            	/* 100 nanodeg */
  int32_t longitude;           	/* 100 nanodeg */
  int32_t altitude;            	/* mm */
  uint8_t satillite_count;      /*  */
  uint8_t time_to_first_fix;    /* seconds / counts */

  /* Sensors */
  float battery;                /* Volts */
  float solar;                  /* Volts */
  int32_t main_pressure;        /* Pa */
  float thermistor_temperature; /* ºC */
  float bmp180_temperature;     /* ºC */
  float radio_die_temperature;  /* ºC */
  uint32_t xosc_error;          /* Hertz */

  /* system */
  uint8_t flash_status;

} tracker_datapoint;


uint32_t get_epoch_from_time(struct tracker_time *t);
void collect_data_async(void);
struct tracker_datapoint* collect_data(void);

#endif /* DATA_H */
