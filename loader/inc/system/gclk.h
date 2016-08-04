/**
 * SAM D20/D21/R21 Generic Clock Driver
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
#ifndef SYSTEM_CLOCK_GCLK_H_INCLUDED
#define SYSTEM_CLOCK_GCLK_H_INCLUDED

#include <stdbool.h>
#include "samd20.h"

/**
 * List of Available GCLK generators. This enum is used in the peripheral
 * device drivers to select the GCLK generator to be used for its operation.
 *
 * The number of GCLK generators available is device dependent.
 */
enum gclk_generator {
  /** GCLK generator channel 0. */
  GCLK_GENERATOR_0,
#if (GCLK_GEN_NUM_MSB > 0)
  /** GCLK generator channel 1. */
  GCLK_GENERATOR_1,
#endif
#if (GCLK_GEN_NUM_MSB > 1)
  /** GCLK generator channel 2. */
  GCLK_GENERATOR_2,
#endif
#if (GCLK_GEN_NUM_MSB > 2)
  /** GCLK generator channel 3. */
  GCLK_GENERATOR_3,
#endif
#if (GCLK_GEN_NUM_MSB > 3)
  /** GCLK generator channel 4. */
  GCLK_GENERATOR_4,
#endif
#if (GCLK_GEN_NUM_MSB > 4)
  /** GCLK generator channel 5. */
  GCLK_GENERATOR_5,
#endif
#if (GCLK_GEN_NUM_MSB > 5)
  /** GCLK generator channel 6. */
  GCLK_GENERATOR_6,
#endif
#if (GCLK_GEN_NUM_MSB > 6)
  /** GCLK generator channel 7. */
  GCLK_GENERATOR_7,
#endif
#if (GCLK_GEN_NUM_MSB > 7)
  /** GCLK generator channel 8. */
  GCLK_GENERATOR_8,
#endif
#if (GCLK_GEN_NUM_MSB > 8)
  /** GCLK generator channel 9. */
  GCLK_GENERATOR_9,
#endif
#if (GCLK_GEN_NUM_MSB > 9)
  /** GCLK generator channel 10. */
  GCLK_GENERATOR_10,
#endif
#if (GCLK_GEN_NUM_MSB > 10)
  /** GCLK generator channel 11. */
  GCLK_GENERATOR_11,
#endif
#if (GCLK_GEN_NUM_MSB > 11)
  /** GCLK generator channel 12. */
  GCLK_GENERATOR_12,
#endif
#if (GCLK_GEN_NUM_MSB > 12)
  /** GCLK generator channel 13. */
  GCLK_GENERATOR_13,
#endif
#if (GCLK_GEN_NUM_MSB > 13)
  /** GCLK generator channel 14. */
  GCLK_GENERATOR_14,
#endif
#if (GCLK_GEN_NUM_MSB > 14)
  /** GCLK generator channel 15. */
  GCLK_GENERATOR_15,
#endif
#if (GCLK_GEN_NUM_MSB > 15)
  /** GCLK generator channel 16. */
  GCLK_GENERATOR_16,
#endif
};

/* Generators */
void system_gclk_init		(void);
void system_gclk_gen_set_config	(const uint8_t generator,
				 const uint8_t source_clock,
				 const bool high_when_disabled,
				 const uint32_t division_factor,
				 const bool run_in_standby,
				 const bool output_enable);
void system_gclk_gen_enable	(const uint8_t generator);
void system_gclk_gen_disable	(const uint8_t generator);
bool system_gclk_gen_is_enabled	(const uint8_t generator);
uint32_t system_gclk_gen_get_hz	(const uint8_t generator);

/* Channels */
void system_gclk_chan_set_config(const uint8_t channel,
				 enum gclk_generator source_generator);
void system_gclk_chan_enable	(const uint8_t channel);
void system_gclk_chan_disable	(const uint8_t channel);
bool system_gclk_chan_is_enabled(const uint8_t channel);
void system_gclk_chan_lock	(const uint8_t channel);
bool system_gclk_chan_is_locked	(const uint8_t channel);
uint32_t system_gclk_chan_get_hz(const uint8_t channel);

#endif
