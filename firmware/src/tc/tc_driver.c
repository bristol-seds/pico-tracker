/**
 * SAM D20/D21/R21 TC - Timer Counter Driver
 *
 * Copyright (C) 2013-2014 Atmel Corporation. All rights reserved.
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

#include <assert.h>

#include "system/clock.h"
#include "tc/tc_driver.h"
#include "samd20.h"

#define WAIT_FOR_SYNC(tc)	while (tc->COUNT8.STATUS.reg & TC_STATUS_SYNCBUSY)
#define TC_INST_APBCMASKS {			\
  PM_APBCMASK_TC0,				\
  PM_APBCMASK_TC1,				\
  PM_APBCMASK_TC2,				\
  PM_APBCMASK_TC3,				\
  PM_APBCMASK_TC4,				\
  PM_APBCMASK_TC5,				\
}
#define TC_INST_GCLK_IDS {			\
  TC0_GCLK_ID,					\
  TC1_GCLK_ID,					\
  TC2_GCLK_ID,					\
  TC3_GCLK_ID,					\
  TC4_GCLK_ID,					\
  TC5_GCLK_ID,					\
}


/**
 * Enables a TC module that has been previously initialized. The counter will
 * start when the counter is enabled.
 *
 * \note When the counter is configured to re-trigger on an event, the counter
 *       will not start until the start function is used.
 */
static inline void tc_enable(Tc* const hw)
{
  assert(hw);

  WAIT_FOR_SYNC(hw);

  /* Enable TC module */
  hw->COUNT8.CTRLA.reg |= TC_CTRLA_ENABLE;
}
/**
 * Disables a TC module and stops the counter.
 */
static inline void tc_disable(Tc* const hw)
{
  assert(hw);

  WAIT_FOR_SYNC(hw);

  /* Disable TC module */
  hw->COUNT8.CTRLA.reg  &= ~TC_CTRLA_ENABLE;
}

/**
 * Starts or restarts an initialized TC module's counter.
 */
static inline void tc_start_counter(Tc* const hw)
{
  assert(hw);

  /* Make certain that there are no conflicting commands in the register */
  WAIT_FOR_SYNC(hw);
  hw->COUNT8.CTRLBCLR.reg = TC_CTRLBCLR_CMD_NONE;

  /* Write command to execute */
  WAIT_FOR_SYNC(hw);
  hw->COUNT8.CTRLBSET.reg = TC_CTRLBSET_CMD(1);
}
/**
 * Stops the counter. When the counter is stopped
 * the value in the count value is set to 0 if the counter was
 * counting up, or max or the top value if the counter was counting
 * down when stopped.
 */
static inline void tc_stop_counter(Tc* const hw)
{
  assert(hw);

  WAIT_FOR_SYNC(hw);

  /* Write command to execute */
  hw->COUNT8.CTRLBSET.reg = TC_CTRLBSET_CMD(2);
}

/**
 * Retrieves the status of the module, giving overall state information.
 *
 * \return Bitmask of \c TC_STATUS_* flags
 *
 * \retval TC_STATUS_CHANNEL_0_MATCH   Timer channel 0 compare/capture match
 * \retval TC_STATUS_CHANNEL_1_MATCH   Timer channel 1 compare/capture match
 * \retval TC_STATUS_SYNC_READY        Timer read synchronization has completed
 * \retval TC_STATUS_CAPTURE_OVERFLOW  Timer capture data has overflowed
 * \retval TC_STATUS_COUNT_OVERFLOW    Timer count value has overflowed
 */
static inline uint32_t tc_get_status(Tc* const hw)
{
  assert(hw);

  uint32_t int_flags = hw->COUNT8.INTFLAG.reg;
  uint32_t status_flags = 0;

  /* Check for TC channel 0 match */
  if (int_flags & TC_INTFLAG_MC(1)) {
    status_flags |= TC_STATUS_CHANNEL_0_MATCH;
  }

  /* Check for TC channel 1 match */
  if (int_flags & TC_INTFLAG_MC(2)) {
    status_flags |= TC_STATUS_CHANNEL_1_MATCH;
  }

  /* Check for TC read synchronization ready */
  if (int_flags & TC_INTFLAG_SYNCRDY) {
    status_flags |= TC_STATUS_SYNC_READY;
  }

  /* Check for TC capture overflow */
  if (int_flags & TC_INTFLAG_ERR) {
    status_flags |= TC_STATUS_CAPTURE_OVERFLOW;
  }

  /* Check for TC count overflow */
  if (int_flags & TC_INTFLAG_OVF) {
    status_flags |= TC_STATUS_COUNT_OVERFLOW;
  }

  return status_flags;
}
/**
 * Clears the given status flag of the module.
 *
 * \param[in] status_flags  Bitmask of \c TC_STATUS_* flags to clear
 */
static inline void tc_clear_status(Tc* const hw,
				   const uint32_t status_flags)
{
  assert(hw);

  uint32_t int_flags = 0;

  /* Check for TC channel 0 match */
  if (status_flags & TC_STATUS_CHANNEL_0_MATCH) {
    int_flags |= TC_INTFLAG_MC(1);
  }

  /* Check for TC channel 1 match */
  if (status_flags & TC_STATUS_CHANNEL_1_MATCH) {
    int_flags |= TC_INTFLAG_MC(2);
  }

  /* Check for TC read synchronization ready */
  if (status_flags & TC_STATUS_SYNC_READY) {
    int_flags |= TC_INTFLAG_SYNCRDY;
  }

  /* Check for TC capture overflow */
  if (status_flags & TC_STATUS_CAPTURE_OVERFLOW) {
    int_flags |= TC_INTFLAG_ERR;
  }

  /* Check for TC count overflow */
  if (status_flags & TC_STATUS_COUNT_OVERFLOW) {
    int_flags |= TC_INTFLAG_OVF;
  }

  /* Clear interrupt flag */
  hw->COUNT8.INTFLAG.reg = int_flags;
}

/**
 * Sets the current timer count value of a initialized TC module. The
 * specified TC module may be started or stopped.
 *
 * \param[in] count        New timer count value to set
 *
 * \return Status of the count update procedure.
 */
void tc_set_count_value(Tc* const hw, const uint32_t count)
{
  assert(hw);

  WAIT_FOR_SYNC(hw);

  /* Write to based on the TC counter_size */
  switch (hw->COUNT8.CTRLA.reg & TC_CTRLA_MODE_Msk) {
    case TC_COUNTER_SIZE_8BIT:
      assert((count & ~0xFF) == 0);

      hw->COUNT8.COUNT.reg  = (uint8_t)count;
      return;

    case TC_COUNTER_SIZE_16BIT:
      assert((count & ~0xFFFF) == 0);

      hw->COUNT16.COUNT.reg = (uint16_t)count;
      return;

    case TC_COUNTER_SIZE_32BIT:
      hw->COUNT32.COUNT.reg = (uint32_t)count;
      return;

    default:
      assert(TC_ERROR_INVALID_STATE);
  }
}

/**
 * Retrieves the current count value of a TC module. The specified TC module
 * may be started or stopped.
 *
 * \param[in] module_inst  Pointer to the software module instance struct
 *
 * \return Count value of the specified TC module.
 */
uint32_t tc_get_count_value(Tc* const hw)
{
  assert(hw);

  WAIT_FOR_SYNC(hw);

  /* Read from based on the TC counter size */
  switch (hw->COUNT8.CTRLA.reg & TC_CTRLA_MODE_Msk) {
    case TC_COUNTER_SIZE_8BIT:
      return (uint32_t)hw->COUNT8.COUNT.reg;

    case TC_COUNTER_SIZE_16BIT:
      return (uint32_t)hw->COUNT16.COUNT.reg;

    case TC_COUNTER_SIZE_32BIT:
      return hw->COUNT32.COUNT.reg;

    default:
      assert(TC_ERROR_INVALID_STATE);
      return 0;
  }
}

/**
 * Retrieves the capture value in the indicated TC module capture channel.
 *
 * \param[in]  module_inst    Pointer to the software module instance struct
 * \param[in]  channel_index  Index of the Compare Capture channel to read
 *
 * \return Capture value stored in the specified timer channel.
 */
uint32_t tc_get_capture_value(Tc* const hw,
			      const enum tc_compare_capture_channel channel_index)
{
  assert(hw);

  WAIT_FOR_SYNC(hw);

  /* Read out based on the TC counter size */
  switch (hw->COUNT8.CTRLA.reg & TC_CTRLA_MODE_Msk) {
    case TC_COUNTER_SIZE_8BIT:
      if (channel_index < NUMBER_OF_COMPARE_CAPTURE_CHANNELS) {
	return hw->COUNT8.CC[channel_index].reg;
      }

    case TC_COUNTER_SIZE_16BIT:
      if (channel_index < NUMBER_OF_COMPARE_CAPTURE_CHANNELS) {
	return hw->COUNT16.CC[channel_index].reg;
      }

    case TC_COUNTER_SIZE_32BIT:
      if (channel_index < NUMBER_OF_COMPARE_CAPTURE_CHANNELS) {
	return hw->COUNT32.CC[channel_index].reg;
      }

    default:
      assert(TC_ERROR_INVALID_STATE);
      return 0;
  }

  assert(TC_ERROR_INVALID_ARG);
  return 0;
}

/**
 * Writes a compare value to the given TC module compare/capture channel.
 *
 * \param[in]  module_inst    Pointer to the software module instance struct
 * \param[in]  channel_index  Index of the compare channel to write to
 * \param[in]  compare        New compare value to set
 *
 * \return Status of the compare update procedure.
 *
 * \retval  STATUS_OK               The compare value was updated successfully
 * \retval  STATUS_ERR_INVALID_ARG  An invalid channel index was supplied
 */
void tc_set_compare_value(Tc* const hw,
			  const enum tc_compare_capture_channel channel_index,
			  const uint32_t compare)
{
  assert(hw);
  assert(compare);

  WAIT_FOR_SYNC(hw);

  /* Read out based on the TC counter size */
  switch (hw->COUNT8.CTRLA.reg & TC_CTRLA_MODE_Msk) {
    case TC_COUNTER_SIZE_8BIT:
      if (channel_index < NUMBER_OF_COMPARE_CAPTURE_CHANNELS) {
	hw->COUNT8.CC[channel_index].reg = (uint8_t)compare;
	return;
      }

    case TC_COUNTER_SIZE_16BIT:
      if (channel_index < NUMBER_OF_COMPARE_CAPTURE_CHANNELS) {
	hw->COUNT16.CC[channel_index].reg = (uint16_t)compare;
	return;
      }

    case TC_COUNTER_SIZE_32BIT:
      if (channel_index < NUMBER_OF_COMPARE_CAPTURE_CHANNELS) {
	hw->COUNT32.CC[channel_index].reg = (uint32_t)compare;
	return;
      }

    default:
      assert(TC_ERROR_INVALID_STATE);
      return;
  }

  /* Channel index was wrong */
  assert(TC_ERROR_INVALID_ARG);
  return;
}

/**
 * Returns the instance number
 */
static inline uint32_t tc_get_instance_number(Tc* const hw)
{
  uint32_t i;

  Tc* const tc_instances[] = TC_INSTS;

  for (i = 0; i < TC_INST_NUM; i++) {
    if (tc_instances[i] == hw) {
      return i;
    }
  }

  assert(TC_ERROR_INVALID_DEVICE);
  return 0;
}
/**
 * Gets the slave instance of a given TC instance. Asserts if no such
 * instance exists.
 */
const Tc* tc_get_slave_instance(Tc* const hw) {
  uint32_t i;

  /* Search through all even numbered TC instances, excluding the last
   * instance (which should be odd anyway) */
  Tc* const tc_instances[] = TC_INSTS;

  for (i = 0; i < TC_INST_NUM - 1; i += 2) {
    /* Match with the current */
    if (tc_instances[i] == hw) {
      return tc_instances[i + 1];
    }
  }

  assert(TC_ERROR_NO_32BIT_SLAVE_EXISTS);
  return 0;
}

/**
 * Resets the TC module, restoring all hardware module registers to their
 * default values and disabling the module. The TC module will not be
 * accessible while the reset is being performed.
 *
 * \note When resetting a 32-bit counter only the master TC module's instance
 *       structure should be passed to the function.
 *
 * \param[in]  module_inst    Pointer to the software module instance struct
 *
 * \return Status of the procedure
 * \retval STATUS_OK                   The module was reset successfully
 * \retval STATUS_ERR_UNSUPPORTED_DEV  A 32-bit slave TC module was passed to
 *                                     the function. Only use reset on master
 *                                     TC.
 */
void tc_reset(Tc* const hw)
{
  if (hw->COUNT8.STATUS.reg & TC_STATUS_SLAVE) {
    assert(TC_ERROR_INVALID_DEVICE);
    return;
  }

  /* Disable this module if it is running */
  if (hw->COUNT8.CTRLA.reg & TC_CTRLA_ENABLE) {
    tc_disable(hw);
    WAIT_FOR_SYNC(hw);
  }

  /* Reset this TC module */
  hw->COUNT8.CTRLA.reg |= TC_CTRLA_SWRST;

  return;
}

/**
 * \brief Set the timer TOP/period value.
 *
 * For 8-bit counter size this function writes the top value to the period
 * register.
 *
 * For 16- and 32-bit counter size this function writes the top value to
 * Capture Compare register 0. The value in this register can not be used for
 * any other purpose.
 *
 * \note This function is designed to be used in PWM or frequency
 *       match modes only. When the counter is set to 16- or 32-bit counter
 *       size. In 8-bit counter size it will always be possible to change the
 *       top value even in normal mode.
 *
 * \param[in]  top_value     New timer TOP value to set
 *
 * \return Status of the TOP set procedure.
 *
 * \retval STATUS_OK              The timer TOP value was updated successfully
 * \retval STATUS_ERR_INVALID_ARG The configured TC module counter size in the
 *                                module instance is invalid.
 */
void tc_set_top_value(Tc* const hw,
		      const uint32_t top_value)
{
  WAIT_FOR_SYNC(hw);

  switch (hw->COUNT8.CTRLA.reg & TC_CTRLA_MODE_Msk) {
    case TC_COUNTER_SIZE_8BIT:
      assert((top_value & ~0xFF) == 0);
      hw->COUNT8.PER.reg    = (uint8_t)top_value;
      return;

    case TC_COUNTER_SIZE_16BIT:
      assert((top_value & ~0xFFFF) == 0);
      hw->COUNT16.CC[0].reg = (uint16_t)top_value;
      return;

    case TC_COUNTER_SIZE_32BIT:
      hw->COUNT32.CC[0].reg = (uint32_t)top_value;
      return;

    default:
      assert(TC_ERROR_INVALID_STATE);
  }

  return;
}


/**
 * \brief Enables a TC module event input or output.
 *
 * Enables one or more input or output events to or from the TC module.
 * See \ref tc_events for a list of events this module supports.
 *
 * \note Events cannot be altered while the module is enabled.
 *
 * \param[in]  events       Struct containing flags of events to enable
 */
static inline void tc_enable_events(Tc* const hw,
				    struct tc_events *const events)
{
  /* Sanity check arguments */
  assert(hw);
  assert(events);

  uint32_t event_mask = 0;

  if (events->invert_event_input == true) {
    event_mask |= TC_EVCTRL_TCINV;
  }

  if (events->on_event_perform_action == true) {
    event_mask |= TC_EVCTRL_TCEI;
  }

  if (events->generate_event_on_overflow == true) {
    event_mask |= TC_EVCTRL_OVFEO;
  }

  for (uint8_t i = 0; i < NUMBER_OF_COMPARE_CAPTURE_CHANNELS; i++) {
    if (events->generate_event_on_compare_channel[i] == true) {
      event_mask |= (TC_EVCTRL_MCEO(1) << i);
    }
  }

  hw->COUNT8.EVCTRL.reg |= event_mask | events->event_action;
}
/**
 * \brief Disables a TC module event input or output.
 *
 * Disables one or more input or output events to or from the TC module.
 * See \ref tc_events for a list of events this module supports.
 *
 * \note Events cannot be altered while the module is enabled.
 *
 * \param[in]  events       Struct containing flags of events to disable
 */
static inline void tc_disable_events(Tc* const hw,
				     struct tc_events *const events)
{
  assert(hw);
  assert(events);

  uint32_t event_mask = 0;

  if (events->invert_event_input == true) {
    event_mask |= TC_EVCTRL_TCINV;
  }

  if (events->on_event_perform_action == true) {
    event_mask |= TC_EVCTRL_TCEI;
  }

  if (events->generate_event_on_overflow == true) {
    event_mask |= TC_EVCTRL_OVFEO;
  }

  for (uint8_t i = 0; i < NUMBER_OF_COMPARE_CAPTURE_CHANNELS; i++) {
    if (events->generate_event_on_compare_channel[i] == true) {
      event_mask |= (TC_EVCTRL_MCEO(1) << i);
    }
  }

  hw->COUNT8.EVCTRL.reg &= ~event_mask;
}

/**
 * Enables the clock and initializes the TC module, based on the given
 * configuration values.
 *
 * \param[in]     hw           Pointer to the TC hardware module
 */
enum tc_status_t tc_init(Tc* const hw,
			 enum gclk_generator source_clock,
			 enum tc_counter_size counter_size, /* 8, 16 or 32bit */
			 enum tc_clock_prescaler clock_prescaler,
			 enum tc_wave_generation wave_generation,
			 enum tc_reload_action reload_action,
			 enum tc_count_direction count_direction,
			 uint8_t waveform_invert_output,
			 bool oneshot,
			 bool run_in_standby,
			 uint32_t value,
			 uint32_t top_value,
			 bool* enable_capture_channels,
			 uint32_t* compare_channel_values)
{
  assert(hw);

  const Tc* slave;
  uint16_t ctrla_tmp = 0;
  uint8_t ctrlbset_tmp = 0;
  uint8_t ctrlc_tmp = 0;
  uint8_t tc_instance_n = tc_get_instance_number(hw);
  uint32_t const tc_apbcmasks[] = TC_INST_APBCMASKS;
  uint8_t tc_gclk_ids[] = TC_INST_GCLK_IDS;

  if (hw->COUNT8.CTRLA.reg & TC_CTRLA_SWRST) {
    /* We are in the middle of a reset. Abort. */
    return TC_STATUS_BUSY;
  }

  if (hw->COUNT8.STATUS.reg & TC_STATUS_SLAVE) {
    /* Module is used as a slave */
    return TC_STATUS_DENIED;
  }

  if (hw->COUNT8.CTRLA.reg & TC_CTRLA_ENABLE) {
    /* Module must be disabled before initialization. Abort. */
    return TC_STATUS_DENIED;
  }

  /* Enable the user interface clock in the PM */
  system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC,
			    tc_apbcmasks[tc_instance_n]);

  /* Enable the slave counter if counter_size is 32 bit */
  if (counter_size == TC_COUNTER_SIZE_32BIT)
  {
    if ((slave = tc_get_slave_instance(hw))) {
      /* Enable the user interface clock in the PM */
      system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC,
				tc_apbcmasks[tc_instance_n + 1]);
    }
  }

  /* Setup clock for module */
  system_gclk_chan_set_config(tc_gclk_ids[tc_instance_n], source_clock);
  system_gclk_chan_enable(tc_gclk_ids[tc_instance_n]);

  /* Set ctrla register */
  ctrla_tmp = (uint32_t)counter_size | (uint32_t)wave_generation |
              (uint32_t)reload_action | (uint32_t)clock_prescaler;
  if (run_in_standby) {
    ctrla_tmp |= TC_CTRLA_RUNSTDBY;
  }
  /* Write configuration to register */
  WAIT_FOR_SYNC(hw);
  hw->COUNT8.CTRLA.reg = ctrla_tmp;

  /* Set ctrlb register */
  if (oneshot) {
    ctrlbset_tmp = TC_CTRLBSET_ONESHOT;
  }
  if (count_direction) {
    ctrlbset_tmp |= TC_CTRLBSET_DIR;
  }
  /* Clear old ctrlb configuration */
  WAIT_FOR_SYNC(hw);
  hw->COUNT8.CTRLBCLR.reg = 0xFF;
  /* Check if we actually need to go into a wait state. */
  if (ctrlbset_tmp) {
    WAIT_FOR_SYNC(hw);
    /* Write configuration to register */
    hw->COUNT8.CTRLBSET.reg = ctrlbset_tmp;
  }

  /* Set ctrlc register */
  ctrlc_tmp = waveform_invert_output;
  for (uint8_t i = 0; i < NUMBER_OF_COMPARE_CAPTURE_CHANNELS; i++) {
    if (enable_capture_channels[i] == true) {
      ctrlc_tmp |= (TC_CTRLC_CPTEN(1) << i);
    }
  }
  /* Write configuration to register */
  WAIT_FOR_SYNC(hw);
  hw->COUNT8.CTRLC.reg = ctrlc_tmp;


  /* Write Start Value */
  tc_set_count_value(hw, value);

  /* Write Top Value - Only cool for 8 bit!*/
  tc_set_top_value(hw, top_value);

  /* Write Compare Values */
  for (uint8_t i = 0; i < NUMBER_OF_COMPARE_CAPTURE_CHANNELS; i++) {
    tc_set_compare_value(hw, i, compare_channel_values[i]);
  }

  return TC_STATUS_OK;
}


/**
 * Enables the clock and initializes the TC module, based on default
 * values.
 *
 * \param[in]     hw           Pointer to the TC hardware module
 */
enum tc_status_t tc_init_default(Tc* const hw)
{
  bool capture_channel_enables[]    = {false, false};
  uint32_t capture_channel_values[] = {0x0000, 0x0000};

  return tc_init(hw,
                 GCLK_GENERATOR_0,
		 TC_COUNTER_SIZE_16BIT,
		 TC_CLOCK_PRESCALER_DIV1,
		 TC_WAVE_GENERATION_NORMAL_FREQ,
		 TC_RELOAD_ACTION_GCLK,
		 TC_COUNT_DIRECTION_UP,
		 TC_WAVEFORM_INVERT_OUTPUT_NONE,
		 false,			/* Oneshot = false */
		 false,			/* Run in standby = false */
		 0x0000,		/* Initial value */
		 0x0000,		/* Top value */
		 capture_channel_enables,	/* Capture Channel Enables */
		 capture_channel_values);	/* Compare Channels Values */
}
