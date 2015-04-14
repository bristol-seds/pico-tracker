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

/**
 * Reference APRS Protocol Spec  http://www.aprs.org/doc/APRS101.PDF
 */

/**
 * This should be a full licensed callsign you own. Not mine plz k thx bai
 *
 * Max. 6 characters
 */
#define APRS_CALLSIGN	"M0SBU"
#define APRS_SSID	11

/**
 * APRS Map Symbol.  See Appendix 2: APRS Symbol Tables
 */
#define APRS_SYMBOL	"/O"    /* Balloon */

void aprs_set_location(float lat, float lon, float altitude);

void aprs_start(void);
uint8_t aprs_tick(void);

#endif /* APRS_H */
