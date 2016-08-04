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
#ifndef WDT_H_INCLUDED
#define WDT_H_INCLUDED

/**
 * SAM D20/D21/R21 Watchdog Driver (WDT)
 *
 * This driver for SAM D20/D21/R21 devices provides an interface for
 * the configuration and management of the device's Watchdog Timer
 * module, including the enabling, disabling and kicking within the
 * device. The following driver API modes are covered by this manual:
 *
 *  - Polled APIs
 *
 * The following peripherals are used by this module:
 *
 *  - WDT (Watchdog Timer)
 *
 * Module Overview
 *
 * The Watchdog module (WDT) is designed to give an added level of
 * safety in critical systems, to ensure a system reset is triggered
 * in the case of a deadlock or other software malfunction that
 * prevents normal device operation.
 *
 * At a basic level, the Watchdog is a system timer with a fixed
 * period; once enabled, it will continue to count ticks of its
 * asynchronous clock until it is periodically reset, or the timeout
 * period is reached. In the event of a Watchdog timeout, the module
 * will trigger a system reset identical to a pulse of the device's
 * reset pin, resetting all peripherals to their power-on default
 * states and restarting the application software from the reset
 * vector.
 *
 * In many systems, there is an obvious upper bound to the amount of
 * time each iteration of the main application loop can be expected to
 * run, before a malfunction can be assumed (either due to a deadlock
 * waiting on hardware or software, or due to other means). When the
 * Watchdog is configured with a timeout period equal to this upper
 * bound, a malfunction in the system will force a full system reset
 * to allow for a graceful recovery.
 *
 * Locked Mode
 *
 * The Watchdog configuration can be set in the device fuses and
 * locked in hardware, so that no software changes can be made to the
 * Watchdog configuration. Additionally, the Watchdog can be locked on
 * in software if it is not already locked, so that the module
 * configuration cannot be modified until a power on reset of the
 * device.
 *
 * The locked configuration can be used to ensure that faulty software
 * does not cause the Watchdog configuration to be changed, preserving
 * the level of safety given by the module.
 *
 * Window Mode
 *
 * Just as there is a reasonable upper bound to the time the main
 * program loop should take for each iteration, there is also in many
 * applications a lower bound, i.e. a minimum time for which each loop
 * iteration should run for under normal circumstances. To guard
 * against a system failure resetting the Watchdog in a tight loop (or
 * a failure in the system application causing the main loop to run
 * faster than expected) a "Window" mode can be enabled to disallow
 * resetting of the Watchdog counter before a certain period of time.
 * If the Watchdog is not reset after the window opens but not before
 * the Watchdog expires, the system will reset.
 *
 * Early Warning
 *
 * In some cases it is desirable to receive an early warning that the
 * Watchdog is about to expire, so that some system action (such as
 * saving any system configuration data for failure analysis purposes)
 * can be performed before the system reset occurs. The Early Warning
 * feature of the Watchdog module allows such a notification to be
 * requested; after the configured early warning time (but before the
 * expiry of the Watchdog counter) the Early Warning flag will become
 * set, so that the user application can take an appropriate action.
 *
 * \note It is important to note that the purpose of the Early Warning feature
 *       is not to allow the user application to reset the Watchdog; doing
 *       so will defeat the safety the module gives to the user application.
 *       Instead, this feature should be used purely to perform any tasks that
 *       need to be undertaken before the system reset occurs.
 *
 * Special Considerations
 *
 * On some devices the Watchdog configuration can be fused to be always on in
 * a particular configuration; if this mode is enabled the Watchdog is not
 * software configurable and can have its count reset and early warning state
 * checked/cleared only.
 *
 */

#include "system/clock.h"
#include "system/gclk.h"

/**
 * Enum for the possible period settings of the Watchdog timer module, for
 * values requiring a period as a number of Watchdog timer clock ticks.
 */
enum wdt_period {
  /** No Watchdog period. This value can only be used when setting the
   *  Window and Early Warning periods; its use as the Watchdog Reset
   *  Period is invalid. */
  WDT_PERIOD_NONE     = 0,
  /** Watchdog period of 8 clocks of the Watchdog Timer Generic Clock. */
  WDT_PERIOD_8CLK     = 1,
  /** Watchdog period of 16 clocks of the Watchdog Timer Generic Clock. */
  WDT_PERIOD_16CLK    = 2,
  /** Watchdog period of 32 clocks of the Watchdog Timer Generic Clock. */
  WDT_PERIOD_32CLK    = 3,
  /** Watchdog period of 64 clocks of the Watchdog Timer Generic Clock. */
  WDT_PERIOD_64CLK    = 4,
  /** Watchdog period of 128 clocks of the Watchdog Timer Generic Clock. */
  WDT_PERIOD_128CLK   = 5,
  /** Watchdog period of 256 clocks of the Watchdog Timer Generic Clock. */
  WDT_PERIOD_256CLK   = 6,
  /** Watchdog period of 512 clocks of the Watchdog Timer Generic Clock. */
  WDT_PERIOD_512CLK   = 7,
  /** Watchdog period of 1024 clocks of the Watchdog Timer Generic Clock. */
  WDT_PERIOD_1024CLK  = 8,
  /** Watchdog period of 2048 clocks of the Watchdog Timer Generic Clock. */
  WDT_PERIOD_2048CLK  = 9,
  /** Watchdog period of 4096 clocks of the Watchdog Timer Generic Clock. */
  WDT_PERIOD_4096CLK  = 10,
  /** Watchdog period of 8192 clocks of the Watchdog Timer Generic Clock. */
  WDT_PERIOD_8192CLK  = 11,
  /** Watchdog period of 16384 clocks of the Watchdog Timer Generic Clock. */
  WDT_PERIOD_16384CLK = 12,
};

void wdt_set_config(bool always_on,
		    bool enable,
		    enum gclk_generator clock_source,
		    enum wdt_period timeout_period,
		    enum wdt_period window_period,
		    enum wdt_period early_warning_period);
void wdt_set_config_default(void);

/**
 *  Determines if the Watchdog timer is currently enabled and locked, so that
 *  it cannot be disabled or otherwise reconfigured.
 *
 *  \return Current Watchdog lock state.
 */
static inline bool wdt_is_locked(void)
{
  Wdt *const WDT_module = WDT;

  return (WDT_module->CTRL.reg & WDT_CTRL_ALWAYSON);
}

/**
 *  Clears the Watchdog timer Early Warning period elapsed flag, so that a new
 *  early warning period can be detected.
 */
static inline void wdt_clear_early_warning(void)
{
  Wdt *const WDT_module = WDT;

  WDT_module->INTFLAG.reg = WDT_INTFLAG_EW;
}

/**
 *  Determines if the Watchdog timer Early Warning period has elapsed.
 *
 *  \note If no early warning period was configured, the value returned by this
 *        function is invalid.
 *
 *  \return Current Watchdog Early Warning state.
 */
static inline bool wdt_is_early_warning(void)
{
  Wdt *const WDT_module = WDT;

  return (WDT_module->INTFLAG.reg & WDT_INTFLAG_EW);
}

void wdt_reset_count(void);

#endif /* WDT_H_INCLUDED */
