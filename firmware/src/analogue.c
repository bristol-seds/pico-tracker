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
#include "system/interrupt.h"
#include "hw_config.h"

float battery_v, solar_v;

#define ADC_GAINF		ADC_GAIN_FACTOR_DIV2
#define ADC_GAINF_VAL		0.5
#define ADC_RESOLUTION		ADC_RESOLUTION_12BIT
#define ADC_RESOLUTION_VAL	4096

enum {
  ADC_PHASE_NONE,
  ADC_PHASE_CONVERT_BATTERY,
  ADC_PHASE_CONVERT_SOLAR,
  ADC_PHASE_DONE,
} adc_phase = ADC_PHASE_NONE;

void adc_complete_callback(void);

void configure_adc(enum adc_positive_input input)
{
  struct adc_config config_adc;
  adc_get_config_defaults(&config_adc);

  config_adc.clock_source = GCLK_GENERATOR_0;
  config_adc.reference = ADC_REFERENCE_INT1V;
  config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV64;
  config_adc.resolution = ADC_RESOLUTION;
  config_adc.gain_factor = ADC_GAIN_FACTOR_DIV2;
  config_adc.positive_input = input;
  config_adc.accumulate_samples = ADC_ACCUMULATE_DISABLE;
  config_adc.run_in_standby = true;

  adc_init(ADC, &config_adc);
  adc_enable();

  adc_register_callback(adc_complete_callback, ADC_CALLBACK_READ_BUFFER);
  adc_enable_interrupt(ADC_INTERRUPT_RESULT_READY);

  irq_register_handler(ADC_IRQn, ADC_INT_PRIO);
}

/**
 * Called on a ADC result ready event
 */
void adc_complete_callback(void) {
  uint16_t result;
  float pin_v;

  adc_read(&result);
  pin_v = (float)result / (ADC_GAINF_VAL * ADC_RESOLUTION_VAL);

  if (adc_phase == ADC_PHASE_CONVERT_BATTERY) {
    /* Battery */

    /* Calcuate the battery votage */
    battery_v = pin_v / BATTERY_ADC_CHANNEL_DIV;

    /* Next up: Solar */
    configure_adc(SOLAR_ADC_CHANNEL);
    adc_start_conversion();
  } else {

    /* Solar */

    /* Calculate the solar voltage */
    solar_v = pin_v / SOLAR_ADC_CHANNEL_DIV;
  }

  adc_phase++;
}

void start_adc_conversion_sequence(void)
{
  /* First up: Battery */
  configure_adc(BATTERY_ADC_CHANNEL);
  adc_start_conversion();
  adc_phase = ADC_PHASE_CONVERT_BATTERY;
}
uint8_t is_adc_conversion_done(void) {
  return (adc_phase == ADC_PHASE_DONE);
}
float get_battery(void)
{
  return battery_v;
}
float get_solar(void)
{
  return solar_v;
}

/* float get_temperature(void) */
/* { */
/*   configure_adc(TEMP_ADC); */
/*   adc_start_conversion(&adc_instance); */

/*   uint16_t result; */

/*   do { */
/*     /\* Wait for conversion to be done and read out result *\/ */
/*     } while (adc_read(&adc_instance, &result) == ADC_STATUS_BUSY); */

/*   /\* 12-bit, 1V ref, div 2 *\/ */
/*   float voltage = (float)(result * 2) / 4096; */
/*   float millivolt_offset = (voltage - 0.667) * 1000; */

/*   /\* Temperature? Uncalibrated.. *\/ */
/*   return 25 + (millivolt_offset / 2.4); */
/* } */
