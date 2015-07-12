/*
 * Bristol SEDS pico-tracker
 * Copyright (C) 2014  Richard Meadows <richardeoin>
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

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "samd20.h"
#include "hw_config.h"
#include "init.h"
#include "gps.h"
#include "mfsk.h"
#include "watchdog.h"
#include "telemetry.h"
#include "timer.h"
#include "contestia.h"
#include "aprs.h"
#include "location.h"
#include "rf_tests.h"
#include "data.h"
#include "backlog.h"
#include "pips.h"

#define CALLSIGN	"UBSEDSx"
#define APRS_COMMENT	"RTTY/434.6U8N2"

/**
 * Formats a UKHAS telemetry string for the given datapoint
 *
 * The telemetry string starts with the specified number of dollar signs
 */
uint16_t format_telemetry_string(char* string, struct tracker_datapoint* dp,
                                 uint8_t dollars, uint8_t reduce_char_set)
{
  double lat_fmt = 0.0;
  double lon_fmt = 0.0;
  uint32_t altitude = 0;
  uint16_t len;

  lat_fmt = (double)dp->latitude / 10000000.0;  /* degrees */
  lon_fmt = (double)dp->longitude / 10000000.0; /* degrees */
  altitude = dp->altitude / 1000;               /* meters */

  /* sprintf - preamble */
  memset(string, '$', dollars);
  len = dollars;

  /* sprintf - full string */
  len += sprintf(telemetry_string + len,
                 "%s,%02u:%02u:%02u,%02.5f,%03.5f,%ld,%u,%.2f,%.2f,%.1f,%ld",
                 CALLSIGN,
                 dp->time.hour, dp->time.minute, dp->time.second,
                 lat_fmt, lon_fmt, altitude, dp->satillite_count,
                 dp->battery, dp->solar, dp->temperature, dp->xosc_error);

  if (reduce_char_set) {
    /* Reduce character set */
    contestiaize(telemetry_string + dollars);
  }

  /* sprintf - checksum. don't include dollars */
  len += sprintf(telemetry_string + len,
		 "*%04X\r",
		 crc_checksum(telemetry_string + dollars));

  /* Length should be no more than 100 characters!! */
  if (len <= 100) {
    return len;
  }

  /* Okay, let's use a shorter backup format */
  len = dollars;

  /* sprintf - short format */
  len += sprintf(telemetry_string + len,
                 "%s,%02u:%02u:%02u,%02.5f,%03.5f,%ld",
                 CALLSIGN,
                 dp->time.hour, dp->time.minute, dp->time.second,
                 lat_fmt, lon_fmt, altitude);

  if (reduce_char_set) {
    /* Reduce character set */
    contestiaize(telemetry_string + dollars);
  }

  /* sprintf - checksum. don't include dollars */
  len += sprintf(telemetry_string + len,
		 "*%04X\r",
		 crc_checksum(telemetry_string + dollars));

  return len;
}


/**
 * RTTY telemetry. Uses 5 dollar symbols
 */
#define RTTY_DOLLARS 5
void rtty_telemetry(struct tracker_datapoint* dp) {
  uint16_t len;

  len = format_telemetry_string(telemetry_string, dp, RTTY_DOLLARS, 0);

  /* Main telemetry */
  telemetry_start(TELEMETRY_RTTY, len);

  /* Sleep Wait for main telemetry */
  while (telemetry_active()) {
    idle(IDLE_TELEMETRY_ACTIVE);
  }
}
/**
 * Contestia telemetry. Uses 2 dollar symbols
 */
#define CONTESTIA_DOLLARS 2
void contestia_telemetry(struct tracker_datapoint* dp) {
  uint16_t len;

  len = format_telemetry_string(telemetry_string, dp, CONTESTIA_DOLLARS, 1);

  /* RSID */
  telemetry_start_rsid(RSID_CONTESTIA_32_1000);

  /* Sleep Wait for RSID */
  while (telemetry_active()) {
    idle(IDLE_TELEMETRY_ACTIVE);
  }

  /* Main telemetry */
  telemetry_start(TELEMETRY_CONTESTIA, len);

  /* Sleep Wait for main telemetry */
  while (telemetry_active()) {
    idle(IDLE_TELEMETRY_ACTIVE);
  }
}
/**
 * APRS telemetry if required
 */
void aprs_telemetry(struct tracker_datapoint* dp) {

  struct tracker_datapoint* backlog_dp_ptr;

  if (!gps_is_locked()) return; /* Don't bother with no GPS */

  float lat = (float)dp->latitude / 10000000.0;  /* degrees */
  float lon = (float)dp->longitude / 10000000.0; /* degrees */

  /* Update location */
  aprs_location_update(lon, lat);

#if APRS_USE_GEOFENCE
  /* aprs okay here? */
  if (aprs_location_tx_allow()) {
#endif

    /* Set location */
    aprs_set_datapoint(dp);

    /* Set comment */
    if ((dp->time.minute % 4) == 0) {
      aprs_set_comment(APRS_COMMENT);
    } else {
      backlog_dp_ptr = get_backlog();

      if (backlog_dp_ptr) {     /* Backlog comment if we can */
        aprs_set_backlog_comment(backlog_dp_ptr);
      } else {
        aprs_set_comment(APRS_COMMENT);
      }
    }

    /* Set frequency */
#if APRS_USE_GEOFENCE
    telemetry_aprs_set_frequency(aprs_location_frequency());
#else
    telemetry_aprs_set_frequency(144800000);
#endif

    /* Transmit packet and wait */
    telemetry_start(TELEMETRY_APRS, 0xFFFF);
    while (telemetry_active()) {
      idle(IDLE_TELEMETRY_ACTIVE);
    }

#if APRS_USE_GEOFENCE
  }
#endif
}
/**
 * Pips telemetry
 */
void pips_telemetry(void)
{
  /* Pips */
  telemetry_start(TELEMETRY_PIPS, 0xFFFF);

  while (telemetry_active()) {
    idle(IDLE_TELEMETRY_ACTIVE);
  }
}

uint8_t tick_flag = 0;

/**
 * Called at 1Hz by the GPS
 */
void gps_tick(uint32_t sequence)
{
  /* Sequence not used */
  (void)sequence;

  /* Update internal time representation */
  cron_tick();

  /* Raise the tick flag */
  tick_flag = 1;
}

/**
 * MAIN
 * =============================================================================
 */
int main(void)
{
  /* Init */
  init(gps_tick, INIT_NORMAL);

  /* Maybe do some rf tests */
  rf_tests();

  /* Turn off LED to show we've initialised correctly */
  led_off();

  while (1) {
    /* Run cron job */
    if (tick_flag) {
      tick_flag = 0; do_cron();
    }

    /* Idle */
    idle(IDLE_WAIT_FOR_NEXT_TELEMETRY);
  }
}
