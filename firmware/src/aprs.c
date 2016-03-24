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

uint16_t aprs_telemetry_sequence = 0;

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
  /* base91_encode(str+2, 2, (dp->solar * 1000));   /\* Solar never > 8V *\/ */
  base91_encode(str+2, 2, ((dp->thermistor_temperature+273.2)*10)); /* Temp never > 526º! */
  base91_encode(str+4, 2, ((dp->radio_die_temperature+273.2)*10)); /* Temp never > 526º! */
  base91_encode(str+6, 2, dp->satillite_count);        /* Small! */
  base91_encode(str+8, 2, dp->time_to_first_fix);      /* Small! */
}

/**
 * Comment string for backlog
 */
#define BACKLOG_COMMENT_LEN	(7 + (2 * 4) + 2 + (4 * 2) + 1)
void encode_backlog(char* str, tracker_datapoint* dp, char* prefix)
{
  char compressed_lat[5];
  char compressed_lon[5];
  char compressed_altitude[3];
  char telemetry[TELEMETRY_FIELD_LEN];

  /* Process lat/lon/alt */
  float lat = (float)dp->latitude / 10000000.0;  /* degrees */
  float lon = (float)dp->longitude / 10000000.0; /* degrees */
  uint32_t altitude = dp->altitude / 1000;       /* meters */

  /* Prepare the aprs position report */
  encode_latitude(compressed_lat, lat);
  encode_longitude(compressed_lon, lon);
  encode_altitude(compressed_altitude, altitude);

  /* Encode telemetry string */
  encode_telemetry(telemetry, dp);

  /* Encode backlog string */
  sprintf(str,
          "%s%02d%02d%02dz%s%s%s%s", prefix,
          dp->time.day, dp->time.hour, dp->time.minute,
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
char* _callsign = NULL;
char* _path_str = NULL;
uint8_t _path_id = 0;
char backlog_comment[BACKLOG_COMMENT_LEN+100]; /* TEMP */

char aprs_path_wide2[] = "WIDE2";
char aprs_path_ariss[] = "ARISS";

void aprs_set_datapoint(tracker_datapoint* dp) {
  _dp = dp;
}
void aprs_set_comment(char* comment) {
  _comment = comment;
}
void aprs_set_backlog_comment(tracker_datapoint* log_dp, char* prefix) {
  encode_backlog(backlog_comment, log_dp, prefix);
  _comment = backlog_comment;
}
void aprs_set_callsign(char* call) {
  _callsign = aprs_callsign(call);
}
void aprs_set_path(enum aprs_path path) {
  switch (path) {
    default:                    /* WIDE2-1, this is the default */
    case APRS_PATH_WIDE2_1:
      _path_str = aprs_path_wide2;
      _path_id = 1;
      break;
    case APRS_PATH_ARISS:       /* ARISS Path */
      _path_str = aprs_path_ariss;
      _path_id =0;
      break;
  }
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
  char telemetry_sequence[3];
  char telemetry[TELEMETRY_FIELD_LEN];

  /* Don't run without a valid position */
  if (!_dp || (_dp->latitude == 0 && _dp->longitude == 0)) return 0;

  /* Process lat/lon/alt */
  float lat = (float)_dp->latitude / 10000000.0;  /* degrees */
  float lon = (float)_dp->longitude / 10000000.0; /* degrees */
  uint32_t altitude = _dp->altitude / 1000;       /* meters */

  /* Ensure callsign is set */
  if (!_callsign) {
    _callsign = aprs_callsign("xxxxxx");
  }
  /* Ensure path is set */
  if (!_path_str) {
    aprs_set_path(APRS_PATH_DEFAULT);
  }

  /* Encode the destination / source / path addresses */
  uint32_t addresses_len = sprintf(addresses, "%-6s%c%-6s%c%-6s%c",
                                   "APRS", 0,
                                   _callsign, APRS_SSID,
                                   _path_str, _path_id);

  /* Prepare the aprs position report */
  encode_latitude(compressed_lat, lat);
  encode_longitude(compressed_lon, lon);
  uint32_t altitude_feet = altitude * 3.2808; /* Oh yeah feet! Everyone loves feet */

  /* Encode telemetry string */
  base91_encode(telemetry_sequence, 2, aprs_telemetry_sequence);
  aprs_telemetry_sequence = (aprs_telemetry_sequence + 1) & 0x1FFF;
  encode_telemetry(telemetry, _dp);

  /* Encode the information field */
  /* Compressed Lat/Long position report, no timestamp */
  uint32_t information_len = sprintf(information,
                                     "!%c%s%s%c%s%c/A=%06ld %s|%s%s|",
                                     APRS_SYMBOL[0], /* Symbol Table ID */
                                     compressed_lat,
                                     compressed_lon,
                                     APRS_SYMBOL[1], /* Symbol Code */
                                     "  ",           /* Compressed Altitude */
                                     ' ',            /* Compression Type */
                                     altitude_feet,   /* Altitude */
                                     _comment ? _comment : "",
                                     telemetry_sequence, /* Telemetry Sequence */
                                     telemetry           /* Telemetry */
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
