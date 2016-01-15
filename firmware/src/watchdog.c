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
#include "system/interrupt.h"
#include "system/port.h"
#include "tc/tc_driver.h"
#include "si_trx.h"
#include "init.h"

struct idle_counter idle_count, idle_count_max;

idle_wait_t last_idle_t = IDLE_NONE;

#define kick_external_watchdog()	do { port_pin_set_output_level(WDT_WDI_PIN, 1); \
    __NOP();                      /* > 50ns high */                     \
    port_pin_set_output_level(WDT_WDI_PIN, 0);                          \
  } while(0)

/**
 * Increments the specified idle counter
 */
void increment_idle_counter(idle_wait_t idle_t)
{

  switch (idle_t) {
    case IDLE_TELEMETRY_ACTIVE:
      idle_count.while_telemetry_active++;
      break;
    case IDLE_WAIT_FOR_NEXT_TELEMETRY:
      idle_count.wait_for_next_telemetry++;
      break;
    case IDLE_WAIT_FOR_GPS:
      idle_count.wait_for_gps++;
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
  if ((idle_count.while_telemetry_active > MAXIDLE_WHILE_TELEMETRY_ACTIVE) ||
      (idle_count.wait_for_next_telemetry > MAXIDLE_WAIT_FOR_NEXT_TELEMETRY) ||
      (idle_count.wait_for_next_telemetry > MAXIDLE_WAIT_FOR_GPS)) {
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
  SET_COUNT_MAX(while_telemetry_active);
  SET_COUNT_MAX(wait_for_next_telemetry);
  SET_COUNT_MAX(wait_for_gps);

  /* Zero out counter */
  memset(&idle_count, 0, sizeof(struct idle_counter));
}

/**
 * To be run when we wake from sleep
 */
void awake_do_watchdog(void)
{
#ifdef DEBUG_USE_INTWATCHDOG
  wdt_reset_count();
#endif

  /* WDI high */
  port_pin_set_output_level(WDT_WDI_PIN, 1);
}
/**
 * Kick
 */
void kick_the_watchdog(void)
{
#ifdef DEBUG_USE_INTWATCHDOG
  wdt_reset_count();
#endif
  kick_external_watchdog();
}
/**
 * Called in idle loops. Kicks the watchdog
 *
 * idle_t - The type of idle loop
 */
void idle(idle_wait_t idle_t)
{
  /* Check valid */
  if ((idle_t != IDLE_TELEMETRY_ACTIVE) &&
      (idle_t != IDLE_WAIT_FOR_NEXT_TELEMETRY) &&
      (idle_t != IDLE_WAIT_FOR_GPS)) {
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

  /* Kick the watchdog */
#ifdef DEBUG_USE_INTWATCHDOG
  wdt_reset_count();
#endif

  /* WDI toggle */
  port_pin_toggle_output_level(WDT_WDI_PIN);

  /* And sleep */
  system_sleep();
}


/**
 * The internal watchdog is used to bring the processor to a halt and
 * coredump to external memory (todo)
 * 0.6s < t_early_w < 0.96s
 *
 * The external watchdog then hard resets the MCU and GPS to bring the
 * system back up in a clean state.
 * 0.8s < tout < 2.1s
 */

void watchdog_init(void)
{
  /* Setup the external watchdog interrupt pin */
  port_pin_set_config(WDT_WDI_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */
  kick_external_watchdog();                     /* Kick External */

#if DEBUG_USE_INTWATCHDOG
  /* /\* 0.5s early warn. So 2^(15-1) cycles of the 32.768kHz ulposc *\/ */
  system_gclk_gen_set_config(WDT_GCLK,
        		     GCLK_SOURCE_OSCULP32K, /* Source 		*/
        		     false,		/* High When Disabled	*/
        		     3,			/* Division Factor	*/
        		     false,		/* Run in standby	*/
        		     true);		/* Output Pin Enable	*/
  system_gclk_gen_enable(WDT_GCLK);

  /* Set the watchdog timer. On ~11kHz gclk  */
  wdt_set_config(false,			/* Lock WDT		*/
                 true,			/* Enable WDT		*/
                 WDT_GCLK,		/* Clock Source		*/
                 WDT_PERIOD_16384CLK,	/* Timeout Period	*/
                 WDT_PERIOD_NONE,	/* Window Period	*/
                 WDT_PERIOD_8192CLK);	/* Early Warning Period	*/

  WDT->INTENSET.reg |= WDT_INTENSET_EW;
  WDT->INTFLAG.reg |= WDT_INTFLAG_EW;
  irq_register_handler(WDT_IRQn, WDT_INT_PRIO);

  wdt_reset_count();
#endif
}


void WDT_Handler(void)
{
  /* Bring the system into a safe state */
  si_trx_shutdown();

  /* LED on */
  led_on();

  /* Coredump */

  /* Wait for the external watchdog to kill us */
  while (1) {
    led_on();
    for (int i = 0; i < 25*1000; i++);
    led_off();
    for (int i = 0; i < 25*1000; i++);

    /**
     * Whilst this is generally bad practice in this system we have an
     * external watchdog for the actual reset.
     */
    wdt_reset_count();
  }
}
