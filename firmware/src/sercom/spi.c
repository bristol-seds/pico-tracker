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

#include "sercom/sercom.h"
#include "sercom/spi.h"
#include "samd20.h"

#define SPI_WAIT_FOR_SYNC(hw)	while(hw->STATUS.reg & SERCOM_SPI_STATUS_SYNCBUSY)
#define SPI_MODE_MASTER(hw)	((hw->CTRLA.reg & SERCOM_SPI_CTRLA_MODE_Msk) \
				 == SERCOM_SPI_CTRLA_MODE_SPI_MASTER)
#define SPI_MODE_SLAVE(hw)	((hw->CTRLA.reg & SERCOM_SPI_CTRLA_MODE_Msk) \
				 == SERCOM_SPI_CTRLA_MODE_SPI_SLAVE)
/**
 * Resets the SPI module
 *
 * This function will reset the SPI module to its power on default values and
 * disable it.
 *
 * \param[in,out] module Pointer to the software instance struct
 */
void spi_reset(SercomSpi* const hw)
{
  /* Sanity check arguments */
  assert(hw);

  /* Disable the module */
  spi_disable(hw);

  USART_WAIT_FOR_SYNC();

  /* Software reset the module */
  hw->CTRLA.reg |= SERCOM_SPI_CTRLA_SWRST;
}

/**
 * Set the baudrate of the SPI module
 *
 * This function will set the baudrate of the SPI module.
 *
 * \param[in]  module  Pointer to the software instance struct
 * \param[in]  baudrate  The baudrate wanted
 *
 * \return The status of the configuration
 * \retval STATUS_ERR_INVALID_ARG  If invalid argument(s) were provided.
 * \retval STATUS_OK               If the configuration was written
 */
enum sercom_status_t spi_set_baudrate(SercomSpi* const hw,
				      uint32_t baudrate)
{
  /* Sanity check arguments */
  assert(hw);

  /* Value to write to BAUD register */
  uint16_t baud = 0;

  /* Disable the module */
  spi_disable(hw);

  SPI_WAIT_FOR_SYNC(hw);

  /* Find frequency of the internal SERCOMi_GCLK_ID_CORE */
  uint32_t sercom_index = _sercom_get_sercom_inst_index((Sercom*)hw);
  uint32_t gclk_index   = sercom_index + SERCOM0_GCLK_ID_CORE;
  uint32_t internal_clock = system_gclk_chan_get_hz(gclk_index);

  /* Get baud value, based on baudrate and the internal clock frequency */
  enum sercom_status_t error_code = _sercom_get_sync_baud_val(
    baudrate, internal_clock, &baud);

  if (error_code != SERCOM_STATUS_OK) {
    /* Baud rate calculation error, return status code */
    return SERCOM_STATUS_INVALID_ARG;
  }

  hw->BAUD.reg = (uint8_t)baud;

  SPI_WAIT_FOR_SYNC(hw);

  /* Enable the module */
  spi_enable(hw);

  SPI_WAIT_FOR_SYNC(hw);

  return SERCOM_STATUS_OK;
}

/**
 * Clears the Transmit Complete interrupt flag.
 *
 * \param[in]  module  Pointer to the software instance struct
 */
static void _spi_clear_tx_complete_flag(SercomSpi* const hw)
{
  /* Sanity check arguments */
  assert(hw);

  /* Clear interrupt flag */
  hw->INTFLAG.reg = SPI_INTERRUPT_FLAG_TX_COMPLETE;
}

/**
 * This function will initialize the SERCOM SPI module
 *
 * \param[out]  module  Pointer to the software instance struct
 * \param[in]   hw      Pointer to hardware instance
 * \param[in]   config  Pointer to the config struct
 *
 * \return Status of the initialization
 * \retval STATUS_OK               Module initiated correctly.
 * \retval STATUS_ERR_DENIED       If module is enabled.
 * \retval STATUS_BUSY             If module is busy resetting.
 * \retval STATUS_ERR_INVALID_ARG  If invalid argument(s) were provided.
 */
enum sercom_status_t spi_init(SercomSpi *const hw,
			      enum spi_mode mode,
			      enum spi_data_order data_order,
			      enum spi_transfer_mode transfer_mode,
			      enum spi_signal_mux_setting mux_setting,
			      enum spi_character_size character_size,
			      bool run_in_standby,
			      bool receiver_enable,
			      uint32_t master_baudrate,
			      enum spi_frame_format slave_frame_format,
			      enum spi_addr_mode slave_address_mode,
			      uint8_t slave_address,
			      uint8_t slave_address_mask,
			      bool slave_preload_enable,
#  ifdef FEATURE_SPI_SLAVE_SELECT_LOW_DETECT
			      bool select_slave_low_detect_enable,
#  endif
#  ifdef FEATURE_SPI_HARDWARE_SLAVE_SELECT
			      bool master_slave_select_enable,
#  endif
			      enum gclk_generator generator_source,
			      uint32_t pinmux_pad0,
			      uint32_t pinmux_pad1,
			      uint32_t pinmux_pad2,
			      uint32_t pinmux_pad3)
{

  /* Sanity check arguments */
  assert(hw);

  /* Check if module is enabled. */
  if (hw->CTRLA.reg & SERCOM_SPI_CTRLA_ENABLE) {
    return SERCOM_STATUS_DENIED;
  }

  /* Check if reset is in progress. */
  if (hw->CTRLA.reg & SERCOM_SPI_CTRLA_SWRST){
    return SERCOM_STATUS_BUSY;
  }

  uint32_t sercom_index = _sercom_get_sercom_inst_index((Sercom*)hw);
  uint32_t pm_index     = sercom_index + PM_APBCMASK_SERCOM0_Pos;
  uint32_t gclk_index   = sercom_index + SERCOM0_GCLK_ID_CORE;

  /* Turn on module in PM */
  system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC, 1 << pm_index);

  /* Set up the GCLK for the module */
  system_gclk_chan_set_config(gclk_index, generator_source);
  system_gclk_chan_enable(gclk_index);
  sercom_set_gclk_generator(generator_source);

  if (mode == SPI_MODE_MASTER) {
    /* Set the SERCOM in SPI master mode */
    hw->CTRLA.reg |= SERCOM_SPI_CTRLA_MODE_SPI_MASTER;
  } else if (mode == SPI_MODE_SLAVE) {
    /* Set the SERCOM in SPI slave mode */
    hw->CTRLA.reg |= SERCOM_SPI_CTRLA_MODE_SPI_SLAVE;
  }

  uint32_t pad_pinmuxes[] = {
    pinmux_pad0, pinmux_pad1,
    pinmux_pad2, pinmux_pad3
  };

  /* Configure the SERCOM pins according to the user configuration */
  for (uint8_t pad = 0; pad < 4; pad++) {
    uint32_t current_pinmux = pad_pinmuxes[pad];

    if (current_pinmux == PINMUX_DEFAULT) {
      current_pinmux = _sercom_get_default_pad((Sercom*)hw, pad);
    }

    if (current_pinmux != PINMUX_UNUSED) {
      system_pinmux_pin_set_config(current_pinmux >> 16,
				   current_pinmux & 0xFFFF,
				   SYSTEM_PINMUX_PIN_DIR_INPUT,	/* Direction */
				   SYSTEM_PINMUX_PIN_PULL_UP, /* Pull */
				   false); /* Powersave */
    }
  }

  /* Value to write to BAUD register */
  uint16_t baud = 0;
  /* Value to write to CTRLA register */
  uint32_t ctrla = 0;
  /* Value to write to CTRLB register */
  uint32_t ctrlb = 0;

  /* Find baud value and write it */
  if (mode == SPI_MODE_MASTER) {
    /* Find frequency of the internal SERCOMi_GCLK_ID_CORE */
    uint32_t sercom_index = _sercom_get_sercom_inst_index((Sercom*)hw);
    uint32_t gclk_index   = sercom_index + SERCOM0_GCLK_ID_CORE;
    uint32_t internal_clock = system_gclk_chan_get_hz(gclk_index);

    /* Get baud value, based on baudrate and the internal clock frequency */
    enum sercom_status_t error_code = _sercom_get_sync_baud_val(
      master_baudrate, internal_clock, &baud);

    if (error_code != SERCOM_STATUS_OK) {
      /* Baud rate calculation error, return status code */
      return SERCOM_STATUS_INVALID_ARG;
    }

    hw->BAUD.reg = (uint8_t)baud;
  } else if (mode == SPI_MODE_SLAVE) {
    /* Set frame format */
    ctrla = slave_frame_format;

    /* Set address mode */
    ctrlb = slave_address_mode;

    /* Set address and address mask*/
    hw->ADDR.reg |=
      (slave_address      << SERCOM_SPI_ADDR_ADDR_Pos) |
      (slave_address_mask << SERCOM_SPI_ADDR_ADDRMASK_Pos);

    if (slave_preload_enable) {
      /* Enable pre-loading of shift register */
      ctrlb |= SERCOM_SPI_CTRLB_PLOADEN;
    }
  }

  /* Set data order */
  ctrla |= data_order;

  /* Set clock polarity and clock phase */
  ctrla |= transfer_mode;

  /* Set mux setting */
  ctrla |= mux_setting;

  /* Set SPI character size */
  ctrlb |= character_size;

  /* Set whether module should run in standby. */
  if (run_in_standby || system_is_debugger_present()) {
    ctrla |= SERCOM_SPI_CTRLA_RUNSTDBY;
  }

  if (receiver_enable) {
    /* Enable receiver */
    ctrlb |= SERCOM_SPI_CTRLB_RXEN;
  }
#  ifdef FEATURE_SPI_SLAVE_SELECT_LOW_DETECT
  if (select_slave_low_detect_enable) {
    /* Enable Slave Select Low Detect */
    ctrlb |= SERCOM_SPI_CTRLB_SSDE;
  }
#  endif
#  ifdef FEATURE_SPI_HARDWARE_SLAVE_SELECT
  if (master_slave_select_enable) {
    /* Enable Master Slave Select */
    ctrlb |= SERCOM_SPI_CTRLB_MSSEN;
  }
#  endif
  /* Write CTRLA register */
  hw->CTRLA.reg |= ctrla;

  /* Write CTRLB register */
  hw->CTRLB.reg |= ctrlb;

  return SERCOM_STATUS_OK;
}

/**
 * This function will initialize the SERCOM SPI module with the
 * default values.
 *
 *
 * The default configuration is as follows:
 *  - Master mode enabled
 *  - MSB of the data is transmitted first
 *  - Transfer mode 0
 *  - MUX Setting D
 *  - Character size 8 bit
 *  - Not enabled in sleep mode
 *  - Receiver enabled
 *  - Baudrate 100000
 *  - Default pinmux settings for all pads
 *  - GCLK generator 0
 *
 */
enum sercom_status_t spi_init_default(SercomSpi *const hw)
{
  return spi_init(hw,
		  SPI_MODE_MASTER,		/** SPI mode */
		  SPI_DATA_ORDER_MSB,		/** Data order */
		  SPI_TRANSFER_MODE_0,		/** Transfer mode */
		  SPI_SIGNAL_MUX_SETTING_D,	/** Mux setting */
		  SPI_CHARACTER_SIZE_8BIT,	/** SPI character size */
		  false,			/** Enabled in sleep */
		  true,				/** Enable receiver */
		  100000,			/** Master - Baud rate */
		  0,				/** Slave - Frame format */
		  0,				/** Slave - Address mode */
		  0,				/** Slave - Address */
		  0,				/** Slave - Address mask */
		  false,			/** Slave - Preload data  */
#  ifdef FEATURE_SPI_SLAVE_SELECT_LOW_DETECT
		  true,				/** Enable Slave Select Low Detect */
#  endif
#  ifdef FEATURE_SPI_HARDWARE_SLAVE_SELECT
		  false,			/** Enable Master Slave Select */
#  endif
		  GCLK_GENERATOR_0,		/** GCLK generator to use */
		  PINMUX_DEFAULT,		/** PAD0 pinmux */
		  PINMUX_DEFAULT,		/** PAD1 pinmux */
		  PINMUX_DEFAULT,		/** PAD2 pinmux */
		  PINMUX_DEFAULT);		/** PAD3 pinmux */
}

/**
 * Reads buffer of \c length SPI characters
 *
 * This function will read a buffer of data from an SPI peripheral by sending
 * dummy SPI character if in master mode, or by waiting for data in slave mode.
 *
 * \note If address matching is enabled for the slave, the first character
 *       received and placed in the buffer will be the address.
 *
 * \param[in]  module   Pointer to the software instance struct
 * \param[out] rx_data  Data buffer for received data
 * \param[in]  length   Length of data to receive
 * \param[in]  dummy    8- or 9-bit dummy byte to shift out in master mode
 *
 * \return Status of the read operation
 * \retval STATUS_OK              If the read was completed
 * \retval STATUS_ABORTED          If transaction was ended by master before
 *                                 entire buffer was transferred
 * \retval STATUS_ERR_INVALID_ARG If invalid argument(s) were provided.
 * \retval STATUS_ERR_TIMEOUT     If the operation was not completed within the
 *                                timeout in slave mode.
 * \retval STATUS_ERR_DENIED      If the receiver is not enabled
 * \retval STATUS_ERR_OVERFLOW    If the data is overflown
 */
enum sercom_status_t spi_read_buffer_wait(SercomSpi* const hw,
				      uint8_t *rx_data,
				      uint16_t length,
				      uint16_t dummy)
{
  /* Sanity check arguments */
  assert(hw);

  /* Sanity check arguments */
  if (length == 0) {
    return SERCOM_STATUS_INVALID_ARG;
  }

//  if ((hw->mode == SPI_MODE_SLAVE) && (spi_is_write_complete(hw))) {
    /* Clear TX complete flag */
//    _spi_clear_tx_complete_flag(hw);
//  }

  uint16_t rx_pos = 0;

  while (length--) {
    if (SPI_MODE_MASTER(hw)) {
      /* Wait until the module is ready to write a character */
      while (!spi_is_ready_to_write(hw)) {
      }

      /* Send dummy SPI character to read in master mode */
      spi_write(hw, dummy);
    } else if (SPI_MODE_SLAVE(hw)) {

      /* Start timeout period for slave */
      for (uint32_t i = 0; i <= SPI_TIMEOUT; i++) {
	if (spi_is_ready_to_read(hw)) {
	  break;
	}
      }
      /* Check if master has ended the transaction */
      if (spi_is_write_complete(hw)) {
	_spi_clear_tx_complete_flag(hw);
	return SERCOM_STATUS_ABORTED;
      }

      if (!spi_is_ready_to_read(hw)) {
	/* Not ready to read data within timeout period */
	return SERCOM_STATUS_TIMEOUT;
      }
    } else {
      //ERROR
    }

    /* Wait until the module is ready to read a character */
    while (!spi_is_ready_to_read(hw)) {
    }

    uint16_t received_data = 0;
    enum sercom_status_t retval = spi_read(hw, &received_data);

    if (retval != SERCOM_STATUS_OK) {
      /* Overflow, abort */
      return retval;
    }

    /* Read value will be at least 8-bits long */
    rx_data[rx_pos++] = received_data;

    /* If 9-bit data, write next received byte to the buffer */
//    if (hw->character_size == SPI_CHARACTER_SIZE_9BIT) {
//      rx_data[rx_pos++] = (received_data >> 8);
//    }
  }

  return SERCOM_STATUS_OK;
}

/**
 * Sends and reads a single SPI character
 *
 * This function will transfer a single SPI character via SPI and return the
 * SPI character that is shifted into the shift register.
 *
 * In master mode the SPI character will be sent immediately and the received
 * SPI character will be read as soon as the shifting of the data is
 * complete.
 *
 * In slave mode this function will place the data to be sent into the transmit
 * buffer. It will then block until an SPI master has shifted a complete
 * SPI character, and the received data is available.
 *
 * \note The data to be sent might not be sent before the next transfer, as
 *       loading of the shift register is dependent on SCK.
 * \note If address matching is enabled for the slave, the first character
 *       received and placed in the buffer will be the address.
 *
 * \param[in]  module   Pointer to the software instance struct
 * \param[in]  tx_data  SPI character to transmit
 * \param[out] rx_data  Pointer to store the received SPI character
 *
 * \return Status of the operation.
 * \retval STATUS_OK            If the operation was completed
 * \retval STATUS_ERR_TIMEOUT   If the operation was not completed within the
 *                              timeout in slave mode
 * \retval STATUS_ERR_DENIED    If the receiver is not enabled
 * \retval STATUS_ERR_OVERFLOW  If the incoming data is overflown
 */
enum sercom_status_t spi_transceive_wait(SercomSpi* const hw,
				     uint16_t tx_data,
				     uint16_t *rx_data)
{
  /* Sanity check arguments */
  assert(hw);

  uint16_t j;
  enum sercom_status_t retval = SERCOM_STATUS_OK;

  /* Start timeout period for slave */
  if (SPI_MODE_SLAVE(hw)) {
    for (j = 0; j <= SPI_TIMEOUT; j++) {
      if (spi_is_ready_to_write(hw)) {
	break;
      } else if (j == SPI_TIMEOUT) {
	/* Not ready to write data within timeout period */
	return SERCOM_STATUS_TIMEOUT;
      }
    }
  }

  /* Wait until the module is ready to write the character */
  while (!spi_is_ready_to_write(hw)) {
  }

  /* Write data */
  spi_write(hw, tx_data);

  /* Start timeout period for slave */
  if (SPI_MODE_SLAVE(hw)) {
    for (j = 0; j <= SPI_TIMEOUT; j++) {
      if (spi_is_ready_to_read(hw)) {
	break;
      } else if (j == SPI_TIMEOUT) {
	/* Not ready to read data within timeout period */
	return SERCOM_STATUS_TIMEOUT;
      }
    }
  }

  /* Wait until the module is ready to read the character */
  while (!spi_is_ready_to_read(hw)) {
  }

  /* Read data */
  retval = spi_read(hw, rx_data);

  return retval;
}

/**
 * Selects slave device
 *
 * This function will drive the slave select pin of the selected device low or
 * high depending on the select boolean.
 * If slave address recognition is enabled, the address will be sent to the
 * slave when selecting it.
 *
 * \param[in] module  Pointer to the software module struct
 * \param[in] slave   Pointer to the attached slave
 * \param[in] select  Boolean stating if the slave should be selected or
 *                    deselected
 *
 * \return Status of the operation
 * \retval STATUS_OK                   If the slave device was selected
 * \retval STATUS_ERR_UNSUPPORTED_DEV  If the SPI module is operating in slave
 *                                     mode
 * \retval STATUS_BUSY                 If the SPI module is not ready to write
 *                                     the slave address
 */
enum sercom_status_t spi_select_slave(SercomSpi* const hw,
				      uint8_t ss_pin,
				      bool address_enabled,
				      uint8_t address,
				      const bool select)
{
  /* Sanity check arguments */
  assert(hw);

  /* Check that the SPI module is operating in master mode */
  if (!SPI_MODE_MASTER(hw)) {
    return SERCOM_STATUS_UNSUPPORTED_DEV;
  }
#  ifdef FEATURE_SPI_HARDWARE_SLAVE_SELECT
  if(!(master_slave_select_enable))
#  endif
  {
    if (select) {
      /* Check if address recognition is enabled */
      if (address_enabled) {
	/* Check if the module is ready to write the address */
	if (!spi_is_ready_to_write(hw)) {
	  /* Not ready, do not select slave and return */
	  port_pin_set_output_level(ss_pin, true);
	  return SERCOM_STATUS_BUSY;
	}

	/* Drive Slave Select low */
	port_pin_set_output_level(ss_pin, false);

	/* Write address to slave */
	spi_write(hw, address);

//	if (!(hw->receiver_enabled)) {
	  /* Flush contents of shift register shifted back from slave */
//	  while (!spi_is_ready_to_read(hw)) {
//	  }
//	  uint16_t flush = 0;
//	  spi_read(hw, &flush);
//	}
      } else {
	/* Drive Slave Select low */
	port_pin_set_output_level(ss_pin, false);
      }
    } else {
      /* Drive Slave Select high */
      port_pin_set_output_level(ss_pin, true);
    }
  }
  return SERCOM_STATUS_OK;
}

/**
 * Sends a buffer of \c length SPI characters
 *
 * This function will send a buffer of SPI characters via the SPI
 * and discard any data that is received. To both send and receive a buffer of
 * data, use the \ref spi_transceive_buffer_wait function.
 *
 * Note that this function does not handle the _SS (slave select) pin(s) in
 * master mode; this must be handled by the user application.
 *
 * \param[in] module   Pointer to the software instance struct
 * \param[in] tx_data  Pointer to the buffer to transmit
 * \param[in] length   Number of SPI characters to transfer
 *
 * \return Status of the write operation
 * \retval STATUS_OK               If the write was completed
 * \retval STATUS_ABORTED          If transaction was ended by master before
 *                                 entire buffer was transferred
 * \retval STATUS_ERR_INVALID_ARG  If invalid argument(s) were provided
 * \retval STATUS_ERR_TIMEOUT      If the operation was not completed within the
 *                                 timeout in slave mode
 */
enum sercom_status_t spi_write_buffer_wait(SercomSpi* const hw,
				       const uint8_t *tx_data,
				       uint16_t length)
{
  /* Sanity check arguments */
  assert(hw);

  if (length == 0) {
    return SERCOM_STATUS_INVALID_ARG;
  }

  if (SPI_MODE_SLAVE(hw) && (spi_is_write_complete(hw))) {
    /* Clear TX complete flag */
    _spi_clear_tx_complete_flag(hw);
  }

  uint16_t tx_pos = 0;
  uint16_t flush_length = length;

  /* Write block */
  while (length--) {
    /* Start timeout period for slave */
    if (SPI_MODE_SLAVE(hw)) {
      for (uint32_t i = 0; i <= SPI_TIMEOUT; i++) {
	if (spi_is_ready_to_write(hw)) {
	  break;
	}
      }
      /* Check if master has ended the transaction */
      if (spi_is_write_complete(hw)) {
	_spi_clear_tx_complete_flag(hw);
	return SERCOM_STATUS_ABORTED;
      }

      if (!spi_is_ready_to_write(hw)) {
	/* Not ready to write data within timeout period */
	return SERCOM_STATUS_TIMEOUT;
      }
    }

    /* Wait until the module is ready to write a character */
    while (!spi_is_ready_to_write(hw)) {
    }

    /* Write value will be at least 8-bits long */
    uint16_t data_to_send = tx_data[tx_pos++];

    /* If 9-bit data, get next byte to send from the buffer */
//    if (hw->character_size == SPI_CHARACTER_SIZE_9BIT) {
//      data_to_send |= (tx_data[tx_pos++] << 8);
//    }

    /* Write the data to send */
    spi_write(hw, data_to_send);

//    if (hw->receiver_enabled) {
      /* Start timeout period for slave */
      if (SPI_MODE_SLAVE(hw)) {
	for (uint32_t i = 0; i <= SPI_TIMEOUT; i++) {
	  if (spi_is_ready_to_write(hw)) {
	    data_to_send = tx_data[tx_pos++];
	    /* If 9-bit data, get next byte to send from the buffer */
//	    if (module->character_size == SPI_CHARACTER_SIZE_9BIT) {
//	      data_to_send |= (tx_data[tx_pos++] << 8);
//	    }

	    /* Write the data to send */
	    spi_write(hw, data_to_send);
	    length--;
	  }
	  if (spi_is_ready_to_read(hw)) {
	    break;
	  }
	}

	/* Check if master has ended the transaction */
	if (spi_is_write_complete(hw)) {
	  _spi_clear_tx_complete_flag(hw);
	  return SERCOM_STATUS_ABORTED;
	}

	if (!spi_is_ready_to_read(hw)) {
	  /* Not ready to read data within timeout period */
	  return SERCOM_STATUS_TIMEOUT;
	}
      }

      while (!spi_is_ready_to_read(hw)) {
      }

      /* Flush read buffer */
      uint16_t flush;
      spi_read(hw, &flush);
      flush_length--;
//    }
  }

  if (SPI_MODE_MASTER(hw)) {
    /* Wait for last byte to be transferred */
    while (!spi_is_write_complete(hw)) {
    }
  }

  if (SPI_MODE_SLAVE(hw)) {
//    if (module->receiver_enabled) {
//      while (flush_length) {
	/* Start timeout period for slave */
//	for (uint32_t i = 0; i <= SPI_TIMEOUT; i++) {
//	  if (spi_is_ready_to_read(hw)) {
//	    break;
//	  }
//	}
//	if (!spi_is_ready_to_read(hw)) {
	  /* Not ready to read data within timeout period */
//	  return STATUS_ERR_TIMEOUT;
//	}
	/* Flush read buffer */
//	uint16_t flush;
//	spi_read(hw, &flush);
//	flush_length--;
//      }
//    }
  }

  return SERCOM_STATUS_OK;
}

/**
 * Sends and receives a buffer of \c length SPI characters
 *
 * This function will send and receive a buffer of data via the SPI.
 *
 * In master mode the SPI characters will be sent immediately and the
 * received SPI character will  be read as soon as the shifting of the SPI
 * character is complete.
 *
 * In slave mode this function will place the data to be sent into the transmit
 * buffer. It will then block until an SPI master has shifted the complete
 * buffer and the received data is available.
 *
 * \param[in]  module   Pointer to the software instance struct
 * \param[in]  tx_data  Pointer to the buffer to transmit
 * \param[out] rx_data  Pointer to the buffer where received data will be stored
 * \param[in]  length   Number of SPI characters to transfer
 *
 * \return Status of the operation
 * \retval STATUS_OK               If the operation was completed
 * \retval STATUS_ERR_INVALID_ARG  If invalid argument(s) were provided.
 * \retval STATUS_ERR_TIMEOUT      If the operation was not completed within the
 *                                 timeout in slave mode.
 * \retval STATUS_ERR_DENIED       If the receiver is not enabled
 * \retval STATUS_ERR_OVERFLOW     If the data is overflown
 */
enum sercom_status_t spi_transceive_buffer_wait(SercomSpi* const hw,
					    uint8_t *tx_data,
					    uint8_t *rx_data,
					    uint16_t length)
{
  /* Sanity check arguments */
  assert(hw);

  /* Sanity check arguments */
  if (length == 0) {
    return SERCOM_STATUS_INVALID_ARG;
  }

//  if (!(hw->receiver_enabled)) {
//    return SERCOM_STATUS_DENIED;
//  }

  if (SPI_MODE_SLAVE(hw) && (spi_is_write_complete(hw))) {
    /* Clear TX complete flag */
    _spi_clear_tx_complete_flag(hw);
  }

  uint16_t tx_pos = 0;
  uint16_t rx_pos = 0;
  uint16_t rx_length = length;

  /* Send and receive buffer */
  while (length--) {
    /* Start timeout period for slave */
    if (SPI_MODE_SLAVE(hw)) {
      for (uint32_t i = 0; i <= SPI_TIMEOUT; i++) {
	if (spi_is_ready_to_write(hw)) {
	  break;
	}
      }
      /* Check if master has ended the transaction */
      if (spi_is_write_complete(hw)) {
	_spi_clear_tx_complete_flag(hw);
	return SERCOM_STATUS_ABORTED;
      }

      if (!spi_is_ready_to_write(hw)) {
	/* Not ready to write data within timeout period */
	return SERCOM_STATUS_TIMEOUT;
      }
    }

    /* Wait until the module is ready to write a character */
    while (!spi_is_ready_to_write(hw)) {
    }

    /* Write value will be at least 8-bits long */
    uint16_t data_to_send = tx_data[tx_pos++];

    /* If 9-bit data, get next byte to send from the buffer */
//    if (module->character_size == SPI_CHARACTER_SIZE_9BIT) {
//      data_to_send |= (tx_data[tx_pos++] << 8);
//    }

    /* Write the data to send */
    spi_write(hw, data_to_send);

    /* Start timeout period for slave */
    if (SPI_MODE_SLAVE(hw)) {
      for (uint32_t i = 0; i <= SPI_TIMEOUT; i++) {
	if (spi_is_ready_to_write(hw)) {
	  data_to_send = tx_data[tx_pos++];
	  /* If 9-bit data, get next byte to send from the buffer */
//	  if (module->character_size == SPI_CHARACTER_SIZE_9BIT) {
//	    data_to_send |= (tx_data[tx_pos++] << 8);
//	  }

	  /* Write the data to send */
	  spi_write(hw, data_to_send);
	  length--;
	}
	if (spi_is_ready_to_read(hw)) {
	  break;
	}
      }
      /* Check if master has ended the transaction */
      if (spi_is_write_complete(hw)) {
	_spi_clear_tx_complete_flag(hw);
	return SERCOM_STATUS_ABORTED;
      }

      if (!spi_is_ready_to_read(hw)) {
	/* Not ready to read data within timeout period */
	return SERCOM_STATUS_TIMEOUT;
      }
    }

    /* Wait until the module is ready to read a character */
    while (!spi_is_ready_to_read(hw)) {
    }

    enum sercom_status_t retval;
    uint16_t received_data = 0;
    rx_length--;

    retval = spi_read(hw, &received_data);

    if (retval != SERCOM_STATUS_OK) {
      /* Overflow, abort */
      return retval;
    }

    /* Read value will be at least 8-bits long */
    rx_data[rx_pos++] = received_data;

    /* If 9-bit data, write next received byte to the buffer */
//    if (module->character_size == SPI_CHARACTER_SIZE_9BIT) {
//      rx_data[rx_pos++] = (received_data >> 8);
//    }
  }

  if (SPI_MODE_MASTER(hw)) {
    /* Wait for last byte to be transferred */
    while (!spi_is_write_complete(hw)) {
    }
  } else if (SPI_MODE_SLAVE(hw)) {
    while (rx_length) {
      /* Start timeout period for slave */
      for (uint32_t i = 0; i <= SPI_TIMEOUT; i++) {
	if (spi_is_ready_to_read(hw)) {
	  break;
	}
      }
      if (!spi_is_ready_to_read(hw)) {
	/* Not ready to read data within timeout period */
	return SERCOM_STATUS_TIMEOUT;
      }
      enum sercom_status_t retval;
      uint16_t received_data = 0;
      rx_length--;

      retval = spi_read(hw, &received_data);

      if (retval != SERCOM_STATUS_OK) {
	/* Overflow, abort */
	return retval;
      }
      /* Read value will be at least 8-bits long */
      rx_data[rx_pos++] = received_data;

      /* If 9-bit data, write next received byte to the buffer */
//      if (module->character_size == SPI_CHARACTER_SIZE_9BIT) {
//	rx_data[rx_pos++] = (received_data >> 8);
//      }
    }
  } else {
    // ERROR
  }
  return SERCOM_STATUS_OK;
}



static void _spi_transceive_buffer(SercomSpi* const hw)
{
  /* Enable the Data Register Empty and RX Complete Interrupt */
  hw->INTENSET.reg = (SPI_INTERRUPT_FLAG_DATA_REGISTER_EMPTY |
		      SPI_INTERRUPT_FLAG_RX_COMPLETE);

  if (SPI_MODE_SLAVE(hw)) {
    /* Clear TXC flag if set */
    hw->INTFLAG.reg = SPI_INTERRUPT_FLAG_TX_COMPLETE;
    /* Enable transmit complete interrupt for slave */
    hw->INTENSET.reg = SPI_INTERRUPT_FLAG_TX_COMPLETE;
  }
}


/* #  if CONF_SPI_MASTER_ENABLE == true */
/*   if (module->mode == SPI_MODE_MASTER && module->dir == SPI_DIRECTION_READ) { */
/*     /\* Enable Data Register Empty interrupt for master *\/ */
/*     tmp_intenset |= SPI_INTERRUPT_FLAG_DATA_REGISTER_EMPTY; */
/*   } */
/* #  endif */
/* #  if CONF_SPI_SLAVE_ENABLE == true */
/*   if (module->mode == SPI_MODE_SLAVE) { */
/*     /\* Clear TXC flag if set *\/ */
/*     hw->INTFLAG.reg = SPI_INTERRUPT_FLAG_TX_COMPLETE; */
/*     /\* Enable transmit complete interrupt for slave *\/ */
/*     tmp_intenset |= SPI_INTERRUPT_FLAG_TX_COMPLETE; */
/*   } */
/* #  endif */

/*   /\* Enable all interrupts simultaneously *\/ */
/*   hw->INTENSET.reg = tmp_intenset; */
/* } */

/**
 * Reads a character from the Data register to the RX buffer.
 *
 * \param[in,out]  module  Pointer to SPI software instance struct
 */
static void _spi_read(SercomSpi* const hw)
{
  uint16_t received_data = (hw->DATA.reg & SERCOM_SPI_DATA_MASK);

  /* Read value will be at least 8-bits long */
  /* *(module->rx_buffer_ptr) = received_data; */
  /* /\* Increment 8-bit pointer *\/ */
  /* module->rx_buffer_ptr += 1; */

  /* if(module->character_size == SPI_CHARACTER_SIZE_9BIT) { */
  /*   /\* 9-bit data, write next received byte to the buffer *\/ */
  /*   *(module->rx_buffer_ptr) = (received_data >> 8); */
  /*   /\* Increment 8-bit pointer *\/ */
  /*   module->rx_buffer_ptr += 1; */
  /* } */
}

/**
 * Handles interrupts as they occur, and it will run callback functions
 * which are registered and enabled.
 *
 * \note This function will be called by the Sercom_Handler, and should
 *       not be called directly from any application code.
 *
 * \param[in]  instance  ID of the SERCOM instance calling the interrupt
 *                       handler.
 */
void _spi_interrupt_handler(SercomSpi* const hw)
{
  /* Combine callback registered and enabled masks. */
//  uint8_t callback_mask =
//    module->enabled_callback & module->registered_callback;

  /* Read and mask interrupt flag register */
  uint16_t interrupt_status = hw->INTFLAG.reg;
  interrupt_status &= hw->INTENSET.reg;

  /* Data register empty interrupt */
/*   if (interrupt_status & SPI_INTERRUPT_FLAG_DATA_REGISTER_EMPTY) { */
/* #  if CONF_SPI_MASTER_ENABLE == true */
/*     if ((module->mode == SPI_MODE_MASTER) && */
/* 	(module->dir == SPI_DIRECTION_READ)) { */
/*       /\* Send dummy byte when reading in master mode *\/ */
/*       _spi_write_dummy(module); */
/*       if (module->remaining_dummy_buffer_length == 0) { */
/* 	/\* Disable the Data Register Empty Interrupt *\/ */
/* 	hw->INTENCLR.reg */
/* 	  = SPI_INTERRUPT_FLAG_DATA_REGISTER_EMPTY; */
/*       } */
/*     } */
/* #  endif */

/*     if (0 */
/* #  if CONF_SPI_MASTER_ENABLE == true */
/* 	|| ((module->mode == SPI_MODE_MASTER) && */
/* 	    (module->dir != SPI_DIRECTION_READ)) */
/* #  endif */
/* #  if CONF_SPI_SLAVE_ENABLE == true */
/* 	|| ((module->mode == SPI_MODE_SLAVE) && */
/* 	    (module->dir != SPI_DIRECTION_READ)) */
/* #  endif */
/*       ) { */
/*       /\* Write next byte from buffer *\/ */
/*       _spi_write(module); */
/*       if (module->remaining_tx_buffer_length == 0) { */
/* 	/\* Disable the Data Register Empty Interrupt *\/ */
/* 	hw->INTENCLR.reg */
/* 	  = SPI_INTERRUPT_FLAG_DATA_REGISTER_EMPTY; */

/* 	if (module->dir == SPI_DIRECTION_WRITE && */
/* 	    !(module->receiver_enabled)) { */
/* 	  /\* Buffer sent with receiver disabled *\/ */
/* 	  module->dir = SPI_DIRECTION_IDLE; */
/* 	  module->status = STATUS_OK; */
/* 	  /\* Run callback if registered and enabled *\/ */
/* 	  if (callback_mask & (1 << SPI_CALLBACK_BUFFER_TRANSMITTED)){ */
/* 	    (module->callback[SPI_CALLBACK_BUFFER_TRANSMITTED]) */
/* 	      (module); */
/* 	  } */
/* 	} */
/*       } */
/*     } */
/*   } */

/*   /\* Receive complete interrupt*\/ */
/*   if (interrupt_status & SPI_INTERRUPT_FLAG_RX_COMPLETE) { */
/*     /\* Check for overflow *\/ */
/*     if (hw->STATUS.reg & SERCOM_SPI_STATUS_BUFOVF) { */
/*       if (module->dir != SPI_DIRECTION_WRITE) { */
/* 	/\* Store the error code *\/ */
/* 	module->status = STATUS_ERR_OVERFLOW; */

/* 	/\* End transaction *\/ */
/* 	module->dir = SPI_DIRECTION_IDLE; */

/* 	hw->INTENCLR.reg = SPI_INTERRUPT_FLAG_RX_COMPLETE | */
/* 	  SPI_INTERRUPT_FLAG_DATA_REGISTER_EMPTY; */
/* 	/\* Run callback if registered and enabled *\/ */
/* 	if (callback_mask & (1 << SPI_CALLBACK_ERROR)) { */
/* 	  (module->callback[SPI_CALLBACK_ERROR])(module); */
/* 	} */
/*       } */
/*       /\* Flush *\/ */
/*       uint16_t flush = hw->DATA.reg; */
/*       UNUSED(flush); */
/*       /\* Clear overflow flag *\/ */
/*       hw->STATUS.reg |= SERCOM_SPI_STATUS_BUFOVF; */
/*     } else { */
/*       if (module->dir == SPI_DIRECTION_WRITE) { */
/* 	/\* Flush receive buffer when writing *\/ */
/* 	_spi_read_dummy(module); */
/* 	if (module->remaining_dummy_buffer_length == 0) { */
/* 	  hw->INTENCLR.reg = SPI_INTERRUPT_FLAG_RX_COMPLETE; */
/* 	  module->status = STATUS_OK; */
/* 	  module->dir = SPI_DIRECTION_IDLE; */
/* 	  /\* Run callback if registered and enabled *\/ */
/* 	  if (callback_mask & */
/* 	      (1 << SPI_CALLBACK_BUFFER_TRANSMITTED)){ */
/* 	    (module->callback[SPI_CALLBACK_BUFFER_TRANSMITTED])(module); */
/* 	  } */
/* 	} */
/*       } else { */
/* 	/\* Read data register *\/ */
/* 	_spi_read(module); */

/* 	/\* Check if the last character have been received *\/ */
/* 	if (module->remaining_rx_buffer_length == 0) { */
/* 	  module->status = STATUS_OK; */
/* 	  /\* Disable RX Complete Interrupt and set status *\/ */
/* 	  hw->INTENCLR.reg = SPI_INTERRUPT_FLAG_RX_COMPLETE; */
/* 	  if(module->dir == SPI_DIRECTION_BOTH) { */
/* 	    if (callback_mask & (1 << SPI_CALLBACK_BUFFER_TRANSCEIVED)) { */
/* 	      (module->callback[SPI_CALLBACK_BUFFER_TRANSCEIVED])(module); */
/* 	    } */
/* 	  } else if (module->dir == SPI_DIRECTION_READ) { */
/* 	    if (callback_mask & (1 << SPI_CALLBACK_BUFFER_RECEIVED)) { */
/* 	      (module->callback[SPI_CALLBACK_BUFFER_RECEIVED])(module); */
/* 	    } */
/* 	  } */
/* 	} */
/*       } */
/*     } */
/*   } */

/*   /\* Transmit complete *\/ */
/*   if (interrupt_status & SPI_INTERRUPT_FLAG_TX_COMPLETE) { */
/* #  if CONF_SPI_SLAVE_ENABLE == true */
/*     if (module->mode == SPI_MODE_SLAVE) { */
/*       /\* Transaction ended by master *\/ */

/*       /\* Disable interrupts *\/ */
/*       hw->INTENCLR.reg = */
/* 	SPI_INTERRUPT_FLAG_TX_COMPLETE | */
/* 	SPI_INTERRUPT_FLAG_RX_COMPLETE | */
/* 	SPI_INTERRUPT_FLAG_DATA_REGISTER_EMPTY; */
/*       /\* Clear interrupt flag *\/ */
/*       hw->INTFLAG.reg = SPI_INTERRUPT_FLAG_TX_COMPLETE; */


/*       /\* Reset all status information *\/ */
/*       module->dir = SPI_DIRECTION_IDLE; */
/*       module->remaining_tx_buffer_length = 0; */
/*       module->remaining_rx_buffer_length = 0; */
/*       module->status = STATUS_OK; */

/*       if (callback_mask & */
/* 	  (1 << SPI_CALLBACK_SLAVE_TRANSMISSION_COMPLETE)) { */
/* 	(module->callback[SPI_CALLBACK_SLAVE_TRANSMISSION_COMPLETE]) */
/* 	  (module); */
/*       } */

/*     } */
/* #  endif */
/*   } */

/* #  ifdef FEATURE_SPI_SLAVE_SELECT_LOW_DETECT */
/* #  if CONF_SPI_SLAVE_ENABLE == true */
/*   /\* When a high to low transition is detected on the _SS pin in slave mode *\/ */
/*   if (interrupt_status & SPI_INTERRUPT_FLAG_SLAVE_SELECT_LOW) { */
/*     if (module->mode == SPI_MODE_SLAVE) { */
/*       /\* Disable interrupts *\/ */
/*       hw->INTENCLR.reg = SPI_INTERRUPT_FLAG_SLAVE_SELECT_LOW; */
/*       /\* Clear interrupt flag *\/ */
/*       hw->INTFLAG.reg = SPI_INTERRUPT_FLAG_SLAVE_SELECT_LOW; */

/*       if (callback_mask & (1 << SPI_CALLBACK_SLAVE_SELECT_LOW)) { */
/* 	(module->callback[SPI_CALLBACK_SLAVE_SELECT_LOW])(module); */
/*       } */
/*     } */
/*   } */
/* #  endif */
/* #  endif */

/* #  ifdef FEATURE_SPI_ERROR_INTERRUPT */
/*   /\* When combined error happen *\/ */
/*   if (interrupt_status & SPI_INTERRUPT_FLAG_COMBINED_ERROR) { */
/*     /\* Disable interrupts *\/ */
/*     hw->INTENCLR.reg = SPI_INTERRUPT_FLAG_COMBINED_ERROR; */
/*     /\* Clear interrupt flag *\/ */
/*     hw->INTFLAG.reg = SPI_INTERRUPT_FLAG_COMBINED_ERROR; */

/*     if (callback_mask & (1 << SPI_CALLBACK_COMBINED_ERROR)) { */
/*       (module->callback[SPI_CALLBACK_COMBINED_ERROR])(module); */
/*     } */
/*   } */
/* #  endif */
}
