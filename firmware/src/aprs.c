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
#include "data.h"

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
 * Compressed latitude in base-91 representation. Four characters
 *
 * Latitude should be given in decimal degrees
 */
void encode_latitude(char* str, float lat)
{
  uint32_t compressed_lat_value = (uint32_t)round(380926 * ( 90 - lat));
  base91_encode(str, 4, compressed_lat_value);
}
/**
 * Compressed longitude in base-91 representation. Four characters
 *
 * Longitude should be given in decimal degrees
 */
void encode_longitude(char* str, float lon)
{
  uint32_t compressed_lon_value = (uint32_t)round(190463 * (180 + lon));
  base91_encode(str, 4, compressed_lon_value);
}
/**
 * Compressed altitude in base-91 representation. Two characters
 *
 * Altitude should be given in meters
 */
void encode_altitude(char* str, uint32_t altitude_meters) {
  uint32_t altitude_feet = altitude_meters * 3.2808; /* Oh yeah feet! Everyone loves feet */
  uint16_t compressed_value = log(altitude_feet) / log(1.002);

  base91_encode(str, 2, compressed_value);
}

/**
 * String for telemetry
 *
 * String length should be >= 9
 */
#define TELEMETRY_FIELD_LEN	((4 * 2) + 1)
void encode_telemetry(char* str, tracker_datapoint* dp)
{
  base91_encode(str+0, 2, (dp->battery * 1000)); /* Battery never > 8V */
  base91_encode(str+2, 2, (dp->solar * 1000));   /* Solar never > 8V */
  base91_encode(str+4, 2, ((dp->temperature+273.2)*10)); /* Temp never > 526º! */
  base91_encode(str+6, 2, dp->satillite_count);        /* Small! */
}

/**
 * Comment string for backlog
 */
#define BACKLOG_COMMENT_LEN	(7 + (2 * 4) + 2 + (4 * 2) + 1)
void encode_backlog(char* str, tracker_datapoint* dp)
{
  char compressed_lat[5];
  char compressed_lon[5];
  char compressed_altitude[3];
  char telemetry[TELEMETRY_FIELD_LEN];

  /* Prepare the aprs position report */
  encode_latitude(compressed_lat, dp->latitude);
  encode_longitude(compressed_lon, dp->longitude);
  encode_altitude(compressed_altitude, dp->altitude);

  /* Encode telemetry string */
  encode_telemetry(telemetry, dp);

  /* Encode backlog string */
  sprintf(str,
          "%02d%02d%02dz%s%s%s%s",
          dp->time.day, dp->time.hour, dp->time.second,
          compressed_lat, compressed_lon, compressed_altitude,
          telemetry
    );
}

/**
 * SET VALUES
 * =============================================================================
 */

struct tracker_datapoint* _dp = NULL;
char* _comment = NULL;
char backlog_comment[BACKLOG_COMMENT_LEN];
void aprs_set_datapoint(tracker_datapoint* dp) {
  _dp = dp;
}
void aprs_set_comment(char* comment) {
  _comment = comment;
}
void aprs_set_backlog_comment(tracker_datapoint* log_dp) {
  encode_backlog(backlog_comment, log_dp);
  _comment = backlog_comment;
}


/**
 * START / TICK
 * =============================================================================
 */

/**
 * Start the transmission of an aprs frame
 */
uint8_t aprs_start(void)
{
  char addresses[50];
  char information[150];
  char compressed_lat[5];
  char compressed_lon[5];
  char telemetry[TELEMETRY_FIELD_LEN];

  /* Don't run without a valid position */
  if (!_dp || (_dp->latitude == 0 && _dp->longitude == 0)) return 0;

  /* Encode the destination / source / path addresses */
  uint32_t addresses_len = sprintf(addresses, "%-6s%c%-6s%c%-6s%c",
                                   "APRS", 0,
                                   APRS_CALLSIGN, APRS_SSID,
                                   "WIDE2", 1);

  /* Prepare the aprs position report */
  encode_latitude(compressed_lat, _dp->latitude);
  encode_longitude(compressed_lon, _dp->longitude);
  uint32_t altitude_feet = _dp->altitude * 3.2808; /* Oh yeah feet! Everyone loves feet */

  /* Encode telemetry string */
  encode_telemetry(telemetry, _dp);

  /* Encode the information field */
  /* Compressed Lat/Long position report, no timestamp */
  uint32_t information_len = sprintf(information,
                                     "!%c%s%s%c%s%c/A=%06ld %s|%s|",
                                     APRS_SYMBOL[0], /* Symbol Table ID */
                                     compressed_lat,
                                     compressed_lon,
                                     APRS_SYMBOL[1], /* Symbol Code */
                                     "  ",           /* Compressed Altitude */
                                     ' ',            /* Compression Type */
                                     altitude_feet,   /* Altitude */
                                     _comment ? _comment : "",
                                     telemetry
    );

  /* Transmit the frame using ax25 */
  ax25_start(addresses,   addresses_len,
             information, information_len);

  return 1;
}

uint8_t aprs_tick(void)
{
  return ax25_tick();
}
