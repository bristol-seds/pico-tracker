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
#include "accumulator.h"
#include "battery.h"
#include "rtc.h"


void rtty_telemetry(struct tracker_datapoint* dp);
void contestia_telemetry(struct tracker_datapoint* dp);
void aprs_telemetry(struct tracker_datapoint* dp, uint32_t n);
void ariss_telemetry(struct tracker_datapoint* dp);
void pips_telemetry(void);


/**
 * Run telemetry sequence
 */
void telemetry_sequence(struct tracker_datapoint* dp, uint32_t n)
{
  /* Always update geofence */
  location_telemetry_update(dp->latitude, dp->longitude);
  location_aprs_update(dp->latitude, dp->longitude);
  kick_the_watchdog();          /* might take time */
  location_prefix_update(dp->latitude, dp->longitude);
  kick_the_watchdog();

  /* Telemetry */
#if RF_TX_ENABLE
#if TELEMETRY_ENABLE
#if TELEMETRY_USE_GEOFENCE
  if (location_telemetry_active() ||      /* in geofence */
      (gps_is_locked() == GPS_NO_LOCK)) { /* or no lock  */
#endif

    /* Pips */
    pips_telemetry();

    /* Contestia */
    contestia_telemetry(dp);

#if TELEMETRY_USE_GEOFENCE
  }
#endif
#endif  /* TELEMETRY_ENABLE */


  /* CEASE APRS AT THE END OF 2016 */
  if (dp->time.year > 2016) { return; }

  /* APRS */
#if APRS_ENABLE
  if (get_since_aprs_s() >= 60) { /* limit APRS frequency to once per minute */
    clear_since_aprs_s();
#if APRS_USE_GEOFENCE
    if (location_aprs_should_tx()) { /* transmit only when we *should* */
#endif

      /* APRS */
      aprs_telemetry(dp, n);

#if APRS_USE_GEOFENCE
    }
#endif

    /* CEASE ARISS AT THE END OF SEPTEMBER */
    if (dp->time.month > 9) { return; }

    /* ARISS */
#if ARISS_ENABLE
    if ((get_battery_use_state() == BATTERY_GOOD) &&        /* battery good, */
        (get_battery_charge_state() == BATTERY_CHARGING) && /* receiving power and */
        ((n % 3) == 0)) {                                   /* one-in-three times */
#if ARISS_USE_GEOFENCE
      if (location_aprs_could_tx()) { /* transmit anywhere it's not disallowed */
#endif

        /* ARISS */
        ariss_telemetry(dp);

#if ARISS_USE_GEOFENCE
      }
#endif
    }
#endif  /* ARISS_ENABLE */
  }
#endif  /* APRS_ENABLE */


#endif  /* RF_TX_ENABLE */
}


/**
 * Run sequence n
 */
void run_sequencer(uint32_t n, uint32_t cycle_time_s)
{
  struct tracker_datapoint* dp;

  /* Async data */
  collect_data_async();

  /* Data */
  dp = collect_data();

  /* Telemetry  */
  telemetry_sequence(dp, n);

  /* Backlog */
  if ((gps_is_locked() == GPS_LOCKED) && /* gps is locked. we can use this data */
      (cycle_time_s > 0)) {              /* and an actual cycle */

    /* Accumulator for backlog */
    accumulator_add(dp);

    /* Record */
    uint32_t rate = 3600 / cycle_time_s; /* once per hour */
    if ((n % rate) == 0) {

      /* replace some values from this sample with averages */
      accumulator_read(dp);

      record_backlog(dp);
    }
  }

  /* Battery */
  update_battery(dp);
}
