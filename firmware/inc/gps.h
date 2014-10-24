/*
 * Functions for the UBLOX 6 GPS
 * Copyright (C) 2014  Richard Meadows <richardeoin>
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

#ifndef GPS_H
#define GPS_H

/**
 * GPS Error types
 */
enum gps_error_t {
  GPS_ERROR_BAD_CHECKSUM,
  GPS_ERROR_INVALID_FRAME,
};

void gps_update(void);
void gps_update_wait(void);

struct ubx_nav_posllh gps_get_nav_posllh();
struct ubx_nav_sol gps_get_nav_sol();
struct ubx_nav_timeutc gps_get_nav_timeutc();

uint8_t gps_is_locked(void);

void gps_init(void);
void usart_loopback_test(void);

#endif /* GPS_H */
