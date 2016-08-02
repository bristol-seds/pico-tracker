/**
 * SAM D20/D21/R21 GPIO Port Driver
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
#ifndef PORT_H_INCLUDED
#define PORT_H_INCLUDED

/**
 * Port Driver (PORT)
 *
 * This driver for SAM D20/D21/R21 devices provides an interface for
 * the configuration and management of the device's General Purpose
 * Input/Output (GPIO) pin functionality, for manual pin state reading
 * and writing.
 *
 * The following peripherals are used by this module:
 *
 *  - PORT (GPIO Management)
 *
 * Module Overview
 *
 * The device GPIO (PORT) module provides an interface between the
 * user application logic and external hardware peripherals, when
 * general pin state manipulation is required. This driver provides an
 * easy-to-use interface to the physical pin input samplers and output
 * drivers, so that pins can be read from or written to for general
 * purpose external hardware control.
 *
 * Physical and Logical GPIO Pins
 *
 * SAM D20/D21/R21 devices use two naming conventions for the I/O pins
 * in the device; one physical, and one logical. Each physical pin on
 * a device package is assigned both a physical port and pin
 * identifier (e.g. "PORTA.0") as well as a monotonically incrementing
 * logical GPIO number (e.g. "GPIO0"). While the former is used to map
 * physical pins to their physical internal device module
 * counterparts, for simplicity the design of this driver uses the
 * logical GPIO numbers instead.
 *
 * Special Considerations
 *
 * The SAM D20/D21/R21 port pin input sampler can be disabled when the pin is configured
 * in pure output mode to save power; reading the pin state of a pin configured
 * in output-only mode will read the logical output state that was last set.
 *
 */

#include "system/pinmux.h"

/** Convenience definition for GPIO module group A on the device (if
 *  available). */
#if (PORT_GROUPS > 0)
#  define PORTA             PORT->Group[0]
#endif
#if (PORT_GROUPS > 1)
/** Convenience definition for GPIO module group B on the device (if
 *  available). */
#  define PORTB             PORT->Group[1]
#endif
#if (PORT_GROUPS > 2)
/** Convenience definition for GPIO module group C on the device (if
 *  available). */
#  define PORTC             PORT->Group[2]
#endif
#if (PORT_GROUPS > 3)
/** Convenience definition for GPIO module group D on the device (if
 *  available). */
#  define PORTD             PORT->Group[3]
#endif

/**
 *  Enum for the possible pin direction settings of the port pin configuration
 *  structure, to indicate the direction the pin should use.
 */
enum port_pin_dir {
  /** The pin's input buffer should be enabled, so that the pin state can
   *  be read. */
  PORT_PIN_DIR_INPUT               = SYSTEM_PINMUX_PIN_DIR_INPUT,
  /** The pin's output buffer should be enabled, so that the pin state can
   *  be set. */
  PORT_PIN_DIR_OUTPUT              = SYSTEM_PINMUX_PIN_DIR_OUTPUT,
  /** The pin's output and input buffers should be enabled, so that the pin
   *  state can be set and read back. */
  PORT_PIN_DIR_OUTPUT_WTH_READBACK = SYSTEM_PINMUX_PIN_DIR_OUTPUT_WITH_READBACK,
};

/**
 *  Enum for the possible pin pull settings of the port pin configuration
 *  structure, to indicate the type of logic level pull the pin should use.
 */
enum port_pin_pull {
  /** No logical pull should be applied to the pin. */
  PORT_PIN_PULL_NONE = SYSTEM_PINMUX_PIN_PULL_NONE,
  /** Pin should be pulled up when idle. */
  PORT_PIN_PULL_UP   = SYSTEM_PINMUX_PIN_PULL_UP,
  /** Pin should be pulled down when idle. */
  PORT_PIN_PULL_DOWN = SYSTEM_PINMUX_PIN_PULL_DOWN,
};


/**
 *  Retrieves the PORT module group instance from a given GPIO pin number.
 *
 *  Retrieves the PORT module group instance associated with a given
 *  logical GPIO pin number.
 *
 *  \param[in] gpio_pin  Index of the GPIO pin to convert.
 *
 *  \return Base address of the associated PORT module.
 */
static inline PortGroup* port_get_group_from_gpio_pin(const uint8_t gpio_pin)
{
  return system_pinmux_get_group_from_gpio_pin(gpio_pin);
}

/**
 *  Retrieves the state of a group of port pins that are configured as inputs.
 *
 *  Reads the current logic level of a port module's pins and returns
 *  the current levels as a bitmask.
 *
 *  \param[in] port  Base of the PORT module to read from.
 *  \param[in] mask  Mask of the port pin(s) to read.
 *
 *  \return Status of the port pin(s) input buffers.
 */
static inline uint32_t port_group_get_input_level(const PortGroup *const port,
						  const uint32_t mask)
{
  /* Sanity check arguments */


  return (port->IN.reg & mask);
}

/**
 *  Retrieves the state of a group of port pins that are configured as outputs.
 *
 *  Reads the current logical output level of a port module's pins and
 *  returns the current levels as a bitmask.
 *
 *  \param[in] port  Base of the PORT module to read from.
 *  \param[in] mask  Mask of the port pin(s) to read.
 *
 *  \return Status of the port pin(s) output buffers.
 */
static inline uint32_t port_group_get_output_level(const PortGroup *const port,
						   const uint32_t mask)
{
  /* Sanity check arguments */


  return (port->OUT.reg & mask);
}

/**
 *  Sets the state of a group of port pins that are configured as outputs.
 *
 *  \param[out] port        Base of the PORT module to write to.
 *  \param[in]  mask        Mask of the port pin(s) to change.
 *  \param[in]  level_mask  Mask of the port level(s) to set.
 */
static inline void port_group_set_output_level(PortGroup *const port,
					       const uint32_t mask,
					       const uint32_t level_mask)
{
  /* Sanity check arguments */


  port->OUTSET.reg = (mask &  level_mask);
  port->OUTCLR.reg = (mask & ~level_mask);
}

/**
 *  Toggles the state of a group of port pins that are configured as an outputs.
 *
 *  \param[out] port  Base of the PORT module to write to.
 *  \param[in]  mask  Mask of the port pin(s) to toggle.
 */
static inline void port_group_toggle_output_level(PortGroup *const port,
						  const uint32_t mask)
{
  /* Sanity check arguments */


  port->OUTTGL.reg = mask;
}

void port_pin_set_config(const uint8_t gpio_pin,
			 enum port_pin_dir direction,
			 enum port_pin_pull input_pull,
			 bool powersave);
void port_pin_set_config_default(const uint8_t gpio_pin);


void port_group_set_config(PortGroup *const port,
			   const uint32_t mask,
			   enum port_pin_dir direction,
			   enum port_pin_pull input_pull,
			   bool powersave);
void port_group_set_config_default(PortGroup *const port,
				   const uint32_t mask);


/**
 *  Reads the current logic level of a port pin and returns the current
 *  level as a boolean value.
 *
 *  \param[in] gpio_pin  Index of the GPIO pin to read.
 *
 *  \return Status of the port pin's input buffer.
 */
static inline bool port_pin_get_input_level(const uint8_t gpio_pin)
{
  PortGroup *const port_base = port_get_group_from_gpio_pin(gpio_pin);
  uint32_t pin_mask  = (1UL << (gpio_pin % 32));

  return (port_base->IN.reg & pin_mask);
}

/**
 *  Reads the current logical output level of a port pin and returns the current
 *  level as a boolean value.
 *
 *  \param[in] gpio_pin  Index of the GPIO pin to read.
 *
 *  \return Status of the port pin's output buffer.
 */
static inline bool port_pin_get_output_level(const uint8_t gpio_pin)
{
  PortGroup *const port_base = port_get_group_from_gpio_pin(gpio_pin);
  uint32_t pin_mask  = (1UL << (gpio_pin % 32));

  return (port_base->OUT.reg & pin_mask);
}

/**
 *  Sets the current output level of a port pin to a given logic level.
 *
 *  \param[in] gpio_pin  Index of the GPIO pin to write to.
 *  \param[in] level     Logical level to set the given pin to.
 */
static inline void port_pin_set_output_level(const uint8_t gpio_pin,
					     const bool level)
{
  PortGroup *const port_base = port_get_group_from_gpio_pin(gpio_pin);
  uint32_t pin_mask  = (1UL << (gpio_pin % 32));

  /* Set the pin to high or low atomically based on the requested level */
  if (level) {
    port_base->OUTSET.reg = pin_mask;
  } else {
    port_base->OUTCLR.reg = pin_mask;
  }
}

/**
 *  Toggles the current output level of a port pin.
 *
 *  \param[in] gpio_pin  Index of the GPIO pin to toggle.
 */
static inline void port_pin_toggle_output_level(const uint8_t gpio_pin)
{
  PortGroup *const port_base = port_get_group_from_gpio_pin(gpio_pin);
  uint32_t pin_mask  = (1UL << (gpio_pin % 32));

  /* Toggle pin output level */
  port_base->OUTTGL.reg = pin_mask;
}

#endif
