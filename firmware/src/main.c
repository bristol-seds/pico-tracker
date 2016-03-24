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
#include "contestia.h"
#include "aprs.h"
#include "location.h"
#include "rf_tests.h"
#include "data.h"
#include "analogue.h"
#include "backlog.h"
#include "pips.h"
#include "xosc.h"
#include "sequencer.h"
#include "thermistor.h"

#define CALLSIGN	"UBSEDS14"
#define APRS_COMMENT	""

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

  lat_fmt = (double)dp->latitude / 10000000.0;  /* hundred nanodeg -> degrees */
  lon_fmt = (double)dp->longitude / 10000000.0; /* hundred nanodeg -> degrees */
  altitude = dp->altitude / 1000;               /* mm -> meters */

  /* sprintf - preamble */
  memset(string, '$', dollars);
  len = dollars;

  /* sprintf - full string (approx 90 chars) */
  len += sprintf(telemetry_string + len,
                 "%s,%02u:%02u:%02u,%02u%02u%02u,%02.4f,%03.4f,%ld,%u,%u,%.2f,%.1f,%.1f",
                 CALLSIGN,      /* 2+6+2+1=11 */
                 dp->time.hour, dp->time.minute, dp->time.second, /* 2+1+2+1+2+1=9 */
                 dp->time.year%100, dp->time.month, dp->time.day, /* 2+2+2+1=7 */
                 lat_fmt, lon_fmt, altitude, dp->satillite_count, /* 3+1+4+1 + 4+1+4+1 + 5+1 + 2+1 = 28 */
                 dp->time_to_first_fix, /* 2+1 = 3 */
                 dp->battery, dp->thermistor_temperature,  /* 1+1+2+1 + 3+1+1+1 = 11 */
                 dp->radio_die_temperature); /* 3+1+1+1 = 6 */
  /* sum = 75 (must be less than or equal to 114) */

  if (reduce_char_set) {
    /* Reduce character set */
    contestiaize(telemetry_string + dollars);
  }

  /* sprintf - checksum. don't include dollars */
  len += sprintf(telemetry_string + len,
		 "*%04X\r",
		 crc_checksum(telemetry_string + dollars));

  /* Length should be no more than 120 characters!! (24 seconds transmission time) */
  if (len <= 120) {
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
void rtty_telemetry(struct tracker_datapoint* dp)
{
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
void contestia_telemetry(struct tracker_datapoint* dp)
{
  uint16_t len;

  len = format_telemetry_string(telemetry_string, dp, CONTESTIA_DOLLARS, 1);

  /* RSID */
  telemetry_start_rsid(RSID_CONTESTIA_16_1000);

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
void aprs_telemetry(struct tracker_datapoint* dp)
{
  struct tracker_datapoint* backlog_dp_ptr;

  if (gps_is_locked() == GPS_NO_LOCK) return; /* Don't bother with no GPS */

  char* prefix = location_prefix();
  char* call = location_aprs_call();

  /* Set location */
  aprs_set_datapoint(dp);

  /* Set callsign and path */
  aprs_set_callsign(call);
  aprs_set_path(APRS_PATH_WIDE2_1);

  /* Set comment */
  backlog_dp_ptr = get_backlog();

  if (backlog_dp_ptr != NULL) {     /* Backlog comment if we can */
    aprs_set_backlog_comment(backlog_dp_ptr, prefix);
  } else {
    aprs_set_comment(prefix);
  }

  /* Set frequency */
  telemetry_aprs_set_frequency(location_aprs_frequency());

  /* Transmit packet and wait */
  telemetry_start(TELEMETRY_APRS, 0xFFFF);
  while (telemetry_active()) {
    idle(IDLE_TELEMETRY_ACTIVE);
  }
}
/**
 * ARISS telemetry
 */
void ariss_telemetry(struct tracker_datapoint* dp)
{
  struct tracker_datapoint* backlog_dp_ptr;

  if (gps_is_locked() == GPS_NO_LOCK) return; /* Don't bother with no GPS */

  char* prefix = location_prefix();
  char* call = location_aprs_call();

  /* Set location */
  aprs_set_datapoint(dp);

  /* Set callsign and path */
  aprs_set_callsign(call);
  aprs_set_path(APRS_PATH_ARISS);

  /* Set comment */
  backlog_dp_ptr = get_backlog();

  if (backlog_dp_ptr != NULL) {     /* Backlog comment if we can */
    aprs_set_backlog_comment(backlog_dp_ptr, prefix);
  } else {
    aprs_set_comment(prefix);
  }

  /* Set frequency */
  telemetry_aprs_set_frequency(ARISS_FREQUENCY);

  /* Transmit packet and wait */
  telemetry_start(TELEMETRY_APRS, 0xFFFF);
  while (telemetry_active()) {
    idle(IDLE_TELEMETRY_ACTIVE);
  }
}
/**
 * Pips telemetry
 */
void pips_telemetry(void)
{
  /* Pips - 10 seconds */
  telemetry_start(TELEMETRY_PIPS, 10);

  while (telemetry_active()) {
    idle(IDLE_TELEMETRY_ACTIVE);
  }
}

/**
 * Timing helpers
 * =============================================================================
 */

volatile uint8_t run_flag = 1;  /* run immediately after init */
uint32_t hibernate_time_s = 1;

uint8_t in_cold_out = 1;        /* test temperature immediately after init */
uint32_t cold_out_count = 0;

/**
 * Sets the hibernate time in seconds
 */
void set_hibernate_time(uint8_t cold_out)
{
  if (cold_out == 0) {                  /* Normal operations */
    if (gps_is_locked() == GPS_NO_LOCK) {   /* no lock  */
      hibernate_time_s = 0;       /* shortest hibernate */

    } else if (gps_get_flight_state() == GPS_FLIGHT_STATE_LAUNCH) {
      hibernate_time_s = 60-20;   /* approx every minute */

    } else {
      hibernate_time_s = 240-20;  /* approx every 4 minutes  */
    }
  } else {                      /* cold out */
    hibernate_time_s = COLD_OUT_SECONDS;
  }
}

/**
 * Called on each tick of the low frequency clock
 */
void lf_tick(uint32_t tick)
{
  /* When we're due to run again */
  /* Called at 2Hz */
  if (tick >= 2*hibernate_time_s) {
    /* Stop */
    lf_tick_stop();

    /* Raise the run flag */
    run_flag = 1;
  }
}

/**
 * MAIN
 * =============================================================================
 */
int main(void)
{
  uint32_t n = 1;
  float external_temperature;

  /* Init */
  init(INIT_NORMAL);

  /* Maybe do some rf tests */
  rf_tests();

  /* Turn off LED to show we've initialised correctly */
  led_off();

  while (1) {
    /* Run sequence - starts immediately on first iteration */
    if (run_flag) {
      run_flag = 0;

      /* Clocks on */
      hf_clock_enable();
      gclk0_to_hf_clock();
      system_set_sleepmode(SYSTEM_SLEEPMODE_IDLE_2); /* Low power */

      /* Check temperature sensor */
      if (in_cold_out == 1) {
        start_adc_sequence();
        while (is_adc_sequence_done() == 0); /* wait for adc */

        external_temperature = thermistor_ratio_to_temperature(get_thermistor()); /* read */
        if ((external_temperature < COLD_OUT_TEMPERATURE) && /* check temperature */
            (cold_out_count++ < COLD_OUT_COUNT_MAX)) {       /* and max iterations */
          in_cold_out = 1;            /* cold */

        } else {
          in_cold_out = 0;            /* ready to go! */
          gps_init();                 /* init the gps! */
          run_sequencer(n++);         /* run for the first time! */
        }
      } else {
        run_sequencer(n++); /* Run */
      }

      /* Clocks off */
      system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY); /* Lowest power */

      /* Disable to save power */
      system_apb_clock_clear_mask(SYSTEM_CLOCK_APB_APBA,
                                  PM_APBAMASK_EIC |
                                  PM_APBAMASK_PAC0 |
                                  PM_APBAMASK_WDT);

      gclk0_to_lf_clock();
      hf_clock_disable();

      /* LF timing */
      set_hibernate_time(in_cold_out);
      lf_tick_start();
    }

    /* Idle */
    idle(IDLE_WAIT_FOR_NEXT_TELEMETRY);
  }
}
