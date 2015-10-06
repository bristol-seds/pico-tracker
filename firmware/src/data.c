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
#include "gps.h"
#include "ubx_messages.h"
#include "telemetry.h"
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
  /* Ask GPS for latest fix */
  gps_update_position();

  /* Measure XOSC against gps timepulse */
  measure_xosc(XOSC_MEASURE_TIMEPULSE, xosc_measure_callback);

  /* Analogue Measurements */
  start_adc_sequence();
}
/**
 * Collects data synchronously and return datapoint
 */
struct tracker_datapoint* collect_data(void)
{
  /**
   * ---- Analogue ----
   */
  datapoint.battery = get_battery(); /* Will return zero by default */
  datapoint.solar = get_solar();     /* Will return zero by default */
  datapoint.temperature = telemetry_si_temperature();

  /**
   * ---- GPS ----
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

  return &datapoint;
}
