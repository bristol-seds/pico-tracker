/*
 * Functions related to the watchdog.
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

#ifndef WATCHDOG_H
#define WATCHDOG_H

#include "samd20.h"

/**
 * These are random constants to prevent run-away code from
 * accidentially hitting them.
 */
typedef enum {
  IDLE_NONE,
  IDLE_WAIT_FOR_GPS 		= 0x90137526,
  IDLE_TELEMETRY_ACTIVE		= 0x15476064,
  IDLE_WAIT_FOR_NEXT_TELEMETRY	= 0x36749870,
} idle_wait_t;

/**
 * Define how many iterations these loops are permitted before a reset
 * is triggered. Values defined to be well above values encountered in
 * normal operation.
 */
#define MAXIDLE_WAIT_FOR_GPS		2400
#define MAXIDLE_WHILE_TELEMETRY_ACTIVE	60000
#define MAXIDLE_WAIT_FOR_NEXT_TELEMETRY	30000

struct idle_counter {
  uint32_t wait_for_gps;
  uint32_t while_telemetry_active;
  uint32_t wait_for_next_telemetry;
};

void awake_do_watchdog(void);
void kick_the_watchdog(void);
void idle(idle_wait_t idle_t);
void watchdog_init(void);

#endif /* WATCHDOG_H */
