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

#ifndef __NMEA_TIME_H__
#define __NMEA_TIME_H__

#include "config.h"

/**
 * Date and time data
 */
typedef struct _nmeaTIME
{
  int     year;       /**< Years since 1900 */
  int     mon;        /**< Months since January - [0,11] */
  int     day;        /**< Day of the month - [1,31] */
  int     hour;       /**< Hours since midnight - [0,23] */
  int     min;        /**< Minutes after the hour - [0,59] */
  int     sec;        /**< Seconds after the minute - [0,59] */
  int     hsec;       /**< Hundredth part of second - [0,99] */

} nmeaTIME;

#endif /* __NMEA_TIME_H__ */
