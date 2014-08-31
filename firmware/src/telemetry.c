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

#include "samd20.h"
#include "semihosting.h"
#include "gps.h"
#include "ubx_messages.h"

#define SEMIHOST_LOG

char telemetry_string[0x200];

/**
 *
 */
void set_telemetry_string(void)
{
  double lat_fmt = 0.0;
  double lon_fmt = 0.0;
  uint32_t altitude = 0;

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
  } else {

  }



  /* Analogue */


#ifdef SEMIHOST_LOG

  semihost_printf("%02.7f,%03.7f,%ld", lat_fmt, lon_fmt, altitude);
  semihost_printf("Lock: %d Sats: %d\n", lock, satillite_count);
  semihost_printf("%02u:%02u:%02u\n", hours, minutes, seconds);


#else
  /* sprintf */
  sprintf(telemetry_string, "$$emf,%02.7f,%03.7f,%ld,%u",
	  lat_fmt, lon_fmt, altitude, ubx_nav_sol.numSV);

  rtty_set_string(telemetry_string, strlen(telemetry_string));
#endif
}
