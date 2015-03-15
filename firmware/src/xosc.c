/*
 * Functions for controlling and calibrating against the external oscillator
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

#include <string.h>

#include "samd20.h"
#include "system/clock.h"
#include "system/gclk.h"
#include "system/interrupt.h"
#include "system/pinmux.h"
#include "system/events.h"
#include "system/extint.h"
#include "tc/tc_driver.h"
#include "hw_config.h"
#include "xosc.h"


enum measure_state_t {
  MEASURE_WAIT_FOR_FIRST_EVENT,
  MEASURE_MEASUREMENT,
} measure_state = MEASURE_WAIT_FOR_FIRST_EVENT;
enum xosc_measurement_t _measurement_t;
measurement_result_t _callback;

/**
 * Configures external oscillator, waits for it to stabilise, and
 * connects it to GLCK1.
 */
void xosc_init(void) {
  system_clock_source_xosc_set_config(SYSTEM_CLOCK_EXTERNAL_CLOCK,
                                      SYSTEM_XOSC_STARTUP_1,
                                      true,
                                      XOSC_FREQUENCY,
                                      false,
                                      false);
  system_clock_source_enable(SYSTEM_CLOCK_SOURCE_XOSC);

  while (!system_clock_source_is_ready(SYSTEM_CLOCK_SOURCE_XOSC));

  /* Configure GCLK1 to XOSC */
  system_gclk_gen_set_config(GCLK_GENERATOR_1,
        		     GCLK_SOURCE_XOSC, /* Source 		*/
        		     false,		/* High When Disabled	*/
        		     XOSC_COUNT_RESOLUTION,/* Division Factor	*/
        		     false,		/* Run in standby	*/
        		     false);		/* Output Pin Enable	*/


  /* Enable GCLK1 */
  system_gclk_gen_enable(GCLK_GENERATOR_1);
}

struct osc8m_calibration_t osc8m_get_calibration(void) {
  uint16_t calib_word = SYSCTRL->OSC8M.bit.CALIB;
  struct osc8m_calibration_t calib;

  calib.temperature	= (calib_word >> 6) & 0x3F;
  calib.process		= (calib_word >> 0) & 0x3F;

  return calib;
}
void osc8m_set_calibration(struct osc8m_calibration_t calib) {
  uint16_t calib_word = ((calib.temperature & 0x3F) << 6) | (calib.process & 0x3F);

  system_clock_source_write_calibration(SYSTEM_CLOCK_SOURCE_OSC8M, calib_word, 0x1);
}



/**
 * Configure timer 4 to generate events at 1Hz of OSC8M
 */
void osc8m_event_source(void) {

  /* Timer 4 runs on GCLK0 (4MHz) */
  bool t4_capture_channel_enables[]    = {false, false};
  uint32_t t4_compare_channel_values[] = {15625, 0x0000};
  /* Divide by 256*15625 = 1Hz events */
  tc_init(TC4,
	  GCLK_GENERATOR_0,
	  TC_COUNTER_SIZE_16BIT,
	  TC_CLOCK_PRESCALER_DIV256,
	  TC_WAVE_GENERATION_NORMAL_FREQ,
	  TC_RELOAD_ACTION_GCLK,
	  TC_COUNT_DIRECTION_UP,
	  TC_WAVEFORM_INVERT_OUTPUT_NONE,
	  false,			/* Oneshot  */
	  false,			/* Run in standby */
	  0x0000,			/* Initial value */
	  0xFFFF,			/* Top value */
	  t4_capture_channel_enables,	/* Capture Channel Enables */
	  t4_compare_channel_values);	/* Compare Channels Values */

  /* Timer 4 generates an event on compare channel 0 */
  struct tc_events events;
  events.generate_event_on_compare_channel[0] = true;
  events.generate_event_on_compare_channel[1] = false;
  events.generate_event_on_overflow = false;
  events.invert_event_input = false;
  events.on_event_perform_action = true;
  events.event_action = TC_EVENT_ACTION_RETRIGGER;
  tc_enable_events(TC4, &events);

  events_attach_user(0, 4); // Timer 4 is event user on channel 1

  /* This event is picked up on event channel 0 */
  events_allocate(0,
                  EVENTS_EDGE_DETECT_NONE,
                  EVENTS_PATH_ASYNCHRONOUS,
                  0x29, /* TC4 MC0 */
                  0);

  /* This event is picked up on event channel 1 */
  events_allocate(1,
                  EVENTS_EDGE_DETECT_NONE,
                  EVENTS_PATH_ASYNCHRONOUS,
                  0x29, /* TC4 MC0 */
                  0);

  tc_enable(TC4); /* Retrigger event means counter doesn't start yet */
  tc_start_counter(TC4); /* We start it manually now */
}
void osc8m_event_source_disable(void) {
  tc_disable(TC4);
}

/**
 * Configure the timepulse extint to generate events
 */
void timepulse_extint_event_source(void) {
  /* Nothing to do: event should be already in place */
}
void timepulse_extint_event_source_disable(void) {
  /* Nothing to do here */
}

/**
 * Triggers a measurements the number of cycles on XOSC
 *
 * A callback from the timer interrupt is available. Obviously don't dwell here too long.
 */
void measure_xosc(enum xosc_measurement_t measurement_t,
                  measurement_result_t callback) {

  measure_state = MEASURE_WAIT_FOR_FIRST_EVENT;
  _measurement_t = measurement_t;
  _callback = callback;

  /* Timer 2 runs on GLCK1: XOSC */
  bool t2_capture_channel_enables[]    = {true, true};
  uint32_t t2_compare_channel_values[] = {0x0000, 0x0000};

  tc_init(TC2,
	  GCLK_GENERATOR_1,
	  TC_COUNTER_SIZE_32BIT,
	  TC_CLOCK_PRESCALER_DIV1,
	  TC_WAVE_GENERATION_NORMAL_FREQ,
	  TC_RELOAD_ACTION_GCLK,
	  TC_COUNT_DIRECTION_UP,
	  TC_WAVEFORM_INVERT_OUTPUT_NONE,
	  false,			/* Oneshot  */
	  false,			/* Run in standby */
	  0x0000,			/* Initial value */
	  0xFFFFFFFF,			/* Top value */
	  t2_capture_channel_enables,	/* Capture Channel Enables */
	  t2_compare_channel_values);	/* Compare Channels Values */

  /* Timer 2 event input captures period in CC0, pulse width in CC1 */
  struct tc_events events;
  events.generate_event_on_compare_channel[0] = false;
  events.generate_event_on_compare_channel[1] = false;
  events.generate_event_on_overflow = false;
  events.invert_event_input = false;
  events.on_event_perform_action = true;
  events.event_action = TC_EVENT_ACTION_PPW;
  tc_enable_events(TC2, &events);

  /* Enable Interrupt */
  TC2->COUNT32.INTENSET.reg = (1 << 4); // MC0
  irq_register_handler(TC2_IRQn, TC2_INT_PRIO); /* Lowish Priority */

  /* Timer 2 is event user on channel 0 */
  events_attach_user(0, 2);

  /* Configure an event source */
  switch (measurement_t) {
  case XOSC_MEASURE_OSC8M:
    osc8m_event_source();     // osc8m issues events
    break;
  case XOSC_MEASURE_TIMEPULSE:
    timepulse_extint_event_source(); // timepulse issues events
    break;
  }

  tc_enable(TC2);
}
void measure_xosc_disable(enum xosc_measurement_t measurement_t) {

  tc_disable(TC2);

  switch (measurement_t) {
  case XOSC_MEASURE_OSC8M:
    osc8m_event_source_disable();
    break;
  case XOSC_MEASURE_TIMEPULSE:
    timepulse_extint_event_source_disable();
    break;
  }
}


/**
 * Triggered on timer 2 capture
 */
void TC2_Handler(void) {
  uint32_t capture_value;

  if (tc_get_status(TC2) & TC_STATUS_CHANNEL_0_MATCH) {
    tc_clear_status(TC2, TC_STATUS_CHANNEL_0_MATCH);

    switch (measure_state) {
    case MEASURE_WAIT_FOR_FIRST_EVENT:
      measure_state = MEASURE_MEASUREMENT; /* Start measurement */
      break;
    case MEASURE_MEASUREMENT:
      /* Measurement done. Read off data */
      capture_value = tc_get_capture_value(TC2, 0);

      /* Callback if we have one */
      if (_callback) {
        _callback(capture_value * XOSC_COUNT_RESOLUTION);
      }

      /* Disable measurement system */
      measure_xosc_disable(_measurement_t);
    }
  }
}
