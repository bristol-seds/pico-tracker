/**
 * SAM D20/D21/R21 SERCOM USART Driver
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

#include "sercom/sercom.h"
#include "sercom/usart.h"
#include "system/pinmux.h"
#include "system/interrupt.h"

/**
 * Initializes the USART device.
 *
 * \param[in]  hw      Pointer to USART hardware instance
 *
 * \return Status of the initialization
 *
 * \retval STATUS_OK                       The initialization was successful
 * \retval STATUS_BUSY                     The USART module is busy
 *                                         resetting
 * \retval STATUS_ERR_DENIED               The USART have not been disabled in
 *                                         advance of initialization
 * \retval STATUS_ERR_INVALID_ARG          The configuration struct contains
 *                                         invalid configuration
 * \retval STATUS_ERR_ALREADY_INITIALIZED  The SERCOM instance has already been
 *                                         initialized with different clock
 *                                         configuration
 * \retval STATUS_ERR_BAUD_UNAVAILABLE     The BAUD rate given by the
 *                                         configuration
 *                                         struct cannot be reached with
 *                                         the current clock configuration
 */
enum sercom_status_t usart_init(SercomUsart* const hw,
				enum usart_dataorder data_order,
				enum usart_transfer_mode transfer_mode,
				enum usart_parity parity,
				enum usart_stopbits stopbits,
				enum usart_character_size character_size,
				enum usart_signal_mux_settings mux_setting,
#ifdef FEATURE_USART_OVER_SAMPLE
				enum usart_sample_rate sample_rate,
				enum usart_sample_adjustment sample_adjustment,
#endif
				bool immediate_buffer_overflow_notification,
				bool encoding_format_enable,
				uint8_t receive_pulse_length,
				bool lin_slave_enable,
				bool start_frame_detection_enable,
				bool collision_detection_enable,
				uint32_t baudrate,
				bool receiver_enable,
				bool transmitter_enable,
				bool clock_polarity_inverted,
				bool use_external_clock,
				uint32_t ext_clock_freq,
				bool run_in_standby,
				enum gclk_generator generator_source,
				uint32_t pinmux_pad0,
				uint32_t pinmux_pad1,
				uint32_t pinmux_pad2,
				uint32_t pinmux_pad3)

{
  /* Sanity check arguments */
  assert(hw);

  enum sercom_status_t status_code = SERCOM_STATUS_OK;

  uint32_t sercom_index = _sercom_get_sercom_inst_index((Sercom*)hw);
  uint32_t pm_index     = sercom_index + PM_APBCMASK_SERCOM0_Pos;
  uint32_t gclk_index   = sercom_index + SERCOM0_GCLK_ID_CORE;

  /* Cache new register values to minimize the number of register writes */
  uint32_t ctrla = 0;
  uint32_t ctrlb = 0;
  uint16_t baud  = 0;

  enum sercom_asynchronous_operation_mode mode = SERCOM_ASYNC_OPERATION_MODE_ARITHMETIC;
  enum sercom_asynchronous_sample_num sample_num = SERCOM_ASYNC_SAMPLE_NUM_16;

  if (hw->CTRLA.reg & SERCOM_USART_CTRLA_SWRST) {
    /* The module is busy resetting itself */
    return SERCOM_STATUS_BUSY;
  }

  if (hw->CTRLA.reg & SERCOM_USART_CTRLA_ENABLE) {
    /* Check the module is enabled */
    return SERCOM_STATUS_DENIED;
  }

  /* Turn on module in PM */
  system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC, 1 << pm_index);

  /* Set up the GCLK for the module */
  system_gclk_chan_set_config(gclk_index, generator_source);
  system_gclk_chan_enable(gclk_index);
  _sercom_set_gclk_generator(generator_source);

#ifdef FEATURE_USART_OVER_SAMPLE
  switch (sample_rate) {
    case USART_SAMPLE_RATE_16X_ARITHMETIC:
      mode = SERCOM_ASYNC_OPERATION_MODE_ARITHMETIC;
      sample_num = SERCOM_ASYNC_SAMPLE_NUM_16;
      break;
    case USART_SAMPLE_RATE_8X_ARITHMETIC:
      mode = SERCOM_ASYNC_OPERATION_MODE_ARITHMETIC;
      sample_num = SERCOM_ASYNC_SAMPLE_NUM_8;
      break;
    case USART_SAMPLE_RATE_3X_ARITHMETIC:
      mode = SERCOM_ASYNC_OPERATION_MODE_ARITHMETIC;
      sample_num = SERCOM_ASYNC_SAMPLE_NUM_3;
      break;
    case USART_SAMPLE_RATE_16X_FRACTIONAL:
      mode = SERCOM_ASYNC_OPERATION_MODE_FRACTIONAL;
      sample_num = SERCOM_ASYNC_SAMPLE_NUM_16;
      break;
    case USART_SAMPLE_RATE_8X_FRACTIONAL:
      mode = SERCOM_ASYNC_OPERATION_MODE_FRACTIONAL;
      sample_num = SERCOM_ASYNC_SAMPLE_NUM_8;
      break;
  }
#endif

  /* Set data order, internal muxing, and clock polarity */
  ctrla =
    (uint32_t)data_order |
    (uint32_t)mux_setting |
#ifdef FEATURE_USART_OVER_SAMPLE
    sample_adjustment |
    sample_rate |
#endif
    (immediate_buffer_overflow_notification << SERCOM_USART_CTRLA_IBON_Pos) |
    (clock_polarity_inverted << SERCOM_USART_CTRLA_CPOL_Pos);

  /* Get baud value from mode and clock */
  switch (transfer_mode)
  {
    case USART_TRANSFER_SYNCHRONOUSLY:
      if (!use_external_clock) {
	status_code = _sercom_get_sync_baud_val(baudrate,
						system_gclk_chan_get_hz(gclk_index), &baud);
      }
      break;

    case USART_TRANSFER_ASYNCHRONOUSLY:
      if (use_external_clock) {
	status_code =
	  _sercom_get_async_baud_val(baudrate,
				     ext_clock_freq, &baud, mode, sample_num);
      } else {
	status_code =
	  _sercom_get_async_baud_val(baudrate,
				     system_gclk_chan_get_hz(gclk_index), &baud, mode, sample_num);
      }
      break;
  }

  /* Check if calculating the baud rate failed */
  if (status_code != SERCOM_STATUS_OK) {
    /* Abort */
    return SERCOM_STATUS_BAUDRATE_UNAVAILABLE;
  }

//  if(encoding_format_enable) {
//    hw->RXPL.reg = receive_pulse_length;
//  }

  /* Wait until synchronization is complete */
  USART_WAIT_FOR_SYNC(hw);

  /* Set baud val */
  hw->BAUD.reg = baud;

  /* Set sample mode */
  ctrla |= transfer_mode;

  if (use_external_clock == false) {
    ctrla |= SERCOM_USART_CTRLA_MODE_USART_INT_CLK;
  }
  else {
    ctrla |= SERCOM_USART_CTRLA_MODE_USART_EXT_CLK;
  }

  /* Set stopbits, character size and enable transceivers */
  ctrlb = (uint32_t)stopbits | (uint32_t)character_size |
//    (encoding_format_enable << SERCOM_USART_CTRLB_ENC_Pos) |
    (start_frame_detection_enable << SERCOM_USART_CTRLB_SFDE_Pos) |
//    (collision_detection_enable << SERCOM_USART_CTRLB_COLDEN_Pos) |
    (receiver_enable << SERCOM_USART_CTRLB_RXEN_Pos) |
    (transmitter_enable << SERCOM_USART_CTRLB_TXEN_Pos);

  /* Check parity mode bits */
  if (parity != USART_PARITY_NONE) {
    if(lin_slave_enable) {
      ctrla |= SERCOM_USART_CTRLA_FORM(0x5);
    }
    ctrla |= SERCOM_USART_CTRLA_FORM(1);
    ctrlb |= parity;
  } else {
    if(lin_slave_enable) {
      ctrla |= SERCOM_USART_CTRLA_FORM(0x4);
    }
    ctrla |= SERCOM_USART_CTRLA_FORM(0);
  }

  /* Set whether module should run in standby. */
  if (run_in_standby || system_is_debugger_present()) {
    ctrla |= SERCOM_USART_CTRLA_RUNSTDBY;
  }

  /* Wait until synchronization is complete */
  USART_WAIT_FOR_SYNC(hw);

  /* Write configuration to CTRLB */
  hw->CTRLB.reg = ctrlb;

  /* Wait until synchronization is complete */
  USART_WAIT_FOR_SYNC(hw);

  /* Write configuration to CTRLA */
  hw->CTRLA.reg = ctrla;

  uint32_t pad_pinmuxes[] = { pinmux_pad0, pinmux_pad1,
			      pinmux_pad2, pinmux_pad3 };

  /* Configure the SERCOM pins according to the user configuration */
  for (uint8_t pad = 0; pad < 4; pad++) {
    uint32_t current_pinmux = pad_pinmuxes[pad];

    if (current_pinmux == PINMUX_DEFAULT) {
      current_pinmux = _sercom_get_default_pad((Sercom* const)hw, pad);
    }

    if (current_pinmux != PINMUX_UNUSED) {
      system_pinmux_pin_set_config(current_pinmux >> 16,    	/* GPIO Pin   */
				   current_pinmux & 0xFFFF,	/* Mux Position */
				   SYSTEM_PINMUX_PIN_DIR_INPUT, /* Direction */
				   SYSTEM_PINMUX_PIN_PULL_NONE, /* Pull */
				   false);    			/* Powersave */
    }
  }

  return status_code;
}

/**
 * Initialize the USART device to predefined defaults:
 * - 8-bit asynchronous USART
 * - No parity
 * - 1 stop bit
 * - 9600 baud
 * - Transmitter enabled
 * - Receiver enabled
 * - GCLK generator 0 as clock source
 * - Default pin configuration
 */
enum sercom_status_t usart_init_default(SercomUsart* const hw)
{
  return usart_init(hw,
	     USART_DATAORDER_LSB, 		/** Bit order (MSB or LSB first) */
	     USART_TRANSFER_ASYNCHRONOUSLY,	/** Asynchronous or synchronous mode */
	     USART_PARITY_NONE,			/** USART parity */
	     USART_STOPBITS_1,			/** Number of stop bits */
	     USART_CHARACTER_SIZE_8BIT, 	/** USART character size */
	     USART_RX_1_TX_2_XCK_3,		/** USART pin out */
#ifdef FEATURE_USART_OVER_SAMPLE
	     USART_SAMPLE_RATE_16X_ARITHMETIC, 	/** USART sample rate */
	     USART_SAMPLE_ADJUSTMENT_7_8_9, 	/** USART sample adjustment */
#endif
	     false,	     /** Immediate buffer overflow notification */
	     false,	     /** Enable IrDA encoding format */
	     19,	     /** Minimum pulse length required for IrDA rx */
	     false,	     /** Enable LIN Slave Support */
	     false,	     /** Enable start of frame dection */
	     false,	     /** Enable collision dection */
	     9600,	     /** USART Baud rate */
	     true,	     /** Enable receiver */
	     true,	     /** Enable transmitter */
	     false,	     /** Sample on the rising edge of XLCK */
	     false,	     /** Use the external clock applied to the XCK pin. */
	     0,		     /** External clock frequency in synchronous mode. */
	     false,	     /** Run in standby */
	     GCLK_GENERATOR_0,		/** GCLK generator source */
	     PINMUX_DEFAULT, 		/** PAD0 pinmux */
	     PINMUX_DEFAULT,		/** PAD1 pinmux */
	     PINMUX_DEFAULT,		/** PAD2 pinmux */
	     PINMUX_DEFAULT);		/** PAD3 pinmux */
}



/**
 * This blocking function will transmit a single character via the
 * USART.
 *
 * \param[in]  module   Pointer to the software instance struct
 * \param[in]  tx_data  Data to transfer
 *
 * \return Status of the operation
 * \retval STATUS_OK         If the operation was completed
 * \retval STATUS_BUSY       If the operation was not completed, due to the USART
 *                           module being busy.
 * \retval STATUS_ERR_DENIED If the transmitter is not enabled
 */
enum sercom_status_t usart_write_wait(SercomUsart* const hw,
				  const uint16_t tx_data)
{
  /* Sanity check arguments */
  assert(hw);

  /* Check if USART is ready for new data */
  if (!(hw->INTFLAG.reg & SERCOM_USART_INTFLAG_DRE)) {
    /* Return error code */
    return SERCOM_STATUS_BUSY;
  }

  /* Wait until synchronization is complete */
  USART_WAIT_FOR_SYNC(hw);

  /* Write data to USART module */
  hw->DATA.reg = tx_data;

  while (!(hw->INTFLAG.reg & SERCOM_USART_INTFLAG_TXC)) {
    /* Wait until data is sent */
  }

  return SERCOM_STATUS_OK;
}

/**
 * Receive a character via the USART
 *
 * This blocking function will receive a character via the USART.
 *
 * \param[in]   module   Pointer to the software instance struct
 * \param[out]  rx_data  Pointer to received data
 *
 * \return Status of the operation
 * \retval STATUS_OK                If the operation was completed
 * \retval STATUS_BUSY              If the operation was not completed,
 *                                  due to the USART module being busy
 * \retval STATUS_ERR_BAD_FORMAT    If the operation was not completed,
 *                                  due to configuration mismatch between USART
 *                                  and the sender
 * \retval STATUS_ERR_BAD_OVERFLOW  If the operation was not completed,
 *                                  due to the baud rate being too low or the
 *                                  system frequency being too high
 * \retval STATUS_ERR_BAD_DATA      If the operation was not completed, due to
 *                                  data being corrupted
 * \retval STATUS_ERR_DENIED        If the receiver is not enabled
 */
enum sercom_status_t usart_read_wait(SercomUsart* const hw,
				 uint16_t *const rx_data)
{
  /* Sanity check arguments */
  assert(hw);

  /* Error variable */
  uint8_t error_code;

  /* If USART has no new data, abort */
  if (!(hw->INTFLAG.reg & SERCOM_USART_INTFLAG_RXC)) {
    /* Return error code */
    return SERCOM_STATUS_BUSY;
  }

  /* Wait until synchronization is complete */
  USART_WAIT_FOR_SYNC(hw);

  /* Read out the status code and mask away all but the 3 LSBs*/
  error_code = (uint8_t)(hw->STATUS.reg & SERCOM_USART_STATUS_MASK);

  /* Check if an error has occurred during the receiving */
  if (error_code) {
    /* Check which error occurred */
    if (error_code & SERCOM_USART_STATUS_FERR) {
      /* Clear flag by writing a 1 to it and
       * return with an error code */
      hw->STATUS.reg = SERCOM_USART_STATUS_FERR;

      return SERCOM_STATUS_BAD_FORMAT;
    } else if (error_code & SERCOM_USART_STATUS_BUFOVF) {
      /* Clear flag by writing a 1 to it and
       * return with an error code */
      hw->STATUS.reg = SERCOM_USART_STATUS_BUFOVF;

      return SERCOM_STATUS_OVERFLOW;
    } else if (error_code & SERCOM_USART_STATUS_PERR) {
      /* Clear flag by writing a 1 to it and
       * return with an error code */
      hw->STATUS.reg = SERCOM_USART_STATUS_PERR;

      return SERCOM_STATUS_BAD_DATA;
//    } else if (error_code & SERCOM_USART_STATUS_ISF) {
      /* Clear flag by writing 1 to it  and
       *  return with an error code */
//      hw->STATUS.reg |= SERCOM_USART_STATUS_ISF;

//      return STATUS_ERR_PROTOCOL;
//    } else if (error_code & SERCOM_USART_STATUS_COLL) {
      /* Clear flag by writing 1 to it
       *  return with an error code */
//      hw->STATUS.reg |= SERCOM_USART_STATUS_COLL;

//      return SERCOM_STATUS_PACKET_COLLISION;
    }
  }

  /* Read data from USART module */
  *rx_data = hw->DATA.reg;

  return SERCOM_STATUS_OK;
}

/**
 * Transmit a buffer of characters via the USART
 *
 * This blocking function will transmit a block of \c length characters
 * via the USART
 *
 * \note Using this function in combination with the interrupt (\c _job) functions is
 *       not recommended as it has no functionality to check if there is an
 *       ongoing interrupt driven operation running or not.
 *
 * \param[in]  module   Pointer to USART software instance struct
 * \param[in]  tx_data  Pointer to data to transmit
 * \param[in]  length   Number of characters to transmit
 *
 * \return Status of the operation
 * \retval STATUS_OK              If operation was completed
 * \retval STATUS_ERR_INVALID_ARG If operation was not completed, due to invalid
 *                                arguments
 * \retval STATUS_ERR_TIMEOUT     If operation was not completed, due to USART
 *                                module timing out
 * \retval STATUS_ERR_DENIED      If the transmitter is not enabled
 */
enum sercom_status_t usart_write_buffer_wait(SercomUsart* const hw,
					 const uint8_t *tx_data,
					 uint16_t length)
{
  /* Sanity check arguments */
  assert(hw);

  /* Wait until synchronization is complete */
  USART_WAIT_FOR_SYNC(hw);

  uint16_t tx_pos = 0;

  /* Blocks while buffer is being transferred */
  while (length--) {
    /* Wait for the USART to be ready for new data and abort
     * operation if it doesn't get ready within the timeout*/
    for (uint32_t i = 0; i <= USART_TIMEOUT; i++) {
      if (hw->INTFLAG.reg & SERCOM_USART_INTFLAG_DRE) {
	break;
      } else if (i == USART_TIMEOUT) {
	return SERCOM_STATUS_TIMEOUT;
      }
    }

    /* Data to send is at least 8 bits long */
    uint16_t data_to_send = tx_data[tx_pos++];

    /* Check if the character size exceeds 8 bit */
//    if (module->character_size == USART_CHARACTER_SIZE_9BIT) {
//      data_to_send |= (tx_data[tx_pos++] << 8);
//    }

    /* Send the data through the USART module */
    usart_write_wait(hw, data_to_send);
  }

  /* Wait until Transmit is complete or timeout */
  for (uint32_t i = 0; i <= USART_TIMEOUT; i++) {
    if (hw->INTFLAG.reg & SERCOM_USART_INTFLAG_TXC) {
      break;
    } else if (i == USART_TIMEOUT) {
      return SERCOM_STATUS_TIMEOUT;
    }
  }

  return SERCOM_STATUS_OK;
}

/**
 * Receive a buffer of \c length characters via the USART
 *
 * This blocking function will receive a block of \c length characters
 * via the USART.
 *
 * \note Using this function in combination with the interrupt (\c *_job)
 *       functions is not recommended as it has no functionality to check if
 *       there is an ongoing interrupt driven operation running or not.
 *
 * \param[in]  module   Pointer to USART software instance struct
 * \param[out] rx_data  Pointer to receive buffer
 * \param[in]  length   Number of characters to receive
 *
 * \return Status of the operation.
 * \retval STATUS_OK                If operation was completed
 * \retval STATUS_ERR_INVALID_ARG   If operation was not completed, due to an
 *                                  invalid argument being supplied
 * \retval STATUS_ERR_TIMEOUT       If operation was not completed, due
 *                                  to USART module timing out
 * \retval STATUS_ERR_BAD_FORMAT    If the operation was not completed,
 *                                  due to a configuration mismatch
 *                                  between USART and the sender
 * \retval STATUS_ERR_BAD_OVERFLOW  If the operation was not completed,
 *                                  due to the baud rate being too low or the
 *                                  system frequency being too high
 * \retval STATUS_ERR_BAD_DATA      If the operation was not completed, due
 *                                  to data being corrupted
 * \retval STATUS_ERR_DENIED        If the receiver is not enabled
 */
enum sercom_status_t usart_read_buffer_wait(SercomUsart* hw,
					uint8_t *rx_data,
					uint16_t length)
{
  /* Sanity check arguments */
  assert(hw);

  uint16_t rx_pos = 0;

  /* Blocks while buffer is being received */
  while (length--) {
    /* Wait for the USART to have new data and abort operation if it
     * doesn't get ready within the timeout*/
    for (uint32_t i = 0; i <= USART_TIMEOUT; i++) {
      if (hw->INTFLAG.reg & SERCOM_USART_INTFLAG_RXC) {
	break;
      } else if (i == USART_TIMEOUT) {
	return SERCOM_STATUS_TIMEOUT;
      }
    }

    enum sercom_status_t retval;
    uint16_t received_data = 0;

    retval = usart_read_wait(hw, &received_data);

    if (retval != SERCOM_STATUS_OK) {
      /* Overflow, abort */
      return retval;
    }

    /* Read value will be at least 8-bits long */
    rx_data[rx_pos++] = received_data;

    /* If 9-bit data, write next received byte to the buffer */
//    if (module->character_size == USART_CHARACTER_SIZE_9BIT) {
//      rx_data[rx_pos++] = (received_data >> 8);
//    }
  }

  return SERCOM_STATUS_OK;
}

/**
 * -------------------------------- Interrupts -------------------------------
 */

/**
 * Rx Callbacks for all instances
 */
static usart_rx_callback_t _usart_rx_callbacks[SERCOM_INST_NUM] = {
#if SERCOM_INST_NUM > 0
  NULL,
#endif
#if SERCOM_INST_NUM > 1
  NULL,
#endif
#if SERCOM_INST_NUM > 2
  NULL,
#endif
#if SERCOM_INST_NUM > 3
  NULL,
#endif
#if SERCOM_INST_NUM > 4
  NULL,
#endif
#if SERCOM_INST_NUM > 5
  NULL,
#endif
};

/**
 * Handles interrupts as they occur, and it will run registered
 * callback functions
 */
void _usart_interrupt_handler(Sercom* const sercom_instance, uint8_t instance_index)
{
  /* Hardware instance */
  SercomUsart* const hw = (SercomUsart*)sercom_instance;

  /* Temporary variables */
  uint16_t interrupt_status;
  uint8_t error_code;

  /* Wait for the synchronization to complete */
  USART_WAIT_FOR_SYNC(hw);

  /* Read and mask interrupt flag register */
  interrupt_status = hw->INTFLAG.reg;
  interrupt_status &= hw->INTENSET.reg;

  /* Check if a DATA READY interrupt has occurred,
   * and if there is more to transfer */
  if (interrupt_status & SERCOM_USART_INTFLAG_DRE) {

    /* if (module->remaining_tx_buffer_length) { */
    /*   /\* Write value will be at least 8-bits long *\/ */
    /*   uint16_t data_to_send = *(module->tx_buffer_ptr); */
    /*   /\* Increment 8-bit pointer *\/ */
    /*   (module->tx_buffer_ptr)++; */

    /*   if (module->character_size == USART_CHARACTER_SIZE_9BIT) { */
    /* 	data_to_send |= (*(module->tx_buffer_ptr) << 8); */
    /* 	/\* Increment 8-bit pointer *\/ */
    /* 	(module->tx_buffer_ptr)++; */
    /*   } */
    /*   /\* Write the data to send *\/ */
    /*   hw->DATA.reg = (data_to_send & SERCOM_USART_DATA_MASK); */

    /*   if (--(module->remaining_tx_buffer_length) == 0) { */
    /* 	/\* Disable the Data Register Empty Interrupt *\/ */
    /* 	hw->INTENCLR.reg = SERCOM_USART_INTFLAG_DRE; */
    /* 	/\* Enable Transmission Complete interrupt *\/ */
    /* 	hw->INTENSET.reg = SERCOM_USART_INTFLAG_TXC; */

    /* TODO: Turn off data ready interrupt */
    hw->INTENCLR.reg = SERCOM_USART_INTFLAG_DRE;
  }

  /* Check if the Transmission Complete interrupt has occurred and
   * that the transmit buffer is empty */
  if (interrupt_status & SERCOM_USART_INTFLAG_TXC) {

    /* Disable TX Complete Interrupt, and set STATUS_OK */
    hw->INTENCLR.reg = SERCOM_USART_INTFLAG_TXC;

    /* TODO: Callback */
  }

  /* Check if the Receive Complete interrupt has occurred, and that
   * there's more data to receive */
  while ((hw->INTFLAG.reg & hw->INTENSET.reg) & SERCOM_USART_INTFLAG_RXC) {

    /* Read out the status code and mask away all but the 4 LSBs */
    error_code = (uint8_t)(hw->STATUS.reg & SERCOM_USART_STATUS_MASK);
#ifndef SAMD20
    /* CTS status should not be considered as an error */
    if(error_code & SERCOM_USART_STATUS_CTS) {
      error_code &= ~SERCOM_USART_STATUS_CTS;
    }
#endif
    /* Check if an error has occurred during the receiving */
    if (error_code) {
      /* Check which error occurred */
      if (error_code & SERCOM_USART_STATUS_FERR) {
	/* Store the error code and clear flag by writing 1 to it */
	//module->rx_status = STATUS_ERR_BAD_FORMAT;
	hw->STATUS.reg |= SERCOM_USART_STATUS_FERR;
      } else if (error_code & SERCOM_USART_STATUS_BUFOVF) {
	/* Store the error code and clear flag by writing 1 to it */
	//module->rx_status = STATUS_ERR_OVERFLOW;
	hw->STATUS.reg |= SERCOM_USART_STATUS_BUFOVF;
      } else if (error_code & SERCOM_USART_STATUS_PERR) {
	/* Store the error code and clear flag by writing 1 to it */
	//module->rx_status = STATUS_ERR_BAD_DATA;
	hw->STATUS.reg |= SERCOM_USART_STATUS_PERR;
      }
#ifdef FEATURE_USART_LIN_SLAVE
      else if (error_code & SERCOM_USART_STATUS_ISF) {
	  /* Store the error code and clear flag by writing 1 to it */
	  //module->rx_status = STATUS_ERR_PROTOCOL;
	hw->STATUS.reg |= SERCOM_USART_STATUS_ISF;
      }
#endif
#ifdef FEATURE_USART_COLLISION_DECTION
      else if (error_code & SERCOM_USART_STATUS_COLL) {
	/* Store the error code and clear flag by writing 1 to it */
	// module->rx_status = STATUS_ERR_PACKET_COLLISION;
	hw->STATUS.reg |= SERCOM_USART_STATUS_COLL;
      }
#endif

      /* TODO: Error callback */

    } else { /* No Error code occoured */

      /* Read current packet from DATA register,
       * increment buffer pointer and decrement buffer length */
      uint16_t received_data = (hw->DATA.reg & SERCOM_USART_DATA_MASK);

      /* Rx Callback */
      if (_usart_rx_callbacks[instance_index]) {
	_usart_rx_callbacks[instance_index](hw, received_data);
      }

      /* TODO: Disable this Rx interrupt at some point */
      //hw->INTENCLR.reg = SERCOM_USART_INTFLAG_RXC;
    }
  }

#ifdef FEATURE_USART_HARDWARE_FLOW_CONTROL
  if (interrupt_status & SERCOM_USART_INTFLAG_CTSIC) {
    /* Disable interrupts */
    hw->INTENCLR.reg = SERCOM_USART_INTENCLR_CTSIC;
    /* Clear interrupt flag */
    hw->INTFLAG.reg = SERCOM_USART_INTFLAG_CTSIC;

    /* TODO: Callback here */
  }
#endif

#ifdef FEATURE_USART_LIN_SLAVE
  if (interrupt_status & SERCOM_USART_INTFLAG_RXBRK) {
    /* Disable interrupts */
    hw->INTENCLR.reg = SERCOM_USART_INTENCLR_RXBRK;
    /* Clear interrupt flag */
    hw->INTFLAG.reg = SERCOM_USART_INTFLAG_RXBRK;

    /* TODO: Callback here */
  }
#endif

#ifdef FEATURE_USART_START_FRAME_DECTION
  if (interrupt_status & SERCOM_USART_INTFLAG_RXS) {
    /* Disable interrupts */
    hw->INTENCLR.reg = SERCOM_USART_INTENCLR_RXS;
    /* Clear interrupt flag */
    hw->INTFLAG.reg = SERCOM_USART_INTFLAG_RXS;

    /* TODO: Callback here */
  }
#endif
}

/**
 * Register a callback for received bytes
 */
void usart_register_rx_callback(SercomUsart* const hw,
				usart_rx_callback_t callback,
				uint32_t priority)
{
  /* Get instance index */
  uint8_t instance_index = _sercom_get_sercom_inst_index((Sercom*)hw);

  /* Enable the Rx interrupt */
  hw->INTENSET.reg = SERCOM_USART_INTFLAG_RXC;

  /* Set the rx handler */
  _usart_rx_callbacks[instance_index] = callback;

  /* Set our out interrupt handler with the main sercom module */
  _sercom_set_handler((Sercom*)hw, _usart_interrupt_handler);

  /* And enable this interrupt in the NVIC */
  irq_register_handler(SERCOM0_IRQn + instance_index, priority);
}
