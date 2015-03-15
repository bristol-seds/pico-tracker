/*
 * Functions for running system timings
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
#include "hw_config.h"
#include "timer.h"
#include "system/extint.h"
#include "system/events.h"
#include "system/interrupt.h"

uint32_t gps_timepulse_count = 0;
uint32_t telemetry_interval_count = TELEMETRY_INTERVAL;
uint32_t timepulse_sequence = 0;

timepulse_callback_t _timer_callback;

/**
 * Enables an interrupt on the GPS_TIMEPULSE and routes it to event channel 0
 */
void timepulse_extint_init(void) {

  /* Enable extint events for gps timepulse */
  struct extint_events events;
  memset(&events, 0, sizeof(struct extint_events));
  events.generate_event_on_detect[GPS_TIMEPULSE_EXTINT] = true;
  extint_enable_events(&events);

  /* Configure extinit channel */
  struct extint_chan_conf config;
  config.gpio_pin = GPS_TIMEPULSE_PIN;
  config.gpio_pin_mux = GPS_TIMEPULSE_PINMUX;
  config.gpio_pin_pull = EXTINT_PULL_NONE; // ???
  config.wake_if_sleeping = false; // ???
  config.filter_input_signal = false;
  config.detection_criteria = EXTINT_DETECT_RISING;
  extint_chan_set_config(GPS_TIMEPULSE_EXTINT, &config);

  /* We route this event to event channel 0 */
  events_allocate(0,
                  EVENTS_EDGE_DETECT_NONE,
                  EVENTS_PATH_ASYNCHRONOUS,
                  0xC + GPS_TIMEPULSE_EXTINT, /* External Interrupt Number */
                  0);

  /* Interrupt handler below */
  EIC->INTENSET.reg = (1 << GPS_TIMEPULSE_EXTINT);
  irq_register_handler(EIC_IRQn, EIC_INT_PRIO);

  extint_enable();
}

void timepulse_set_callback(timepulse_callback_t callback) {
  _timer_callback = callback;
}

/**
 * EIC Handler, triggered by the GPS at GPS_TIMEPULSE_FREQ Hz
 */
void EIC_Handler(void) {

  if (EIC->INTFLAG.reg & (1 << GPS_TIMEPULSE_EXTINT)) {
    EIC->INTFLAG.reg = (1 << GPS_TIMEPULSE_EXTINT);

    gps_timepulse_count++;
    /* Runs at 1Hz */
    if (gps_timepulse_count >= GPS_TIMEPULSE_FREQ) {
      gps_timepulse_count = 0;

      telemetry_interval_count++;
      /* Runs at the rate of telemetry packets */
      if (telemetry_interval_count >= TELEMETRY_INTERVAL) {
        telemetry_interval_count = 0;

        /* Make the callback if we have one */
        if (_timer_callback) {
          _timer_callback(timepulse_sequence++);
        }
      }
    }
  }
}
