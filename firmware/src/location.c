/*
 * Location related things
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


#include <stdbool.h>
#include <math.h>

#include "samd20.h"
#include "geofence_no_aprs.h"
#include "geofence_aprs_zones.h"
#include "geofence_telemetry.h"
#include "geofence_prefix.h"

int32_t current_no_aprs_outline = -1;
int32_t current_aprs_zone = -2, current_aprs_zone_outline = -2;

int32_t current_prefix = -2, current_prefix_outline = -2;

int32_t current_telemetry_outline = -1;


#define polyX(i)	(poly[(i*2)+0])
#define polyY(i)	(poly[(i*2)+1])

/**
 * point_in_polygon function from http://alienryderflex.com/polygon/
 *
 * The function will return `true` if the point x,y is inside the
 * polygon, or `false` if it is not.  If the point is exactly on the
 * edge of the polygon, then the function may return `true` or
 * `false`.
 *
 * x - longitude
 * y - latitude
 *
 * Note that division by zero is avoided because the division is
 * protected by the "if" clause which surrounds it.
 */
bool point_in_polygon(const int16_t* poly, uint16_t points, int32_t x, int32_t y)
{
  uint32_t i, j = points-1;
  bool oddNodes = false;

  for (i = 0; i < points; i++) {

    if (((polyY(i) < y && polyY(j) >= y) || (polyY(j) < y && polyY(i) >= y)) &&
        (polyX(i) <= x || polyX(j) <= x)) {

      if (polyX(i) + (float)(y-polyY(i)) / (polyY(j) - polyY(i)) * (polyX(j)-polyX(i)) < x) {
        oddNodes =! oddNodes;
      }
    }

    j = i;
  }

  return oddNodes;
}
/**
 * Returns if a latitude and longitude is in a polygon
 *
 * lat_hn, lon_hn in 100 nanodeg
 */
bool latlon_in_polygon(const int16_t* poly, uint16_t points, int32_t lat_hn, int32_t lon_hn)
{
  int32_t x = lon_hn / (1e6 / 18); /* longitude : 100 nano -> degrees/180 */
  int32_t y = lat_hn / (1e6 / 36); /* latitude  : 100 nano -> degrees/360 */

  return point_in_polygon(poly, points, x, y);
}


/**
 * =============================================================================
 * Updates =====================================================================
 * =============================================================================
 */

/**
 * ============================== Telemetry ============================
 */

/**
 * Returns if a latitude and longitude is in a given telemetry outline
 *
 * lat_hn, lon_hn in 100 nanodeg
 */
bool latlon_in_telemetry(int32_t telemetry_outline, int32_t lat_hn, int32_t lon_hn)
{
  return latlon_in_polygon(
    telemetry_outlines[telemetry_outline],
    telemetry_outline_lengths[telemetry_outline],
    lat_hn, lon_hn);
}
/**
 * Updates the current telemetry location based on the current lat/lon
 *
 * lat_hn, lon_hn in 100 nanodeg
 */
void location_telemetry_update(int32_t lat_hn, int32_t lon_hn)
{
  uint32_t outline;

  /* Were we in a telemetry outline last time? */
  if (current_telemetry_outline >= 0) {

    /* Are we still in this outline? */
    if (latlon_in_telemetry(current_telemetry_outline, lat_hn, lon_hn)) {
      return; /* Still in this outline */
    }
  }

  /* Check all the telemetry outlines */
  for (outline = 0;
       outline < sizeof(telemetry_outlines) / sizeof(int32_t*);
       outline++) {

    if (latlon_in_telemetry(outline, lat_hn, lon_hn)) { /* If we're in this zone */

      /* Record the current outline */
      current_telemetry_outline = outline;

      return;      /* Done. We return the first outline we match */
    }
  }

  /* We're not in an outline */
  current_telemetry_outline = -1;
}

/**
 * ============================== APRS Zones ================================
 */

/**
 * Returns if a latitude and longitude is in a given aprs zone outline
 *
 * lat_hn, lon_hn in 100 nanodeg
 */
bool latlon_in_aprs_zone(int32_t aprs_zone, int32_t aprs_zone_outline,
                         int32_t lat_hn, int32_t lon_hn)
{
  return latlon_in_polygon(
    aprs_zones[aprs_zone].outlines[aprs_zone_outline],
    aprs_zones[aprs_zone].outline_lengths[aprs_zone_outline],
    lat_hn, lon_hn);
}
/**
 * Updates the aprs location based on the current lat/lon
 *
 * lat_hn, lon_hn in 100 nanodeg
 */
void location_aprs_zone_update(int32_t lat_hn, int32_t lon_hn)
{
  uint32_t z, outline;

  /* Were we in an aprs zone last time? */
  if (current_aprs_zone >= 0 && current_aprs_zone_outline >= 0) {

    /* Are we still in the outline? */
    if (latlon_in_aprs_zone(current_aprs_zone,
                            current_aprs_zone_outline,
                            lat_hn, lon_hn)) {                    /* Still in outline */
      return;
    }
  }

  /* Find which aprs zone we are in and save it */
  uint32_t n_zones = sizeof(aprs_zones) / sizeof(struct aprs_zone_t);
  for (z = 0; z < n_zones; z++) { /* For each zone */

    for (outline = 0; outline < aprs_zones[z].outline_count; outline++) {

      if (latlon_in_aprs_zone(z, outline, lat_hn, lon_hn)) { /* If we're in this zone */

        /* Record the current zone */
        current_aprs_zone = z;
        current_aprs_zone_outline = outline;

        return;                 /* Go home. We return the first zone we match */
      }
    }
  }

  /* We're not in a zone */
  current_aprs_zone = -1;
}

/**
 * ============================== APRS Zones ================================
 */

/**
 * Returns if a latitude and longitude is in a given aprs zone outline
 *
 * lat_hn, lon_hn in 100 nanodeg
 */
bool latlon_in_prefix(int32_t prefix, int32_t prefix_outline,
                         int32_t lat_hn, int32_t lon_hn)
{
  return latlon_in_polygon(
    prefixes[prefix].outlines[prefix_outline],
    prefixes[prefix].outline_lengths[prefix_outline],
    lat_hn, lon_hn);
}
/**
 * Updates the aprs location based on the current lat/lon
 *
 * lat_hn, lon_hn in 100 nanodeg
 */
void location_prefix_update(int32_t lat_hn, int32_t lon_hn)
{
  uint32_t z, outline;

  /* Were we in an aprs zone last time? */
  if (current_prefix >= 0 && current_prefix_outline >= 0) {

    /* Are we still in the outline? */
    if (latlon_in_prefix(current_prefix,
                            current_prefix_outline,
                            lat_hn, lon_hn)) {                    /* Still in outline */
      return;
    }
  }

  /* Find which prefix we are in and save it */
  uint32_t n_zones = sizeof(prefixes) / sizeof(struct prefix_t);
  for (z = 0; z < n_zones; z++) { /* For each zone */

    for (outline = 0; outline < prefixes[z].outline_count; outline++) {

      if (latlon_in_prefix(z, outline, lat_hn, lon_hn)) { /* If we're in this zone */

        /* Record the current zone */
        current_prefix = z;
        current_prefix_outline = outline;

        return;                 /* Go home. We return the first zone we match */
      }
    }
  }

  /* We're not in a zone */
  current_prefix = -1;
}

/**
 * ============================== No APRS ============================
 */

/**
 * Returns if a latitude and longitude is in a given no aprs outline
 *
 * lat_hn, lon_hn in 100 nanodeg
 */
bool latlon_in_no_aprs(int32_t no_aprs_outline, int32_t lat_hn, int32_t lon_hn)
{
  return latlon_in_polygon(
    no_aprs_outlines[no_aprs_outline],
    no_aprs_outline_lengths[no_aprs_outline],
    lat_hn, lon_hn);
}
/**
 * Updates the current no_aprs location based on the current lat/lon
 *
 * lat_hn, lon_hn in 100 nanodeg
 */
void location_no_aprs_update(int32_t lat_hn, int32_t lon_hn)
{
  uint32_t outline;

  /* Were we in a no_aprs outline last time? */
  if (current_no_aprs_outline >= 0) {

    /* Are we still in this outline? */
    if (latlon_in_no_aprs(current_no_aprs_outline, lat_hn, lon_hn)) {
      return; /* Still in this outline */
    }
  }

  /* Check all the no_aprs outlines */
  for (outline = 0; outline < sizeof(no_aprs_outlines) / sizeof(int32_t*); outline++) {

    if (latlon_in_no_aprs(outline, lat_hn, lon_hn)) { /* If we're in this zone */

      /* Record the current outline */
      current_no_aprs_outline = outline;

      return;      /* Done. We return the first outline we match */
    }
  }

  /* We're not in an outline */
  current_no_aprs_outline = -1;
}

/**
 * ============================== Wrapper for both APRS  ========================
 */

void location_aprs_update(int32_t lat_hn, int32_t lon_hn)
{
  location_aprs_zone_update(lat_hn, lon_hn);
  location_no_aprs_update(lat_hn, lon_hn);
}


/**
 * =============================================================================
 * Decisions ===================================================================
 * =============================================================================
 */



/**
 * Return if telemetry should be transmitted in the current location
 */
bool location_telemetry_active(void)
{
  /* Are we outside the no telemetry zones? */
  return (current_telemetry_outline == -1);
}


/**
 * Returns if aprs could be transmitted in the current location
 */
bool location_aprs_could_tx(void)
{
  /* true if aprs disallowed */
  uint8_t no_aprs = (current_no_aprs_outline != -1);

  return !no_aprs;
}
/**
 * Returns if aprs should be transmitted in the current location
 */
bool location_aprs_should_tx(void)
{
  /* true if aprs disallowed */
  uint8_t no_aprs = (current_no_aprs_outline != -1);

  /* true if in any zone*/
  uint8_t in_zone = (current_aprs_zone != -1);

  return in_zone && (!no_aprs);
}
/**
 * Returns the local aprs frequency in the current zone.
 *
 * Where aprs is inactive this function return 144.8MHz anyhow
 */
int32_t location_aprs_frequency(void)
{
  if (current_aprs_zone >= 0) {
    return aprs_zones[current_aprs_zone].frequency;
  }

  return 144800000;
}

char blank_string[] = "";
/**
 * Returns the call to use in the current zone
 */
char* location_aprs_call(void) {
  if (current_aprs_zone >= 0) {
    return aprs_zones[current_aprs_zone].call;
  }

  return blank_string;
}

/**
 * Returns the deviation in the current zone
 */
int32_t location_aprs_deviation(void) {
  if (current_aprs_zone >= 0) {
    return ((float)aprs_zones[current_aprs_zone].deviation * (5.0/6.0));
  }

  return 2500;                  /* 2.5kHz default */
}

/**
 * Returns the current prefix
 */
char* location_prefix(void) {
  if (current_prefix >= 0) {
    return prefixes[current_prefix].prefix;
  }

  return blank_string;
}
