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

float battery_v = 0.0, thermistor_ratio = 0.0, solar_v = 0.0;

#define ADC_GAINF		ADC_GAIN_FACTOR_DIV2
#define ADC_GAINF_VAL		0.5
#define ADC_RESOLUTION		ADC_RESOLUTION_12BIT
#define ADC_RESOLUTION_VAL	4096

enum adc_phase_t {
  ADC_PHASE_NONE,
  ADC_PHASE_START,
#if BATTERY_ADC
  ADC_PHASE_CONVERT_BATTERY,
#endif
#if THERMISTOR_ADC
  ADC_PHASE_CONVERT_THERMISTOR,
#endif
#if SOLAR_ADC
  ADC_PHASE_CONVERT_SOLAR,
#endif
  ADC_PHASE_DONE,
} adc_phase = ADC_PHASE_NONE;

void adc_complete_callback(void);

/**
 * Configures ADC for a given channel
 */
void configure_adc(enum adc_positive_input input, enum adc_reference reference)
{
  struct adc_config config_adc;
  adc_get_config_defaults(&config_adc);

  config_adc.clock_source = GCLK_GENERATOR_0;
  config_adc.reference = reference;
  config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV64;
  config_adc.resolution = ADC_RESOLUTION;
  config_adc.gain_factor = ADC_GAINF;
  config_adc.positive_input = input;
  config_adc.sample_length = 15; /* len = 15+1 = 16 */
  config_adc.accumulate_samples = ADC_ACCUMULATE_DISABLE;
  config_adc.run_in_standby = true;

  adc_init(ADC, &config_adc);
  adc_enable();

  adc_register_callback(adc_complete_callback, ADC_CALLBACK_READ_BUFFER);
  adc_enable_interrupt(ADC_INTERRUPT_RESULT_READY);

  irq_register_handler(ADC_IRQn, ADC_INT_PRIO);
}


/**
 * Gets the channel to sample in the current phase
 */
enum adc_positive_input adc_get_channel(enum adc_phase_t phase)
{
  switch (phase) {
#if BATTERY_ADC
    case ADC_PHASE_CONVERT_BATTERY:	return BATTERY_ADC_CHANNEL;
#endif
#if THERMISTOR_ADC
    case ADC_PHASE_CONVERT_THERMISTOR:	return THERMISTOR_ADC_CHANNEL;
#endif
#if SOLAR_ADC
    case ADC_PHASE_CONVERT_SOLAR: 	return SOLAR_ADC_CHANNEL;
#endif
    default:	return SOLAR_ADC_CHANNEL;
  }
}
/**
 * Gets the reference to use in the current phase
 */
enum adc_reference adc_get_reference(enum adc_phase_t phase)
{
  switch (phase) {
#if BATTERY_ADC
    case ADC_PHASE_CONVERT_BATTERY:	return BATTERY_ADC_REFERENCE;
#endif
#if THERMISTOR_ADC
    case ADC_PHASE_CONVERT_THERMISTOR:	return THERMISTOR_ADC_REFERENCE;
#endif
#if SOLAR_ADC
    case ADC_PHASE_CONVERT_SOLAR: 	return SOLAR_ADC_REFERENCE;
#endif
    default:	return SOLAR_ADC_CHANNEL;
  }
}
/**
 * Assigns the value for the current phase
 */
void assign_adc_value(enum adc_phase_t phase, float pin_v)
{
  switch (phase) {
#if BATTERY_ADC
    case ADC_PHASE_CONVERT_BATTERY:
      battery_v = pin_v / BATTERY_ADC_CHANNEL_DIV;
      break;
#endif
#if THERMISTOR_ADC
    case ADC_PHASE_CONVERT_THERMISTOR:
      thermistor_ratio = pin_v / THERMISTOR_ADC_CHANNEL_DIV;
      break;
#endif
#if SOLAR_ADC
    case ADC_PHASE_CONVERT_SOLAR:
      solar_v = pin_v / SOLAR_ADC_CHANNEL_DIV;
      break;
#endif
    default:
      break;
  }
}
/**
 * Is the ADC sequence done?
 */
uint8_t is_adc_sequence_done(void) {
  return (adc_phase == ADC_PHASE_DONE);
}

/**
 * Called on a ADC result ready event
 */
void adc_complete_callback(void) {
  uint16_t result = 0;
  float pin_v;

  adc_read(&result);
  adc_disable();

  pin_v = (float)result / (ADC_GAINF_VAL * ADC_RESOLUTION_VAL);

  /* Assign this value to the correct channel */
  assign_adc_value(adc_phase, pin_v);

  adc_phase++;
  if (!is_adc_sequence_done()) { /* Another channel still to do.. */

    /* Start conversion on this channel */
    configure_adc(adc_get_channel(adc_phase), adc_get_reference(adc_phase));
    adc_start_conversion();
  }
}

/**
 * Called to start the ADC sequence
 */
void start_adc_sequence(void)
{
  /* Move to the first sampling phase */
  adc_phase = ADC_PHASE_START;
  adc_phase++;

  /* Start conversion on this channel */
  configure_adc(adc_get_channel(adc_phase), adc_get_reference(adc_phase));
  adc_start_conversion();
}

/**
 * Getters
 */
float get_battery(void)
{
  return battery_v;
}
float get_thermistor(void)
{
  return thermistor_ratio;
}
float get_solar(void)
{
  return solar_v;
}
