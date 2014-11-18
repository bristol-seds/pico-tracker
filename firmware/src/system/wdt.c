/**
 * SAM D20/D21/R21 Watchdog Driver
 *
 * Copyright (C) 2012-2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include "system/wdt.h"
#include "system/system.h"

#define WDT_WAIT_FOR_SYNC(hw)	while (hw->STATUS.reg & WDT_STATUS_SYNCBUSY)

/**
 * Writes a given configuration of a WDT configuration to the
 * hardware module
 */
void wdt_set_config(bool always_on,
		    bool enable,
		    enum gclk_generator clock_source,
		    enum wdt_period timeout_period,
		    enum wdt_period window_period,
		    enum wdt_period early_warning_period)
{
  Wdt *const hw = WDT;

  /* Turn on the digital interface clock */
  system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBA, PM_APBAMASK_WDT);

  /* Check of the Watchdog has been locked to be always on, if so, abort */
  if (wdt_is_locked()) {
    return;// STATUS_ERR_IO;
  }

  /* Check for an invalid timeout period, abort if found */
  if (timeout_period == WDT_PERIOD_NONE) {
    return;// STATUS_ERR_INVALID_ARG;
  }

  /* Make sure the Window and Early Warning periods are not more than the
   * reset period, abort if either is invalid */
  if ((timeout_period < window_period) ||
      (timeout_period < early_warning_period)) {
    return;// STATUS_ERR_INVALID_ARG;
  }

  /* Disable the Watchdog module */
  hw->CTRL.reg &= ~WDT_CTRL_ENABLE;

  if(enable == false) {
    return;// STATUS_OK;
  }

  /* Configure GCLK channel and enable clock */
  system_gclk_chan_set_config(WDT_GCLK_ID, clock_source);
  system_gclk_chan_enable(WDT_GCLK_ID);
  if (always_on) {
    system_gclk_chan_lock(WDT_GCLK_ID);
  }

  WDT_WAIT_FOR_SYNC(hw);

  uint32_t new_config = 0;

  /* Update the timeout period value with the requested period */
  new_config |= (timeout_period - 1) << WDT_CONFIG_PER_Pos;

  /* Check if the user has requested a reset window period */
  if (window_period != WDT_PERIOD_NONE) {
    hw->CTRL.reg |= WDT_CTRL_WEN;

    /* Update and enable the timeout period value */
    new_config |= (window_period - 1) << WDT_CONFIG_WINDOW_Pos;
  } else {
    /* Ensure the window enable control flag is cleared */
    hw->CTRL.reg &= ~WDT_CTRL_WEN;
  }

  WDT_WAIT_FOR_SYNC(hw);

  /* Write the new Watchdog configuration */
  hw->CONFIG.reg = new_config;

  /* Check if the user has requested an early warning period */
  if (early_warning_period != WDT_PERIOD_NONE) {
    WDT_WAIT_FOR_SYNC(hw);

    /* Set the Early Warning period */
    hw->EWCTRL.reg
      = (early_warning_period - 1) << WDT_EWCTRL_EWOFFSET_Pos;
  }

  WDT_WAIT_FOR_SYNC(hw);

  /* Either enable or lock-enable the Watchdog timer depending on the user
   * settings */
  if (always_on) {
    hw->CTRL.reg |= WDT_CTRL_ALWAYSON;
  } else {
    hw->CTRL.reg |= WDT_CTRL_ENABLE;
  }
}
/**
 * Writes the default configuration of thw WDT to the hardware module
 *
 *  - Not locked, to allow for further (re-)configuration
 *  - Enable WDT
 *  - Watchdog timer sourced from Generic Clock Channel 4
 *  - A timeout period of 16384 clocks of the Watchdog module clock
 *  - No window period, so that the Watchdog count can be reset at any time
 *  - No early warning period to indicate the Watchdog will soon expire
 */
void wdt_set_config_default(void)
{
  wdt_set_config(false,			/* Lock WDT		*/
		 true,			/* Enable WDT		*/
		 GCLK_GENERATOR_4,	/* Clock Source		*/
		 WDT_PERIOD_16384CLK,	/* Timeout Period	*/
		 WDT_PERIOD_NONE,	/* Window Period	*/
		 WDT_PERIOD_NONE);	/* Early Warning Period	*/
}

/**
 * Resets the current count of the Watchdog Timer, restarting the timeout
 * period count elapsed. This function should be called after the window
 * period (if one was set in the module configuration) but before the timeout
 * period to prevent a reset of the system.
 */
void wdt_reset_count(void)
{
  Wdt *const hw = WDT;

  WDT_WAIT_FOR_SYNC(hw);

  /* Reset the Watchdog module */
  hw->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;
}
