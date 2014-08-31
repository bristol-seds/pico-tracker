/*
 * Telemetry strings and formatting
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
#include <string.h>

#include "samd20.h"
#include "semihosting.h"
#include "analogue.h"
#include "gps.h"
#include "rtty.h"
#include "ubx_messages.h"
#include "si4060.h"

//#define SEMIHOST_LOG

char telemetry_string[0x200];

/**
 * CRC Function for the XMODEM protocol.
 * http://www.nongnu.org/avr-libc/user-manual/group__util__crc.html#gaca726c22a1900f9bad52594c8846115f
 */
uint16_t crc_xmodem_update(uint16_t crc, uint8_t data)
{
  int i;

  crc = crc ^ ((uint16_t)data << 8);
  for (i = 0; i < 8; i++) {
    if (crc & 0x8000) {
      crc = (crc << 1) ^ 0x1021;
    } else {
      crc <<= 1;
    }
  }

  return crc;
}

/**
 * Calcuates the CRC checksum for a communications string
 * See http://ukhas.org.uk/communication:protocol
 */
uint16_t crc_checksum(char *string)
{
  size_t i;
  uint16_t crc;
  uint8_t c;

  crc = 0xFFFF;

  // Calculate checksum ignoring the first two $s
  for (i = 2; i < strlen(string); i++) {
    c = string[i];
    crc = crc_xmodem_update(crc, c);
  }

  return crc;
}
/**
 * Sets output telemetry
 */
void set_telemetry_string(void)
{
  double lat_fmt = 0.0;
  double lon_fmt = 0.0;
  uint32_t altitude = 0;

  /* Analogue */
  float battery = get_battery();

  /* Time */
  struct ubx_nav_timeutc time = gps_get_nav_timeutc();
  uint8_t hours = time.payload.hour;
  uint8_t minutes = time.payload.min;
  uint8_t seconds = time.payload.sec;

  /* GPS Status */
  struct ubx_nav_sol sol = gps_get_nav_sol();
  uint8_t lock = sol.payload.gpsFix;
  uint8_t satillite_count = sol.payload.numSV;

  /* GPS Position */
  if (lock == 0x2 || lock == 0x3 || lock == 0x4) {
    struct ubx_nav_posllh pos = gps_get_nav_posllh();
    lat_fmt = (double)pos.payload.lat / 10000000.0;
    lon_fmt = (double)pos.payload.lon / 10000000.0;
    altitude = pos.payload.height / 1000;
  }


//#ifdef SEMIHOST_LOG
//  semihost_printf("Batt %f, Temp %f\n", battery, temperature);
//  semihost_printf("%02.7f,%03.7f,%ld\n", lat_fmt, lon_fmt, altitude);
//  semihost_printf("Lock: %d Sats: %d\n", lock, satillite_count);
//  semihost_printf("%02u:%02u:%02u\n", hours, minutes, seconds);
//#endif

  /* sprintf */
  uint16_t len = sprintf(telemetry_string,
			 "$$EMF2,%02u:%02u:%02u,%02.7f,%03.7f,%ld,%u,%.1f",
			 hours, minutes, seconds, lat_fmt, lon_fmt, altitude,
			 satillite_count, battery);

  sprintf(telemetry_string + len, "*%04X\n", crc_checksum(telemetry_string));

  rtty_set_string(telemetry_string, strlen(telemetry_string));
}
