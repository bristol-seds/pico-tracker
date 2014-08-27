/**
 * SAM D20 Clock Driver
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
#include "system/clock.h"
#include "system/conf_clocks.h"
#include "system/system.h"
#include "samd20.h"

/* Syncronisation Macros */
#define DFLL_WAIT_FOR_SYNC()		\
  while (!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY))
#define OSC32K_WAIT_FOR_SYNC()		\
  while (!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_OSC32KRDY))

/**
 * Determines the current operating frequency of a given clock source.
 *
 * \param[in] clock_source  Clock source to get the frequency of
 *
 * \returns Frequency of the given clock source, in Hz
 */
uint32_t system_clock_source_get_hz(const enum system_clock_source clock_source)
{
  switch (clock_source) {
    case SYSTEM_CLOCK_SOURCE_XOSC:
      return 0;//TODO _system_clock_inst.xosc.frequency;

    case SYSTEM_CLOCK_SOURCE_OSC8M:
      return 8000000UL >> SYSCTRL->OSC8M.bit.PRESC;

    case SYSTEM_CLOCK_SOURCE_OSC32K:
      return 32768UL;

    case SYSTEM_CLOCK_SOURCE_ULP32K:
      return 32768UL;

    case SYSTEM_CLOCK_SOURCE_XOSC32K:
      return 0;//TODO _system_clock_inst.xosc32k.frequency;

    case SYSTEM_CLOCK_SOURCE_DFLL:

      /* Check if the DFLL has been configured */
      if (!(SYSCTRL->DFLLCTRL.reg & SYSCTRL_DFLLCTRL_ENABLE)) {
	return 0; /* Not configured */
      }

      /* Make sure that the DFLL module is ready */
      DFLL_WAIT_FOR_SYNC();

      /* Check if operating in closed loop mode */
      if (SYSCTRL->DFLLCTRL.reg & SYSCTRL_DFLLCTRL_MODE) {
	return system_gclk_chan_get_hz(SYSCTRL_GCLK_ID_DFLL48) *
	  (SYSCTRL->DFLLMUL.reg & 0xffff);
      }

      return 48000000UL;

    default:
      return 0;
  }
}

/**
 * Configures the 8MHz (nominal) internal RC oscillator with the given
 * configuration settings.
 *
 * \param[in] config  OSC8M configuration structure containing the new config
 */
void system_clock_source_osc8m_set_config(enum system_osc8m_div prescaler,
					  bool run_in_standby,
					  bool on_demand)
{
  SYSCTRL_OSC8M_Type temp = SYSCTRL->OSC8M;

  /* Use temporary struct to reduce register access */
  temp.bit.PRESC    = prescaler;
  temp.bit.ONDEMAND = on_demand;
  temp.bit.RUNSTDBY = run_in_standby;

  SYSCTRL->OSC8M = temp;
}
/**
 * Configures the 8MHz (nominal) internal RC oscillator with the
 * default configuation settings.
 *
 *   - Clock output frequency divided by a factor of 8
 *   - Don't run in STANDBY sleep mode
 *   - Run only when requested by peripheral (on demand)
 */
void system_clock_source_osc8m_set_config_default(void)
{
  system_clock_source_osc8m_set_config(SYSTEM_OSC8M_DIV_8, /* Prescaler */
				       false,		   /* Run in Standby */
				       false);		   /* Run on Demand */
}

/**
 * Configures the 32KHz (nominal) internal RC oscillator with the given
 * configuration settings.
 *
 * \param[in] config  OSC32K configuration structure containing the new config
 */
void system_clock_source_osc32k_set_config(enum system_osc32k_startup startup_time,
					   bool enable_1khz_output,
					   bool enable_32khz_output,
					   bool run_in_standby,
					   bool on_demand,
					   bool write_once)
{
  SYSCTRL_OSC32K_Type temp = SYSCTRL->OSC32K;

  /* Update settings via a temporary struct to reduce register access */
  temp.bit.EN1K     = enable_1khz_output;
  temp.bit.EN32K    = enable_32khz_output;
  temp.bit.STARTUP  = startup_time;
  temp.bit.ONDEMAND = on_demand;
  temp.bit.RUNSTDBY = run_in_standby;
  temp.bit.WRTLOCK  = write_once;

  SYSCTRL->OSC32K  = temp;
}
/**
 * Configures the 32KHz (nominal) internal RC oscillator with the
 * default configuration settings.
 *
 *   - 1KHz clock output enabled
 *   - 32KHz clock output enabled
 *   - Don't run in STANDBY sleep mode
 *   - Run only when requested by peripheral (on demand)
 *   - Set startup time to 130 cycles
 *   - Don't lock registers after configuration has been written
 */
void system_clock_source_osc32k_set_config_default(void)
{
  system_clock_source_osc32k_set_config(SYSTEM_OSC32K_STARTUP_130, /* Startup Cyles */
					true,		/* 1KHz output enable */
					true,		/* 32KHz output enable */
					false,		/* Run in Standby */
					true,		/* Run on demand */
					false);		/* Lock registers */
}


/**
 * Configures the external oscillator clock source with the given configuration
 * settings.
 *
 * \param[in] config  External oscillator configuration structure containing
 *                    the new config
 */
void system_clock_source_xosc_set_config(enum system_clock_external external_clock,
					 enum system_xosc_startup startup_time,
					 bool auto_gain_control,
					 uint32_t frequency,
					 bool run_in_standby,
					 bool on_demand)
{
  SYSCTRL_XOSC_Type temp = SYSCTRL->XOSC;

  /* Update settings via a temporary struct to reduce register access */
  temp.bit.STARTUP = startup_time;

  if (external_clock == SYSTEM_CLOCK_EXTERNAL_CRYSTAL) {
    temp.bit.XTALEN = 1;
  } else {
    temp.bit.XTALEN = 0;
  }

  temp.bit.AMPGC = auto_gain_control;

  /* Set gain if automatic gain control is not selected */
  if (!auto_gain_control) {
    if (frequency <= 2000000) {
      temp.bit.GAIN = 0;
    } else if (frequency <= 4000000) {
      temp.bit.GAIN = 1;
    } else if (frequency <= 8000000) {
      temp.bit.GAIN = 2;
    } else if (frequency <= 16000000) {
      temp.bit.GAIN = 3;
    } else if (frequency <= 30000000) {
      temp.bit.GAIN = 4;
    }

  }

  temp.bit.ONDEMAND = on_demand;
  temp.bit.RUNSTDBY = run_in_standby;

  SYSCTRL->XOSC = temp;
}
/**
 * Configures the external oscillator clock source with the default
 * configuration settings.
 *
 *   - External Crystal
 *   - Start-up time of 16384 external clock cycles
 *   - Automatic crystal gain control mode enabled
 *   - Frequency of 12MHz
 *   - Don't run in STANDBY sleep mode
 *   - Run only when requested by peripheral (on demand)
 */
void system_clock_source_xosc_set_config_default(void)
{
  /* void system_clock_source_xosc_set_config(SYSTEM_CLOCK_EXTERNAL_CRYSTAL, */
  /* 					   SYSTEM_XOSC_STARTUP_16384, */
  /* 					   true,	/\* Auto gain control *\/ */
  /* 					   12000000UL,	/\* Frequency *\/ */
  /* 					   false,	/\* Run in Standby *\/ */
  /* 					   true);	/\* Run on demand *\/ */
}


/**
 * Configures the external 32KHz oscillator clock source with the given
 * configuration settings.
 *
 * \param[in] config  XOSC32K configuration structure containing the new config
 */
void system_clock_source_xosc32k_set_config(enum system_clock_external external_clock,
					    enum system_xosc32k_startup startup_time,
					    bool auto_gain_control,
					    bool enable_1khz_output,
					    bool enable_32khz_output,
					    bool run_in_standby,
					    bool on_demand,
					    bool write_once)
{
  SYSCTRL_XOSC32K_Type temp = SYSCTRL->XOSC32K;

  /* Update settings via a temporary struct to reduce register access */
  temp.bit.STARTUP = startup_time;

  if (external_clock == SYSTEM_CLOCK_EXTERNAL_CRYSTAL) {
    temp.bit.XTALEN = 1;
  } else {
    temp.bit.XTALEN = 0;
  }

  temp.bit.AAMPEN = auto_gain_control;
  temp.bit.EN1K = enable_1khz_output;
  temp.bit.EN32K = enable_32khz_output;

  temp.bit.ONDEMAND = on_demand;
  temp.bit.RUNSTDBY = run_in_standby;
  temp.bit.WRTLOCK  = write_once;

  SYSCTRL->XOSC32K = temp;
}
/**
 * Configures the external 32KHz oscillator clock source with the
 * default configuration settings.
 *
 *   - External Crystal
 *   - Start-up time of 16384 external clock cycles
 *   - Automatic crystal gain control mode disabled
 *   - 1KHz clock output disabled
 *   - 32KHz clock output enabled
 *   - Don't run in STANDBY sleep mode
 *   - Run only when requested by peripheral (on demand)
 *   - Don't lock registers after configuration has been written
 */
void system_clock_source_xosc32k_set_config_default(void)
{
  system_clock_source_xosc32k_set_config(SYSTEM_CLOCK_EXTERNAL_CRYSTAL,
					 SYSTEM_XOSC32K_STARTUP_16384,
					 false, 	/* Auto gain control */
					 false,		/* Enable 1KHz output */
					 false,		/* Enable 32KHz output */
					 false,		/* Run in standby */
					 true,		/* Run on demand */
					 false);	/* Write once */
}

/**
 * Header file macro copies for runtime support of different revisions
 *
 * These macros are copied from the header file to be able to support
 * both new and old register layout runtime.
 */
#define _SYSTEM_OLD_DFLLVAL_FINE_Pos    0
#define _SYSTEM_OLD_DFLLVAL_FINE_Msk    (0xFFu << _SYSTEM_OLD_DFLLVAL_FINE_Pos)
#define _SYSTEM_OLD_DFLLVAL_FINE(value) ((_SYSTEM_OLD_DFLLVAL_FINE_Msk & ((value) << _SYSTEM_OLD_DFLLVAL_FINE_Pos)))

#define _SYSTEM_OLD_DFLLVAL_COARSE_Pos  8
#define _SYSTEM_OLD_DFLLVAL_COARSE_Msk  (0x1Fu << _SYSTEM_OLD_DFLLVAL_COARSE_Pos)
#define _SYSTEM_OLD_DFLLVAL_COARSE(value) ((_SYSTEM_OLD_DFLLVAL_COARSE_Msk & ((value) << _SYSTEM_OLD_DFLLVAL_COARSE_Pos)))

#define _SYSTEM_NEW_DFLLVAL_FINE_Pos    0
#define _SYSTEM_NEW_DFLLVAL_FINE_Msk    (0x3FFu << _SYSTEM_NEW_DFLLVAL_FINE_Pos)
#define _SYSTEM_NEW_DFLLVAL_FINE(value) ((_SYSTEM_NEW_DFLLVAL_FINE_Msk & ((value) << _SYSTEM_NEW_DFLLVAL_FINE_Pos)))

#define _SYSTEM_NEW_DFLLVAL_COARSE_Pos  10
#define _SYSTEM_NEW_DFLLVAL_COARSE_Msk  (0x3Fu << _SYSTEM_NEW_DFLLVAL_COARSE_Pos)
#define _SYSTEM_NEW_DFLLVAL_COARSE(value) ((_SYSTEM_NEW_DFLLVAL_COARSE_Msk & ((value) << _SYSTEM_NEW_DFLLVAL_COARSE_Pos)))

#define _SYSTEM_OLD_DFLLMUL_FSTEP_Pos   16
#define _SYSTEM_OLD_DFLLMUL_FSTEP_Msk   (0xFFu << _SYSTEM_OLD_DFLLMUL_FSTEP_Pos)
#define _SYSTEM_OLD_DFLLMUL_FSTEP(value) ((_SYSTEM_OLD_DFLLMUL_FSTEP_Msk & ((value) << _SYSTEM_OLD_DFLLMUL_FSTEP_Pos)))

#define _SYSTEM_OLD_DFLLMUL_CSTEP_Pos   24
#define _SYSTEM_OLD_DFLLMUL_CSTEP_Msk   (0x1Fu << _SYSTEM_OLD_DFLLMUL_CSTEP_Pos)
#define _SYSTEM_OLD_DFLLMUL_CSTEP(value) ((_SYSTEM_OLD_DFLLMUL_CSTEP_Msk & ((value) << _SYSTEM_OLD_DFLLMUL_CSTEP_Pos)))

#define _SYSTEM_NEW_DFLLMUL_FSTEP_Pos   16
#define _SYSTEM_NEW_DFLLMUL_FSTEP_Msk   (0x3FFu << _SYSTEM_NEW_DFLLMUL_FSTEP_Pos)
#define _SYSTEM_NEW_DFLLMUL_FSTEP(value) ((_SYSTEM_NEW_DFLLMUL_FSTEP_Msk & ((value) << _SYSTEM_NEW_DFLLMUL_FSTEP_Pos)))

#define _SYSTEM_NEW_DFLLMUL_CSTEP_Pos   26
#define _SYSTEM_NEW_DFLLMUL_CSTEP_Msk   (0x3Fu << _SYSTEM_NEW_DFLLMUL_CSTEP_Pos)
#define _SYSTEM_NEW_DFLLMUL_CSTEP(value) ((_SYSTEM_NEW_DFLLMUL_CSTEP_Msk & ((value) << _SYSTEM_NEW_DFLLMUL_CSTEP_Pos)))

#define _SYSTEM_MCU_REVISION_D 3


/**
 * Configures the Digital Frequency Locked Loop clock source with the given
 * configuration settings.
 *
 * \note The DFLL will be running when this function returns, as the DFLL module
 *       needs to be enabled in order to perform the module configuration.
 *
 * \param[in] config  DFLL configuration structure containing the new config
 */
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
  uint16_t multiply_factor)
{
  uint32_t control, val, mul;

  /* Get MCU revision */
  uint32_t rev = system_get_device_id();
  rev &= DSU_DID_REVISION_Msk;
  rev = rev >> DSU_DID_REVISION_Pos;

  /* Calculate the Value register */
  if (rev < _SYSTEM_MCU_REVISION_D) {
    val =
      _SYSTEM_OLD_DFLLVAL_COARSE(coarse_value) |
      _SYSTEM_OLD_DFLLVAL_FINE(fine_value);
  } else {
    val =
      _SYSTEM_NEW_DFLLVAL_COARSE(coarse_value) |
      _SYSTEM_NEW_DFLLVAL_FINE(fine_value);
  }

  /* Calculate the Control register */
  control =
    (uint32_t)wakeup_lock     |
    (uint32_t)stable_tracking |
    (uint32_t)quick_lock      |
    (uint32_t)chill_cycle     |
    ((uint32_t)on_demand << SYSCTRL_DFLLCTRL_ONDEMAND_Pos);

  /* Set the Multiplication register for closed loop mode */
  if (loop_mode == SYSTEM_CLOCK_DFLL_LOOP_MODE_CLOSED) {
    if(rev < _SYSTEM_MCU_REVISION_D) {
      mul =
  	_SYSTEM_OLD_DFLLMUL_CSTEP(coarse_max_step) |
  	_SYSTEM_OLD_DFLLMUL_FSTEP(fine_max_step)   |
  	SYSCTRL_DFLLMUL_MUL(multiply_factor);
    } else {
      mul =
  	_SYSTEM_NEW_DFLLMUL_CSTEP(coarse_max_step) |
  	_SYSTEM_NEW_DFLLMUL_FSTEP(fine_max_step)   |
  	SYSCTRL_DFLLMUL_MUL(multiply_factor);
    }

    /* Enable the closed loop mode */
    control |= loop_mode;
  }

  /* Workaround for errata 9905 */
  SYSCTRL->DFLLCTRL.reg = control & ~SYSCTRL_DFLLCTRL_ONDEMAND;
  DFLL_WAIT_FOR_SYNC();

  SYSCTRL->DFLLMUL.reg = mul;
  SYSCTRL->DFLLVAL.reg = val;

  /* Write full configuration to DFLL control register */
  SYSCTRL->DFLLCTRL.reg = control;
}

/**
 * Configures the Digital Frequency Locked Loop clock source with the
 * default configuration settings.
 *
 *   - Open loop mode
 *   - QuickLock mode enabled
 *   - Chill cycle enabled
 *   - Output frequency lock maintained during device wake-up
 *   - Continuous tracking of the output frequency
 *   - Default tracking values at the mid-points for both coarse and fine
 *     tracking parameters
 *   - Don't run in STANDBY sleep mode
 *   - Run only when requested by peripheral (on demand)
 */
void system_clock_source_dfll_set_config_default(void)
{
  system_clock_source_dfll_set_config(
    SYSTEM_CLOCK_DFLL_LOOP_MODE_OPEN,		/* Loop Mode */
    true,					/* On demand */
    SYSTEM_CLOCK_DFLL_QUICK_LOCK_ENABLE,	/* Quick Lock */
    SYSTEM_CLOCK_DFLL_CHILL_CYCLE_ENABLE,	/* Chill Cycle */
    SYSTEM_CLOCK_DFLL_WAKEUP_LOCK_KEEP,		/* Lock during wakeup */
    SYSTEM_CLOCK_DFLL_STABLE_TRACKING_TRACK_AFTER_LOCK,
    0x1f / 4,		/* Open Loop   - Coarse calibration value */
    0xff / 4,		/* Open Loop   - Fine calibration value */
    1,			/* Closed Loop - Coarse Maximum step */
    1,			/* Closed Loop - Fine Maximum step */
    6);			/* Frequency Multiplication Factor */
}

/**
 * Writes an oscillator calibration value to the given oscillator control
 * registers. The acceptable ranges are:
 *
 * For OSC32K:
 *  - 7 bits (max value 128)
 * For OSC8MHZ:
 *  - 8 bits (Max value 255)
 * For OSCULP:
 *  - 5 bits (Max value 32)
 *
 * \note The frequency range parameter applies only when configuring the 8MHz
 *       oscillator and will be ignored for the other oscillators.
 *
 * \param[in] clock_source       Clock source to calibrate
 * \param[in] calibration_value  Calibration value to write
 * \param[in] freq_range         Frequency range (8MHz oscillator only)
 *
 * \retval STATUS_OK               The calibration value was written
 *                                 successfully.
 * \retval STATUS_ERR_INVALID_ARG  The setting is not valid for selected clock
 *                                 source.
 */
enum clock_status_t system_clock_source_write_calibration(
  const enum system_clock_source clock_source,
  const uint16_t calibration_value,
  const uint8_t freq_range)
{
  switch (clock_source) {
    case SYSTEM_CLOCK_SOURCE_OSC8M:

      if (calibration_value > 0xfff || freq_range > 4) {
	return CLOCK_STATUS_INVALID_ARG;
      }

      SYSCTRL->OSC8M.bit.CALIB  = calibration_value;
      SYSCTRL->OSC8M.bit.FRANGE = freq_range;
      break;

    case SYSTEM_CLOCK_SOURCE_OSC32K:

      if (calibration_value > 128) {
	return CLOCK_STATUS_INVALID_ARG;
      }

      OSC32K_WAIT_FOR_SYNC();
      SYSCTRL->OSC32K.bit.CALIB = calibration_value;
      break;

    case SYSTEM_CLOCK_SOURCE_ULP32K:

      if (calibration_value > 32) {
	return CLOCK_STATUS_INVALID_ARG;
      }

      SYSCTRL->OSCULP32K.bit.CALIB = calibration_value;
      break;

    default:
      assert(false);
      return CLOCK_STATUS_INVALID_ARG;
      break;
  }

  return CLOCK_STATUS_OK;
}

/**
 * Enables a clock source which has been previously configured.
 *
 * \param[in] clock_source       Clock source to enable
 *
 * \retval STATUS_OK               Clock source was enabled successfully and
 *                                 is ready
 * \retval STATUS_ERR_INVALID_ARG  The clock source is not available on this
 *                                 device
 */
enum clock_status_t system_clock_source_enable(
  const enum system_clock_source clock_source)
{
  switch (clock_source) {
    case SYSTEM_CLOCK_SOURCE_OSC8M:
      SYSCTRL->OSC8M.reg |= SYSCTRL_OSC8M_ENABLE;
      return CLOCK_STATUS_OK;

    case SYSTEM_CLOCK_SOURCE_OSC32K:
      SYSCTRL->OSC32K.reg |= SYSCTRL_OSC32K_ENABLE;
      break;

    case SYSTEM_CLOCK_SOURCE_XOSC:
      SYSCTRL->XOSC.reg |= SYSCTRL_XOSC_ENABLE;
      break;

    case SYSTEM_CLOCK_SOURCE_XOSC32K:
      SYSCTRL->XOSC32K.reg |= SYSCTRL_XOSC32K_ENABLE;
      break;

    case SYSTEM_CLOCK_SOURCE_DFLL:
      SYSCTRL->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_ENABLE;
      break;

    case SYSTEM_CLOCK_SOURCE_ULP32K:
      /* Always enabled */
      return CLOCK_STATUS_OK;

    default:
      assert(false);
      return CLOCK_STATUS_INVALID_ARG;
  }

  return CLOCK_STATUS_OK;
}

/**
 * Disables a clock source that was previously enabled.
 *
 * \param[in] clock_source  Clock source to disable
 *
 * \retval STATUS_OK               Clock source was disabled successfully
 * \retval STATUS_ERR_INVALID_ARG  An invalid or unavailable clock source was
 *                                 given
 */
enum clock_status_t system_clock_source_disable(
  const enum system_clock_source clock_source)
{
  switch (clock_source) {
    case SYSTEM_CLOCK_SOURCE_OSC8M:
      SYSCTRL->OSC8M.reg &= ~SYSCTRL_OSC8M_ENABLE;
      break;

    case SYSTEM_CLOCK_SOURCE_OSC32K:
      SYSCTRL->OSC32K.reg &= ~SYSCTRL_OSC32K_ENABLE;
      break;

    case SYSTEM_CLOCK_SOURCE_XOSC:
      SYSCTRL->XOSC.reg &= ~SYSCTRL_XOSC_ENABLE;
      break;

    case SYSTEM_CLOCK_SOURCE_XOSC32K:
      SYSCTRL->XOSC32K.reg &= ~SYSCTRL_XOSC32K_ENABLE;
      break;

    case SYSTEM_CLOCK_SOURCE_DFLL:
      SYSCTRL->DFLLCTRL.reg &= +SYSCTRL_DFLLCTRL_ENABLE;
      break;

    case SYSTEM_CLOCK_SOURCE_ULP32K:
      /* Not possible to disable */

    default:
      assert(false);
      return CLOCK_STATUS_INVALID_ARG;

  }

  return CLOCK_STATUS_OK;
}

/**
 * Checks if a given clock source is ready to be used.
 *
 * \param[in] clock_source  Clock source to check if ready
 *
 * \returns Ready state of the given clock source.
 *
 * \retval true   Clock source is enabled and ready
 * \retval false  Clock source is disabled or not yet ready
 */
bool system_clock_source_is_ready(
  const enum system_clock_source clock_source)
{
  uint32_t mask = 0;

  switch (clock_source) {
    case SYSTEM_CLOCK_SOURCE_OSC8M:
      mask = SYSCTRL_PCLKSR_OSC8MRDY;
      break;

    case SYSTEM_CLOCK_SOURCE_OSC32K:
      mask = SYSCTRL_PCLKSR_OSC32KRDY;
      break;

    case SYSTEM_CLOCK_SOURCE_XOSC:
      mask = SYSCTRL_PCLKSR_XOSCRDY;
      break;

    case SYSTEM_CLOCK_SOURCE_XOSC32K:
      mask = SYSCTRL_PCLKSR_XOSC32KRDY;
      break;

    case SYSTEM_CLOCK_SOURCE_DFLL:
      if (CONF_CLOCK_DFLL_LOOP_MODE == SYSTEM_CLOCK_DFLL_LOOP_MODE_CLOSED) {
	mask = (SYSCTRL_PCLKSR_DFLLRDY |
		SYSCTRL_PCLKSR_DFLLLCKF | SYSCTRL_PCLKSR_DFLLLCKC);
      } else {
	mask = SYSCTRL_PCLKSR_DFLLRDY;
      }
      break;

    case SYSTEM_CLOCK_SOURCE_ULP32K:
      /* Not possible to disable */
      return true;

    default:
      return false;
  }

  return ((SYSCTRL->PCLKSR.reg & mask) == mask);
}

/* Include some checks for conf_clocks.h validation */
#include "system/clock_config_check.h"

/**
 * Configures a Generic Clock Generator with the configuration from \c conf_clocks.h.
 */

#define _CONF_CLOCK_GCLK_CONFIG(n)					\
  if (CONF_CLOCK_GCLK_##n##_ENABLE == true) {				\
    system_gclk_gen_set_config(						\
      GCLK_GENERATOR_##n,						\
      CONF_CLOCK_GCLK_##n##_CLOCK_SOURCE,	/* Source Clock      */	\
      false,					/* High When Disabled*/	\
      CONF_CLOCK_GCLK_##n##_PRESCALER, 		/* Division Factor   */	\
      CONF_CLOCK_GCLK_##n##_RUN_IN_STANDBY, 	/* Run in standby    */	\
      CONF_CLOCK_GCLK_##n##_OUTPUT_ENABLE); 	/* Output Pin Enable */	\
    									\
    system_gclk_gen_enable(GCLK_GENERATOR_##n);				\
  }

/**
 * Initialize clock system based on the configuration in conf_clocks.h
 *
 * This function will apply the settings in conf_clocks.h when run from the user
 * application. All clock sources and GCLK generators are running when this function
 * returns.
 */
void system_clock_init(void)
{
  /* Various bits in the INTFLAG register can be set to one at startup.
     This will ensure that these bits are cleared */
  SYSCTRL->INTFLAG.reg = SYSCTRL_INTFLAG_BOD33RDY | SYSCTRL_INTFLAG_BOD33DET |
    SYSCTRL_INTFLAG_DFLLRDY;

  system_flash_set_waitstates(CONF_CLOCK_FLASH_WAIT_STATES);

  /* XOSC */
#if CONF_CLOCK_XOSC_ENABLE == true
  struct system_clock_source_xosc_config xosc_conf;
  system_clock_source_xosc_get_config_defaults(&xosc_conf);

  xosc_conf.external_clock    = CONF_CLOCK_XOSC_EXTERNAL_CRYSTAL;
  xosc_conf.startup_time      = CONF_CLOCK_XOSC_STARTUP_TIME;
  xosc_conf.auto_gain_control = CONF_CLOCK_XOSC_AUTO_GAIN_CONTROL;
  xosc_conf.frequency         = CONF_CLOCK_XOSC_EXTERNAL_FREQUENCY;
  xosc_conf.on_demand         = CONF_CLOCK_XOSC_ON_DEMAND;
  xosc_conf.run_in_standby    = CONF_CLOCK_XOSC_RUN_IN_STANDBY;

  system_clock_source_xosc_set_config(&xosc_conf);
  system_clock_source_enable(SYSTEM_CLOCK_SOURCE_XOSC);
#endif


  /* XOSC32K */
#if CONF_CLOCK_XOSC32K_ENABLE == true
  struct system_clock_source_xosc32k_config xosc32k_conf;
  system_clock_source_xosc32k_get_config_defaults(&xosc32k_conf);

  xosc32k_conf.frequency           = 32768UL;
  xosc32k_conf.external_clock      = CONF_CLOCK_XOSC32K_EXTERNAL_CRYSTAL;
  xosc32k_conf.startup_time        = CONF_CLOCK_XOSC32K_STARTUP_TIME;
  xosc32k_conf.auto_gain_control   = CONF_CLOCK_XOSC32K_AUTO_AMPLITUDE_CONTROL;
  xosc32k_conf.enable_1khz_output  = CONF_CLOCK_XOSC32K_ENABLE_1KHZ_OUPUT;
  xosc32k_conf.enable_32khz_output = CONF_CLOCK_XOSC32K_ENABLE_32KHZ_OUTPUT;
  xosc32k_conf.on_demand           = false;
  xosc32k_conf.run_in_standby      = CONF_CLOCK_XOSC32K_RUN_IN_STANDBY;

  system_clock_source_xosc32k_set_config(&xosc32k_conf);
  system_clock_source_enable(SYSTEM_CLOCK_SOURCE_XOSC32K);
  while(!system_clock_source_is_ready(SYSTEM_CLOCK_SOURCE_XOSC32K));
  if (CONF_CLOCK_XOSC32K_ON_DEMAND) {
    SYSCTRL->XOSC32K.bit.ONDEMAND = 1;
  }
#endif


  /* OSCK32K */
#if CONF_CLOCK_OSC32K_ENABLE == true
  SYSCTRL->OSC32K.bit.CALIB =
    (*(uint32_t *)SYSCTRL_FUSES_OSC32KCAL_ADDR >> SYSCTRL_FUSES_OSC32KCAL_Pos);

  struct system_clock_source_osc32k_config osc32k_conf;
  system_clock_source_osc32k_get_config_defaults(&osc32k_conf);

  osc32k_conf.startup_time        = CONF_CLOCK_OSC32K_STARTUP_TIME;
  osc32k_conf.enable_1khz_output  = CONF_CLOCK_OSC32K_ENABLE_1KHZ_OUTPUT;
  osc32k_conf.enable_32khz_output = CONF_CLOCK_OSC32K_ENABLE_32KHZ_OUTPUT;
  osc32k_conf.on_demand           = CONF_CLOCK_OSC32K_ON_DEMAND;
  osc32k_conf.run_in_standby      = CONF_CLOCK_OSC32K_RUN_IN_STANDBY;

  system_clock_source_osc32k_set_config(&osc32k_conf);
  system_clock_source_enable(SYSTEM_CLOCK_SOURCE_OSC32K);
#endif


  /* DFLL Config (Open and Closed Loop) */
#if CONF_CLOCK_DFLL_ENABLE == true

  system_clock_source_dfll_set_config(
    CONF_CLOCK_DFLL_LOOP_MODE,			/* Loop Mode */
    CONF_CLOCK_DFLL_ON_DEMAND,			/* On demand */
#  if CONF_CLOCK_DFLL_QUICK_LOCK == true
    SYSTEM_CLOCK_DFLL_QUICK_LOCK_ENABLE,	/* Quick Lock */
#  else
    SYSTEM_CLOCK_DFLL_QUICK_LOCK_DISABLE,	/* Quick Lock */
#  endif
#  if CONF_CLOCK_DFLL_ENABLE_CHILL_CYCLE == true
    SYSTEM_CLOCK_DFLL_CHILL_CYCLE_ENABLE,	/* Chill Cycle */
#  else
    SYSTEM_CLOCK_DFLL_CHILL_CYCLE_DISABLE,	/* Chill Cycle */
#  endif
#  if CONF_CLOCK_DFLL_KEEP_LOCK_ON_WAKEUP == true
    SYSTEM_CLOCK_DFLL_WAKEUP_LOCK_KEEP,		/* Lock during wakeup */
#  else
    SYSTEM_CLOCK_DFLL_WAKEUP_LOCK_LOSE,		/* Lock during wakeup */
#  endif
#  if CONF_CLOCK_DFLL_TRACK_AFTER_FINE_LOCK == true
    SYSTEM_CLOCK_DFLL_STABLE_TRACKING_TRACK_AFTER_LOCK,
#  else
    SYSTEM_CLOCK_DFLL_STABLE_TRACKING_FIX_AFTER_LOCK,
#  endif
    CONF_CLOCK_DFLL_COARSE_VALUE,		/* Open Loop   - Coarse calib */
    CONF_CLOCK_DFLL_FINE_VALUE,			/* Open Loop   - Fine calib */
    CONF_CLOCK_DFLL_MAX_COARSE_STEP_SIZE,	/* Closed Loop - Coarse Max step */
    CONF_CLOCK_DFLL_MAX_FINE_STEP_SIZE,		/* Closed Loop - Fine Max step */
    CONF_CLOCK_DFLL_MULTIPLY_FACTOR);		/* Frequency Multiplication  */
#endif

  /* OSC8M */
  system_clock_source_osc8m_set_config(CONF_CLOCK_OSC8M_PRESCALER,
				       CONF_CLOCK_OSC8M_ON_DEMAND,
				       CONF_CLOCK_OSC8M_RUN_IN_STANDBY);
  system_clock_source_enable(SYSTEM_CLOCK_SOURCE_OSC8M);


  /* GCLK */
#if CONF_CLOCK_CONFIGURE_GCLK == true
  system_gclk_init();

  /* Configure all GCLK generators except for the main generator, which
   * is configured later after all other clock systems are set up */
  _CONF_CLOCK_GCLK_CONFIG(1);
  _CONF_CLOCK_GCLK_CONFIG(2);
  _CONF_CLOCK_GCLK_CONFIG(3);
  _CONF_CLOCK_GCLK_CONFIG(4);
  _CONF_CLOCK_GCLK_CONFIG(5);
  _CONF_CLOCK_GCLK_CONFIG(6);
  _CONF_CLOCK_GCLK_CONFIG(7);

#  if CONF_CLOCK_DFLL_ENABLE == true
  /* Enable DFLL reference clock if in closed loop mode */
  if (CONF_CLOCK_DFLL_LOOP_MODE == SYSTEM_CLOCK_DFLL_LOOP_MODE_CLOSED) {

    system_gclk_chan_set_config(SYSCTRL_GCLK_ID_DFLL48,
				CONF_CLOCK_DFLL_SOURCE_GCLK_GENERATOR);

    system_gclk_chan_enable(SYSCTRL_GCLK_ID_DFLL48);
  }
#  endif
#endif


  /* DFLL Enable (Open and Closed Loop) */
#if CONF_CLOCK_DFLL_ENABLE == true
  system_clock_source_enable(SYSTEM_CLOCK_SOURCE_DFLL);
  while(!system_clock_source_is_ready(SYSTEM_CLOCK_SOURCE_DFLL));
  if (CONF_CLOCK_DFLL_ON_DEMAND) {
    SYSCTRL->DFLLCTRL.bit.ONDEMAND = 1;
  }
#endif

  /* CPU and BUS clocks */
  system_cpu_clock_set_divider(CONF_CLOCK_CPU_DIVIDER);

#ifdef FEATURE_SYSTEM_CLOCK_FAILURE_DETECT
  system_main_clock_set_failure_detect(CONF_CLOCK_CPU_CLOCK_FAILURE_DETECT);
#endif

  system_apb_clock_set_divider(SYSTEM_CLOCK_APB_APBA, CONF_CLOCK_APBA_DIVIDER);
  system_apb_clock_set_divider(SYSTEM_CLOCK_APB_APBB, CONF_CLOCK_APBB_DIVIDER);

  /* GCLK 0 */
#if CONF_CLOCK_CONFIGURE_GCLK == true
  /* Configure the main GCLK last as it might depend on other generators */
  _CONF_CLOCK_GCLK_CONFIG(0);
#endif
}
