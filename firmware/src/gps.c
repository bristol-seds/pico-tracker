/*
 * Functions for the UBLOX 6 GPS
 * Copyright (C) 2014  Richard Meadows <richardeoin>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <string.h>

#include "samd20.h"

#include "semihosting.h"
#include "hw_config.h"
#include "system/system.h"
#include "system/interrupt.h"
#include "sercom/sercom.h"
#include "sercom/usart.h"
#include "util/dbuffer.h"
#include "gps.h"

/**
 * UBX Constants
 */
const uint16_t ubx_header = (0xB5 | (0x62 << 8));
/**
 * UBX Class Types
 */
enum {
  UBX_NAV	= 0x01,
  UBX_RXM	= 0x02,
  UBX_INF	= 0x04,
  UBX_ACK	= 0x05,
  UBX_CFG	= 0x06,
  UBX_MON	= 0x0A,
  UBX_AID	= 0x0B,
  UBX_TIM	= 0x0D,
  UBX_ESF	= 0x10,
};
/**
 * UBX NAV Message Types
 */
enum {
  UBX_NAV_POSLLH	= (UBX_NAV | (0x02 << 8)),
  UBX_NAV_SOL		= (UBX_NAV | (0x06 << 8)),
  UBX_NAV_TIMEGPS	= (UBX_NAV | (0x20 << 8)),
  UBX_NAV_TIMEUTC	= (UBX_NAV | (0x21 << 8)),
};
/**
 * UBX CFG Message Types
 */
enum {
  UBX_CFG_PRT	= (UBX_CFG | (0x00 << 8)),
  UBX_CFG_TP	= (UBX_CFG | (0x07 << 8)),
  UBX_CFG_ANT	= (UBX_CFG | (0x13 << 8)),
  UBX_CFG_NAV5	= (UBX_CFG | (0x24 << 8)),
  UBX_CFG_TP5	= (UBX_CFG | (0x31 << 8)),
};
/**
 * UBX ACK Message Types
 */
enum {
  UBX_ACK_NACK	= (UBX_ACK | (0x00 << 8)),
  UBX_ACK_ACK	= (UBX_ACK | (0x01 << 8)),
};

#define UBX_BUFFER_LEN	0x80

#define SFD1	0xB5
#define SFD2	0x62

enum sfd_state {
  SFD_WAITING = -1,
  SFD_GOT1,
} sfd_state = SFD_WAITING;

int32_t ubx_index = SFD_WAITING;
uint16_t ubx_payload_length = 0;
uint8_t ubx_irq_buffer[UBX_BUFFER_LEN];

#define _send_buffer(tx_data, length)			\
  usart_write_buffer_wait(GPS_SERCOM, tx_data, length)
#define _get_buffer(rx_data, length)			\
  usart_read_buffer_wait(GPS_SERCOM, rx_data, length)

/**
 * Flags for pending ubx pakcets
 */
enum ubx_packet_state {
  UBX_PACKET_WAITING,
  UBX_PACKET_ACK,
  UBX_PACKET_NACK,
};
enum ubx_packet_state _ubx_cfg_tp_state;
enum ubx_packet_state _ubx_cfg_tp5_state;

/**
 * Processes UBX ack/nack packets
 */
void ubx_process_ack(uint16_t message, enum ubx_packet_state state)
{
  switch (message) {
    case UBX_CFG_TP:
      _ubx_cfg_tp_state = state;
      break;
    case UBX_CFG_TP5:
      _ubx_cfg_tp5_state = state;
      break;
    default:
      break;
  }
}
/**
 * Macro for the function below
 */
#define UBX_POPULATE_STRUCT(ubx_type)					\
  if (payload_length == sizeof(ubx_type)) {				\
    memcpy((void*)&ubx_type, frame + 4, payload_length);		\
  }
/**
 * Process a single ubx frame. Runs in the IRQ so should be short and sweet.
 */
void ubx_process_frame(uint8_t* frame)
{
  uint16_t* frame16 = (uint16_t*)frame;
  uint16_t payload_length = frame16[1];

  /* Checksum.. */

  switch (frame[0]) { /* Switch by Class */
    case UBX_NAV:
      switch (frame16[0]) {
	case UBX_NAV_SOL: /* Navigation Solution Information */
	  UBX_POPULATE_STRUCT(ubx_nav_sol);
	  break;
	case UBX_NAV_TIMEUTC: /* UTC Time Solution */
	  UBX_POPULATE_STRUCT(ubx_nav_timeutc);
	  break;
	case UBX_NAV_POSLLH: /* Geodetic Position Solution */
	  UBX_POPULATE_STRUCT(ubx_nav_posllh);
	  break;
      }
      break;
    case UBX_CFG:
      switch (frame16[0]) {
	case UBX_CFG_NAV5: /* Navigation Engine Settings */
	  UBX_POPULATE_STRUCT(ubx_cfg_nav5);
	  break;
	case UBX_CFG_ANT: /* Antenna Control Settings */
	  UBX_POPULATE_STRUCT(ubx_cfg_ant);
	  break;
	case UBX_CFG_TP: /* TimePulse Parameters */
	  UBX_POPULATE_STRUCT(ubx_cfg_tp);
	  break;
	case UBX_CFG_TP5: /* TimePulse Parameters */
	  UBX_POPULATE_STRUCT(ubx_cfg_tp5);
	  break;
      }
      break;
    case UBX_ACK:
      if (payload_length == 2) { /* All ACK packets should have a payload len of 2 */
	switch (frame16[0]) {
	  case UBX_ACK_ACK:
	    ubx_process_ack(frame16[2], UBX_PACKET_ACK);
	    break;
	  case UBX_ACK_NACK:
	    ubx_process_ack(frame16[2], UBX_PACKET_NACK);
	    break;
	}
      }
      break;
    default:
      break;
  }
}

/**
 * Rx Callback. Processes a stream of
 */
void gps_rx_callback(SercomUsart* const hw, uint16_t data)
{
  (void)hw;

  /* Start of Frame Delimeter 1 */
  if (data == SFD1) {
    sfd_state = SFD_GOT1;
  } else {
    /* SFD2 preceeded by SFD1 */
    if (sfd_state == SFD_GOT1 && data == SFD2) {

      /* Start a new frame */
      ubx_index = 0;
      ubx_payload_length = 0;
      sfd_state = SFD_WAITING;
      return;
    }

    sfd_state = SFD_WAITING;
  }

  /**
   * If we have a valid index. That is:
   * We're in a frame
   * And we've not reached class/message + length + payload + checksum
   * And we've not reached the end of the buffer
   */
  if (ubx_index != SFD_WAITING &&
      ubx_index < (4 + ubx_payload_length + 2) &&
      ubx_index < UBX_BUFFER_LEN) {

    /* Data in */
    ubx_irq_buffer[ubx_index++] = data;

    /* Extract length */
    if (ubx_index == 4) {
      ubx_payload_length = ((uint16_t*)ubx_irq_buffer)[1];
    }

    /* Complete Frame? */
    if (ubx_index >= (4 + ubx_payload_length + 2)) {
      /* Process it */
      ubx_process_frame(ubx_irq_buffer);
    }
  }
}

/**
 * Calculate a UBX checksum using 8-bit Fletcher (RFC1145)
 */
uint16_t _ubx_checksum(uint8_t* data, uint8_t len)
{
  uint16_t ck = 0;
  uint8_t* cka = (uint8_t*)&ck;
  uint8_t* ckb = (cka + 1);
  for(uint8_t i = 0; i < len; i++) {
    *cka += *data;
    *ckb += *cka;
    data++;
  }

  return ck;
}
/**
 * Sends a standard UBX message
 */
void _ubx_send_message(uint16_t message, uint8_t* payload, uint16_t length)
{
  uint8_t ubx[UBX_BUFFER_LEN];
  uint8_t* ubx_buffer = ubx;

  /* Copy little endian */
  memcpy(ubx_buffer, &ubx_header, 2); ubx_buffer += 2; 	/* Header	*/
  memcpy(ubx_buffer, &message, 2); ubx_buffer += 2;	/* Message Type	*/
  memcpy(ubx_buffer, &length, 2); ubx_buffer += 2;	/* Length	*/
  memcpy(ubx_buffer, payload, length); ubx_buffer += length; /* Payload*/
  uint16_t checksum = _ubx_checksum(ubx + 2, length + 4);
  memcpy(ubx_buffer, &checksum, 2); ubx_buffer += 2;	/* Checksum	*/

  _send_buffer(ubx, length + 8);
}
/**
 * Sends a UBX Poll Request
 */
void _ubx_poll(uint16_t message) {
  _ubx_send_message(message, NULL, 0);
}

/**
 * Disable NMEA messages on the uBlox
 */
void gps_disable_nmea(void)
{
  for (int i = 0; i < 1000*100; i++);

  uint8_t setNMEAoff[] = {
    0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x80, 0x25,
    0x00, 0x00, 0x07, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
  };

  _ubx_send_message(UBX_CFG_PRT, setNMEAoff, sizeof(setNMEAoff)/sizeof(uint8_t));

  for (int i = 0; i < 1000*100; i++);
}

/**
 * Check the navigation status to determine the quality of the
 * fix currently held by the receiver with a NAV-STATUS message.
 */
void gps_check_lock()
{
  /* Send the poll request */
  _ubx_poll(UBX_NAV_SOL);
}

/**
 * Verify that the uBlox 6 GPS receiver is set to the <1g airborne
 * navigaion mode.
 */
void gps_check_nav(void)
{
  /* Send the poll request */
  _ubx_poll(UBX_CFG_NAV5);
}

/**
 * Set the GPS timepulse settings using the CFG_TP message
 */
void gps_set_timepulse(void)
{
  /* Clear the packet state */
  _ubx_cfg_tp_state = UBX_PACKET_WAITING;

  /* Send the Request */
  _ubx_poll(UBX_CFG_TP);

  /* Define the settings we want */
  struct ubx_cfg_tp timepulse;
  memset(&timepulse, 0, sizeof(ubx_cfg_tp));
  timepulse.interval = 2;	/* 2µS		*/
  timepulse.length = 1;		/* 1µS		*/
  timepulse.status = 1;		/* On, Positive	*/
  timepulse.timeRef = 1;	/* Align GPS time */
  timepulse.flags = 0x1;	/* Run outside lock */
  timepulse.antennaCableDelay = 50; /* 50 nS	*/

  /* Wait for acknoledge */
  while (_ubx_cfg_tp_state == UBX_PACKET_WAITING);

  /* Compare with current settings */
  if (memcmp((void*)&ubx_cfg_tp, &timepulse, sizeof(ubx_cfg_tp)) != 0) {
    /* Write the new settings */
    _ubx_send_message(UBX_CFG_TP, (uint8_t*)&timepulse, sizeof(ubx_cfg_tp));
  }
}
/**
 * Set the GPS timepulse settings using the CFG_TP5 message
 */
void gps_set_timepulse_five(uint32_t frequency)
{
  /* Clear the packet state */
  _ubx_cfg_tp5_state = UBX_PACKET_WAITING;

  /* Send the Request */
  _ubx_poll(UBX_CFG_TP5);

  /* Define the settings we want */
  struct ubx_cfg_tp5 timepulse5;
  memset(&timepulse5, 0, sizeof(ubx_cfg_tp5));
  timepulse5.tpIdx = 0;
  timepulse5.antCableDelay = 50; /* 50 nS	*/
  /* GPS time, duty cyle, frequency, lock to GPS, active */
  timepulse5.flags = 0x80 | 0x8 | 0x3;
  timepulse5.freqPeriod = frequency;
  timepulse5.pulseLenRatio = 0x80000000; /* 50 % duty cycle*/

  /* Wait for acknoledge */
  while (_ubx_cfg_tp5_state == UBX_PACKET_WAITING);

  /* Compare with current settings */
  if (memcmp((void*)&ubx_cfg_tp5, &timepulse5, sizeof(ubx_cfg_tp5)) != 0) {
    /* Write the new settings */
    _ubx_send_message(UBX_CFG_TP5, (uint8_t*)&timepulse5, sizeof(ubx_cfg_tp5));
  }
}

/**
 * Init
 */
void gps_init(void)
{
  /* USART */
  usart_init(GPS_SERCOM,
	     USART_DATAORDER_LSB, 		/** Bit order (MSB or LSB first) */
	     USART_TRANSFER_ASYNCHRONOUSLY,	/** Asynchronous or synchronous mode */
	     USART_PARITY_NONE,			/** USART parity */
	     USART_STOPBITS_1,			/** Number of stop bits */
	     USART_CHARACTER_SIZE_8BIT, 	/** USART character size */
	     GPS_SERCOM_MUX,			/** USART pinout */
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
	     true,	     /** Run in standby */
	     GCLK_GENERATOR_0,			/** GCLK generator source */
	     GPS_SERCOM_MOGI_PINMUX, 		/** PAD0 pinmux */
	     GPS_SERCOM_MIGO_PINMUX,		/** PAD1 pinmux */
	     PINMUX_UNUSED,			/** PAD2 pinmux */
	     PINMUX_UNUSED);			/** PAD3 pinmux */

  usart_enable(GPS_SERCOM);

  /* We use ubx protocol */
  gps_disable_nmea();

  /* Incoming ubx messages are handled in an irq */
  usart_register_rx_callback(GPS_SERCOM, gps_rx_callback, 0);

  /* Fill some configuration structures */

  gps_check_lock();
  _ubx_poll(UBX_CFG_ANT);

  /* */
  gps_check_nav();

  /* Set the timepulse */
  gps_set_timepulse_five(GPS_TIMEPULSE_FREQ);
}


/**
 * Quick and dirty loopback test. Should print 0x34
 */
void usart_loopback_test(void)
{
  uint16_t data;

  usart_init(GPS_SERCOM,
	     USART_DATAORDER_LSB, 		/** Bit order (MSB or LSB first) */
	     USART_TRANSFER_ASYNCHRONOUSLY,	/** Asynchronous or synchronous mode */
	     USART_PARITY_NONE,			/** USART parity */
	     USART_STOPBITS_1,			/** Number of stop bits */
	     USART_CHARACTER_SIZE_8BIT, 	/** USART character size */
	     USART_MUX_LOOPBACK,		/** USART pin out */
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
	     PINMUX_UNUSED,		/** PAD2 pinmux */
	     PINMUX_UNUSED);		/** PAD3 pinmux */

  usart_enable(GPS_SERCOM);

  usart_write_wait(GPS_SERCOM, 0x34);
  usart_read_wait(GPS_SERCOM, &data);

  semihost_printf("Rx'ed: 0x%02x\n", data);
}

