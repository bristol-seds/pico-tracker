/*
 * Functions for the UBLOX 8 GPS
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
#include "ubx_messages.h"
#include "gps.h"

/**
 * UBX Constants
 */
const uint16_t ubx_header = (0xB5 | (0x62 << 8));

/**
 * UBX ACK Message Types
 */
enum {
  UBX_ACK_NACK	= (UBX_ACK | (0x00 << 8)),
  UBX_ACK_ACK	= (UBX_ACK | (0x01 << 8)),
};

/**
 * Internal buffers and things
 */
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

/**
 * UBX Messages
 */
volatile struct ubx_cfg_ant ubx_cfg_ant		= { .id = (UBX_CFG | (0x13 << 8)) };
volatile struct ubx_cfg_gnss ubx_cfg_gnss       = { .id = (UBX_CFG | (0x3E << 8)) };
volatile struct ubx_cfg_nav5 ubx_cfg_nav5	= { .id = (UBX_CFG | (0x24 << 8)) };
volatile struct ubx_cfg_tp5 ubx_cfg_tp5		= { .id = (UBX_CFG | (0x31 << 8)) };
volatile struct ubx_cfg_prt ubx_cfg_prt		= { .id = (UBX_CFG | (0x00 << 8)) };
volatile struct ubx_nav_posllh ubx_nav_posllh	= { .id = (UBX_NAV | (0x02 << 8)) };
volatile struct ubx_nav_timeutc ubx_nav_timeutc	= { .id = (UBX_NAV | (0x21 << 8)) };
volatile struct ubx_nav_sol ubx_nav_sol		= { .id = (UBX_NAV | (0x06 << 8)) };
volatile struct ubx_nav_status ubx_nav_status	= { .id = (UBX_NAV | (0x03 << 8)) };
/**
 * UBX Message Type List
 */
volatile ubx_message_t* const ubx_messages[] = {
  (ubx_message_t*)&ubx_cfg_ant,
  (ubx_message_t*)&ubx_cfg_gnss,
  (ubx_message_t*)&ubx_cfg_nav5,
  (ubx_message_t*)&ubx_cfg_tp5,
  (ubx_message_t*)&ubx_cfg_prt,
  (ubx_message_t*)&ubx_nav_posllh,
  (ubx_message_t*)&ubx_nav_timeutc,
  (ubx_message_t*)&ubx_nav_sol,
  (ubx_message_t*)&ubx_nav_status};

/**
 * Platform specific handlers
 */
#define _send_buffer(tx_data, length)			\
  usart_write_buffer_wait(GPS_SERCOM, tx_data, length)
#define _error_handler(error_type)			\
  /* TODO */



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
 * Processes UBX ack/nack packets
 */
void ubx_process_ack(ubx_message_id_t message_id, enum ubx_packet_state state)
{
  for (uint32_t i = 0; i < sizeof(ubx_messages)/sizeof(ubx_message_t*); i++) {
    if (message_id == ubx_messages[i]->id) { /* Match! */
      /* Set the message state */
      ubx_messages[i]->state = state;
    }
  }
}
/**
 * Process a single ubx frame. Runs in the IRQ so should be short and sweet.
 */
void ubx_process_frame(uint8_t* frame)
{
  uint16_t* frame16 = (uint16_t*)frame;
  uint16_t message_id = frame16[0];
  uint16_t payload_length = frame16[1];
  uint16_t checksum = ((uint16_t*)(frame + payload_length + 4))[0];
  uint16_t calculated_checksum = _ubx_checksum(frame, payload_length + 4);

  /* Checksum.. */
  if (calculated_checksum != checksum) {
    _error_handler(GPS_ERROR_BAD_CHECKSUM);
    return;
  }

  /** Parse the message ID */
  if (message_id == UBX_ACK_ACK) {
    /* Ack */
    ubx_process_ack(frame16[2], UBX_PACKET_ACK);
    return;

  } else if (message_id == UBX_ACK_NACK) {
    /* Nack */
    ubx_process_ack(frame16[2], UBX_PACKET_NACK);
    return;

  } else {
    /** Otherwise it could be a message frame, search for a type */
    for (uint32_t i = 0; i < sizeof(ubx_messages)/sizeof(ubx_message_t*); i++) {
      if (message_id == ubx_messages[i]->id) { // Match!
	/* Populate struct */
	memcpy((void*)(ubx_messages[i]+1), frame + 4, payload_length);

	return;
      }
    }
  }

  /* Unknown frame */
  _error_handler(GPS_ERROR_INVALID_FRAME);
}

/**
 * Rx Callback. Processes a stream of usart bytes
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
 * Sends a standard UBX message
 */
void _ubx_send_message(ubx_message_t* message, uint8_t* payload, uint16_t length)
{
  uint8_t ubx[UBX_BUFFER_LEN];
  uint8_t* ubx_buffer = ubx;

  /* Copy little endian */
  memcpy(ubx_buffer, &ubx_header, 2); ubx_buffer += 2; 	/* Header	*/
  memcpy(ubx_buffer, &message->id, 2); ubx_buffer += 2;	/* Message Type	*/
  memcpy(ubx_buffer, &length, 2); ubx_buffer += 2;	/* Length	*/
  memcpy(ubx_buffer, payload, length); ubx_buffer += length; /* Payload */
  uint16_t checksum = _ubx_checksum(ubx + 2, length + 4);
  memcpy(ubx_buffer, &checksum, 2); ubx_buffer += 2;	/* Checksum	*/

  _send_buffer(ubx, length + 8);
}
/**
 * Polls the GPS for packets
 */
void _ubx_poll(ubx_message_t* message) {
  /* Clear the packet state */
  message->state = UBX_PACKET_WAITING;

  _ubx_send_message(message, NULL, 0);

  /* Wait for acknoledge */
  while (message->state == UBX_PACKET_WAITING);
}

/**
 * Disable NMEA messages on the uBlox
 */
void gps_disable_nmea(void)
{
  ubx_cfg_prt.payload.portID = 1;
  ubx_cfg_prt.payload.res0 = 0;
  ubx_cfg_prt.payload.txReady = 0;
  ubx_cfg_prt.payload.mode = 0x08D0; /* 8 bit, No Parity */
  ubx_cfg_prt.payload.baudRate = 9600;
  ubx_cfg_prt.payload.inProtoMask = 0x7; /* UBX */
  ubx_cfg_prt.payload.outProtoMask = 0x1; /* UBX */
  ubx_cfg_prt.payload.flags = 0;

  _ubx_send_message((ubx_message_t*)&ubx_cfg_prt,
		    (uint8_t*)&ubx_cfg_prt.payload,
		    sizeof(ubx_cfg_prt.payload));

  for (int i = 0; i < 1000*100; i++);
}

/**
 * Sends messages to the GPS to update the messages we want
 */
void gps_update()
{
  _ubx_send_message((ubx_message_t*)&ubx_nav_posllh, NULL, 0);
  _ubx_send_message((ubx_message_t*)&ubx_nav_sol, NULL, 0);
  _ubx_send_message((ubx_message_t*)&ubx_nav_timeutc, NULL, 0);
  _ubx_send_message((ubx_message_t*)&ubx_nav_status, NULL, 0);
  _ubx_send_message((ubx_message_t*)&ubx_cfg_gnss, NULL, 0);
}
/**
 * Return the latest received messages
 */
struct ubx_nav_posllh gps_get_nav_posllh()
{
  return ubx_nav_posllh;
}
struct ubx_nav_sol gps_get_nav_sol()
{
  return ubx_nav_sol;
}
struct ubx_nav_timeutc gps_get_nav_timeutc()
{
  return ubx_nav_timeutc;
}

/**
 * Verify that the uBlox 6 GPS receiver is set to the <1g airborne
 * navigaion mode.
 */
void gps_set_platform_model(void)
{
  /* Send the poll request */
  _ubx_poll((ubx_message_t*)&ubx_cfg_nav5);

  /* If we need to update */
  if (ubx_cfg_nav5.payload.dynModel != GPS_PLATFORM_MODEL) {
    /* Update */
    ubx_cfg_nav5.payload.dynModel = GPS_PLATFORM_MODEL;
    /* Send */
    _ubx_send_message((ubx_message_t*)&ubx_cfg_nav5,
		      (uint8_t*)&ubx_cfg_nav5.payload,
		      sizeof(ubx_cfg_nav5.payload));
  }
}

/**
 * Set the GPS timepulse settings using the CFG_TP5 message
 */
void gps_set_timepulse_five(uint32_t frequency)
{
  /* Send the Request */
  _ubx_poll((ubx_message_t*)&ubx_cfg_tp5);

  /* Define the settings we want */
  ubx_cfg_tp5.payload.tpIdx = 0;
  ubx_cfg_tp5.payload.antCableDelay = 50; /* 50 nS	*/
  /* GPS time, duty cyle, frequency, lock to GPS, active */
  ubx_cfg_tp5.payload.flags = 0x80 | 0x8 | 0x3;
  ubx_cfg_tp5.payload.freqPeriod = frequency;
  ubx_cfg_tp5.payload.pulseLenRatio = 0x80000000; /* 50 % duty cycle*/

  /* Write the new settings */
  _ubx_send_message((ubx_message_t*)&ubx_cfg_tp5,
		    (uint8_t*)&ubx_cfg_tp5.payload,
		    sizeof(ubx_cfg_tp5.payload));
}
/**
 * Set which GNSS constellations to use
 */
void gps_set_gnss(void)
{
  /* Read the current settings */
  _ubx_poll((ubx_message_t*)&ubx_cfg_gnss);

  switch (ubx_cfg_gnss.payload.msgVer) {
    case 0:
      /* For each configuration block */
      for (uint8_t i = 0; i < ubx_cfg_gnss.payload.numConfigBlocks; i++) {

	/* If it's the configuration for something other than GPS */
	if (ubx_cfg_gnss.payload.block[i].gnssID != UBX_GNSS_GPS) {

	  /* Disable this GNSS system */
	  ubx_cfg_gnss.payload.block[i].flags &= ~0x1;
	}
      }
      break;
    default:
      break;
  }

  /* Write the new settings */
  _ubx_send_message((ubx_message_t*)&ubx_cfg_gnss,
		    (uint8_t*)&ubx_cfg_gnss.payload,
		    4 + (8 * ubx_cfg_gnss.payload.numConfigBlocks));
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

  /* Set the platform model */
  gps_set_platform_model();

  /* Set which GNSS constellation we'd like to use */
  gps_set_gnss();

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
