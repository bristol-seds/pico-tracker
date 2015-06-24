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
#include "semihosting.h"
#include "hw_config.h"
#include "system/system.h"
#include "sercom/usart.h"
#include "system/port.h"
#include "system/events.h"
#include "system/extint.h"
#include "tc/tc_driver.h"
#include "gps.h"
#include "mfsk.h"
#include "ubx_messages.h"
#include "watchdog.h"
#include "xosc.h"
#include "telemetry.h"
#include "timer.h"
#include "contestia.h"
#include "rsid.h"
#include "aprs.h"
#include "location.h"
#include "si_trx.h"
#include "si_trx_defs.h"
#include "analogue.h"
#include "spi_bitbang.h"
#include "system/interrupt.h"

#define CALLSIGN	"UBSEDSx"

void xosc_measure_callback(uint32_t result);
void timepulse_callback(uint32_t sequence);

int32_t _xosc_error = 0;

/**
 * Initialises the status LED. SHOULD TURN ON THE LED
 */
static inline void led_reset(void)
{
  port_pin_set_config(LED0_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */
  port_pin_set_output_level(LED0_PIN, 0);	/* LED is active low */
}
/**
 * Turns the status LED on
 */
static inline void led_on(void)
{
  port_pin_set_output_level(LED0_PIN, 0);	/* LED is active low */
}
/**
 * Turns the status LED off
 */
static inline void led_off(void)
{
  port_pin_set_output_level(LED0_PIN, 1);	/* LED is active low */
}

/**
 * Power Management
 */
void powermananger_init(void)
{
  system_apb_clock_clear_mask(SYSTEM_CLOCK_APB_APBA,
//			      PM_APBAMASK_EIC | /* EIC is used now */
//			      PM_APBAMASK_RTC | /* RTC is used now */
    0);
}


/**
 * Telemetry String
 * =============================================================================
 */
void output_telemetry_string(enum telemetry_t type)
{
  double lat_fmt = 0.0;
  double lon_fmt = 0.0;
  uint32_t altitude = 0;
  uint16_t len;
  uint8_t dollars = 2;

  /**
   * Collect Data
   * ---------------------------------------------------------------------------
   */

  /* Analogue */
  float battery = get_battery();
  float temperature = telemetry_si_temperature();

  /* GPS Time */
  gps_update_time();

  /* Sleep Wait */
  while (gps_update_time_pending()) {
    system_sleep();
  }

  /* Time */
  struct ubx_nav_timeutc time = gps_get_nav_timeutc();
  uint8_t hours = time.payload.hour;
  uint8_t minutes = time.payload.min;
  uint8_t seconds = time.payload.sec;

  /* Request updates from the gps */
  gps_update_position();
  if (gps_is_locked()) {
    led_on();
  } else {
    led_off();
  }

  /* Wait for the gps update */
  while (gps_update_position_pending()) {
    system_sleep();
  }

  if (gps_is_locked()) {
    led_off();
  } else {
    led_on();
  }

  /* GPS Status */
  struct ubx_nav_sol sol = gps_get_nav_sol();
  uint8_t satillite_count = sol.payload.numSV;

  /* GPS Position */
  if (gps_is_locked()) {
    struct ubx_nav_posllh pos = gps_get_nav_posllh();
    lat_fmt = (double)pos.payload.lat / 10000000.0;
    lon_fmt = (double)pos.payload.lon / 10000000.0;
    altitude = pos.payload.height / 1000;
  }

  /* GPS Powersave */
  gps_set_powersave_auto();

  /**
   * Format
   * ---------------------------------------------------------------------------
   */

  if (type == TELEMETRY_RTTY) {
    dollars = 5; // Extra dollars for RTTY
  }

  /* sprintf - preamble */
  memset(telemetry_string, '$', dollars);
  len = dollars;

  /* sprintf - full string */
  len += sprintf(telemetry_string + len,
		"%s,%02u:%02u:%02u,%02.5f,%03.5f,%ld,%u,%.2f,%.1f,%ld",
		CALLSIGN, hours, minutes, seconds, lat_fmt, lon_fmt,
                 altitude, satillite_count, battery, temperature, _xosc_error);

  if (type == TELEMETRY_CONTESTIA) { contestiaize(telemetry_string + dollars); }

  /* sprintf - checksum. don't include dollars */
  len += sprintf(telemetry_string + len,
		 "*%04X\r",
		 crc_checksum(telemetry_string + dollars));



/**
 * Starting up the radio blocks on high-prio interrupt for ~100ms: todo fixme
 */

  /* RSID */
  /* start - SI NOW BELONGS TO TELEMETRY, WE CANNOT ACCESS */
  if (type == TELEMETRY_CONTESTIA) {
    telemetry_start_rsid(RSID_CONTESTIA_32_1000);
  }

  /* Sleep Wait for RSID */
  while (telemetry_active()) {
    system_sleep();
  }

  /* Main telemetry */
  telemetry_start(type, len);

  /* Sleep Wait for main telemetry */
  while (telemetry_active()) {
    system_sleep();
  }
}

void aprs_telemetry(void) {

  if (!gps_is_locked()) return; /* Don't bother with no GPS */

  struct ubx_nav_posllh pos = gps_get_nav_posllh();
  float lat = (float)pos.payload.lat / 10000000.0; // This division is from the gps reciver, not for geofence
  float lon = (float)pos.payload.lon / 10000000.0;
  uint32_t altitude = pos.payload.height / 1000;

  /* Update location */
  aprs_location_update(lon, lat);

  /* aprs okay here? */
  if (aprs_location_tx_allow()) {

    /* Set location */
    aprs_set_location(lat, lon, altitude);

    /* Set frequency */
    telemetry_aprs_set_frequency(aprs_location_frequency());

    /* Transmit packet and wait */
    telemetry_start(TELEMETRY_APRS, 0xFFFF);
    while (telemetry_active()) {
      system_sleep();
    }
  }
}

/**
 * APRS function for use during testing. Not for flight
 */
void aprs_test(void)
{
  if (!gps_is_locked()) return; /* Don't bother with no GPS */

  while(1) {
    struct ubx_nav_posllh pos = gps_get_nav_posllh();
    float lat = (float)pos.payload.lat / 10000000.0; // This division is from the gps reciver, not for geofence
    float lon = (float)pos.payload.lon / 10000000.0;
    uint32_t altitude = pos.payload.height / 1000;

    /* Set location */
    aprs_set_location(lat, lon, altitude);

    /* Set frequency */
    telemetry_aprs_set_frequency(144800000);

    /* Transmit packet and wait */
    telemetry_start(TELEMETRY_APRS, 0xFFFF);
    while (telemetry_active()) {
      system_sleep();
    }
  }
}

/**
 * Internal initialisation
 * =============================================================================
 */
void init(void)
{
  /**
   * Reset to get the system in a safe state
   * --------------------------------------------------------------------------
   */
  led_reset();
  si_trx_shutdown();

  /* If the reset was caused by the internal watchdog... */
  if (PM->RCAUSE.reg & PM_RCAUSE_WDT) {
    /* External hardware is in an undefined state. Wait here for the
       external watchdog to trigger an external reset */

    while (1);
  }

  /**
   * Internal initialisation
   * ---------------------------------------------------------------------------
   */

  /* Clock up to 14MHz with 0 wait states */
  system_flash_set_waitstates(SYSTEM_WAIT_STATE_1_8V_14MHZ);

  /* Up the clock rate to 4MHz */
  system_clock_source_osc8m_set_config(SYSTEM_OSC8M_DIV_2, /* Prescaler */
				       false,		   /* Run in Standby */
				       false);		   /* Run on Demand */

  /* Restart the GCLK Module */
  system_gclk_init();
  system_events_init();
  system_extint_init();

  /* Remember the HW watchdog has been running since reset */
  //watchdog_init();

  /* Configure Sleep Mode */
  //system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);
  system_set_sleepmode(SYSTEM_SLEEPMODE_IDLE_2); /* Disable CPU, AHB and APB */

  /* Configure the Power Manager */
  //powermananger_init();

  /**
   * System initialisation
   * ---------------------------------------------------------------------------
   */

  /* Enable the xosc on gclk1 */
  xosc_init();

  /* GPS init */
//  gps_init();

  /* Enable timer interrupt and event channel */
  timepulse_extint_init();
  timepulse_set_callback(timepulse_callback);

  /* Initialise Si4060 interface */
  si_trx_init();
}


void xosc_measure_callback(uint32_t result)
{
  _xosc_error = result - XOSC_FREQUENCY;
}

uint32_t telemetry_interval_count = TELEMETRY_INTERVAL;
uint32_t aprs_interval_count = APRS_INTERVAL;
uint8_t telemetry_trigger_flag = 0;
uint8_t aprs_trigger_flag = 0;

/**
 * Called by the timer at 1Hz
 */
void timepulse_callback(uint32_t sequence)
{
  telemetry_interval_count++;
  aprs_interval_count++;

  /* Runs at the rate of telemetry packets */
  if (telemetry_interval_count >= TELEMETRY_INTERVAL) {
    telemetry_interval_count = 0;
    telemetry_trigger_flag = 1;
  }

  /* Runs at the rate of aprs packets */
  if (aprs_interval_count >= APRS_INTERVAL) {
    aprs_interval_count = 0;
    aprs_trigger_flag = 1;
  }
}

/**
 * MAIN
 * =============================================================================
 */
int main(void)
{
  uint32_t telemetry_alternate = 0;

  init();

  while (1) {
    /* Send a packet */
    output_telemetry_string((telemetry_alternate++ & 1) ?
                            TELEMETRY_CONTESTIA :
                            TELEMETRY_RTTY);

    /* Maybe aprs? */
#if APRS_ENABLE
    if (aprs_trigger_flag) {
      aprs_telemetry();
    }
    aprs_trigger_flag = 0;
#endif

    /* Pips */
    telemetry_start(TELEMETRY_PIPS, 0xFFFF);

    /* Measure XOSC against gps timepulse */
    measure_xosc(XOSC_MEASURE_TIMEPULSE, xosc_measure_callback);

    /* Sleep wait for next telemetry */
    while (telemetry_trigger_flag == 0) {
      system_sleep();
    }
    telemetry_trigger_flag = 0;

    /* End pips */
    telemetry_stop();
    while (telemetry_active()) {
      system_sleep();
    }
  }
}
