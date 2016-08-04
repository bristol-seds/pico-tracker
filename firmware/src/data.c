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

#include <string.h>

#include "samd20.h"
#include "data.h"
#include "cron.h"
#include "xosc.h"
#include "hw_config.h"
#include "analogue.h"
#include "barometer.h"
#include "gps.h"
#include "ubx_messages.h"
#include "telemetry.h"
#include "thermistor.h"
#include "watchdog.h"

struct tracker_datapoint datapoint = {.time={0}};

void xosc_measure_callback(uint32_t result)
{
  datapoint.xosc_error = result - XOSC_FREQUENCY;
}

/**
 * Gets epoch from year/month/day/hour/minute/seconds time structure.
 */
uint32_t get_epoch_from_time(struct tracker_time *t)
{
  uint32_t days = 0;
  int i;

  /* collect years */
  for (i = 1970; i < t->year; i++) {
    days += (i % 4 == 0) ?
      ((i % 100 == 0) ?
       ((i % 400 == 0) ? 366        /* div 400, leap            */
        : 365)                      /* div 100, not 400, common */
       : 366)                       /* div 4, not 100, leap     */
      : 365;                        /* Not div 4, common        */
  }
  /* collect months */
  uint8_t t_month = t->month;
  for (i = 1; i < t_month; i++) {
    t->month = i;
    days += days_in_month(t);
  }
  /* collect days */
  days += t->day-1;

  return (((((days*24)+t->hour)*60)+t->minute)*60)+t->second;
}

/**
 * Collect data asynchronously. Should be run a few seconds before the collect_data routine
 */
void collect_data_async(void)
{
#ifdef GPS_TYPE_UBX
  /* Ask GPS for latest time and fix */
  gps_update_time();
  gps_update_position();
#endif  /* GPS_TYPE_UBX */

#if MEASURE_XOSC
  /* Measure XOSC against gps timepulse */
  /* single shot, ongoing until two rising edges on GPS TIMEPULSE */
  measure_xosc(XOSC_MEASURE_TIMEPULSE, xosc_measure_callback, 1);
#endif

  /* Analogue Measurements */
  start_adc_sequence();
}
/**
 * Collects data synchronously and return datapoint
 */
struct tracker_datapoint* collect_data(void)
{
#if defined(GPS_TYPE_OSP) || defined(GPS_TYPE_DUMMY)
  /**
   * ---- GPS OSP ----
   */

  struct gps_data_t data = gps_get_data_wrapped(); /* use wrapped function that re-inits GPS as needed */

  datapoint.latitude = data.latitude; /* hndeg */
  datapoint.longitude = data.longitude; /* hdeg */
  datapoint.altitude = data.altitude;   /* mm */
  datapoint.satillite_count = data.satillite_count;
  datapoint.time_to_first_fix = data.time_to_first_fix; /* seconds / counts */

  datapoint.time.year = data.year;
  datapoint.time.month = data.month;
  datapoint.time.day = data.day;
  datapoint.time.hour = data.hour;
  datapoint.time.minute = data.minute;
  datapoint.time.second = data.second; /* seconds */

  /* calculate epoch */
  datapoint.time.epoch = get_epoch_from_time(&datapoint.time);

#endif /* GPS_TYPE_OSP */


  /**
   * ---- Analogue ----
   */
  while (is_adc_sequence_done() == 0); /* wait for adc */
  datapoint.battery = get_battery(); /* will return zero by default */
  datapoint.solar = get_solar();     /* will return zero by default */
  datapoint.radio_die_temperature = telemetry_si_temperature();
  datapoint.thermistor_temperature = thermistor_ratio_to_temperature(get_thermistor());

  /**
   * ---- Barometer ----
   */
#if USE_BAROMETER
  struct barometer* b = get_barometer();
  datapoint.main_pressure = b->pressure;
  datapoint.bmp180_temperature = (float)b->temperature;
#endif

#ifdef GPS_TYPE_UBX
  /**
   * ---- GPS UBX ----
   */
  /* wait for GPS, if it takes forever the watchdog will save us */
  while (gps_update_time_pending() || gps_update_position_pending()) {
    idle(IDLE_WAIT_FOR_GPS);
  }


  if (gps_get_error_state() != GPS_NOERROR) {
    /* Error updating GPS position */

    /* TODO: Hit reset on the GPS? */
    /* In the meantime just wait for the watchdog */
    while (1);

  } else {                      /* GPS position updated correctly */

    struct gps_data_t data = gps_get_data();

    datapoint.latitude = data.latitude; /* hndeg */
    datapoint.longitude = data.longitude; /* hdeg */
    datapoint.altitude = data.altitude;   /* mm */
    datapoint.satillite_count = data.satillite_count;
    datapoint.time_to_first_fix = data.time_to_first_fix; /* seconds / counts */

    datapoint.time.year = data.year;
    datapoint.time.month = data.month;
    datapoint.time.day = data.day;
    datapoint.time.hour = data.hour;
    datapoint.time.minute = data.minute;
    datapoint.time.second = data.second; /* seconds */

    /* calculate epoch */
    datapoint.time.epoch = get_epoch_from_time(&datapoint.time);

    /* GPS Powersave */
    gps_set_powersave_auto();
  }
#endif  /* GPS_TYPE_UBX */

  return &datapoint;
}
