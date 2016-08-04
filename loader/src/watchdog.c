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
    case IDLE_LOADER:
      idle_count.while_loader++;
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
  if ((idle_count.while_loader > MAXIDLE_WHILE_LOADER)) {
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
  SET_COUNT_MAX(while_loader);

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
  if ((idle_t != IDLE_LOADER)) {
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
 * Put the external watchdog is a safe state where it will trigger if
 * watchdog_init is not called within tout
 */
void external_watchdog_safe(void)
{
  port_pin_set_output_level(WDT_WDI_PIN, 0);
  /* Setup the external watchdog interrupt pin */
  port_pin_set_config(WDT_WDI_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */
}
/**
 * The external watchdog then resets the MCU and GPS to bring the
 * system back up in a clean state.
 * 1.2s < tout < 2.4s (ADM6823 Table 1.)
 */
void watchdog_init(void)
{
  /* Setup the external watchdog interrupt pin */
  port_pin_set_config(WDT_WDI_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */
  kick_external_watchdog();                     /* Kick External */
}
