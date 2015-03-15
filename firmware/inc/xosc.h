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
  XOSC_MEASURE_OSC8M,
  XOSC_MEASURE_TIMEPULSE,
};
struct osc8m_calibration_t {
  uint8_t temperature;
  uint8_t process;
};

typedef void (*measurement_result_t)(uint32_t result);

void xosc_init(void);
struct osc8m_calibration_t osc8m_get_calibration(void);
void osc8m_set_calibration(struct osc8m_calibration_t calib);
void measure_xosc(enum xosc_measurement_t measurement_t, measurement_result_t callback);

#endif /* XOSC_H */
