/*
 * Functions related to the watchdog.
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
#include "watchdog.h"
#include "hw_config.h"
#include "system/gclk.h"
#include "system/wdt.h"

struct idle_counter idle_count, idle_count_max;

idle_wait_t last_idle_t = IDLE_NONE;


/**
 * Increments the specified idle counter
 */
void increment_idle_counter(idle_wait_t idle_t)
{
  switch (idle_t) {
    case IDLE_WAIT_FOR_GPS:
      idle_count.wait_for_gps++;
      break;
    case IDLE_WHILE_TELEMETRY_ACTIVE:
      idle_count.while_telemetry_active++;
      break;
    case IDLE_WAIT_FOR_NEXT_TELEMETRY:
      idle_count.wait_for_next_telemetry++;
      break;
    default:
      /* Oh no no no. Let's die here */
      while(1);
  }
}
/**
 * Halts if any idle counter is above it's max
 */
void check_idle_counters(void)
{
  if ((idle_count.wait_for_gps > MAXIDLE_WAIT_FOR_GPS) ||
      (idle_count.while_telemetry_active > MAXIDLE_WHILE_TELEMETRY_ACTIVE) ||
      (idle_count.wait_for_next_telemetry > MAXIDLE_WAIT_FOR_NEXT_TELEMETRY)) {
    /* Oh dear. Let's die here */
    while (1);
  }
}
#define MAX(a,b)		((a>b)?a:b)
#define SET_COUNT_MAX(A)	idle_count_max.A = MAX(idle_count_max.A, idle_count.A)
/**
 * Clears the idle counters
 */
void clear_idle_counters(void)
{
  SET_COUNT_MAX(wait_for_gps);
  SET_COUNT_MAX(while_telemetry_active);
  SET_COUNT_MAX(wait_for_next_telemetry);

  /* Zero out counter */
  memset(&idle_count, 0, sizeof(struct idle_counter));
}

/**
 * Called in idle loops. Kicks the watchdog
 *
 * idle_t - The type of idle loop
 */
void watchdog_do_idle(idle_wait_t idle_t)
{
  /* Check valid */
  if ((idle_t != IDLE_WAIT_FOR_GPS) &&
      (idle_t != IDLE_WHILE_TELEMETRY_ACTIVE) &&
      (idle_t != IDLE_WAIT_FOR_NEXT_TELEMETRY)) {
    /* Oh dear */
    while (1);
  }

  /* Maybe clear */
  if (idle_t != last_idle_t) {
    clear_idle_counters();
    last_idle_t = idle_t;
  }

  /* Increment the idle counter */
  increment_idle_counter(idle_t);

  /* Check idle counter is still okay */
  check_idle_counters();

  /* And finally kick the watchdog */
  wdt_reset_count();
}


/**
 * The internal watchdog is used to bring the processor to a halt and
 * coredump to external memory.
 * 0.2s < tout < 0.32s
 *
 * The external watchdog then hard resets the MCU and GPS to bring the
 * system back up in a clean state.
 * 0.8s < tout < 2.1s
 */

void watchdog_init(void)
{
  /* 0.25 seconds timeout. So 2^(15-2) cycles of the 32.768kHz wdt clock */
  system_gclk_gen_set_config(WDT_GCLK,
			     GCLK_SOURCE_OSCULP32K, /* Source 		*/
			     false,		/* High When Disabled	*/
			     128,		/* Division Factor 2^7	*/
			     false,		/* Run in standby	*/
			     true);		/* Output Pin Enable	*/
  system_gclk_gen_enable(WDT_GCLK);

  /* Set the watchdog timer. On 256Hz gclk  */
  wdt_set_config(true,			/* Lock WDT		*/
  		 true,			/* Enable WDT		*/
  		 WDT_GCLK,		/* Clock Source */
  		 WDT_PERIOD_64CLK,	/* Timeout Period div 2^6 */
  		 WDT_PERIOD_NONE,	/* Window Period	*/
  		 WDT_PERIOD_NONE);	/* Early Warning Period	*/

  wdt_reset_count();
}


/**
 * Called for the watchdog early warning interrupt
 */
void WDT_Handler(void) {

  /* Coredump */

  /* Wait for the watchdog to kill us */
  while (1);
}
