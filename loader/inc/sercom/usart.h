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
#ifndef USART_H_INCLUDED
#define USART_H_INCLUDED

/**
 * This driver for SAM D20/D21/R21 devices provides an interface for the configuration
 * and management of the SERCOM module in its USART mode to transfer or receive
 * USART data frames.
 *
 * The following peripherals are used by this module:
 *
 * - SERCOM (Serial Communication Interface)
 *
 * Prerequisites
 *
 * To use the USART you need to have a GCLK generator enabled and running
 * that can be used as the SERCOM clock source. This can either be configured
 * in conf_clocks.h or by using the system clock driver.
 *
 * Module Overview
 *
 * This driver will use one (or more) SERCOM interfaces on the system
 * and configure it to run as a USART interface in either synchronous
 * or asynchronous mode.
 *
 * Frame Format
 *
 * Communication is based on frames, where the frame format can be customized
 * to accommodate a wide range of standards. A frame consists of a start bit,
 * a number of data bits, an optional parity bit for error detection as well
 * as a configurable length stop bit(s) - see
 * \ref asfdoc_sam0_sercom_usart_frame_diagram "the figure below".
 * \ref asfdoc_sam0_sercom_usart_frame_params "The table below" shows the
 * available parameters you can change in a frame.
 *
 * \anchor asfdoc_sam0_sercom_usart_frame_params
 * <table>
 *  <caption>USART Frame Parameters</caption>
 *  <tr>
 *      <th>Parameter</th>
 *      <th>Options</th>
 *  </tr>
 *  <tr>
 *      <td>Start bit</td>
 *      <td>1</td>
 *  </tr>
 *  <tr>
 *      <td>Data bits</td>
 *      <td>5, 6, 7, 8, 9</td>
 *  </tr>
 *  <tr>
 *      <td>Parity bit</td>
 *      <td>None, Even, Odd</td>
 *  </tr>
 *  <tr>
 *      <td>Stop bits</td>
 *      <td>1, 2</td>
 *  </tr>
 * </table>
 *
 * Synchronous mode
 *
 * In synchronous mode a dedicated clock line is provided; either by the USART
 * itself if in master mode, or by an external master if in slave mode.
 * Maximum transmission speed is the same as the GCLK clocking the USART
 * peripheral when in slave mode, and the GCLK divided by two if in
 * master mode. In synchronous mode the interface needs three lines to
 * communicate:
 * - TX (Transmit pin)
 * - RX (Receive pin)
 * - XCK (Clock pin)
 *
 * Data sampling

 * In synchronous mode the data is sampled on either the rising or falling edge
 * of the clock signal. This is configured by setting the clock polarity in the
 * configuration struct.
 *
 * Asynchronous mode
 *
 * In asynchronous mode no dedicated clock line is used, and the
 * communication is based on matching the clock speed on the
 * transmitter and receiver. The clock is generated from the internal
 * SERCOM baudrate generator, and the frames are synchronized by using
 * the frame start bits. Maximum transmission speed is limited to the
 * SERCOM GCLK divided by 16.
 * In asynchronous mode the interface only needs two lines to communicate:
 * - TX (Transmit pin)
 * - RX (Receive pin)
 *
 * Transmitter/receiver clock matching
 *
 * For successful transmit and receive using the asynchronous mode the
 * receiver and transmitter clocks needs to be closely matched. When
 * receiving a frame that does not match the selected baud rate
 * closely enough the receiver will be unable to synchronize the
 * frame(s), and garbage transmissions will result.
 *
 * Parity
 *
 * Parity can be enabled to detect if a transmission was in
 * error. This is done by counting the number of "1" bits in the
 * frame. When using Even parity the parity bit will be set if the
 * total number of "1"s in the frame are an even number. If using Odd
 * parity the parity bit will be set if the total number of "1"s are
 * Odd.
 *
 * When receiving a character the receiver will count the number of
 * "1"s in the frame and give an error if the received frame and
 * parity bit disagree.
 *
 * GPIO configuration
 *
 * The SERCOM module has four internal pads; the RX pin can be placed
 * freely on any one of the four pads, and the TX and XCK pins have
 * two predefined positions that can be selected as a pair. The pads
 * can then be routed to an external GPIO pin using the normal pin
 * multiplexing scheme on the SAM D20/D21/R21.
 *
 * Special Considerations
 *
 * Never execute large portions of code in the callbacks. These
 * are run from the interrupt routine, and thus having long callbacks will
 * keep the processor in the interrupt handler for an equally long time.
 * A common way to handle this is to use global flags signaling the
 * main application that an interrupt event has happened, and only do the
 * minimal needed processing in the callback.
 */

#include "sercom/sercom.h"
#include "system/pinmux.h"
#include "samd20.h"

#define USART_WAIT_FOR_SYNC(hw)				\
  while(hw->STATUS.reg & SERCOM_USART_STATUS_SYNCBUSY)

/**
 * Define SERCOM USART features set according to different device family.
 */
#if (SAMD21) || (SAMR21)
/** Usart sync scheme version 2. */
#  define FEATURE_USART_SYNC_SCHEME_V2
/** Usart over sampling. */
#  define FEATURE_USART_OVER_SAMPLE
/** Usart hardware control flow. */
#  define FEATURE_USART_HARDWARE_FLOW_CONTROL
/** IrDA mode. */
#  define FEATURE_USART_IRDA
/** LIN slave mode. */
#  define FEATURE_USART_LIN_SLAVE
/** Usart collision detection. */
#  define FEATURE_USART_COLLISION_DECTION
/** Usart start frame detection. */
#  define FEATURE_USART_START_FRAME_DECTION
/** Usart start buffer overflow notification. */
#  define FEATURE_USART_IMMEDIATE_BUFFER_OVERFLOW_NOTIFICATION
#endif

#ifndef PINMUX_DEFAULT
/** Default pin mux. */
#  define PINMUX_DEFAULT 0
#endif

#ifndef PINMUX_UNUSED
/** Unused PIN mux. */
#  define PINMUX_UNUSED 0xFFFFFFFF
#endif

#ifndef USART_TIMEOUT
/** USART timeout value. */
#  define USART_TIMEOUT 0xFFFF
#endif

/**
 * Callbacks for the Asynchronous USART driver
 */
enum usart_callback {
  /** Callback for buffer transmitted */
  USART_CALLBACK_BUFFER_TRANSMITTED,
  /** Callback for buffer received */
  USART_CALLBACK_BUFFER_RECEIVED,
  /** Callback for error */
  USART_CALLBACK_ERROR,
#ifdef FEATURE_USART_LIN_SLAVE
  /** Callback for break character is received. */
  USART_CALLBACK_BREAK_RECEIVED,
#endif
#ifdef FEATURE_USART_HARDWARE_FLOW_CONTROL
  /** Callback for a change is detected on the CTS pin. */
  USART_CALLBACK_CTS_INPUT_CHANGE,
#endif
#ifdef FEATURE_USART_START_FRAME_DECTION
  /** Callback for a start condition is detected on the RxD line. */
  USART_CALLBACK_START_RECEIVED,
#endif
#  if !defined(__DOXYGEN__)
  /** Number of available callbacks. */
  USART_CALLBACK_N,
#  endif
};

/**
 * The data order decides which of MSB or LSB is shifted out first when data is
 * transferred
 */
enum usart_dataorder {
  /** The MSB will be shifted out first during transmission,
   *  and shifted in first during reception */
  USART_DATAORDER_MSB = 0,
  /** The LSB will be shifted out first during transmission,
   *  and shifted in first during reception */
  USART_DATAORDER_LSB = SERCOM_USART_CTRLA_DORD,
};

/**
 * Select USART transfer mode
 */
enum usart_transfer_mode {
  /** Transfer of data is done synchronously */
  USART_TRANSFER_SYNCHRONOUSLY = (SERCOM_USART_CTRLA_CMODE),
  /** Transfer of data is done asynchronously */
  USART_TRANSFER_ASYNCHRONOUSLY = 0
};

/*
 * Select parity USART parity mode
 */
enum usart_parity {
  /** For odd parity checking, the parity bit will be set if number of
   *  ones being transferred is even */
  USART_PARITY_ODD  = SERCOM_USART_CTRLB_PMODE,

  /** For even parity checking, the parity bit will be set if number of
   *  ones being received is odd */
  USART_PARITY_EVEN = 0,

  /** No parity checking will be executed, and there will be no parity bit
   *  in the received frame */
  USART_PARITY_NONE = 0xFF,
};

/**
 * Set the functionality of the SERCOM pins.
 *
 * See \ref asfdoc_sam0_sercom_usart_mux_settings for a description of the
 * various MUX setting options.
 */
enum usart_signal_mux_settings {
#ifdef FEATURE_USART_HARDWARE_FLOW_CONTROL
  /** MUX setting RX_0_TX_0_XCK_1 */
  USART_RX_0_TX_0_XCK_1 = (SERCOM_USART_CTRLA_RXPO(0) | SERCOM_USART_CTRLA_TXPO(0)),
  /** MUX setting RX_0_TX_2_XCK_3 */
  USART_RX_0_TX_2_XCK_3 = (SERCOM_USART_CTRLA_RXPO(0) | SERCOM_USART_CTRLA_TXPO(1)),
  /** MUX setting USART_RX_0_TX_0_RTS_2_CTS_3 */
  USART_RX_0_TX_0_RTS_2_CTS_3 = (SERCOM_USART_CTRLA_RXPO(0) | SERCOM_USART_CTRLA_TXPO(2)),
  /** MUX setting RX_1_TX_0_XCK_1 */
  USART_RX_1_TX_0_XCK_1 = (SERCOM_USART_CTRLA_RXPO(1) | SERCOM_USART_CTRLA_TXPO(0)),
  /** MUX setting RX_1_TX_2_XCK_3 */
  USART_RX_1_TX_2_XCK_3 = (SERCOM_USART_CTRLA_RXPO(1) | SERCOM_USART_CTRLA_TXPO(1)),
  /** MUX setting USART_RX_1_TX_0_RTS_2_CTS_3 */
  USART_RX_1_TX_0_RTS_2_CTS_3 = (SERCOM_USART_CTRLA_RXPO(1) | SERCOM_USART_CTRLA_TXPO(2)),
  /** MUX setting RX_2_TX_0_XCK_1 */
  USART_RX_2_TX_0_XCK_1 = (SERCOM_USART_CTRLA_RXPO(2) | SERCOM_USART_CTRLA_TXPO(0)),
  /** MUX setting RX_2_TX_2_XCK_3 */
  USART_RX_2_TX_2_XCK_3 = (SERCOM_USART_CTRLA_RXPO(2) | SERCOM_USART_CTRLA_TXPO(1)),
  /** MUX setting USART_RX_2_TX_0_RTS_2_CTS_3 */
  USART_RX_2_TX_0_RTS_2_CTS_3 = (SERCOM_USART_CTRLA_RXPO(2) | SERCOM_USART_CTRLA_TXPO(2)),
  /** MUX setting RX_3_TX_0_XCK_1 */
  USART_RX_3_TX_0_XCK_1 = (SERCOM_USART_CTRLA_RXPO(3) | SERCOM_USART_CTRLA_TXPO(0)),
  /** MUX setting RX_3_TX_2_XCK_3 */
  USART_RX_3_TX_2_XCK_3 = (SERCOM_USART_CTRLA_RXPO(3) | SERCOM_USART_CTRLA_TXPO(1)),
  /** MUX setting USART_RX_3_TX_0_RTS_2_CTS_3 */
  USART_RX_3_TX_0_RTS_2_CTS_3 = (SERCOM_USART_CTRLA_RXPO(3) | SERCOM_USART_CTRLA_TXPO(2)),
#else
  /** MUX setting RX_0_TX_0_XCK_1 */
  USART_RX_0_TX_0_XCK_1 = (SERCOM_USART_CTRLA_RXPO(0)),
  /** MUX setting RX_0_TX_2_XCK_3 */
  USART_RX_0_TX_2_XCK_3 = (SERCOM_USART_CTRLA_RXPO(0) | SERCOM_USART_CTRLA_TXPO),
  /** MUX setting RX_1_TX_0_XCK_1 */
  USART_RX_1_TX_0_XCK_1 = (SERCOM_USART_CTRLA_RXPO(1)),
  /** MUX setting RX_1_TX_2_XCK_3 */
  USART_RX_1_TX_2_XCK_3 = (SERCOM_USART_CTRLA_RXPO(1) | SERCOM_USART_CTRLA_TXPO),
  /** MUX setting RX_2_TX_0_XCK_1 */
  USART_RX_2_TX_0_XCK_1 = (SERCOM_USART_CTRLA_RXPO(2)),
  /** MUX setting RX_2_TX_2_XCK_3 */
  USART_RX_2_TX_2_XCK_3 = (SERCOM_USART_CTRLA_RXPO(2) | SERCOM_USART_CTRLA_TXPO),
  /** MUX setting RX_3_TX_0_XCK_1 */
  USART_RX_3_TX_0_XCK_1 = (SERCOM_USART_CTRLA_RXPO(3)),
  /** MUX setting RX_3_TX_2_XCK_3 */
  USART_RX_3_TX_2_XCK_3 = (SERCOM_USART_CTRLA_RXPO(3) | SERCOM_USART_CTRLA_TXPO),
#endif
};

/**
 * Number of stop bits for a frame.
 */
enum usart_stopbits {
  /** Each transferred frame contains 1 stop bit */
  USART_STOPBITS_1 = 0,
  /** Each transferred frame contains 2 stop bits */
  USART_STOPBITS_2 = SERCOM_USART_CTRLB_SBMODE,
};

/**
 * Number of bits for the character sent in a frame.
 */
enum usart_character_size {
  /** The char being sent in a frame is 5 bits long */
  USART_CHARACTER_SIZE_5BIT = SERCOM_USART_CTRLB_CHSIZE(5),
  /** The char being sent in a frame is 6 bits long */
  USART_CHARACTER_SIZE_6BIT = SERCOM_USART_CTRLB_CHSIZE(6),
  /** The char being sent in a frame is 7 bits long */
  USART_CHARACTER_SIZE_7BIT = SERCOM_USART_CTRLB_CHSIZE(7),
  /** The char being sent in a frame is 8 bits long */
  USART_CHARACTER_SIZE_8BIT = SERCOM_USART_CTRLB_CHSIZE(0),
  /** The char being sent in a frame is 9 bits long */
  USART_CHARACTER_SIZE_9BIT = SERCOM_USART_CTRLB_CHSIZE(1),
};

#ifdef FEATURE_USART_OVER_SAMPLE
/**
 * The value of sample rate and baud rate generation mode.
 */
enum usart_sample_rate {
  /** 16x over-sampling using arithmetic baud rate generation */
  USART_SAMPLE_RATE_16X_ARITHMETIC = SERCOM_USART_CTRLA_SAMPR(0),
  /** 16x over-sampling using fractional baud rate generation */
  USART_SAMPLE_RATE_16X_FRACTIONAL = SERCOM_USART_CTRLA_SAMPR(1),
  /** 8x over-sampling using arithmetic baud rate generation */
  USART_SAMPLE_RATE_8X_ARITHMETIC = SERCOM_USART_CTRLA_SAMPR(2),
  /** 8x over-sampling using fractional baud rate generation */
  USART_SAMPLE_RATE_8X_FRACTIONAL = SERCOM_USART_CTRLA_SAMPR(3),
  /** 3x over-sampling using arithmetic baud rate generation */
  USART_SAMPLE_RATE_3X_ARITHMETIC = SERCOM_USART_CTRLA_SAMPR(4),
};

/**
 * The value of sample number used for majority voting
 */
enum usart_sample_adjustment {
  /** The first, middle and last sample number used for majority voting is 7-8-9 */
  USART_SAMPLE_ADJUSTMENT_7_8_9 = SERCOM_USART_CTRLA_SAMPA(0),
  /** The first, middle and last sample number used for majority voting is 9-10-11 */
  USART_SAMPLE_ADJUSTMENT_9_10_11 = SERCOM_USART_CTRLA_SAMPA(1),
  /** The first, middle and last sample number used for majority voting is 11-12-13 */
  USART_SAMPLE_ADJUSTMENT_11_12_13 = SERCOM_USART_CTRLA_SAMPA(2),
  /** The first, middle and last sample number used for majority voting is 13-14-15 */
  USART_SAMPLE_ADJUSTMENT_13_14_15 = SERCOM_USART_CTRLA_SAMPA(3),
};
#endif

/**
 * Select Receiver or Transmitter
 */
enum usart_transceiver_type {
  /** The parameter is for the Receiver */
  USART_TRANSCEIVER_RX,
  /** The parameter is for the Transmitter */
  USART_TRANSCEIVER_TX,
};

/**
 * Enables the USART module
 *
 * \param[in]  module  Pointer to USART software instance struct
 */
static inline void usart_enable(SercomUsart* const hw)
{
  /* Sanity check arguments */


  /* Wait until synchronization is complete */
  USART_WAIT_FOR_SYNC(hw);

  /* Enable USART module */
  hw->CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
}

/**
 * Disables the USART module
 *
 * \param[in]  module  Pointer to USART software instance struct
 */
static inline void usart_disable(SercomUsart* const hw)
{
  /* Sanity check arguments */


  /* Wait until synchronization is complete */
  USART_WAIT_FOR_SYNC(hw);

  /* Disable USART module */
  hw->CTRLA.reg &= ~SERCOM_USART_CTRLA_ENABLE;
}

/**
 * Disables and resets the USART module.
 *
 * \param[in]  module  Pointer to the USART software instance struct
 */
static inline void usart_reset(SercomUsart* const hw)
{
  /* Sanity check arguments */


  usart_disable(hw);

  /* Wait until synchronization is complete */
  USART_WAIT_FOR_SYNC(hw);

  /* Reset module */
  hw->CTRLA.reg = SERCOM_USART_CTRLA_SWRST;
}

/**
 * Enable the given transceiver. Either RX or TX.
 *
 * \param[in]  module            Pointer to USART software instance struct
 * \param[in]  transceiver_type  Transceiver type.
 */
static inline void usart_enable_transceiver(SercomUsart* const hw,
					    enum usart_transceiver_type transceiver_type)
{
  /* Sanity check arguments */


  /* Wait until synchronization is complete */
  USART_WAIT_FOR_SYNC(hw);

  switch (transceiver_type) {
    case USART_TRANSCEIVER_RX:
      /* Enable RX */
      hw->CTRLB.reg |= SERCOM_USART_CTRLB_RXEN;
      break;

    case USART_TRANSCEIVER_TX:
      /* Enable TX */
      hw->CTRLB.reg |= SERCOM_USART_CTRLB_TXEN;
      break;
  }
}

/**
 * Disable the given transceiver (RX or TX).
 *
 * \param[in]  module            Pointer to USART software instance struct
 * \param[in]  transceiver_type  Transceiver type.
 */
static inline void usart_disable_transceiver(SercomUsart* const hw,
					     enum usart_transceiver_type transceiver_type)
{
  /* Sanity check arguments */


  /* Wait until synchronization is complete */
  USART_WAIT_FOR_SYNC(hw);

  switch (transceiver_type) {
    case USART_TRANSCEIVER_RX:
      /* Disable RX */
      hw->CTRLB.reg &= ~SERCOM_USART_CTRLB_RXEN;
      break;

    case USART_TRANSCEIVER_TX:
      /* Disable TX */
      hw->CTRLB.reg &= ~SERCOM_USART_CTRLB_TXEN;
      break;
  }
}

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
				uint32_t pinmux_pad3);

enum sercom_status_t usart_write_wait(SercomUsart* const hw,
				      const uint16_t tx_data);

enum sercom_status_t usart_read_wait(SercomUsart* const hw,
				     uint16_t *const rx_data);

enum sercom_status_t usart_write_buffer_wait(SercomUsart* const hw,
					     const uint8_t *tx_data,
					     uint16_t length);

enum sercom_status_t usart_read_buffer_wait(SercomUsart* const hw,
					    uint8_t *rx_data,
					    uint16_t length);

/**
 * -------------------------------- Interrupts -------------------------------
 */

/**
 * Rx Callback type for usart
 */
typedef void (*usart_rx_callback_t)(SercomUsart* const sercom_instance,
				    uint16_t data);

void usart_register_rx_callback(SercomUsart* const hw,
				usart_rx_callback_t callback,
				uint32_t priority);

#endif /* USART_H_INCLUDED */
