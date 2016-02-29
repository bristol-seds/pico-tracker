/*
 * Board init functions
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

#include "samd20.h"
#include "hw_config.h"
#include "system/system.h"
#include "system/port.h"
#include "system/events.h"
#include "system/extint.h"
#include "i2c_bb.h"
#include "gps.h"
#include "si_trx.h"
#include "watchdog.h"
#include "xosc.h"
#include "cron.h"
#include "data.h"
#include "telemetry.h"
#include "barometer.h"
#include "init.h"

/**
 * Initialises the status LED. SHOULD TURN ON THE LED
 */
static inline void led_reset(void)
{
  port_pin_set_config(LED0_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */
  port_pin_set_output_level(LED0_PIN, 0);	/* LED is active low */
}

/**
 * Power Management
 */
void powermananger_init(void)
{
  system_apb_clock_clear_mask(SYSTEM_CLOCK_APB_APBA,
//			      PM_APBAMASK_EIC | /* EIC is used now */
			      PM_APBAMASK_RTC |
    0);
}

/**
 * Internal initialisation
 * =============================================================================
 */
void init(enum init_type init_t)
{
  /**
   * Make the external watchdog safely armed, ready to trigger if we don't call watchdog_init.
   * (hw v0.987.3 adds an external resistor for this, this call can be removed then)
   */
  external_watchdog_safe();

  /**
   * OSC8M should be considered unstable due to the temperature range. Therefore
   * we need to switch to a stable low frequency clock right away.
   * --------------------------------------------------------------------------
   */
  lf_clock_startup();              /* ~500ms startup */
  gclk0_to_lf_clock();          /* switch, clocking at 32kHz now */
  system_clock_source_disable(SYSTEM_CLOCK_SOURCE_OSC8M);

  /**
   * Reset to get the system in a safe state
   * --------------------------------------------------------------------------
   */
  led_reset();
  gps_reset();
  si_trx_shutdown();

  /**
   * Watchdog bringup, includes first kick
   * --------------------------------------------------------------------------
   */
  if (init_t != INIT_TESTCASE) {
    watchdog_init();
  }

  /**
   * Internal initialisation
   * ---------------------------------------------------------------------------
   */

  /* Switch to high frequency clock */
  hf_clock_init();              /* TCXO powered on */
  hf_clock_enable();            /* TCXO startup time.. */
  gclk0_to_hf_clock();          /* and switch, clock at 8MHz */
  gclk1_init();
  gclk2_init();

  /* Clock up to 14MHz with 0 wait states */
  system_flash_set_waitstates(SYSTEM_WAIT_STATE_1_8V_14MHZ);

  /* Restart the GCLK Module */
  system_events_init();
  system_extint_init();

  /* Configure Sleep Mode */
  system_set_sleepmode(SYSTEM_SLEEPMODE_IDLE_2); /* Lowest power */

  /* Configure the Power Manager */
  powermananger_init();

  /* We've done good things, kick wdt */
  kick_the_watchdog();

  /**
   * System initialisation
   * ---------------------------------------------------------------------------
   */

#if USE_BAROMETER
  /* i2c */
  i2c_bb_init();

  /* barometer */
  barometer_init();
#endif

  if (init_t != INIT_TESTCASE) {
    /* Telemetry init depends on gclk */
    telemetry_init();

    /* We need to wait for the GPS 32kHz clock to start (~300ms). TODO: more robust method for this */
    for (int i = 0; i < 300*1000; i++);

    /* GPS init */
    gps_init();
  }

  kick_the_watchdog();

  /* Initialise Si4060 interface */
  si_trx_init();
}
