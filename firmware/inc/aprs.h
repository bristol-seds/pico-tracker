/*
 * Outputs aprs uisng ax25
 * Copyright (C) 2015  Richard Meadows <richardeoin>
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

#ifndef APRS_H
#define APRS_H

#include <string.h>
#include "data.h"

/**
 * Reference APRS Protocol Spec  http://www.aprs.org/doc/APRS101.PDF
 */
#define APRS_FLIGHT_PARAMS
#ifdef APRS_FLIGHT_PARAMS /* ----------- Parameters for flight */

/**
 * This should return a full licensed callsign you own.
 *
 * Max. 6 characters
 */
static inline char* aprs_callsign(char* call) {
  (void)call;

  return "QQQ";
}

/**
 * SSID. Usually 11 for balloons
 */
#define APRS_SSID	11

/**
 * APRS Map Symbol.  See Appendix 2: APRS Symbol Tables
 */
#define APRS_SYMBOL     "/O"    /* Balloon */

#else /* ----------- Parameters for testing */

#warning Using APRS test parameters

static inline char* aprs_callsign(char* call) {
  (void)call;

  return "QQQ";
}
#define APRS_SSID	2
#define APRS_SYMBOL	"/2"

#endif

void encode_backlog(char* str, tracker_datapoint* dp, char* prefix);

void aprs_set_datapoint(tracker_datapoint* dp);
void aprs_set_comment(char* comment);
void aprs_set_backlog_comment(tracker_datapoint* log_dp, char* prefix);
void aprs_set_callsign(char* call);

uint8_t aprs_start(void);
uint8_t aprs_tick(void);

#endif /* APRS_H */
