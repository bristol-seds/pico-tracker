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

#include "si_trx.h"
#include "rsid.h"

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
#define TELEMETRY_STRING_MAX	0x200
char telemetry_string[TELEMETRY_STRING_MAX];

int telemetry_active(void);
int telemetry_start(enum telemetry_t type, int32_t length);
int telemetry_start_rsid(rsid_code_t rsid);
void telemetry_aprs_set_frequency(int32_t frequency);
void telemetry_aprs_set_rf_path(enum si_rf_path path);
float telemetry_si_temperature(void);

void timer0_tick_init(uint32_t count);
void timer0_tick_deinit();
void telemetry_init();

void telemetry_gpio1_pwm_init(void);
void telemetry_gpio1_pwm_duty(float duty_cycle);
void telemetry_gpio1_pwm_deinit(void);

#endif /* TELEMETRY_H */
