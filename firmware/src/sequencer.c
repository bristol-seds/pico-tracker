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
  /* Always update geofence */
  telemetry_location_update(dp->longitude, dp->latitude);

#ifdef TELEMETRY_USE_GEOFENCE
  if (telemetry_location_tx_allow()) {
#endif

      /* Contestia */
    /* if (t->second == TELEM_TOM) { */
    /*   contestia_telemetry(dp); */

    /*   /\* Pip *\/ */
    /* } else if ((t->second % 1) == 0) { */
    /*   pips_telemetry(); */
    /* } */

#ifdef TELEMETRY_USE_GEOFENCE
  }
#endif



  /* APRS */
#ifdef APRS_ENABLE

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
