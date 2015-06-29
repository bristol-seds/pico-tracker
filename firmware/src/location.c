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
#include "geofence_aprs.h"

int32_t current_aprs_zone = -2, current_aprs_zone_outline = -2;
uint32_t _altitude = 0;

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
bool point_in_polygon(const int32_t* poly, uint32_t points, int32_t x, int32_t y) {

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
 */
bool latlon_in_polygon(const int32_t* poly, uint32_t points, float lon, float lat) {

  int32_t x = (int32_t)round(lon * 1000 * 1000); // longitude: µdegrees
  int32_t y = (int32_t)round(lat * 1000 * 1000); // latitude:  µdegrees

  return point_in_polygon(poly, points, x, y);
}



/**
 * Returns if a latitude and longitude is in a given aprs zone outline
 */
bool latlon_in_aprs_zone(int32_t aprs_zone, int32_t aprs_zone_outline, float lon, float lat) {
  return latlon_in_polygon(
    aprs_zones[aprs_zone].outlines[aprs_zone_outline],
    aprs_zones[aprs_zone].outline_lengths[aprs_zone_outline],
    lon, lat);
}


/**
 * Returns if aprs should be transmitted in the current zone
 */
bool aprs_location_tx_allow(void) {

  /* Not in any zone, or in a zone other than Alpha */
  return (current_aprs_zone == -1) || (current_aprs_zone > 0);
}
/**
 * Returns the aprs frequency in the current zone.
 *
 * Where aprs is not allowed this function is unspecified
 */
int32_t aprs_location_frequency(void) {

  if (current_aprs_zone >= 0) {
    return aprs_zones[current_aprs_zone].frequency;
  }

  return 144800000;
}


/**
 * Updates the aprs location based on the current lat/lon
 */
void aprs_location_update(float lon, float lat, uint32_t altitude) {

  uint32_t z, outline;

  /* Record altitude */
  _altitude = altitude;

  /* Were we in an aprs zone last time? */
  if (current_aprs_zone >= 0 && current_aprs_zone_outline >= 0) {

    /* Are we still in the outline? */
    if (latlon_in_aprs_zone(current_aprs_zone,
                            current_aprs_zone_outline,
                            lon, lat)) {                    /* Still in outline */
      return;
    }
  }

  /* Find which aprs zone we are in and save it */
  for (z = 0; z < 12; z++) { /* For each zone */

    for (outline = 0; outline < aprs_zones[z].outline_count; outline++) {

      if (latlon_in_aprs_zone(z, outline, lon, lat)) { /* If we're in this zone */

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
