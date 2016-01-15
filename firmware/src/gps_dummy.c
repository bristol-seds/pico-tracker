/*
 * Functions for a dummy GPS (for testing)
 * Copyright (C) 2014, 2015  Richard Meadows <richardeoin>
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
#include "hw_config.h"

#ifdef GPS_TYPE_DUMMY

#include "system/system.h"
#include "system/port.h"
#include "system/interrupt.h"
#include "sercom/sercom.h"
#include "sercom/usart.h"
#include "gps.h"
#include "watchdog.h"
#include "init.h"

volatile enum gps_error_t gps_error_state;

/**
 * Flight State
 */
enum gps_flight_state_t gps_flight_state = GPS_FLIGHT_STATE_LAUNCH;


/**
 * =============================================================================
 * Flight State  ================================================================
 * =============================================================================
 */

/**
 * Returns current flight state
 */
enum gps_flight_state_t gps_get_flight_state(void)
{
  return gps_flight_state;
}

/**
 * Sets flight state
 *
 * altitude in mm
 */
void gps_set_flight_state(int32_t altitude)
{
  if (altitude > GPS_FLIGHT_STATE_THREASHOLD_M*1000) {
    gps_flight_state = GPS_FLIGHT_STATE_FLOAT;
  } else {
    gps_flight_state = GPS_FLIGHT_STATE_LAUNCH;
  }
}


/**
 * =============================================================================
 * Power State  ================================================================
 * =============================================================================
 */


/**
 * =============================================================================
 * Getters   ===================================================================
 * =============================================================================
 */

/**
 * Gets the current error state of the GPS to check validity of last
 * request
 */
enum gps_error_t gps_get_error_state(void)
{
  return gps_error_state;
}

/**
 * Powers up the GPS, waits up to 60 seconds for a fix
 *
 * Uses the Geodetic Navigation Data frame.
 */
uint32_t dummy_seq = 0;
struct gps_data_t gps_get_data(void)
{
  struct gps_data_t data;

  float lat_deg, lon_deg;

  for(int i = 0; i < 6; i++) {
    for(int j = 0; j < 1000*150; j++);
    led_toggle();
    kick_the_watchdog();
  }
  led_off();

  switch(dummy_seq) {

/* 144.8 */
      /* Copenhagen 144.8 */
    case 0:
      lat_deg = 55.7;
      lon_deg = 12.5;
      break;
      /* Amsterdam 144.8 */
    case 1:
      lat_deg = 52.4;
      lon_deg = 5.7;
      break;
      /* Germany 144.8 */
    case 2:
      lat_deg = 52;
      lon_deg = 9;
      break;
      /* Poland 144.8 */
    case 3:
      lat_deg = 51;
      lon_deg = 20;
      break;
      /* Russia 144.8 */
    case 4:
      lat_deg = 55;
      lon_deg = 37;
      break;
      /* Mongolia 144.8 */
    case 5:
      lat_deg = 48;
      lon_deg = 107;
      break;
      /* Vladivostok 144.8 */
    case 6:
      lat_deg = 43;
      lon_deg = 132;
      break;
      /* Armenia 144.8 */
    case 7:
      lat_deg = 40;
      lon_deg = 44;
      break;
      /* Africa 144.8 */
    case 8:
      lat_deg = 9;
      lon_deg = 2;

/* 144.39 */
      break;
      /* LA 144.39 */
    case 9:
      lat_deg = 34;
      lon_deg = -118;
      break;
      /* New York,144.39 */
    case 10:
      lat_deg = 41;
      lon_deg = -74;
      break;
      /* Canada 144.39 */
    case 11:
      lat_deg = 44;
      lon_deg = -76;

/* 144.62 */
      break;
      /* South Korea,144.62*/
    case 12:
      lat_deg = 37;
      lon_deg = 128;

/* 144.64 */
      break;
      /* West China,144.64*/
    case 13:
      lat_deg = 39;
      lon_deg = 80;
      break;
      /* Beijing 144.64 */
    case 14:
      lat_deg = 40;
      lon_deg = 116;
      break;
      /* Hong Kong 144.64*/
    case 15:
      lat_deg = 22.4;
      lon_deg = 114.1;

/* 144.66 */
      break;
      /* Japan 144.66 */
    case 16:
      lat_deg = 36;
      lon_deg = 140;

/* 145.525 */
      break;
      /* Thailand 145.525 */
    case 17:
      lat_deg = 14;
      lon_deg = 100;
      break;


      /* UK 0 */
    case 18:
      lat_deg = 52;
      lon_deg = -1;
      break;
      /* UK 0 */
    case 19:
      lat_deg = 52;
      lon_deg = 0;
      break;
      /* UK 0 */
    case 20:
      lat_deg = 52;
      lon_deg = 1;
      break;
      /* Wales 0 */
    case 21:
      lat_deg = 52;
      lon_deg = -4;
      break;
      /* Lewis 0 */
    case 22:
      lat_deg = 58;
      lon_deg = -6.5;
      break;
      /* NI 0 */
    case 23:
      lat_deg = 54.5;
      lon_deg = -6.5;
      break;
      /* France 0 */
    case 24:
      lat_deg = 49;
      lon_deg = 2;
      break;
      /* Portugal 0 */
    case 25:
      lat_deg = 40;
      lon_deg = -8;
      break;
      /* Italy 0 */
    case 26:
      lat_deg = 43;
      lon_deg = 12;
      break;
      /* Romania 0 */
    case 27:
      lat_deg = 46;
      lon_deg = 24;
      break;
      /* Sweden 0 */
    case 28:
      lat_deg = 58;
      lon_deg = 12;
      break;
      /* Stockholm 0 */
    case 29:
      lat_deg = 59.3;
      lon_deg = 18;
      break;
      /* Malmo 0 */
    case 30:
      lat_deg = 55.6;
      lon_deg = 13;
      break;
      /* Yemen 0 */
    case 31:
      lat_deg = 15;
      lon_deg = 44;
      break;
      /* NK 0 */
    case 32:
      lat_deg = 39;
      lon_deg = 126;
      break;
  }

  data.is_locked = 1;   /* valid */
  data.latitude  = lat_deg *1000*1000*10;    /* deg -> hndeg */
  data.longitude = lon_deg *1000*1000*10;
  data.altitude = 8*1000 *1000; /* m -> mm*/
  data.satillite_count = 6;
  data.time_to_first_fix = 3;

  gps_set_flight_state(data.altitude);

  return data;
}


/**
 * =============================================================================
 * Setup     ===================================================================
 * =============================================================================
 */

/**
 * GPS configuration
 */
void gps_setup(void)
{
  ;
}

uint8_t gps_is_locked(void)
{
  return 1;                     /* locked */
}

/**
 * =============================================================================
 * Setup      ==================================================================
 * =============================================================================
 */

/**
 * Init + enable for the usart at the given baud rate
 */
void gps_usart_init_enable(uint32_t baud_rate)
{
  (void)baud_rate;
}
void gps_usart_init_disable(void)
{
  ;
}

/**
 * Reset. Places the GPS in a RESET state
 */
void gps_reset(void)
{
  ;
}

/**
 * Init
 */
void gps_init(void)
{
  ;
}


/**
 *
 */
void usart_loopback_test(void)
{
  ;
}

#endif /* GPS_TYPE_DUMMY */
