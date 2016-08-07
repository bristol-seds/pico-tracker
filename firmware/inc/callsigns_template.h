/*
 * Template for callsign configuration file
 * Copyright (C) 2016  Richard Meadows <richardeoin>
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

#ifndef CALLSIGNS_H
#define CALLSIGNS_H

/* ------------------------------- Telemetry -------------------------------- */

#define CALLSIGN	"YOUR_CALL"

/* ---------------------------------- APRS ---------------------------------- */

/**
 * This should return a full licensed callsign you own.
 *
 * Maximum 6 characters
 */
static inline char* aprs_callsign(char* call)
{
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

/**
 * APRS Comment Field
 */
#define APRS_COMMENT	""

/* -------------------------------------------------------------------------- */

#endif  /* CALLSIGNS_H */
