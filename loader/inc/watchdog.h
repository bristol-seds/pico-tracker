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
  IDLE_LOADER		= 0x15476064,
} idle_wait_t;

/**
 * Define how many iterations these loops are permitted before a reset
 * is triggered. Values defined to be well above values encountered in
 * normal operation.
 */
#define MAXIDLE_WHILE_LOADER	1800

struct idle_counter {
  uint32_t while_loader;
};

void awake_do_watchdog(void);
void kick_the_watchdog(void);
void idle(idle_wait_t idle_t);
void external_watchdog_safe(void);
void watchdog_init(void);

#endif /* WATCHDOG_H */
