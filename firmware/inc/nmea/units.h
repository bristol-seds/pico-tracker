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

#ifndef __NMEA_UNITS_H__
#define __NMEA_UNITS_H__

#include "config.h"

/*
 * Distance units
 */

/**< Yards, meter * NMEA_TUD_YARDS = yard */
#define NMEA_TUD_YARDS      (1.0936)

/**< Knots, kilometer / NMEA_TUD_KNOTS = knot */
#define NMEA_TUD_KNOTS      (1.852)

/**< Miles, kilometer / NMEA_TUD_MILES = mile */
#define NMEA_TUD_MILES      (1.609)

/*
 * Speed units
 */

/**< Meters per seconds, (k/h) / NMEA_TUS_MS= (m/s) */
#define NMEA_TUS_MS         (3.6)

#endif /* __NMEA_UNITS_H__ */
