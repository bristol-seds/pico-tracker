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
#include "xosc.h"
#include "hw_config.h"
#include "analogue.h"
#include "barometer.h"
#include "battery.h"
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
 * Collect data asynchronously. Should be run a few seconds before the collect_data routine
 */
void collect_data_async(void)
{
#ifdef GPS_TYPE_UBX
  /* Ask GPS for latest fix */
  gps_update_position();
#endif  /* GPS_TYPE_UBX */

  /* Measure XOSC against gps timepulse */
  /* single shot, ongoing until two rising edges on GPS TIMEPULSE */
  /* NOT IN USE CURRENTLY */
  //measure_xosc(XOSC_MEASURE_TIMEPULSE, xosc_measure_callback, 1);

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

  /**
   * ---- Battery ----
   */
  update_battery(&datapoint);

#ifdef GPS_TYPE_UBX
  /**
   * ---- GPS UBX ----
   */
  if (gps_update_position_pending() || (gps_get_error_state() != GPS_NOERROR)) {
    /* Error updating GPS position */

    /* TODO: Hit reset on the GPS? */
    /* In the meantime just wait for the watchdog */
    while (1);

  } else {                      /* GPS position updated correctly */

    /* GPS Status */

    struct ubx_nav_sol sol = gps_get_nav_sol();
    datapoint.satillite_count = sol.payload.numSV;
    datapoint.time_to_first_fix = 0;

    /* GPS Position */
    if (gps_is_locked()) {
      struct ubx_nav_posllh pos = gps_get_nav_posllh();

      datapoint.latitude = pos.payload.lat;
      datapoint.longitude = pos.payload.lon;
      datapoint.altitude = pos.payload.height;
    }

    /* GPS Powersave */
    gps_set_powersave_auto();
  }
#endif  /* GPS_TYPE_UBX */

  return &datapoint;
}
