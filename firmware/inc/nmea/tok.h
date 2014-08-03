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

#ifndef __NMEA_TOK_H__
#define __NMEA_TOK_H__

#include "config.h"

int     nmea_calc_crc(const char *buff, int buff_sz);
int     nmea_atoi(const char *str, int str_sz, int radix);
double  nmea_atof(const char *str, int str_sz);
int     nmea_printf(char *buff, int buff_sz, const char *format, ...);
int     nmea_scanf(const char *buff, int buff_sz, const char *format, ...);

#endif /* __NMEA_TOK_H__ */
