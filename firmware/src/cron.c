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
#include "location.h"

/* Internal time representation */
struct tracker_time time = {0};

volatile uint32_t ticks = 0;

/* Indicates telemetry wakeup hysteresis. We wake up immediately */
uint8_t has_telemetry_woken_up = 0;
#define TELEMETRY_WAKEUP_TEST(d)	(1)

/* Pointer to latest datapoint */
struct tracker_datapoint* dp;

/* Low Power Mode */
#define LOW_POWER(d)	(d->solar < 0.2)

void rtty_telemetry(struct tracker_datapoint* dp);
void contestia_telemetry(struct tracker_datapoint* dp);
void aprs_telemetry(struct tracker_datapoint* dp);
void pips_telemetry(void);

/**
 * For GPS time timeout
 */
uint32_t ticks_delta_start;

/**
 * Number of days in month. This won't be used much but I guess I have
 * to implement it. Sigh
 *
 * Assumes days and months start at 1 (ubx does do this, I checked)
 */
uint8_t days_in_month(struct tracker_time* t)
{
  switch (t->month) {
    case 1:  return 31;         /* Janua */
    case 2:
      return (t->year % 4) ?
        ((t->year % 100) ?
         ((t->year % 400) ? 29  /* div 400, leap            */
          : 28)                 /* div 100, not 400, common */
         : 29)                  /* div 4, not 100, leap     */
        : 28;                   /* Not div 4, common        */
    case 3:  return 31;         /* March */
    case 4:  return 30;         /* April */
    case 5:  return 31;         /* May   */
    case 6:  return 30;         /* June  */
    case 7:  return 31;         /* July  */
    case 8:  return 31;         /* Augus */
    case 9:  return 30;         /* Septe */
    case 10: return 31;         /* Octob */
    case 11: return 30;         /* Novem */
    case 12: return 31;         /* Decem */
    default: return 31;         /* It's probably 31 */
  }
}

/**
 * Returns the number of ticks the current cron job has been running for
 *
 * ticks = seconds. Can be used for timeouts etc.
 */
uint32_t cron_current_job_ticks(void)
{
  return ticks;
}

/**
 * Reads current time from the GPS
 */
void read_gps_time(void)
{
  /* Record current ticks */
  ticks_delta_start = cron_current_job_ticks();

  /* GPS Time */
  gps_update_time();

  /* Sleep Wait. Timeout after 3 ticks */
  while (gps_update_time_pending() &&
         (cron_current_job_ticks() - ticks_delta_start) <= 3) {

    idle(IDLE_WAIT_FOR_NEXT_TELEMETRY);
  }

  /* If no error and no timeout */
  if ((gps_get_error_state() == GPS_NOERROR) &&
      (cron_current_job_ticks() - ticks_delta_start) <= 3) {

    /* Time */
#ifdef GPS_TYPE_UBX
    struct ubx_nav_timeutc gt = gps_get_nav_timeutc();
    time.year = gt.payload.year;
    time.month = gt.payload.month;
    time.day = gt.payload.day;
    time.hour = gt.payload.hour;
    time.minute = gt.payload.min;
    time.second = gt.payload.sec;
    time.valid = gt.payload.valid;
#endif

    /* TODO calculate epoch time here */

    /* Zero out ticks as we have just set the correct time */
    ticks = 0;
  }
}

/**
 * Pars of cron job that handles telemetry
 */
#define TELEM_TOM	30       /* Telemetry on the 30th second */

void cron_telemetry(struct tracker_time* t, struct tracker_datapoint* dp)
{
#ifdef TELEMETRY_USE_GEOFENCE
  /* ---- Telemetry output ---- */
  if (telemetry_location_tx_allow()) {
#endif

      /* Contestia */
    if (t->second == TELEM_TOM) {
      contestia_telemetry(dp);

      /* Pip */
    } else if ((t->second % 1) == 0) {
      pips_telemetry();
    }

#ifdef TELEMETRY_USE_GEOFENCE
  }
#endif

  /* APRS */
#ifdef APRS_ENABLE
  if ((t->minute % 2) == 0 && t->second == TELEM_TOM) {
    aprs_telemetry(dp);
  }
#endif

  /* ---- Update telemetry geofence ---- */
  if ((t->minute % 5 == 0) && (t->second == 0)) { /* Every 5 minutes */

    if (gps_is_locked()) { /* Don't bother with no GPS */

      telemetry_location_update(dp->latitude, dp->longitude);
    }
  }
}
/**
 * Cron job for the system.
 *
 * Run at the top of the second but may take longer than a second
 */
void do_cron(void)
{
  /* ---- Local representation of the time ---- */
  while (ticks) {
    /* Update time internally */
    ticks--; time.epoch++; time.second++;
    if (time.second >= 60) {
      time.second = 0; time.minute++;
      if (time.minute >= 60) {
        time.minute = 0; time.hour++;
        if (time.hour >= 24) {
          time.hour = 0; time.day++;
          if (time.day > days_in_month(&time)) {
            time.day = 1; time.month++;
            if (time.month > 12) {
              time.month = 0; time.year++;
            }
          }
        }
      }
    }
  }

  /* ---- Data every 30 seconds ---- */
  if (time.second == TELEM_TOM) {
    dp = collect_data();
    memcpy(&dp->time, &time, sizeof(struct tracker_time));
  } else if (time.second == ((TELEM_TOM + 55)%60)) { /* 5 seconds for async things */
    collect_data_async();
  }

  /* ---- Telemetry output ---- */
  if (has_telemetry_woken_up > 0) {
    cron_telemetry(&time, dp);
  } else {
    has_telemetry_woken_up = TELEMETRY_WAKEUP_TEST(dp) ? 0xF : 0;
  }

  /* ---- Record for backlog ---- */
  if ((time.minute == 0) && (time.second == 0)) { /* Once per hour */

    kick_the_watchdog();

    if (gps_is_locked()) { /* Don't bother with no GPS */
      record_backlog(dp);
    }
  }

  /* Update internal time from GPS */
  /* We do this just after midnight or if time is yet to set UTC exactly */
  if (((time.hour == 0) && (time.minute == 0) && (time.second == 0)) ||
      ((time.second == 0) && !(time.valid & UBX_TIMEUTC_VALID_UTC))) {

    kick_the_watchdog();

    read_gps_time();
  }
}

/**
 * Called in an interrupt, increments internal time representation
 */
void cron_tick(void) {
  ticks++;
}
