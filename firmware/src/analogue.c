/*
 * Functions for analogue sensors
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

#include "samd20.h"
#include "adc/adc.h"
#include "hw_config.h"

struct adc_module adc_instance;

#define TEMP_ADC	ADC_POSITIVE_INPUT_TEMP

void configure_adc(enum adc_positive_input input)
{
  struct adc_config config_adc;
  adc_get_config_defaults(&config_adc);

  config_adc.reference = ADC_REFERENCE_INT1V;
  config_adc.gain_factor = ADC_GAIN_FACTOR_DIV2;
  config_adc.positive_input = input;

  adc_init(&adc_instance, ADC, &config_adc);
  adc_enable(&adc_instance);
}

float get_battery(void)
{
  configure_adc(BATTERY_ADC_CHANNEL);
  adc_start_conversion(&adc_instance);

  uint16_t result;

  do {
    /* Wait for conversion to be done and read out result */
  } while (adc_read(&adc_instance, &result) == ADC_STATUS_BUSY);

  /* 12-bit, 1V ref, div 2 */
  return (float)(result * 2) / (4096 * BATTERY_ADC_CHANNEL_DIV);
}
float get_temperature(void)
{
  configure_adc(TEMP_ADC);
  adc_start_conversion(&adc_instance);

  uint16_t result;

  do {
    /* Wait for conversion to be done and read out result */
    } while (adc_read(&adc_instance, &result) == ADC_STATUS_BUSY);

  /* 12-bit, 1V ref, div 2 */
  float voltage = (float)(result * 2) / 4096;
  float millivolt_offset = (voltage - 0.667) * 1000;

  /* Temperature? Uncalibrated.. */
  return 25 + (millivolt_offset / 2.4);
}
