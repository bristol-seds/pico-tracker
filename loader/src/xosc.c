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
#include "system/port.h"
#include "system/pinmux.h"
#include "system/events.h"
#include "system/extint.h"
#include "hw_config.h"
#include "xosc.h"
//#include "rtc.h"
#include "watchdog.h"

enum measure_state_t {
  MEASURE_WAIT_FOR_FIRST_EVENT,
  MEASURE_MEASUREMENT,
} measure_state = MEASURE_WAIT_FOR_FIRST_EVENT;
enum xosc_measurement_t _measurement_t;
uint8_t _measurement_oneshot;
measurement_result_t _callback;

/**
 * =============================================================================
 * HF Clock              =======================================================
 * =============================================================================
 */

/**
 * Init hf clock
 */
void hf_clock_init(void)
{
#ifdef SI4xxx_TCXO_REG_EN_PIN   /* TCXO enable/disable pin */
  port_pin_set_config(SI4xxx_TCXO_REG_EN_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */
  port_pin_set_output_level(SI4xxx_TCXO_REG_EN_PIN, 1); /* Enable by default */
#endif
}
/**
 * Enables a high frequency clock for the system, either XOSC or OSC8M
 */
void hf_clock_enable(void)
{
#if USE_XOSC

  /* Enable TCXO if required */
#ifdef SI4xxx_TCXO_REG_EN_PIN
  port_pin_set_output_level(SI4xxx_TCXO_REG_EN_PIN, 1);
#endif

  /* Setup XOSC  */
  system_clock_source_xosc_set_config(SYSTEM_CLOCK_EXTERNAL_CLOCK,
                                      SYSTEM_XOSC_STARTUP_1,
                                      true,
                                      XOSC_FREQUENCY,
                                      false,
                                      false);
  system_clock_source_enable(SYSTEM_CLOCK_SOURCE_XOSC);

  /* Wait for it to stabilise */
  while (!system_clock_source_is_ready(SYSTEM_CLOCK_SOURCE_XOSC));

#else

  /* Setup OSC8M */
  system_clock_source_osc8m_set_config(SYSTEM_OSC8M_DIV_2, /* Prescaler */
				       false,		   /* Run in Standby */
				       false);		   /* Run on Demand */
  system_clock_source_enable(SYSTEM_CLOCK_SOURCE_OSC8M);

  /* Wait for it to stabilise */
  while(!system_clock_source_is_ready(SYSTEM_CLOCK_SOURCE_OSC8M));

#endif
}
/**
 * Disables the high frequency clock for the system, either XOSC or OSC8M
 */
void hf_clock_disable(void)
{
#if USE_XOSC

  /* Disable XOSC */
  system_clock_source_disable(SYSTEM_CLOCK_SOURCE_XOSC);

  /* Disable TCXO to save power */
#ifdef SI4xxx_TCXO_REG_EN_PIN
#if XOSC_TCXO_SHUTDOWN_EN
  port_pin_set_output_level(SI4xxx_TCXO_REG_EN_PIN, 0);
#endif
#endif

#else

  /* Disable OSC8M */
  system_clock_source_disable(SYSTEM_CLOCK_SOURCE_OSC8M);

#endif
}

/**
 * =============================================================================
 * LF Clock              =======================================================
 * =============================================================================
 */

/**
 * Startup lf clock
 */
void lf_clock_startup(void)
{
#if USE_LFTIMER
  /* Setup XOSC  */
  system_clock_source_xosc32k_set_config(SYSTEM_CLOCK_EXTERNAL_CLOCK,
                                         SYSTEM_XOSC32K_STARTUP_4096, /* 100-200ms startup*/
                                         false,
                                         false,
                                         true,
                                         true,
                                         false,
                                         false); /* write lock */
  system_clock_source_enable(SYSTEM_CLOCK_SOURCE_XOSC32K);

  /* Wait for it to stabilise */
  while (!system_clock_source_is_ready(SYSTEM_CLOCK_SOURCE_XOSC32K));
#else
/* OSCULP32K is always enabled, nothing to do here */
#endif
}

/**
 * =============================================================================
 * GCLK0                 =======================================================
 * =============================================================================
 */

/**
 * Switches GLCK0 to the HF clock
 */
void gclk0_to_hf_clock(void)
{
  /* Configure GCLK0 to XOSC / OSC8M */
  system_gclk_gen_set_config(GCLK_GENERATOR_0,
#if USE_XOSC
        		     GCLK_SOURCE_XOSC,	/* Source 		*/
#else
                             GCLK_SOURCE_OSC8M,	/* Source 		*/
#endif
        		     false,		/* High When Disabled	*/
#if USE_XOSC
        		     XOSC_GCLK_DIVIDE, /* Division Factor	*/
#else
                             OSC8M_GCLK_DIVIDE,               /* Division Factor */
#endif
        		     true,		/* Run in standby	*/
        		     false);		/* Output Pin Enable	*/
}
/**
 * Switches GCLK0 to the LF clock
 */
void gclk0_to_lf_clock(void)
{
  /* Configure GCLK0 to XOSC32K / OSCULP32K */
  system_gclk_gen_set_config(GCLK_GENERATOR_0,
#if USE_LFTIMER
                             GCLK_SOURCE_XOSC32K, /* Source             */
#else
                             GCLK_SOURCE_OSCULP32K, /* Source 		*/
#endif
        		     false,		/* High When Disabled	*/
                             1,                 /* Division Factor */
        		     true,		/* Run in standby	*/
        		     false);		/* Output Pin Enable	*/
}

/**
 * =============================================================================
 * GCLK1                 =======================================================
 * =============================================================================
 */

/**
 * Inits GCLK1. The appropriate source should have been enabled already
 */
void gclk1_init(void)
{
  /* Configure GCLK1 */
  system_gclk_gen_set_config(GCLK_GENERATOR_1,
#if USE_XOSC
        		     GCLK_SOURCE_XOSC,	/* Source 		*/
#else
                             GCLK_SOURCE_OSC8M,	/* Source 		*/
#endif
        		     false,		/* High When Disabled	*/
#if USE_XOSC
        		     XOSC_GCLK_DIVIDE, /* Division Factor	*/
#else
                             OSC8M_GCLK_DIVIDE,/* Division Factor */
#endif
        		     false,		/* Run in standby	*/
        		     false);		/* Output Pin Enable	*/

  /* Enable GCLK1 */
  system_gclk_gen_enable(GCLK_GENERATOR_1);
}

/**
 * =============================================================================
 * GCLK2                 =======================================================
 * =============================================================================
 */

/**
 * Inits GCLK2. The appropriate source should have been enabled already using lf_clock_init
 */
void gclk2_init(void)
{
  /* Configure GCLK2 */
  system_gclk_gen_set_config(GCLK_GENERATOR_2,
#if USE_LFTIMER
        		     GCLK_SOURCE_XOSC32K, /* Source 		*/
#else
                             GCLK_SOURCE_OSCULP32K, /* Source 		*/
#endif
        		     false,		/* High When Disabled	*/
                             32,                /* GLCK2 is 1024Hz nom. */
        		     true,		/* Run in standby	*/
        		     false);		/* Output Pin Enable	*/

  /* Enable GCLK2 */
  system_gclk_gen_enable(GCLK_GENERATOR_2);
}
