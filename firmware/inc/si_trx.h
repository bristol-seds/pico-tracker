/*
 * Functions for controlling Si Labs Transceivers
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

#ifndef SI_TRX_H
#define SI_TRX_H

#include "samd20.h"

struct si_frequency_configuration {
  uint8_t outdiv, band, nprescaler;
  uint32_t m; uint16_t n;
  float lsb_tuning_resolution;
};

enum si_filter_model {
  SI_FILTER_DEFAULT,
  SI_FILTER_APRS,
  SI_FILTER_RSID
};

float si_trx_get_temperature(void);

void si_trx_modem_set_deviation(uint32_t deviation);

void si_trx_get_frequency_configuration(struct si_frequency_configuration* config,
                                        uint32_t frequency);

void si_trx_on(uint8_t modulation_type, struct si_frequency_configuration* fconfig,
                 uint16_t deviation, uint8_t power, enum si_filter_model filter);
void si_trx_off(void);

void si_trx_modem_set_offset(int16_t channel);
#define si_trx_switch_channel si_trx_modem_set_offset


void si_trx_shutdown(void);
void si_trx_init(void);

void spi_loopback_test(void);

#endif /* SI_TRX_H */
