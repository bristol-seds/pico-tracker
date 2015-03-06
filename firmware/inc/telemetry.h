/*
 * Telemetry strings and formatting
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

#ifndef TELEMETRY_H
#define TELEMETRY_H

uint16_t crc_checksum(char *string);

#include "util/dbuffer.h"

enum telemetry_t {
  TELEMETRY_RTTY,
  TELEMETRY_CONTESTIA,
  TELEMETRY_RSID,
  TELEMETRY_APRS,
  TELEMETRY_PIPS,
};

/**
 * Output String
 */
#define TELEMETRY_STRING_MAX	0x1F0
#define TELEMETRY_LARGEST_BLOCK	0x10
/**
 * It's actually a double buffer which we swap for mid-string updates
 */
ARRAY_DBUFFER_T(char, TELEMETRY_STRING_MAX+TELEMETRY_LARGEST_BLOCK) telemetry_dbuffer_string;

int telemetry_active(void);
int telemetry_start(enum telemetry_t type);
int32_t telemetry_get_index(void);
void telemetry_set_length(int32_t length);


float timer0_tick_init(float frequency);
void timer0_tick_deinit();
void telemetry_gpio1_pwm_init(void);
void telemetry_gpio1_pwm_duty(float duty_cycle);
void telemetry_gpio1_pwm_deinit(void);

#endif /* TELEMETRY_H */
