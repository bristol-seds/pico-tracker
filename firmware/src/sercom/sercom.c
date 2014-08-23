/**
 * SAM D20/D21/R21 Serial Peripheral Interface Driver
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

#include "samd20.h"

#include "semihosting.h"
#include "sercom/sercom.h"
#include "util/mrecursion.h"

/**
 * Sercom Configuration
 */
#define SHIFT 32
#define BAUD_INT_MAX   8192
#define BAUD_FP_MAX     8

/**
 * Find index of given instance.
 */
uint8_t _sercom_get_sercom_inst_index(Sercom *const sercom_instance)
{
  /* Save all available SERCOM instances for compare. */
  Sercom *sercom_instances[SERCOM_INST_NUM] = SERCOM_INSTS;

  /* Find index for sercom instance. */
  for (uint32_t i = 0; i < SERCOM_INST_NUM; i++) {
    if ((uintptr_t)sercom_instance == (uintptr_t)sercom_instances[i]) {
      return i;
    }
  }

  /* Invalid data given. */
  assert(false);
  return 0;
}

/** Error callback for unregistered sercom interrupt handlers */
void sercom_unregistered_handler(Sercom* const sercom_instance, uint8_t instance_index) {
  (void)sercom_instance;

  /* Print error message */
  semihost_printf("Unregisted Handler for Sercom %d called! HALT", instance_index);

  while (1);
}

/** Void pointers for sercom interrupt handlers */
static void (*_sercom_interrupt_handlers[SERCOM_INST_NUM])(Sercom* const sercom_instance,
							   uint8_t instance_index) = {
#if SERCOM_INST_NUM > 0
  sercom_unregistered_handler,
#endif
#if SERCOM_INST_NUM > 1
  sercom_unregistered_handler,
#endif
#if SERCOM_INST_NUM > 2
  sercom_unregistered_handler,
#endif
#if SERCOM_INST_NUM > 3
  sercom_unregistered_handler,
#endif
#if SERCOM_INST_NUM > 4
  sercom_unregistered_handler,
#endif
#if SERCOM_INST_NUM > 5
  sercom_unregistered_handler,
#endif
};

/** Sercom interrupt handlers */
#define SERCOM_INTERRUPT_HANDLER(n)			\
  void SERCOM##n##_Handler(void)			\
  {							\
    _sercom_interrupt_handlers[n](SERCOM##n, n);	\
  }
#if SERCOM_INST_NUM > 0
SERCOM_INTERRUPT_HANDLER(0)
#endif
#if SERCOM_INST_NUM > 1
SERCOM_INTERRUPT_HANDLER(1)
#endif
#if SERCOM_INST_NUM > 2
SERCOM_INTERRUPT_HANDLER(2)
#endif
#if SERCOM_INST_NUM > 3
SERCOM_INTERRUPT_HANDLER(3)
#endif
#if SERCOM_INST_NUM > 4
SERCOM_INTERRUPT_HANDLER(4)
#endif
#if SERCOM_INST_NUM > 5
SERCOM_INTERRUPT_HANDLER(5)
#endif

/**
 * Sets an interrupt handler
 */
void _sercom_set_handler(Sercom* const sercom_instance,
			 const sercom_handler_t interrupt_handler)
{
  /* Retrieve the index of the SERCOM being requested */
  uint8_t instance_index = _sercom_get_sercom_inst_index(sercom_instance);

  /* Set the interrupt handler */
  _sercom_interrupt_handlers[instance_index] = interrupt_handler;
}

/**
 * Calculate synchronous baudrate value (SPI/UART)
 */
enum sercom_status_t _sercom_get_sync_baud_val(const uint32_t baudrate,
					       const uint32_t external_clock,
					       uint16_t *const baudvalue)
{
  /* Baud value variable */
  uint16_t baud_calculated = 0;

  /* Check if baudrate is outside of valid range. */
  if (baudrate > (external_clock / 2)) {
    /* Return with error code */
    return SERCOM_STATUS_BAUDRATE_UNAVAILABLE;
  }

  /* Calculate BAUD value from clock frequency and baudrate */
  baud_calculated = (external_clock / (2 * baudrate)) - 1;

  /* Check if BAUD value is more than 255, which is maximum
   * for synchronous mode */
  if (baud_calculated > 0xFF) {
    /* Return with an error code */
    return SERCOM_STATUS_BAUDRATE_UNAVAILABLE;
  } else {
    *baudvalue = baud_calculated;
    return SERCOM_STATUS_OK;
  }
}

/**
 * Calculate asynchronous baudrate value (UART)
 */
enum sercom_status_t _sercom_get_async_baud_val(const uint32_t baudrate,
						const uint32_t peripheral_clock,
						uint16_t *const baudval,
						enum sercom_asynchronous_operation_mode mode,
						enum sercom_asynchronous_sample_num sample_num)
{
  /* Temporary variables  */
  uint64_t ratio = 0;
  uint64_t scale = 0;
  uint64_t baud_calculated = 0;
  uint8_t baud_fp;
  uint32_t baud_int;

  /* Check if the baudrate is outside of valid range */
  if ((baudrate * sample_num) >= peripheral_clock) {
    /* Return with error code */
    return SERCOM_STATUS_BAUDRATE_UNAVAILABLE;
  }

  if(mode == SERCOM_ASYNC_OPERATION_MODE_ARITHMETIC) {
    /* Calculate the BAUD value */
    ratio = ((sample_num * (uint64_t)baudrate) << SHIFT) / peripheral_clock;
    scale = ((uint64_t)1 << SHIFT) - ratio;
    baud_calculated = (65536 * scale) >> SHIFT;

  } else if(mode == SERCOM_ASYNC_OPERATION_MODE_FRACTIONAL) {

    for(baud_fp = 0; baud_fp < BAUD_FP_MAX; baud_fp++) {
      baud_int = BAUD_FP_MAX *
	(uint64_t)peripheral_clock / ((uint64_t)baudrate * sample_num)
	- baud_fp;
      baud_int = baud_int / BAUD_FP_MAX;

      if(baud_int < BAUD_INT_MAX) {
	break;
      }
    }

    if(baud_fp == BAUD_FP_MAX) {
      return SERCOM_STATUS_BAUDRATE_UNAVAILABLE;
    }
    baud_calculated = baud_int | (baud_fp << 13);
  }

  *baudval = baud_calculated;
  return SERCOM_STATUS_OK;
}

/**
 * Set GCLK channel to generator.
 *
 * This will set the appropriate GCLK channel to the requested GCLK generator.
 *
 * \param[in]  generator_source The generator to use for SERCOM.
 */
void _sercom_set_gclk_generator(const enum gclk_generator generator_source)
{
  system_gclk_chan_set_config(SERCOM_GCLK_ID, generator_source);
  system_gclk_chan_enable(SERCOM_GCLK_ID);
}

/**
 * Convert a SERCOM instance and pad index to the default SERCOM pad
 * MUX setting.
 */
#define SERCOM_PAD_DEFAULT(n, pad)		\
  switch (pad) {				\
    case 0:					\
      return SERCOM##n##_PAD0_DEFAULT;		\
    case 1:					\
      return SERCOM##n##_PAD1_DEFAULT;		\
    case 2:					\
      return SERCOM##n##_PAD2_DEFAULT;		\
    case 3:					\
      return SERCOM##n##_PAD3_DEFAULT;		\
  }

/**
 * Returns the PINMUX settings for the given SERCOM and pad. This is used
 * for default configuration of pins.
 *
 * \param[in]  sercom_module   Pointer to the SERCOM module
 * \param[in]  pad             PAD to get default pinout for
 *
 * \returns The default PINMUX for the given SERCOM instance and PAD
 *
 */
uint32_t _sercom_get_default_pad(Sercom* const sercom_module,
				 const uint8_t pad)
{
  switch((uintptr_t)sercom_module) {
#if SERCOM_INST_NUM > 0
    case (uintptr_t)SERCOM0:
      SERCOM_PAD_DEFAULT(0, pad); break;
#endif
#if SERCOM_INST_NUM > 1
    case (uintptr_t)SERCOM1:
      SERCOM_PAD_DEFAULT(1, pad); break;
#endif
#if SERCOM_INST_NUM > 2
    case (uintptr_t)SERCOM2:
      SERCOM_PAD_DEFAULT(2, pad); break;
#endif
#if SERCOM_INST_NUM > 3
    case (uintptr_t)SERCOM3:
      SERCOM_PAD_DEFAULT(3, pad); break;
#endif
#if SERCOM_INST_NUM > 4
    case (uintptr_t)SERCOM4:
      SERCOM_PAD_DEFAULT(4, pad); break;
#endif
#if SERCOM_INST_NUM > 5
    case (uintptr_t)SERCOM5:
      SERCOM_PAD_DEFAULT(5, pad); break;
#endif
  }

  assert(false);
  return 0;
}
