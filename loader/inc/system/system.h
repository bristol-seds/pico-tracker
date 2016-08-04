/**
 * \file
 *
 * \brief SAM D20/D21/R21 System related functionality
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
#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include "system/clock.h"
#include "system/gclk.h"
#include "system/pinmux.h"

/**
 * SAM D20/D21/R21 System Driver (SYSTEM)
 *
 * This driver for SAM D20/D21/R21 devices provides an interface for
 * the configuration and management of the device's system relation
 * functionality, necessary for the basic device operation. This is
 * not limited to a single peripheral, but extends across multiple
 * hardware peripherals,
 *
 * The following peripherals are used by this module:
 *
 * - SYSCTRL (System Control)
 * - PM (Power Manager)
 *
 * Module Overview
 *
 * The System driver provides a collection of interfaces between the
 * user application logic, and the core device functionality (such as
 * clocks, reset cause determination, etc.) that is required for all
 * applications.
 *
 * Voltage References
 *
 * The various analog modules within the SAM D20/D21/R21 devices (such
 * as AC, ADC and DAC) require a voltage reference to be configured to
 * act as a reference point for comparisons and conversions.
 *
 * The SAM D20/D21/R21 devices contain multiple references, including
 * an internal temperature sensor, and a fixed band-gap voltage
 * source. When enabled, the associated voltage reference can be
 * selected within the desired peripheral where applicable.
 *
 * System Reset Cause
 *
 * In some application there may be a need to execute a different
 * program flow based on how the device was reset. For example, if the
 * cause of reset was the Watchdog timer (WDT), this might indicate an
 * error in the application and a form of error handling or error
 * logging might be needed.
 *
 * For this reason, an API is provided to retrieve the cause of the
 * last system reset, so that appropriate action can be taken.
 *
 * Sleep Modes
 *
 * The SAM D20/D21/R21 devices have several sleep modes, where the sleep mode controls
 * which clock systems on the device will remain enabled or disabled when the
 * device enters a low power sleep mode.

 * \ref asfdoc_sam0_system_module_sleep_mode_table "The table below" lists the
 * clock settings of the different sleep modes.
 *
 * \anchor asfdoc_sam0_system_module_sleep_mode_table
 * <table>
 *  <caption>SAM D20/D21/R21 Device Sleep Modes</caption>
 * 	<tr>
 * 		<th>Sleep mode</th>
 * 		<th>CPU clock</th>
 * 		<th>AHB clock</th>
 * 		<th>APB clocks</th>
 * 		<th>Clock sources</th>
 * 		<th>System clock</th>
 * 		<th>32KHz</th>
 * 		<th>Reg mode</th>
 * 		<th>RAM mode</th>
 * 	</tr>
 * 	<tr>
 * 		<td>IDLE 0</td>
 * 		<td>Stop</td>
 * 		<td>Run</td>
 * 		<td>Run</td>
 * 		<td>Run</td>
 * 		<td>Run</td>
 * 		<td>Run</td>
 * 		<td>Normal</td>
 * 		<td>Normal</td>
 * 	</tr>
 * 	<tr>
 * 		<td>IDLE 1</td>
 * 		<td>Stop</td>
 * 		<td>Stop</td>
 * 		<td>Run</td>
 * 		<td>Run</td>
 * 		<td>Run</td>
 * 		<td>Run</td>
 * 		<td>Normal</td>
 * 		<td>Normal</td>
 *	</tr>
 * 	<tr>
 * 		<td>IDLE 2</td>
 * 		<td>Stop</td>
 * 		<td>Stop</td>
 * 		<td>Stop</td>
 * 		<td>Run</td>
 * 		<td>Run</td>
 * 		<td>Run</td>
 * 		<td>Normal</td>
 * 		<td>Normal</td>
 *	</tr>
 * 	<tr>
 * 		<td>STANDBY</td>
 * 		<td>Stop</td>
 * 		<td>Stop</td>
 * 		<td>Stop</td>
 * 		<td>Stop</td>
 * 		<td>Stop</td>
 * 		<td>Stop</td>
 * 		<td>Low Power</td>
 * 		<td>Source/Drain biasing</td>
 * 	</tr>
 * </table>
 *
 * To enter device sleep, one of the available sleep modes must be set, and the
 * function to enter sleep called. The device will automatically wake up in
 * response to an interrupt being generated or other device event.
 *
 * Some peripheral clocks will remain enabled during sleep, depending on their
 * configuration; if desired, modules can remain clocked during sleep to allow
 * them to continue to operate while other parts of the system are powered down
 * to save power.
 *
 */

/**
 * List of available voltage references (VREF) that may be used within
 * the device.
 */
enum system_voltage_reference {
  /** Temperature sensor voltage reference. */
  SYSTEM_VOLTAGE_REFERENCE_TEMPSENSE,
  /** Bandgap voltage reference. */
  SYSTEM_VOLTAGE_REFERENCE_BANDGAP,
};

/**
 * List of available sleep modes in the device. A table of clocks
 * available in different sleep modes can be found in \ref
 * asfdoc_sam0_system_module_overview_sleep_mode.
 */
enum system_sleepmode {
  /** IDLE 0 sleep mode. */
  SYSTEM_SLEEPMODE_IDLE_0,
  /** IDLE 1 sleep mode. */
  SYSTEM_SLEEPMODE_IDLE_1,
  /** IDLE 2 sleep mode. */
  SYSTEM_SLEEPMODE_IDLE_2,
  /** Standby sleep mode. */
  SYSTEM_SLEEPMODE_STANDBY,
};

/**
 * List of possible reset causes of the system.
 */
enum system_reset_cause {
  /** The system was last reset by a software reset. */
  SYSTEM_RESET_CAUSE_SOFTWARE       = PM_RCAUSE_SYST,
  /** The system was last reset by the watchdog timer. */
  SYSTEM_RESET_CAUSE_WDT            = PM_RCAUSE_WDT,
  /** The system was last reset because the external reset line was pulled low. */
  SYSTEM_RESET_CAUSE_EXTERNAL_RESET = PM_RCAUSE_EXT,
  /** The system was last reset by the BOD33. */
  SYSTEM_RESET_CAUSE_BOD33          = PM_RCAUSE_BOD33,
  /** The system was last reset by the BOD12. */
  SYSTEM_RESET_CAUSE_BOD12          = PM_RCAUSE_BOD12,
  /** The system was last reset by the POR (Power on reset). */
  SYSTEM_RESET_CAUSE_POR            = PM_RCAUSE_POR,
};

/**
 * Retrieves the signature of the current device.
 *
 * \return Device ID signature as a 32-bit integer.
 */
static inline uint32_t system_get_device_id(void)
{
  return DSU->DID.reg;
}

/**
 * Enables the selected voltage reference source, making the voltage reference
 * available on a pin as well as an input source to the analog peripherals.
 *
 * \param[in] vref  Voltage reference to enable
 */
static inline void system_voltage_reference_enable(
  const enum system_voltage_reference vref)
{
  switch (vref) {
    case SYSTEM_VOLTAGE_REFERENCE_TEMPSENSE:
      SYSCTRL->VREF.reg |= SYSCTRL_VREF_TSEN;
      break;

    case SYSTEM_VOLTAGE_REFERENCE_BANDGAP:
      SYSCTRL->VREF.reg |= SYSCTRL_VREF_BGOUTEN;
      break;

    default:

      return;
  }
}

/**
 * Disables the selected voltage reference source.
 *
 * \param[in] vref  Voltage reference to disable
 */
static inline void system_voltage_reference_disable(
  const enum system_voltage_reference vref)
{
  switch (vref) {
    case SYSTEM_VOLTAGE_REFERENCE_TEMPSENSE:
      SYSCTRL->VREF.reg &= ~SYSCTRL_VREF_TSEN;
      break;

    case SYSTEM_VOLTAGE_REFERENCE_BANDGAP:
      SYSCTRL->VREF.reg &= ~SYSCTRL_VREF_BGOUTEN;
      break;

    default:

      return;
  }
}

/**
 * Set the sleep mode of the device
 *
 * Sets the sleep mode of the device; the configured sleep mode will be entered
 * upon the next call of the \ref system_sleep() function.
 *
 * For an overview of which systems are disabled in sleep for the different
 * sleep modes, see \ref asfdoc_sam0_system_module_overview_sleep_mode.
 *
 * \param[in] sleep_mode  Sleep mode to configure for the next sleep operation
 *
 * \retval STATUS_OK               Operation completed successfully
 * \retval STATUS_ERR_INVALID_ARG  The requested sleep mode was invalid or not
 *                                 available
 */
static inline void system_set_sleepmode(
  const enum system_sleepmode sleep_mode)
{
  switch (sleep_mode) {
    case SYSTEM_SLEEPMODE_IDLE_0:
    case SYSTEM_SLEEPMODE_IDLE_1:
    case SYSTEM_SLEEPMODE_IDLE_2:
      SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
      PM->SLEEP.reg = sleep_mode;
      break;

    case SYSTEM_SLEEPMODE_STANDBY:
      SCB->SCR |=  SCB_SCR_SLEEPDEEP_Msk;
      break;

    default:
      break;
//      return STATUS_ERR_INVALID_ARG;
  }
}

/**
 * Executes a device DSB (Data Synchronization Barrier) instruction to ensure
 * all ongoing memory accesses have completed, then a WFI (Wait For Interrupt)
 * instruction to place the device into the sleep mode specified by
 * \ref system_set_sleepmode until woken by an interrupt.
 */
static inline void system_sleep(void)
{
  __DSB();
  __WFI();
}

/**
 * Check if debugger is connected to the onboard debug system (DAP)
 *
 * \return A bool identifying if a debugger is present
 *
 * \retval true  Debugger is connected to the system
 * \retval false Debugger is not connected to the system
 *
 */
static inline bool system_is_debugger_present(void)
{
  return DSU->STATUSB.reg & DSU_STATUSB_DBGPRES;
}

/**
 * Resets the MCU and all associated peripherals and registers, except
 * RTC, all 32kHz sources, WDT (if ALWAYSON is set) and GCLK (if
 * WRTLOCK is set).
 *
 */
static inline void system_reset(void)
{
  NVIC_SystemReset();
}

/**
 * Retrieves the cause of the last system reset.
 *
 * \return An enum value indicating the cause of the last system reset.
 */
static inline enum system_reset_cause system_get_reset_cause(void)
{
  return (enum system_reset_cause)PM->RCAUSE.reg;
}

#endif /* SYSTEM_H_INCLUDED */
