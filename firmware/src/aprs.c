/*
 * Outputs aprs uisng ax25
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

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "samd20.h"
#include "aprs.h"
#include "ax25.h"

/**
 * USEFUL RESOURCES
 * =============================================================================
 *
 * http://www.aprs.org/doc/APRS101.PDF
 * http://k9dci.home.comcast.net/~k9dci/APRS%20Beginner%20Guide%20-%20K9DCI%20Ver%205-1.pdf
 */

/**
 * Encodes a base-91 representation of `value` in `str`
 *
 * `str` should have length `n` + 1 and should be big enough to hold `value`
 */
void base91_encode(char *str, uint8_t n, uint32_t value)
{
  /* Start at the end of the string, adding the null terminator */
  for(str += n, *str = '\0'; n; n--) {

    *(--str) = (value % 91) + 33;
    value /= 91;
  }
}


/**
 * SET VALUES
 * =============================================================================
 */

float _lat = 0, _lon = 0, _altitude;
void aprs_set_location(float lat, float lon, float altitude) {
  _lat = lat; _lon = lon; _altitude = altitude;
}




/**
 * START / TICK
 * =============================================================================
 */

/**
 * Start the transmission of an aprs frame
 */
void aprs_start(void)
{
  char addresses[50];
  char information[50];
  char compressed_lat[5];
  char compressed_lon[5];

  /* Don't run without a valid position */
  if (_lat == 0 && _lon == 0) return;

  /* Encode the destination / source / path addresses */
  uint32_t addresses_len = sprintf(addresses, "%-6s%c%-6s%c%-6s%c",
                                   "APRS", 0,
                                   APRS_CALLSIGN, APRS_SSID,
                                   "WIDE2", 1);

  /* Prepare the aprs position report */
  uint32_t compressed_lat_value = (uint32_t)round(380926 * ( 90 - _lat));
  uint32_t compressed_lon_value = (uint32_t)round(190463 * (180 + _lon));
  base91_encode(compressed_lat, 4, compressed_lat_value);
  base91_encode(compressed_lon, 4, compressed_lon_value);
  uint32_t altitude_feet = _altitude * 3.2808; /* Oh yeah feet! Everyone loves feet */

  /* Encode the information field */
  /* Compressed Lat/Long position report, no timestamp */
  uint32_t information_len = sprintf(information,
                                     "!%c%s%s%c%s%c/A=%06ld RTTY/434.6U8N2",
                                     APRS_SYMBOL[0], /* Symbol Table ID */
                                     compressed_lat,
                                     compressed_lon,
                                     APRS_SYMBOL[1], /* Symbol Code */
                                     "  ",           /* Compressed Altitude */
                                     ' ',            /* Compression Type */
                                     altitude_feet   /* Altitude */
    );

  /* Let's actually try that out.. We can add comment etc. later */


  /* Transmit the frame */
  ax25_start(addresses,   addresses_len,
             information, information_len);
}

uint8_t aprs_tick(void)
{
  return ax25_tick();
}
