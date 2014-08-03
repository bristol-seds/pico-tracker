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

#ifndef __NMEA_PARSE_H__
#define __NMEA_PARSE_H__

#include "sentence.h"

int  nmea_pack_type(const char *buff, int buff_sz);
int  nmea_find_tail(const char *buff, int buff_sz, int *res_crc);

int  nmea_parse_GPGGA(const char *buff, int buff_sz, nmeaGPGGA *pack);
int  nmea_parse_GPGSA(const char *buff, int buff_sz, nmeaGPGSA *pack);
int  nmea_parse_GPGSV(const char *buff, int buff_sz, nmeaGPGSV *pack);
int  nmea_parse_GPRMC(const char *buff, int buff_sz, nmeaGPRMC *pack);
int  nmea_parse_GPVTG(const char *buff, int buff_sz, nmeaGPVTG *pack);

void nmea_GPGGA2info(nmeaGPGGA *pack, nmeaINFO *info);
void nmea_GPGSA2info(nmeaGPGSA *pack, nmeaINFO *info);
void nmea_GPGSV2info(nmeaGPGSV *pack, nmeaINFO *info);
void nmea_GPRMC2info(nmeaGPRMC *pack, nmeaINFO *info);
void nmea_GPVTG2info(nmeaGPVTG *pack, nmeaINFO *info);

#endif /* __NMEA_PARSE_H__ */
