/*
 * Data collection and transmission sequence
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
#include "cron.h"
#include "gps.h"
#include "data.h"
#include "hw_config.h"
#include "watchdog.h"
#include "backlog.h"
#include "location.h"


void rtty_telemetry(struct tracker_datapoint* dp);
void contestia_telemetry(struct tracker_datapoint* dp);
void aprs_telemetry(struct tracker_datapoint* dp);
void pips_telemetry(void);


/**
 * Run telemetry sequence
 */
void telemetry_sequence(struct tracker_datapoint* dp, uint32_t n)
{
  (void)n;                      /* unused */

  /* Always update geofence */
  location_telemetry_update(dp->latitude, dp->longitude);
  location_aprs_update(dp->latitude, dp->longitude);

  /* Telemetry */
#if RF_TX_ENABLE
#if TELEMETRY_ENABLE
#if TELEMETRY_USE_GEOFENCE
  if (location_telemetry_active()) {
#endif

    /* Pips */
    pips_telemetry();

    /* Contestia */
    contestia_telemetry(dp);

#if TELEMETRY_USE_GEOFENCE
  }
#endif
#endif

  /* APRS */
#if APRS_ENABLE
#if APRS_USE_GEOFENCE
  if (location_aprs_active()) {
#endif

    /* APRS */
    aprs_telemetry(dp);

#if APRS_USE_GEOFENCE
  }
#endif
#endif
#endif
}


/**
 * Run sequence n
 */
void run_sequencer(uint32_t n)
{
  struct tracker_datapoint* dp;

  /* Async data */
  collect_data_async();

  /* Data */
  dp = collect_data();

  /* Telemetry  */
  telemetry_sequence(dp, n);

  /* Backlog */
  if ((n % 60) == 10) {         /* Every hour, start ten minutes */
    record_backlog(dp);
  }
}
