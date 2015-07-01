/*
 * Cron job for the system
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
#include "ubx_messages.h"
#include "data.h"
#include "hw_config.h"
#include "watchdog.h"
#include "backlog.h"

/* Internal time representation */
struct tracker_time time = {0};

/* Pointer to latest datapoint */
struct tracker_datapoint* dp;


void rtty_telemetry(struct tracker_datapoint* dp);
void contestia_telemetry(struct tracker_datapoint* dp);
void aprs_telemetry(struct tracker_datapoint* dp);
void pips_telemetry(void);


/**
 * Reads current time from the GPS
 */
void read_gps_time(void)
{
  /* GPS Time */
  gps_update_time();

  /* Sleep Wait */
  while (gps_update_time_pending()) {
    idle(IDLE_WAIT_FOR_GPS);
  }

  /* Time */
  struct ubx_nav_timeutc gt = gps_get_nav_timeutc();
  time.year = gt.payload.year;
  time.month = gt.payload.month;
  time.day = gt.payload.day;
  time.hour = gt.payload.hour;
  time.minute = gt.payload.min;
  time.second = gt.payload.sec;
  time.valid = gt.payload.valid;

  /* TODO calculate epoch time here */
}

/**
 * Cron job for the system.
 *
 * Run at the top of the second but may take longer than a second
 */
void do_cron(void)
{
  /* ---- Local representation of the time ---- */
  struct tracker_time t;
  memcpy(&t, &time, sizeof(struct tracker_time));

  /* ---- Data every 30 seconds ---- */
  if ((time.second % 30) == 0) {
    dp = collect_data();
    memcpy(&dp->time, &t, sizeof(struct tracker_time));
  } else if ((time.second % 30) == 20) {
    collect_data_async();
  }


  /* ---- Telemetry output ---- */
  /* RTTY */
  if (time.second == 0) {
    rtty_telemetry(dp);

    /* Contestia */
  } else if (time.second == 30) {
    contestia_telemetry(dp);

    /* APRS */ /* LIMIT CONTESTIA PACKET LENGTH */
#ifdef APRS_ENABLE
  } else if ((time.minute % 2) == 0 && time.second == 55) {
    aprs_telemetry(dp);
#endif
    /* Pips */
  } else if ((time.second % 1) == 0) {
    pips_telemetry();

  }

  /* ---- Record for backlog ---- */
  if ((time.minute == 0) && (time.second == 25)) {
    record_backlog(dp);
  }
}

/**
 * Called in an interrupt, increments internal time representation
 */
void cron_tick(void) {

  /* Update time internally */
  time.epoch++; time.second++;
  if (time.second > 60) {
    time.second = 0; time.minute++;
    if (time.minute > 60) {
      time.minute = 0; time.hour++;
      if (time.hour > 23) {
        time.hour = 0;
      }
    }
  }

  /* Update internal time from GPS */
  /* We do this just after midnight or if time is yet to set UTC exactly */
  if (((time.hour == 0) && (time.minute == 0) && (time.second == 5)) ||
      ((time.second == 5) && !(time.valid & UBX_TIMEUTC_VALID_UTC))) {

    /* Be careful not a call this while the collect_data function is running */
    read_gps_time();
  }
}
