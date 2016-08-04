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

#include "system/port.h"

/**
 *  Writes out a given configuration of a Port pin configuration to the hardware
 *  module.
 *
 *  \note If the pin direction is set as an output, the pull-up/pull-down input
 *        configuration setting is ignored.
 *
 *  \param[in] gpio_pin  Index of the GPIO pin to configure.
 */
void port_pin_set_config(const uint8_t gpio_pin,
			 enum port_pin_dir direction,
			 enum port_pin_pull input_pull,
			 bool powersave)
{
  system_pinmux_pin_set_config(gpio_pin,
			       SYSTEM_PINMUX_GPIO,
			       direction,
			       input_pull,
			       powersave);
}
/**
 *  Writes out the default configuration of a Port pin to the hardware
 *  module.
 *
 *  \note If the pin direction is set as an output, the pull-up/pull-down input
 *        configuration setting is ignored.
 *
 *  \param[in] gpio_pin  Index of the GPIO pin to configure.
 *
 *  - Input mode with internal pullup enabled
 */
void port_pin_set_config_default(const uint8_t gpio_pin)
{
  port_pin_set_config(gpio_pin,
		      PORT_PIN_DIR_INPUT,	/* Direction */
		      PORT_PIN_PULL_UP,		/* Pull */
		      false);			/* Powersave */
}

/**
 *  Writes out a given configuration of a Port group configuration to the
 *  hardware module.
 *
 *  \note If the pin direction is set as an output, the pull-up/pull-down input
 *        configuration setting is ignored.
 *
 *  \param[out] port    Base of the PORT module to write to.
 *  \param[in]  mask    Mask of the port pin(s) to configure.
 */
void port_group_set_config(PortGroup *const port,
			   const uint32_t mask,
			   enum port_pin_dir direction,
			   enum port_pin_pull input_pull,
			   bool powersave)
{
  system_pinmux_group_set_config(port, mask,
				 SYSTEM_PINMUX_GPIO,
				 direction,
				 input_pull,
				 powersave);
}

/**
 *  Writes out a given configuration of a Port group configuration to the
 *  hardware module.
 *
 *  \note If the pin direction is set as an output, the pull-up/pull-down input
 *        configuration setting is ignored.
 *
 *  \param[out] port    Base of the PORT module to write to.
 *  \param[in]  mask    Mask of the port pin(s) to configure.
 *
 *  - Input mode with internal pullup enabled
 */
void port_group_set_config_default(PortGroup *const port,
			   const uint32_t mask)
{
  port_group_set_config(port, mask,
			PORT_PIN_DIR_INPUT,
			PORT_PIN_PULL_UP,
			false);
}
