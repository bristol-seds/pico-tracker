/*
 * Functions for the UBLOX 6 GPS
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

#ifndef GPS_H
#define GPS_H

#include "samd20.h"
#include "hw_config.h"

/**
 * GPS Error types
 */
enum gps_error_t {
  GPS_NOERROR,
  GPS_ERROR_BAD_CHECKSUM,
  GPS_ERROR_INVALID_FRAME,
};

/**
 * GPS Flight State
 */
enum gps_flight_state_t {
  GPS_FLIGHT_STATE_LAUNCH,
  GPS_FLIGHT_STATE_FLOAT,
};

/* UBX ------------------------------------------------------------- */
#ifdef GPS_TYPE_UBX

/**
 * GPS Data
 */
struct gps_data_t {
  int32_t latitude, longitude;  /* hndeg */
  int32_t altitude;             /* mm */
  uint8_t satillite_count;
  uint8_t is_locked;            /* 1 = locked, 0 = not locked */
};

void gps_update_time(void);
void gps_update_position(void);
int gps_update_time_pending(void);
int gps_update_position_pending(void);
enum gps_error_t gps_get_error_state(void);

struct ubx_nav_posllh gps_get_nav_posllh();
struct ubx_nav_sol gps_get_nav_sol();
struct ubx_nav_timeutc gps_get_nav_timeutc();

uint8_t gps_is_locked(void);

void gps_set_powersave(bool powersave_on);
void gps_set_power_state(bool gnss_running);

void gps_set_powersave_auto(void);

#endif  /* GPS_TYPE_UBX */


/* OSP ------------------------------------------------------------- */
#ifdef GPS_TYPE_OSP

/**
 * GPS Data
 */
struct gps_data_t {
  uint16_t year;                /* years */
  uint8_t month, day, hour, minute, second; /* months, days, hours, minutes, seconds */
  int32_t latitude, longitude;  /* hndeg */
  int32_t altitude;             /* mm */
  uint8_t satillite_count;
  uint8_t is_locked;            /* 1 = locked, 0 = not locked */
  uint8_t time_to_first_fix;    /* seconds / counts */
};


enum gps_error_t gps_get_error_state(void);
struct gps_data_t gps_get_data(void);

void gps_setup(void);

#endif  /* GPS_TYPE_OSP */


/* Both ------------------------------------------------------------ */
enum gps_flight_state_t gps_get_flight_state(void);

void gps_usart_init_enable(uint32_t baud_rate);
void gps_reset(void);
void gps_init(void);
void usart_loopback_test(void);

#endif /* GPS_H */
