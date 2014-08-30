/**
 * SAM D20/D21/R21 Clock Driver
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

#ifndef SYSTEM_CLOCK_H_INCLUDED
#define SYSTEM_CLOCK_H_INCLUDED

/**
 * SAM D20/D21/R21 System Clock Management Driver (SYSTEM CLOCK)
 *
 * This driver for SAM D20/D21/R21 devices provides an interface for
 * the configuration and management of the device's clocking related
 * functions. This includes the various clock sources, bus clocks and
 * generic clocks within the device, with functions to manage the
 * enabling, disabling, source selection and prescaling of clocks to
 * various internal peripherals.
 *
 * The following peripherals are used by this module:
 *
 * - GCLK (Generic Clock Management)
 * - PM (Power Management)
 * - SYSCTRL (Clock Source Control)
 *
 * Module Overview
 *
 * The SAM D20/D21/R21 devices contain a sophisticated clocking
 * system, which is designed to give the maximum flexibility to the
 * user application. This system allows a system designer to tune the
 * performance and power consumption of the device in a dynamic
 * manner, to achieve the best trade-off between the two for a
 * particular application.
 *
 * This driver provides a set of functions for the configuration and
 * management of the various clock related functionality within the
 * device.
 *
 * Clock Sources
 *
 * The SAM D20/D21/R21 devices have a number of master clock source
 * modules, each of which being capable of producing a stabilized
 * output frequency which can then be fed into the various peripherals
 * and modules within the device.
 *
 * Possible clock source modules include internal R/C oscillators,
 * internal DFLL modules, as well as external crystal oscillators
 * and/or clock inputs.
 *
 * CPU / Bus Clocks
 *
 * The CPU and AHB/APBx buses are clocked by the same physical clock
 * source (referred in this module as the Main Clock), however the
 * APBx buses may have additional prescaler division ratios set to
 * give each peripheral bus a different clock speed.
 *
 * Clock Masking
 *
 * To save power, the input clock to one or more peripherals on the AHB and APBx
 * buses can be masked away - when masked, no clock is passed into the module.
 * Disabling of clocks of unused modules will prevent all access to the masked
 * module, but will reduce the overall device power consumption.
 *
 * Generic Clocks
 *
 * Within the SAM D20/D21/R21 devices are a number of Generic Clocks;
 * these are used to provide clocks to the various peripheral clock
 * domains in the device in a standardized manner. One or more master
 * source clocks can be selected as the input clock to a Generic Clock
 * Generator, which can prescale down the input frequency to a slower
 * rate for use in a peripheral.
 *
 * Additionally, a number of individually selectable Generic Clock
 * Channels are provided, which multiplex and gate the various
 * generator outputs for one or more peripherals within the
 * device. This setup allows for a single common generator to feed one
 * or more channels, which can then be enabled or disabled
 * individually as required.
 *
 * Generic Clock Generators
 *
 * Each Generic Clock generator within the device can source its input clock
 * from one of the provided Source Clocks, and prescale the output for one or
 * more Generic Clock Channels in a one-to-many relationship. The generators
 * thus allow for several clocks to be generated of different frequencies,
 * power usages and accuracies, which can be turned on and off individually to
 * disable the clocks to multiple peripherals as a group.
 *
 * Generic Clock Channels
 *
 * To connect a Generic Clock Generator to a peripheral within the
 * device, a Generic Clock Channel is used. Each peripheral or
 * peripheral group has an associated Generic Clock Channel, which serves as the
 * clock input for the peripheral(s). To supply a clock to the peripheral
 * module(s), the associated channel must be connected to a running Generic
 * Clock Generator and the channel enabled.
 *
 */

#include <assert.h>
#include "samd20.h"
#include "system/gclk.h"

/**
 * Define system clock features set according to different device family.
 */
#if (SAMD21) || (SAMR21)
/** Digital Phase Locked Loop (DPLL) feature support */
#  define FEATURE_SYSTEM_CLOCK_DPLL
#endif

/**
 * Clock status type
 */
enum clock_status_t {
  CLOCK_STATUS_OK = 0,
  CLOCK_STATUS_INVALID_ARG,

};

/**
 * Available external 32KHz oscillator start-up times, as a number of external
 * clock cycles.
 */
enum system_xosc32k_startup {
  /** Wait 0 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC32K_STARTUP_0,
  /** Wait 32 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC32K_STARTUP_32,
  /** Wait 2048 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC32K_STARTUP_2048,
  /** Wait 4096 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC32K_STARTUP_4096,
  /** Wait 16384 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC32K_STARTUP_16384,
  /** Wait 32768 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC32K_STARTUP_32768,
  /** Wait 65536 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC32K_STARTUP_65536,
  /** Wait 131072 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC32K_STARTUP_131072,
};

/**
 * Available external oscillator start-up times, as a number of external clock
 * cycles.
 */
enum system_xosc_startup {
  /** Wait 1 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC_STARTUP_1,
  /** Wait 2 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC_STARTUP_2,
  /** Wait 4 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC_STARTUP_4,
  /** Wait 8 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC_STARTUP_8,
  /** Wait 16 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC_STARTUP_16,
  /** Wait 32 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC_STARTUP_32,
  /** Wait 64 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC_STARTUP_64,
  /** Wait 128 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC_STARTUP_128,
  /** Wait 256 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC_STARTUP_256,
  /** Wait 512 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC_STARTUP_512,
  /** Wait 1024 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC_STARTUP_1024,
  /** Wait 2048 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC_STARTUP_2048,
  /** Wait 4096 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC_STARTUP_4096,
  /** Wait 8192 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC_STARTUP_8192,
  /** Wait 16384 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC_STARTUP_16384,
  /** Wait 32768 clock cycles until the clock source is considered stable */
  SYSTEM_XOSC_STARTUP_32768,
};

/**
* Available internal 32KHz oscillator start-up times, as a number of internal
* OSC32K clock cycles.
*/
enum system_osc32k_startup {
  /** Wait 3 clock cycles until the clock source is considered stable */
  SYSTEM_OSC32K_STARTUP_3,
  /** Wait 4 clock cycles until the clock source is considered stable */
  SYSTEM_OSC32K_STARTUP_4,
  /** Wait 6 clock cycles until the clock source is considered stable */
  SYSTEM_OSC32K_STARTUP_6,
  /** Wait 10 clock cycles until the clock source is considered stable */
  SYSTEM_OSC32K_STARTUP_10,
  /** Wait 18 clock cycles until the clock source is considered stable */
  SYSTEM_OSC32K_STARTUP_18,
  /** Wait 34 clock cycles until the clock source is considered stable */
  SYSTEM_OSC32K_STARTUP_34,
  /** Wait 66 clock cycles until the clock source is considered stable */
  SYSTEM_OSC32K_STARTUP_66,
  /** Wait 130 clock cycles until the clock source is considered stable */
  SYSTEM_OSC32K_STARTUP_130,
};

/**
 * Available prescalers for the internal 8MHz (nominal) system clock.
 */
enum system_osc8m_div {
  /** Do not divide the 8MHz RC oscillator output */
  SYSTEM_OSC8M_DIV_1,
  /** Divide the 8MHz RC oscillator output by 2 */
  SYSTEM_OSC8M_DIV_2,
  /** Divide the 8MHz RC oscillator output by 4 */
  SYSTEM_OSC8M_DIV_4,
  /** Divide the 8MHz RC oscillator output by 8 */
  SYSTEM_OSC8M_DIV_8,
};

/**
 * Internal 8Mhz RC oscillator frequency range setting
 */
enum system_osc8m_frequency_range {
  /** Frequency range 4 Mhz to 6 Mhz */
  SYSTEM_OSC8M_FREQUENCY_RANGE_4_TO_6,
  /** Frequency range 6 Mhz to 8 Mhz */
  SYSTEM_OSC8M_FREQUENCY_RANGE_6_TO_8,
  /** Frequency range 8 Mhz to 11 Mhz */
  SYSTEM_OSC8M_FREQUENCY_RANGE_8_TO_11,
  /** Frequency range 11 Mhz to 15 Mhz */
  SYSTEM_OSC8M_FREQUENCY_RANGE_11_TO_15,
};

/**
 * Available division ratios for the CPU and APB/AHB bus clocks.
 */
enum system_main_clock_div {
  /** Divide Main clock by 1 */
  SYSTEM_MAIN_CLOCK_DIV_1,
  /** Divide Main clock by 2 */
  SYSTEM_MAIN_CLOCK_DIV_2,
  /** Divide Main clock by 4 */
  SYSTEM_MAIN_CLOCK_DIV_4,
  /** Divide Main clock by 8 */
  SYSTEM_MAIN_CLOCK_DIV_8,
  /** Divide Main clock by 16 */
  SYSTEM_MAIN_CLOCK_DIV_16,
  /** Divide Main clock by 32 */
  SYSTEM_MAIN_CLOCK_DIV_32,
  /** Divide Main clock by 64 */
  SYSTEM_MAIN_CLOCK_DIV_64,
  /** Divide Main clock by 128 */
  SYSTEM_MAIN_CLOCK_DIV_128,
};

/**
 * Available external clock source types.
 */
enum system_clock_external {
  /** The external clock source is a crystal oscillator */
  SYSTEM_CLOCK_EXTERNAL_CRYSTAL,
  /** The connected clock source is an external logic level clock signal */
  SYSTEM_CLOCK_EXTERNAL_CLOCK,
};

/**
 * Available operating modes of the DFLL clock source module,
 */
enum system_clock_dfll_loop_mode {
  /** The DFLL is operating in open loop mode with no feedback */
  SYSTEM_CLOCK_DFLL_LOOP_MODE_OPEN,
  /** The DFLL is operating in closed loop mode with frequency feedback from
   *  a low frequency reference clock
   */
  SYSTEM_CLOCK_DFLL_LOOP_MODE_CLOSED = SYSCTRL_DFLLCTRL_MODE,
  /** The DFLL is operating in USB recovery mode with frequency feedback
   *  from USB SOF
   */
};

/**
 * DFLL lock behavior modes on device wake-up from sleep.
 */
enum system_clock_dfll_wakeup_lock {
  /** Keep DFLL lock when the device wakes from sleep */
  SYSTEM_CLOCK_DFLL_WAKEUP_LOCK_KEEP,
  /** Lose DFLL lock when the devices wakes from sleep */
  SYSTEM_CLOCK_DFLL_WAKEUP_LOCK_LOSE = SYSCTRL_DFLLCTRL_LLAW,
};

/**
 * DFLL fine tracking behavior modes after a lock has been acquired.
 */
enum system_clock_dfll_stable_tracking {
  /** Keep tracking after the DFLL has gotten a fine lock */
  SYSTEM_CLOCK_DFLL_STABLE_TRACKING_TRACK_AFTER_LOCK,
  /** Stop tracking after the DFLL has gotten a fine lock */
  SYSTEM_CLOCK_DFLL_STABLE_TRACKING_FIX_AFTER_LOCK = SYSCTRL_DFLLCTRL_STABLE,
};

/**
 * DFLL chill-cycle behavior modes of the DFLL module. A chill cycle is a period
 * of time when the DFLL output frequency is not measured by the unit, to allow
 * the output to stabilize after a change in the input clock source.
 */
enum system_clock_dfll_chill_cycle {
  /** Enable a chill cycle, where the DFLL output frequency is not measured */
  SYSTEM_CLOCK_DFLL_CHILL_CYCLE_ENABLE,
  /** Disable a chill cycle, where the DFLL output frequency is not measured */
  SYSTEM_CLOCK_DFLL_CHILL_CYCLE_DISABLE = SYSCTRL_DFLLCTRL_CCDIS,
};

/**
 * DFLL QuickLock settings for the DFLL module, to allow for a faster lock of
 * the DFLL output frequency at the expense of accuracy.
 */
enum system_clock_dfll_quick_lock {
  /** Enable the QuickLock feature for looser lock requirements on the DFLL */
  SYSTEM_CLOCK_DFLL_QUICK_LOCK_ENABLE,
  /** Disable the QuickLock feature for strict lock requirements on the DFLL */
  SYSTEM_CLOCK_DFLL_QUICK_LOCK_DISABLE = SYSCTRL_DFLLCTRL_QLDIS,
};

/**
 * Clock sources available to the GCLK generators
 */
enum system_clock_source {
  /** Internal 8MHz RC oscillator */
  SYSTEM_CLOCK_SOURCE_OSC8M    = GCLK_SOURCE_OSC8M,
  /** Internal 32kHz RC oscillator */
  SYSTEM_CLOCK_SOURCE_OSC32K   = GCLK_SOURCE_OSC32K,
  /** External oscillator */
  SYSTEM_CLOCK_SOURCE_XOSC     = GCLK_SOURCE_XOSC ,
  /** External 32kHz oscillator */
  SYSTEM_CLOCK_SOURCE_XOSC32K  = GCLK_SOURCE_XOSC32K,
  /** Digital Frequency Locked Loop (DFLL) */
  SYSTEM_CLOCK_SOURCE_DFLL     = GCLK_SOURCE_DFLL48M,
  /** Internal Ultra Low Power 32kHz oscillator */
  SYSTEM_CLOCK_SOURCE_ULP32K   = GCLK_SOURCE_OSCULP32K,
  /** Generator input pad */
  SYSTEM_CLOCK_SOURCE_GCLKIN     = GCLK_SOURCE_GCLKIN,
  /** Generic clock generator 1 output */
  SYSTEM_CLOCK_SOURCE_GCLKGEN1   = GCLK_SOURCE_GCLKGEN1,
#ifdef FEATURE_SYSTEM_CLOCK_DPLL
  /** Digital Phase Locked Loop (DPLL). */
  SYSTEM_CLOCK_SOURCE_DPLL     = GCLK_SOURCE_FDPLL,
#endif
};

/**
 * Available bus clock domains on the APB bus.
 */
enum system_clock_apb_bus {
  /** Peripheral bus A on the APB bus. */
  SYSTEM_CLOCK_APB_APBA,
  /** Peripheral bus B on the APB bus. */
  SYSTEM_CLOCK_APB_APBB,
  /** Peripheral bus C on the APB bus. */
  SYSTEM_CLOCK_APB_APBC,
};

/**
 * Possible NVM flash wait state settings
 */
enum system_wait_states {
  /** Wait state maximum frequencies at 1.8V */
  SYSTEM_WAIT_STATE_1_8V_14MHZ	= 0,
  SYSTEM_WAIT_STATE_1_8V_28MHZ	= 1,
  SYSTEM_WAIT_STATE_1_8V_42MHZ	= 2,
  SYSTEM_WAIT_STATE_1_8V_48MHZ	= 3,
  /** Wait state maximum frequencies at 3.3V */
  SYSTEM_WAIT_STATE_3_3V_24MHZ	= 0,
  SYSTEM_WAIT_STATE_3_3V_48MHZ	= 1,
};

void system_clock_source_osc8m_set_config(enum system_osc8m_div prescaler,
					  bool run_in_standby,
					  bool on_demand);
void system_clock_source_osc8m_set_config_default(void);

void system_clock_source_osc32k_set_config(enum system_osc32k_startup startup_time,
					   bool enable_1khz_output,
					   bool enable_32khz_output,
					   bool run_in_standby,
					   bool on_demand,
					   bool write_once);
void system_clock_source_osc32k_set_config_default(void);

void system_clock_source_xosc_set_config(enum system_clock_external external_clock,
					 enum system_xosc_startup startup_time,
					 bool auto_gain_control,
					 uint32_t frequency,
					 bool run_in_standby,
					 bool on_demand);
void system_clock_source_xosc_set_config_default(void);

void system_clock_source_xosc32k_set_config(enum system_clock_external external_clock,
					    enum system_xosc32k_startup startup_time,
					    bool auto_gain_control,
					    bool enable_1khz_output,
					    bool enable_32khz_output,
					    bool run_in_standby,
					    bool on_demand,
					    bool write_once);
void system_clock_source_xosc32k_set_config_default(void);

void system_clock_source_dfll_set_config(
  enum system_clock_dfll_loop_mode loop_mode,
  bool on_demand,
  enum system_clock_dfll_quick_lock quick_lock,
  enum system_clock_dfll_chill_cycle chill_cycle,
  enum system_clock_dfll_wakeup_lock wakeup_lock,
  enum system_clock_dfll_stable_tracking stable_tracking,
  uint8_t coarse_value,
  uint16_t fine_value,
  uint8_t coarse_max_step,
  uint16_t fine_max_step,
  uint16_t multiply_factor);
void system_clock_source_dfll_set_config_default(void);

enum clock_status_t system_clock_source_write_calibration(
  const enum system_clock_source system_clock_source,
  const uint16_t calibration_value,
  const uint8_t freq_range);

enum clock_status_t system_clock_source_enable(
  const enum system_clock_source system_clock_source);

enum clock_status_t system_clock_source_disable(
  const enum system_clock_source clk_source);

bool system_clock_source_is_ready(
  const enum system_clock_source clk_source);

uint32_t system_clock_source_get_hz(
  const enum system_clock_source clk_source);

/**
 * Enable or disable the main clock failure detection.
 *
 * This mechanism allows switching automatically the main clock to the safe
 * RCSYS clock, when the main clock source is considered off.
 *
 * This may happen for instance when an external crystal is selected as the
 * clock source of the main clock and the crystal dies. The mechanism is to
 * detect, during a RCSYS period, at least one rising edge of the main clock.
 * If no rising edge is seen the clock is considered failed.
 * As soon as the detector is enabled, the clock failure detector
 * CFD) will monitor the divided main clock. When a clock failure is detected,
 * the main clock automatically switches to the RCSYS clock and the CFD
 * interrupt is generated if enabled.
 *
 * \note The failure detect must be disabled if the system clock is the same or
 *       slower than 32kHz as it will believe the system clock has failed with
 *       a too-slow clock.
 *
 * \param[in] enable  Boolean \c true to enable, \c false to disable detection
 */
static inline void system_main_clock_set_failure_detect(const bool enable)
{
  if (enable) {
//    PM->CTRL.reg |= PM_CTRL_CFDEN;
  } else {
//    PM->CTRL.reg &= ~PM_CTRL_CFDEN;
  }
}

/**
 * Sets the clock divider used on the main clock to provide the CPU clock.
 *
 * \param[in] divider  CPU clock divider to set
 */
static inline void system_cpu_clock_set_divider(
  const enum system_main_clock_div divider)
{
  assert(((uint32_t)divider & PM_CPUSEL_CPUDIV_Msk) == divider);
  PM->CPUSEL.reg = (uint32_t)divider;
}

/**
 * Retrieves the operating frequency of the CPU core, obtained from the main
 * generic clock and the set CPU bus divider.
 *
 * \return Current CPU frequency in Hz.
 */
static inline uint32_t system_cpu_clock_get_hz(void)
{
  return (system_gclk_gen_get_hz(GCLK_GENERATOR_0) >> PM->CPUSEL.reg);
}

/**
 * Set APBx clock divider.
 *
 * Set the clock divider used on the main clock to provide the clock for the
 * given APBx bus.
 *
 * \param[in] divider  APBx bus divider to set
 * \param[in] bus      APBx bus to set divider for
 *
 * \returns Status of the clock division change operation.
 *
 * \retval STATUS_ERR_INVALID_ARG  Invalid bus ID was given
 * \retval STATUS_OK               The APBx clock was set successfully
 */
static inline enum clock_status_t system_apb_clock_set_divider(
  const enum system_clock_apb_bus bus,
  const enum system_main_clock_div divider)
{
  switch (bus) {
    case SYSTEM_CLOCK_APB_APBA:
      PM->APBASEL.reg = (uint32_t)divider;
      break;
    case SYSTEM_CLOCK_APB_APBB:
      PM->APBBSEL.reg = (uint32_t)divider;
      break;
    case SYSTEM_CLOCK_APB_APBC:
      PM->APBCSEL.reg = (uint32_t)divider;
      break;
    default:
      assert(false);
      return CLOCK_STATUS_INVALID_ARG;
  }

  return CLOCK_STATUS_OK;
}

/**
 * Retrieves the operating frequency of an APBx bus, obtained from the main
 * generic clock and the set APBx bus divider.
 *
 * \return Current APBx bus frequency in Hz.
 */
static inline uint32_t system_apb_clock_get_hz(
  const enum system_clock_apb_bus bus)
{
  uint16_t bus_divider = 0;

  switch (bus) {
    case SYSTEM_CLOCK_APB_APBA:
      bus_divider = PM->APBASEL.reg;
      break;
    case SYSTEM_CLOCK_APB_APBB:
      bus_divider = PM->APBBSEL.reg;
      break;
    case SYSTEM_CLOCK_APB_APBC:
      bus_divider = PM->APBCSEL.reg;
      break;
    default:
      assert(false);
      return 0;
  }

  return (system_gclk_gen_get_hz(GCLK_GENERATOR_0) >> bus_divider);
}


/**
 * Set bits in the clock mask for the AHB bus.
 *
 * This function will set bits in the clock mask for the AHB bus.
 * Any bits set to 1 will enable that clock, 0 bits in the mask
 * will be ignored
 *
 * \param[in] ahb_mask  AHB clock mask to enable
 */
static inline void system_ahb_clock_set_mask(const uint32_t ahb_mask)
{
  PM->AHBMASK.reg |= ahb_mask;
}

/**
 * Clear bits in the clock mask for the AHB bus.
 *
 * This function will clear bits in the clock mask for the AHB bus.
 * Any bits set to 1 will disable that clock, 0 bits in the mask
 * will be ignored.
 *
 * \param[in] ahb_mask  AHB clock mask to disable
 */
static inline void system_ahb_clock_clear_mask(const uint32_t ahb_mask)
{
  PM->AHBMASK.reg &= ~ahb_mask;
}

/**
 * Set bits in the clock mask for an APBx bus.
 *
 * This function will set bits in the clock mask for an APBx bus.
 * Any bits set to 1 will enable the corresponding module clock, zero bits in
 * the mask will be ignored.
 *
 * \param[in] mask  APBx clock mask, a \c SYSTEM_CLOCK_APB_APBx constant from
 *                  the device header files
 * \param[in] bus   Bus to set clock mask bits for, a mask of \c PM_APBxMASK_*
 *                  constants from the device header files
 *
 * \returns Status indicating the result of the clock mask change operation.
 *
 * \retval STATUS_ERR_INVALID_ARG  Invalid bus given
 * \retval STATUS_OK               The clock mask was set successfully
 */
static inline enum clock_status_t system_apb_clock_set_mask(
  const enum system_clock_apb_bus bus,
  const uint32_t mask)
{
  switch (bus) {
    case SYSTEM_CLOCK_APB_APBA:
      PM->APBAMASK.reg |= mask;
      break;

    case SYSTEM_CLOCK_APB_APBB:
      PM->APBBMASK.reg |= mask;
      break;

    case SYSTEM_CLOCK_APB_APBC:
      PM->APBCMASK.reg |= mask;
      break;

    default:
      assert(false);
      return CLOCK_STATUS_INVALID_ARG;

  }

  return CLOCK_STATUS_OK;
}

/**
 * Clear bits in the clock mask for an APBx bus.
 *
 * This function will clear bits in the clock mask for an APBx bus.
 * Any bits set to 1 will disable the corresponding module clock, zero bits in
 * the mask will be ignored.
 *
 * \param[in] mask  APBx clock mask, a \c SYSTEM_CLOCK_APB_APBx constant from
 *                  the device header files
 * \param[in] bus   Bus to clear clock mask bits for
 *
 * \returns Status indicating the result of the clock mask change operation.
 *
 * \retval STATUS_ERR_INVALID_ARG  Invalid bus ID was given.
 * \retval STATUS_OK               The clock mask was changed successfully.
 */
static inline enum clock_status_t system_apb_clock_clear_mask(
  const enum system_clock_apb_bus bus,
  const uint32_t mask)
{
  switch (bus) {
    case SYSTEM_CLOCK_APB_APBA:
      PM->APBAMASK.reg &= ~mask;
      break;

    case SYSTEM_CLOCK_APB_APBB:
      PM->APBBMASK.reg &= ~mask;
      break;

    case SYSTEM_CLOCK_APB_APBC:
      PM->APBCMASK.reg &= ~mask;
      break;

    default:
      assert(false);
      return CLOCK_STATUS_INVALID_ARG;
  }

  return CLOCK_STATUS_OK;
}

#ifdef FEATURE_SYSTEM_CLOCK_DPLL
/**
 * Reference clock source of the DPLL module
 */
enum system_clock_source_dpll_reference_clock {
  /** Select CLK_DPLL_REF0 as clock reference */
  SYSTEM_CLOCK_SOURCE_DPLL_REFERENCE_CLOCK_REF0,
  /** Select CLK_DPLL_REF1 as clock reference */
  SYSTEM_CLOCK_SOURCE_DPLL_REFERENCE_CLOCK_REF1,
  /** Select GCLK_DPLL as clock reference */
  SYSTEM_CLOCK_SOURCE_DPLL_REFERENCE_CLOCK_GCLK,
};

/**
 * Lock time-out value of the DPLL module
 */
enum system_clock_source_dpll_lock_time {
  /** Set no time-out as default */
  SYSTEM_CLOCK_SOURCE_DPLL_LOCK_TIME_DEFAULT,
  /** Set time-out if no lock within 8 ms */
  SYSTEM_CLOCK_SOURCE_DPLL_LOCK_TIME_8MS = 0x04,
  /** Set time-out if no lock within 9 ms */
  SYSTEM_CLOCK_SOURCE_DPLL_LOCK_TIME_9MS,
  /** Set time-out if no lock within 10 ms */
  SYSTEM_CLOCK_SOURCE_DPLL_LOCK_TIME_10MS,
  /** Set time-out if no lock within 11 ms */
  SYSTEM_CLOCK_SOURCE_DPLL_LOCK_TIME_11MS,
};

/**
 * Filter type of the DPLL module
 */
enum system_clock_source_dpll_filter {
  /** Default filter mode */
  SYSTEM_CLOCK_SOURCE_DPLL_FILTER_DEFAULT,
  /** Low bandwidth filter */
  SYSTEM_CLOCK_SOURCE_DPLL_FILTER_LOW_BANDWIDTH_FILTER,
  /** High bandwidth filter */
  SYSTEM_CLOCK_SOURCE_DPLL_FILTER_HIGH_BANDWIDTH_FILTER,
  /** High damping filter */
  SYSTEM_CLOCK_SOURCE_DPLL_FILTER_HIGH_DAMPING_FILTER,
};

/**
 * Configuration structure for DPLL
 *
 * DPLL oscillator configuration structure.
 */
struct system_clock_source_dpll_config {
  /** Run On Demand. If this is set the DPLL won't run
   * until requested by a peripheral */
  bool on_demand;
  /** Keep the DPLL enabled in standby sleep mode */
  bool run_in_standby;
  /** Bypass lock signal */
  bool lock_bypass;
  /** Wake up fast. If this is set DPLL output clock is enabled after
   * the startup time */
  bool wake_up_fast;
  /** Enable low power mode  */
  bool low_power_enable;

  /** Output frequency of the clock */
  uint32_t output_frequency;
  /** Reference frequency of the clock */
  uint32_t reference_frequency;
  /** Devider of reference clock */
  uint16_t reference_divider;

  /** Filter type of the DPLL module */
  enum system_clock_source_dpll_filter          filter;
  /** Lock time-out value of the DPLL module */
  enum system_clock_source_dpll_lock_time       lock_time;
  /** Reference clock source of the DPLL module */
  enum system_clock_source_dpll_reference_clock reference_clock;
};

/**
 * \brief Retrieve the default configuration for DPLL
 *
 * Fills a configuration structure with the default configuration for a
 * DPLL oscillator module:
 *   - Run only when requested by peripheral (on demand)
 *   - Don't run in STANDBY sleep mode
 *   - Lock bypass disabled
 *   - Fast wake up disabled
 *   - Low power mode disabled
 *   - Output frequency is 48MHz
 *   - Reference clock frequency is 32768Hz
 *   - Not divide reference clock
 *   - Select REF0 as reference clock
 *   - Set lock time to default mode
 *   - Use default filter
 *
 * \param[out] config  Configuration structure to fill with default values
 */
static inline void system_clock_source_dpll_get_config_defaults(
  struct system_clock_source_dpll_config *const config)
{
  config->on_demand           = true;
  config->run_in_standby      = false;
  config->lock_bypass         = false;
  config->wake_up_fast        = false;
  config->low_power_enable    = false;

  config->output_frequency    = 48000000;
  config->reference_frequency = 32768;
  config->reference_divider   = 1;
  config->reference_clock     = SYSTEM_CLOCK_SOURCE_DPLL_REFERENCE_CLOCK_REF0;

  config->lock_time           = SYSTEM_CLOCK_SOURCE_DPLL_LOCK_TIME_DEFAULT;
  config->filter              = SYSTEM_CLOCK_SOURCE_DPLL_FILTER_DEFAULT;
};

void system_clock_source_dpll_set_config(
  struct system_clock_source_dpll_config *const config);

#endif

void system_clock_init(void);

/**
 * Set flash controller wait states
 *
 * Will set the number of wait states that are used by the onboard
 * flash memory. The number of wait states depend on both device
 * supply voltage and CPU speed. The required number of wait states
 * can be found in the electrical characteristics of the device.
 *
 * \param[in] wait_states Number of wait states to use for internal flash
 */
static inline void system_flash_set_waitstates(const enum system_wait_states wait_states)
{
  assert(NVMCTRL_CTRLB_RWS((uint32_t)wait_states) ==
	 ((uint32_t)wait_states << NVMCTRL_CTRLB_RWS_Pos));

  NVMCTRL->CTRLB.bit.RWS = wait_states;
}

/**
 *	- This driver implements workaround for errata 10558
 *
 *	  "Several reset values of SYSCTRL.INTFLAG are wrong (BOD and DFLL)"
 *	  When system_init is called it will reset these interrupts flags before they are used.
 *
 *	- This driver implements experimental workaround for errata 9905
 *
 *	  "The DFLL clock must be requested before being configured otherwise a
 *	  write access to a DFLL register can freeze the device."
 *	  This driver will enable and configure the DFLL before the ONDEMAND bit is set.
 *
 */

#endif /* SYSTEM_CLOCK_H_INCLUDED */
