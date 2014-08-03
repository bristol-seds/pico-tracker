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

#ifndef __NMEA_PARSER_H__
#define __NMEA_PARSER_H__

#include "info.h"
#include "context.h"

/*
 * high level
 */

typedef struct _nmeaPARSER
{
  void *top_node;
  void *end_node;
  unsigned char buffer[NMEA_DEF_PARSEBUFF];
  int buff_size;
  int buff_use;

} nmeaPARSER;

int     nmea_parser_init(nmeaPARSER *parser);
void    nmea_parser_destroy(nmeaPARSER *parser);

int     nmea_parse(
  nmeaPARSER *parser,
  const char *buff, int buff_sz,
  nmeaINFO *info
  );

/*
 * low level
 */

int     nmea_parser_push(nmeaPARSER *parser, const char *buff, int buff_sz);
int     nmea_parser_top(nmeaPARSER *parser);
int     nmea_parser_pop(nmeaPARSER *parser, void **pack_ptr);
int     nmea_parser_peek(nmeaPARSER *parser, void **pack_ptr);
int     nmea_parser_drop(nmeaPARSER *parser);
int     nmea_parser_buff_clear(nmeaPARSER *parser);
int     nmea_parser_queue_clear(nmeaPARSER *parser);

#endif /* __NMEA_PARSER_H__ */
