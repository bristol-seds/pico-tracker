/**
 * SAM D20/D21/R21 Pin Multiplexer Driver
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
#ifndef PINMUX_H_INCLUDED
#define PINMUX_H_INCLUDED


#include <stdlib.h>
#include <stdbool.h>
#include "samd20.h"

/*
 * This driver for SAM D20/D21/R21 devices provides an interface for
 * the configuration and management of the device's physical I/O Pins,
 * to alter the direction and input/drive characteristics as well as
 * to configure the pin peripheral multiplexer selection.
 *
 * The following peripherals are used by this module:
 *
 *  - PORT (Port I/O Management)
 *
 * Module Overview
 *
 * The SAM D20/D21/R21 devices contain a number of General Purpose I/O
 * pins, used to interface the user application logic and internal
 * hardware peripherals to an external system. The Pin Multiplexer
 * (PINMUX) driver provides a method of configuring the individual pin
 * peripheral multiplexers to select alternate pin functions.
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
 * Peripheral Multiplexing
 *
 * SAM D20/D21/R21 devices contain a peripheral MUX, which is
 * individually controllable for each I/O pin of the device. The
 * peripheral MUX allows you to select the function of a physical
 * package pin - whether it will be controlled as a user controllable
 * GPIO pin, or whether it will be connected internally to one of
 * several peripheral modules (such as an I<SUP>2</SUP>C module). When
 * a pin is configured in GPIO mode, other peripherals connected to
 * the same pin will be disabled.
 *
 * Special Pad Characteristics
 *
 * There are several special modes that can be selected on one or more
 * I/O pins of the device, which alter the input and output
 * characteristics of the pad:
 *
 * Drive Strength
 *
 * The Drive Strength configures the strength of the output driver on
 * the pad. Normally, there is a fixed current limit that each I/O pin
 * can safely drive, however some I/O pads offer a higher drive mode
 * which increases this limit for that I/O pin at the expense of an
 * increased power.
 *
 * Slew Rate
 *
 * The Slew Rate configures the slew rate of the output driver,
 * limiting the rate at which the pad output voltage can change with
 * time.
 *
 * Input Sample Mode
 *
 * The Input Sample Mode configures the input sampler buffer of the
 * pad. By default, the input buffer is only sampled "on-demand",
 * i.e. when the user application attempts to read from the input
 * buffer. This mode is the most power efficient, but increases the
 * latency of the input sample by two clock cycles of the port
 * clock. To reduce latency, the input sampler can instead be
 * configured to always sample the input buffer on each port clock
 * cycle, at the expense of an increased power consumption.
 *
 * Special Considerations
 *
 * The SAM D20/D21/R21 port pin input sampling mode is set in groups
 * of four physical pins; setting the sampling mode of any pin in a
 * sub-group of eight I/O pins will configure the sampling mode of the
 * entire sub-group.
 *
 * High Drive Strength output driver mode is not available on all device pins -
 * refer to your device specific datasheet.
 */

/** Peripheral multiplexer index to select GPIO mode for a pin. */
#define SYSTEM_PINMUX_GPIO    (1 << 7)

/**
 * Enum for the possible pin direction settings of the port pin configuration
 * structure, to indicate the direction the pin should use.
 */
enum system_pinmux_pin_dir {
  /** The pin's input buffer should be enabled, so that the pin state can
   *  be read. */
  SYSTEM_PINMUX_PIN_DIR_INPUT,

  /** The pin's output buffer should be enabled, so that the pin state can
   *  be set (but not read back). */
  SYSTEM_PINMUX_PIN_DIR_OUTPUT,

  /** The pin's output and input buffers should both be enabled, so that the
   *  pin state can be set and read back. */
  SYSTEM_PINMUX_PIN_DIR_OUTPUT_WITH_READBACK,
};

/**
 * Enum for the possible pin pull settings of the port pin
 * configuration structure, to indicate the type of logic level pull
 * the pin should use.
 */
enum system_pinmux_pin_pull {
  /** No logical pull should be applied to the pin. */
  SYSTEM_PINMUX_PIN_PULL_NONE,

  /** Pin should be pulled up when idle. */
  SYSTEM_PINMUX_PIN_PULL_UP,

  /** Pin should be pulled down when idle. */
  SYSTEM_PINMUX_PIN_PULL_DOWN,
};

/**
 * Enum for the possible input sampling modes for the port pin
 * configuration structure, to indicate the type of sampling a port
 * pin should use.
 */
enum system_pinmux_pin_sample {
  /** Pin input buffer should continuously sample the pin state. */
  SYSTEM_PINMUX_PIN_SAMPLE_CONTINUOUS,

  /** Pin input buffer should be enabled when the IN register is read. */
  SYSTEM_PINMUX_PIN_SAMPLE_ONDEMAND,
};

void system_pinmux_pin_set_config(const uint8_t gpio_pin,
  				  const uint8_t mux_position,
				  const enum system_pinmux_pin_dir direction,
				  const enum system_pinmux_pin_pull input_pull,
				  bool powersave);
void system_pinmux_group_set_config(PortGroup *const port,
				    const uint32_t mask,
				    const uint8_t mux_position,
				    const enum system_pinmux_pin_dir direction,
				    const enum system_pinmux_pin_pull input_pull,
				    bool powersave);

/**
 * Retrieves the PORT module group instance associated with a given
 * logical GPIO pin number.
 *
 * \param[in] gpio_pin  Index of the GPIO pin to convert.
 *
 * \return Base address of the associated PORT module.
 */
static inline PortGroup* system_pinmux_get_group_from_gpio_pin(
  const uint8_t gpio_pin)
{
  uint8_t port_index  = (gpio_pin / 128);
  uint8_t group_index = (gpio_pin / 32);

  /* Array of available ports. */
  Port *const ports[PORT_INST_NUM] = PORT_INSTS;

  if (port_index < PORT_INST_NUM) {
    return &(ports[port_index]->Group[group_index]);
  } else {
    return NULL;
  }
}

void system_pinmux_group_set_input_sample_mode(PortGroup *const port,
					       const uint32_t mask,
					       const enum system_pinmux_pin_sample mode);

#endif
