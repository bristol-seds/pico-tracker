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

#ifndef __NMEA_CONTEXT_H__
#define __NMEA_CONTEXT_H__

#include "config.h"

#define NMEA_DEF_PARSEBUFF  (1024)

typedef enum {
  NMEA_ERROR_PARSE_TIME = 1,
  NMEA_ERROR_PARSE_GPGGA,
  NMEA_ERROR_PARSE_GPGAA_TIME,
  NMEA_ERROR_PARSE_GPGSA,
  NMEA_ERROR_PARSE_GPGSV,
  NMEA_ERROR_PARSE_GPRMC,
  NMEA_ERROR_PARSE_GPRMC_TIME,
  NMEA_ERROR_PARSE_GPVTG,
  NMEA_ERROR_PARSE_GPVTG_FORMAT,

  NMEA_ERROR_MEMORY
} nmea_error_t;

typedef void (*nmeaErrorFunc)(nmea_error_t err);

typedef struct _nmeaPROPERTY
{
  nmeaErrorFunc   error_func;
  int             parse_buff_size;

} nmeaPROPERTY;

nmeaPROPERTY * nmea_property();

void nmea_error(nmea_error_t err);

#endif /* __NMEA_CONTEXT_H__ */
