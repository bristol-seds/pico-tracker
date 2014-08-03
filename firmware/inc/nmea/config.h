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

#ifndef __NMEA_CONFIG_H__
#define __NMEA_CONFIG_H__

#define NMEA_VERSION		("0.5.3")
#define NMEA_VERSION_MAJOR	(0)
#define NMEA_VERSION_MINOR	(5)
#define NMEA_VERSION_PATCH	(3)

#define NMEA_CONVSTR_BUF	(256)
#define NMEA_TIMEPARSE_BUF	(256)

#define NMEA_POSIX(x)		x
#define NMEA_INLINE		inline

#include <assert.h>
#define NMEA_ASSERT(x)   	assert(x)


#endif /* __NMEA_CONFIG_H__ */
