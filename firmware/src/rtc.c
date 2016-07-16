/*
 * Initialised RTC to provide 1Hz event and interrupt
 * Copyright (C) 2016  Richard Meadows <richardeoin>
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
#include "hw_config.h"
#include "system/interrupt.h"
#include "system/rtc_count.h"
#include "system/events.h"

/**
 * Initialises RTC to provide 1Hz event and interrupt
 */
void rtc_init(void)
{
  /* 16 bit, 1024Hz clock in on GCLK2 */
  struct rtc_count_config config_rtc_count;
  rtc_count_get_config_defaults(&config_rtc_count);
  config_rtc_count.prescaler           = RTC_COUNT_PRESCALER_DIV_1024;
  config_rtc_count.mode                = RTC_COUNT_MODE_16BIT;
  rtc_count_init(&config_rtc_count);

  /* Enable event generation for PER7 only */
  struct rtc_count_events config_events;
  config_events.generate_event_on_overflow = false;
  for (uint8_t i = 0; i < RTC_NUM_OF_COMP16; i++) {
    config_events.generate_event_on_compare[i] = false;
  }
  for (uint8_t i = 0; i < 8; i++) {
    config_events.generate_event_on_periodic[i] = false;
  }
  config_events.generate_event_on_periodic[7] = true;
  rtc_count_enable_events(&config_events);

  /* Enable interrupts */
  RTC->MODE1.INTENSET.reg |= RTC_MODE1_INTENSET_OVF; /* Overflow interrupt */
  irq_register_handler(RTC_IRQn, RTC_INT_PRIO); /* Lowest Priority */

  /* Enable */
  rtc_count_enable();
  rtc_count_set_period(0);      /* overflow on every tick */
}

/**
 * Tick functions
 * =============================================================================
 */

/* Seconds since APRS transmission */
uint32_t since_aprs_s = 0;
uint32_t get_since_aprs_s(void) {
  return since_aprs_s;
}
void clear_since_aprs_s(void) {
  since_aprs_s = 0;
}


#define HIBERNATE_TIME_MAX	(3600) /* Hibernate should always be set lower that this */
volatile uint32_t hibernate_time_s = 0;
void run_kick(void);

uint32_t tick = 0;

/**
 * Set hibernate time. Must be called on every iteration of the timer
 */
void rtc_hibernate_time(uint32_t time_s)
{
  /* set hibernate time */
  hibernate_time_s = time_s;

  /* clear ticks */
  since_aprs_s += tick;
  tick = 0;
}
/**
 * Interrupt for RTC, called at 1Hz
 */
void RTC_Handler(void)
{
  if (RTC->MODE1.INTFLAG.reg & RTC_MODE1_INTFLAG_OVF) {
    RTC->MODE1.INTFLAG.reg |= RTC_MODE1_INTFLAG_OVF; /* Clear flag */

    /* Check sleep time  */
    if (tick >= hibernate_time_s) {
      /* clear ticks */
      since_aprs_s += tick;
      tick = 0;

      /* set hibernate time to max */
      hibernate_time_s = HIBERNATE_TIME_MAX;

      /* Do something */

    } else {
      /* Increment tick */
      tick++;
    }
  }
}
