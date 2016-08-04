/**
 * SAM D20/D21/R21 External Interrupt Driver
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
#ifndef EXTINT_H_INCLUDED
#define EXTINT_H_INCLUDED

/**
 * SAM D20/D21/R21 External Interrupt Driver (EXTINT)
 *
 * This driver for SAM D20/D21/R21 devices provides an interface for the configuration
 * and management of external interrupts generated by the physical device pins,
 * including edge detection.
 *
 * Module Overview
 *
 * The External Interrupt (EXTINT) module provides a method of asynchronously
 * detecting rising edge, falling edge or specific level detection on individual
 * I/O pins of a device. This detection can then be used to trigger a software
 * interrupt or event, or polled for later use if required. External interrupts
 * can also optionally be used to automatically wake up the device from sleep
 * mode, allowing the device to conserve power while still being able to react
 * to an external stimulus in a timely manner.
 *
 * Logical Channels
 *
 * The External Interrupt module contains a number of logical channels, each of
 * which is capable of being individually configured for a given pin routing,
 * detection mode and filtering/wake up characteristics.
 *
 * Each individual logical external interrupt channel may be routed to a single
 * physical device I/O pin in order to detect a particular edge or level of the
 * incoming signal.
 *
 * NMI Channels
 *
 * One or more Non Maskable Interrupt (NMI) channels are provided within each
 * physical External Interrupt Controller module, allowing a single physical pin
 * of the device to fire a single NMI interrupt in response to a particular
 * edge or level stimulus. A NMI cannot, as the name suggests, be disabled in
 * firmware and will take precedence over any in-progress interrupt sources.
 *
 * NMIs can be used to implement critical device features such as forced
 * software reset or other functionality where the action should be executed in
 * preference to all other running code with a minimum amount of latency.
 *
 * Input Filtering and Detection
 *
 * To reduce the possibility of noise or other transient signals causing
 * unwanted device wake-ups, interrupts and/or events via an external interrupt
 * channel, a hardware signal filter can be enabled on individual channels. This
 * filter provides a Majority-of-Three voter filter on the incoming signal, so
 * that the input state is considered to be the majority vote of three
 * subsequent samples of the pin input buffer.
 *
 * Events and Interrupts
 *
 * Channel detection states may be polled inside the application for synchronous
 * detection, or events and interrupts may be used for asynchronous behavior.
 * Each channel can be configured to give an asynchronous hardware event (which
 * may in turn trigger actions in other hardware modules) or an asynchronous
 * software interrupt.
 *
 * Special Considerations
 *
 * Not all devices support disabling of the NMI channel(s) detection mode - see
 * your device datasheet.
 *
 */

#include "samd20.h"
#include "system/pinmux.h"

/**
 * Configuration option, setting the EIC clock source which can be used for
 *  EIC edge detection or filtering.
 */
#define EXTINT_CLOCK_SOURCE GCLK_GENERATOR_0

/**
 * External interrupt edge detection configuration enum.
 *
 * Enum for the possible signal edge detection modes of the External
 * Interrupt Controller module.
 */
enum extint_detect {
  /** No edge detection. Not allowed as a NMI detection mode on some
   *  devices. */
  EXTINT_DETECT_NONE    = 0,
  /** Detect rising signal edges. */
  EXTINT_DETECT_RISING  = 1,
  /** Detect falling signal edges. */
  EXTINT_DETECT_FALLING = 2,
  /** Detect both signal edges. */
  EXTINT_DETECT_BOTH    = 3,
  /** Detect high signal levels. */
  EXTINT_DETECT_HIGH    = 4,
  /** Detect low signal levels. */
  EXTINT_DETECT_LOW     = 5,
};

/**
 * External interrupt internal pull configuration enum.
 *
 * Enum for the possible pin internal pull configurations.
 *
 * \note Disabling the internal pull resistor is not recommended if the driver
 *       is used in interrupt (callback) mode, due the possibility of floating
 *       inputs generating continuous interrupts.
 */
enum extint_pull {
  /** Internal pull-up resistor is enabled on the pin. */
  EXTINT_PULL_UP        = SYSTEM_PINMUX_PIN_PULL_UP,
  /** Internal pull-down resistor is enabled on the pin. */
  EXTINT_PULL_DOWN      = SYSTEM_PINMUX_PIN_PULL_DOWN,
  /** Internal pull resistor is disconnected from the pin. */
  EXTINT_PULL_NONE      = SYSTEM_PINMUX_PIN_PULL_NONE,
};

/**
 * External Interrupt Controller channel configuration structure.
 *
 *  Configuration structure for the edge detection mode of an external
 *  interrupt channel.
 */
struct extint_chan_conf {
  /** GPIO pin the NMI should be connected to. */
  uint32_t gpio_pin;
  /** MUX position the GPIO pin should be configured to. */
  uint32_t gpio_pin_mux;
  /** Internal pull to enable on the input pin. */
  enum extint_pull gpio_pin_pull;
  /** Wake up the device if the channel interrupt fires during sleep mode. */
  bool wake_if_sleeping;
  /** Filter the raw input signal to prevent noise from triggering an
   *  interrupt accidentally, using a 3 sample majority filter. */
  bool filter_input_signal;
  /** Edge detection mode to use. */
  enum extint_detect detection_criteria;
};

/**
 * External Interrupt event enable/disable structure.
 *
 * Event flags for the \ref extint_enable_events() and
 * \ref extint_disable_events().
 */
struct extint_events {
  /** If \c true, an event will be generated when an external interrupt
   *  channel detection state changes. */
  bool generate_event_on_detect[32 * EIC_INST_NUM];
};

/**
 * \brief External Interrupt Controller NMI configuration structure.
 *
 *  Configuration structure for the edge detection mode of an external
 *  interrupt NMI channel.
 */
struct extint_nmi_conf {
  /** GPIO pin the NMI should be connected to. */
  uint32_t gpio_pin;
  /** MUX position the GPIO pin should be configured to. */
  uint32_t gpio_pin_mux;
  /** Internal pull to enable on the input pin. */
  enum extint_pull gpio_pin_pull;
  /** Filter the raw input signal to prevent noise from triggering an
   *  interrupt accidentally, using a 3 sample majority filter. */
  bool filter_input_signal;
  /** Edge detection mode to use. Not all devices support all possible
   *  detection modes for NMIs.
   */
  enum extint_detect detection_criteria;
};




void system_extint_init(void);
void extint_enable(void);
void extint_disable(void);

/**
 * Retrieves the base EIC module address from a given channel number.
 *
 * Retrieves the base address of a EIC hardware module associated with the
 * given external interrupt channel.
 *
 * \param[in] channel  External interrupt channel index to convert.
 *
 * \return Base address of the associated EIC module.
 */
static inline Eic * _extint_get_eic_from_channel(
  const uint8_t channel)
{
  uint8_t eic_index = (channel / 32);

  if (eic_index < EIC_INST_NUM) {
    /* Array of available EICs. */
    Eic *const eics[EIC_INST_NUM] = EIC_INSTS;

    return eics[eic_index];
  } else {
    return NULL;
  }
}

/**
 * Retrieves the base EIC module address from a given NMI channel number.
 *
 * Retrieves the base address of a EIC hardware module associated with the
 * given non-maskable external interrupt channel.
 *
 * \param[in] nmi_channel  Non-Maskable interrupt channel index to convert.
 *
 * \return Base address of the associated EIC module.
 */
static inline Eic * _extint_get_eic_from_nmi(
  const uint8_t nmi_channel)
{
  uint8_t eic_index = nmi_channel;

  if (eic_index < EIC_INST_NUM) {
    /* Array of available EICs. */
    Eic *const eics[EIC_INST_NUM] = EIC_INSTS;

    return eics[eic_index];
  } else {
    return NULL;
  }
}

/**
 * Determines if the hardware module(s) are currently synchronizing to the bus.
 *
 * Checks to see if the underlying hardware peripheral module(s) are currently
 * synchronizing across multiple clock domains to the hardware bus, This
 * function can be used to delay further operations on a module until such time
 * that it is ready, to prevent blocking delays for synchronization in the
 * user application.
 *
 * \return Synchronization status of the underlying hardware module(s).
 *
 * \retval true  If the module has completed synchronization
 * \retval false If the module synchronization is ongoing
 */
static inline bool extint_is_syncing(void)
{
  Eic *const eics[EIC_INST_NUM] = EIC_INSTS;

  for (uint32_t i = 0; i < EIC_INST_NUM; i++) {
    if (eics[i]->STATUS.reg & EIC_STATUS_SYNCBUSY) {
      return true;
    }
  }

  return false;
}

void extint_enable_events(
  struct extint_events *const events);

void extint_disable_events(
  struct extint_events *const events);

/**
 * Initializes an External Interrupt channel configuration structure to defaults.
 *
 * Initializes a given External Interrupt channel configuration structure to a
 * set of known default values. This function should be called on all new
 * instances of these configuration structures before being modified by the
 * user application.
 *
 * The default configuration is as follows:
 * \li Wake the device if an edge detection occurs whilst in sleep
 * \li Input filtering disabled
 * \li Internal pull-up enabled
 * \li Detect falling edges of a signal
 *
 * \param[out] config  Configuration structure to initialize to default values
 */
static inline void extint_chan_get_config_defaults(
  struct extint_chan_conf *const config)
{
  /* Default configuration values */
  config->gpio_pin            = 0;
  config->gpio_pin_mux        = 0;
  config->gpio_pin_pull       = EXTINT_PULL_UP;
  config->wake_if_sleeping    = true;
  config->filter_input_signal = false;
  config->detection_criteria  = EXTINT_DETECT_FALLING;
}

void extint_chan_set_config(
  const uint8_t channel,
  const struct extint_chan_conf *const config);

/**
 * Initializes an External Interrupt NMI channel configuration structure to defaults.
 *
 * Initializes a given External Interrupt NMI channel configuration structure
 * to a set of known default values. This function should be called on all new
 * instances of these configuration structures before being modified by the
 * user application.
 *
 * The default configuration is as follows:
 * \li Input filtering disabled
 * \li Detect falling edges of a signal
 *
 * \param[out] config  Configuration structure to initialize to default values
 */
static inline void extint_nmi_get_config_defaults(
  struct extint_nmi_conf *const config)
{
  /* Default configuration values */
  config->gpio_pin            = 0;
  config->gpio_pin_mux        = 0;
  config->gpio_pin_pull       = EXTINT_PULL_UP;
  config->filter_input_signal = false;
  config->detection_criteria  = EXTINT_DETECT_FALLING;
}

void extint_nmi_set_config(
  const uint8_t nmi_channel,
  const struct extint_nmi_conf *const config);

/**
 * Retrieves the edge detection state of a configured channel.
 *
 *  Reads the current state of a configured channel, and determines
 *  if the detection criteria of the channel has been met.
 *
 *  \param[in] channel  External Interrupt channel index to check.
 *
 *  \return Status of the requested channel's edge detection state.
 *  \retval true   If the channel's edge/level detection criteria was met
 *  \retval false  If the channel has not detected its configured criteria
 */
static inline bool extint_chan_is_detected(
  const uint8_t channel)
{
  Eic *const eic_module = _extint_get_eic_from_channel(channel);
  uint32_t eic_mask   = (1UL << (channel % 32));

  return (eic_module->INTFLAG.reg & eic_mask);
}

/**
 * Clears the edge detection state of a configured channel.
 *
 *  Clears the current state of a configured channel, readying it for
 *  the next level or edge detection.
 *
 *  \param[in] channel  External Interrupt channel index to check.
 */
static inline void extint_chan_clear_detected(
  const uint8_t channel)
{
  Eic *const eic_module = _extint_get_eic_from_channel(channel);
  uint32_t eic_mask   = (1UL << (channel % 32));

  eic_module->INTFLAG.reg = eic_mask;
}

/**
 * Retrieves the edge detection state of a configured NMI channel.
 *
 *  Reads the current state of a configured NMI channel, and determines
 *  if the detection criteria of the NMI channel has been met.
 *
 *  \param[in] nmi_channel  External Interrupt NMI channel index to check.
 *
 *  \return Status of the requested NMI channel's edge detection state.
 *  \retval true   If the NMI channel's edge/level detection criteria was met
 *  \retval false  If the NMI channel has not detected its configured criteria
 */
static inline bool extint_nmi_is_detected(
  const uint8_t nmi_channel)
{
  Eic *const eic_module = _extint_get_eic_from_nmi(nmi_channel);

  return (eic_module->NMIFLAG.reg & EIC_NMIFLAG_NMI);
}

/**
 * Clears the edge detection state of a configured NMI channel.
 *
 *  Clears the current state of a configured NMI channel, readying it for
 *  the next level or edge detection.
 *
 *  \param[in] nmi_channel  External Interrupt NMI channel index to check.
 */
static inline void extint_nmi_clear_detected(
  const uint8_t nmi_channel)
{
  Eic *const eic_module = _extint_get_eic_from_nmi(nmi_channel);

  eic_module->NMIFLAG.reg = EIC_NMIFLAG_NMI;
}

#endif /* EXTINT_H_INCLUDED */