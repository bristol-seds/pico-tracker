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

#include <string.h>

#include "nmea/info.h"

void nmea_zero_INFO(nmeaINFO *info)
{
    memset(info, 0, sizeof(nmeaINFO));
    info->sig = NMEA_SIG_BAD;
    info->fix = NMEA_FIX_BAD;
}
