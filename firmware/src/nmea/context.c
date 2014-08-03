/*
 *
 * NMEA library
 * URL: http://nmea.sourceforge.net
 * Author: Tim (xtimor@gmail.com)
 * Licence: http://www.gnu.org/licenses/lgpl.html
 *
 * Modified for the Bristol Longshot program
 * URL:
 * Author: Richard Meadows 2014
 *
 */

#include "nmea/context.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

nmeaPROPERTY * nmea_property()
{
  static nmeaPROPERTY prop = {
    0, NMEA_DEF_PARSEBUFF
  };

  return &prop;
}

void nmea_error(nmea_error_t err)
{
  nmeaErrorFunc func = nmea_property()->error_func;

  if(func) {
    (*func)(err);
  }
}
