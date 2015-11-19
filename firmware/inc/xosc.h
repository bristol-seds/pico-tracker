/*
 * Functions for controlling and calibrating against the external oscillator
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

#ifndef XOSC_H
#define XOSC_H

#include "samd20.h"

enum xosc_measurement_t {
  XOSC_MEASURE_GCLK0,
  XOSC_MEASURE_TIMEPULSE,
};

typedef void (*measurement_result_t)(uint32_t result);


/** HF Clock */
void hf_clock_init(void);
void hf_clock_enable(void);
void hf_clock_disable(void);

/** GCLK0 */
void glck0_to_hf_clock(void);
void gclk0_to_lf_clock(void);

/** GCLK1 */
void gclk1_enable(void);
void gclk1_disable(void);

/** Measurement */
void measure_xosc(enum xosc_measurement_t measurement_t, measurement_result_t callback);

#endif /* XOSC_H */
