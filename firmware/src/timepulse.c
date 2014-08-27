/*
 * Functions for turning the GPS timepulse into a HF Clock
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
#include "system/clock.h"
#include "system/gclk.h"
#include "system/pinmux.h"
#include "tc/tc_driver.h"
#include "hw_config.h"

#define DFLL48_MUL	(DFLL48M_CLK / GPS_TIMEPULSE_FREQ)

/* Check that DFLL48_MUL is an integer */
#if ((DFLL48M_CLK * 100000000) / GPS_TIMEPULSE_FREQ != (DFLL48_MUL * 100000000))
#error DFLL48M_CLK must be a integer multiple of GPS_TIMEPULSE_FREQ!
#endif

void timepulse_init(void)
{
  /* Set up the DFLL GCLK channel */
  system_gclk_chan_set_config(SYSCTRL_GCLK_ID_DFLL48, DFLL48M_GCLK);
  system_gclk_chan_enable(SYSCTRL_GCLK_ID_DFLL48);

  /* Configure DFLL48 */
  system_clock_source_dfll_set_config(
    SYSTEM_CLOCK_DFLL_LOOP_MODE_CLOSED,		/* Loop Mode */
    false,					/* On demand */
    SYSTEM_CLOCK_DFLL_QUICK_LOCK_DISABLE,	/* Quick Lock */
    SYSTEM_CLOCK_DFLL_CHILL_CYCLE_ENABLE,	/* Chill Cycle */
    SYSTEM_CLOCK_DFLL_WAKEUP_LOCK_KEEP,		/* Lock during wakeup */
    SYSTEM_CLOCK_DFLL_STABLE_TRACKING_TRACK_AFTER_LOCK,
    0x1f / 4,		/* Open Loop   - Coarse calibration value */
    0xff / 4,		/* Open Loop   - Fine calibration value */
    1,			/* Closed Loop - Coarse Maximum step */
    1,			/* Closed Loop - Fine Maximum step */
    DFLL48_MUL);	/* Frequency Multiplication Factor */

  /* Enable DFLL48 */
  system_clock_source_enable(SYSTEM_CLOCK_SOURCE_DFLL);

  /* Wait for it to be ready */
  while(!system_clock_source_is_ready(SYSTEM_CLOCK_SOURCE_DFLL));

  /* system_clock_source_xosc_set_config(SYSTEM_CLOCK_EXTERNAL_CLOCK, */
  /*  					   SYSTEM_XOSC_STARTUP_16384, */
  /*  					   true,	/\* Auto gain control *\/ */
  /*  					   16000000UL,	/\* Frequency *\/ */
  /*  					   true,	/\* Run in Standby *\/ */
  /*  					   false);	/\* Run on demand *\/ */
  /* system_clock_source_enable(SYSTEM_CLOCK_SOURCE_XOSC); */

  /* Configure the HF GCLK */
  system_gclk_gen_set_config(SI406X_HF_GCLK,
			     GCLK_SOURCE_DFLL48M, /* Source 		*/
			     false,		/* High When Disabled	*/
			     3,			/* Division Factor = 16MHz*/
			     false,		/* Run in standby	*/
			     true);		/* Output Pin Enable	*/

  /* Configure the output pin */
  system_pinmux_pin_set_config(SI406X_HF_CLK_PINMUX >> 16,	/* GPIO Pin	*/
			       SI406X_HF_CLK_PINMUX & 0xFFFF,	/* Mux Position	*/
			       SYSTEM_PINMUX_PIN_DIR_INPUT,	/* Direction	*/
			       SYSTEM_PINMUX_PIN_PULL_NONE,	/* Pull		*/
			       false);    			/* Powersave	*/

  /* Enable the HF GCLK */
  system_gclk_gen_enable(SI406X_HF_GCLK);
}

/**
 * Switches GCLK_MAIN (a.k.a. GCLK0) to the gps timepulse
 */
void switch_gclk_main_to_timepulse(void)
{
  /* Enable GCLK_IO[0] */
  system_pinmux_pin_set_config(GPS_TIME_PINMUX >> 16,		/* GPIO Pin	*/
			       GPS_TIME_PINMUX & 0xFFFF,	/* Mux Position	*/
			       SYSTEM_PINMUX_PIN_DIR_INPUT,	/* Direction	*/
			       SYSTEM_PINMUX_PIN_PULL_NONE,	/* Pull		*/
			       false);    			/* Powersave	*/

  /* Switch GCLK_MAIN to GCLK_IO[0] */
  system_gclk_gen_set_config(GCLK_GENERATOR_0,	/* GCLK 0 */
			     GCLK_SOURCE_GCLKIN,/* Source from pin	*/
			     false,		/* High When Disabled	*/
			     1,			/* Division Factor	*/
			     true,		/* Run in standby	*/
			     true);		/* Output Pin Enable	*/

  /* Wait for switch? */
}

/**
 * Outputs GCLK0 div 2 on the HF CLK pin
 */
void half_glck_main_on_hf_clk(void)
{
  bool capture_channel_enables[]    = {true, true};
  uint32_t compare_channel_values[] = {0x0000, 0x0000};

  tc_init(TC2,
	  GCLK_GENERATOR_0,
	  TC_COUNTER_SIZE_8BIT,
	  TC_CLOCK_PRESCALER_DIV1,
	  TC_WAVE_GENERATION_NORMAL_FREQ,
	  TC_RELOAD_ACTION_GCLK,
	  TC_COUNT_DIRECTION_UP,
	  TC_WAVEFORM_INVERT_OUTPUT_NONE,
	  false,			/* Oneshot = false */
	  false,			/* Run in standby = false */
	  0x0000,			/* Initial value */
	  0x0000,			/* Top value */
	  capture_channel_enables,	/* Capture Channel Enables */
	  compare_channel_values);	/* Compare Channels Values */

  /* Enable the output pin */
  system_pinmux_pin_set_config(PINMUX_PA17F_TC2_WO1 >> 16,	/* GPIO Pin	*/
			       PINMUX_PA17F_TC2_WO1 & 0xFFFF,	/* Mux Position	*/
			       SYSTEM_PINMUX_PIN_DIR_INPUT,	/* Direction	*/
			       SYSTEM_PINMUX_PIN_PULL_NONE,	/* Pull		*/
			       false);    			/* Powersave	*/

  tc_enable(TC2);
  tc_start_counter(TC2);
}

/**
 * Returns the current GCLK_MAIN frequency, as measured against OSC8M
 */
uint32_t gclk_main_frequency(void)
{
  uint32_t osc8m_frequency = 8000000UL >> SYSCTRL->OSC8M.bit.PRESC;

  /* Configure GCLK Gen 6 as reference */
  system_gclk_gen_set_config(GCLK_GENERATOR_6,
			     GCLK_SOURCE_OSC8M, /* Source 		*/
			     false,		/* High When Disabled	*/
			     4,			/* Division Factor	*/
			     false,		/* Run in standby	*/
			     false);		/* Output Pin Enable	*/
  /* Enable GCLK 6 */
  system_gclk_gen_enable(GCLK_GENERATOR_6);

  /* Timer 0 free runs on GLCK 0 */
  bool t0_capture_channel_enables[]    = {false, false};
  uint32_t t0_compare_channel_values[] = {0x0000, 0x0000};

  tc_init(TC0,
	  GCLK_GENERATOR_0,
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
	  t0_capture_channel_enables,	/* Capture Channel Enables */
	  t0_compare_channel_values);	/* Compare Channels Values */

  /* Timer 3 counts 10000 cycles of GLCK 6 */
  bool t1_capture_channel_enables[]    = {false, false};
  uint32_t t1_compare_channel_values[] = {10000, 0x0000};

  tc_init(TC3,
	  GCLK_GENERATOR_6,
	  TC_COUNTER_SIZE_16BIT,
	  TC_CLOCK_PRESCALER_DIV1,
	  TC_WAVE_GENERATION_NORMAL_FREQ,
	  TC_RELOAD_ACTION_GCLK,
	  TC_COUNT_DIRECTION_UP,
	  TC_WAVEFORM_INVERT_OUTPUT_NONE,
	  false,			/* Oneshot  */
	  false,			/* Run in standby */
	  0x0000,			/* Initial value */
	  0xFFFF,			/* Top value */
	  t1_capture_channel_enables,	/* Capture Channel Enables */
	  t1_compare_channel_values);	/* Compare Channels Values */

  tc_enable(TC0);
  tc_enable(TC3);
  tc_start_counter(TC0);
  tc_start_counter(TC3);

  /* Wait 10000 cycles of GCLK 6 */
  while (!(tc_get_status(TC3) & TC_STATUS_CHANNEL_0_MATCH));

  uint32_t gclk_main_count = tc_get_count_value(TC0) - 50;

  return gclk_main_count / 10;
}
