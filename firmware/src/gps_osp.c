/*
 * Functions for SiRFstar IV GPS chipset
 * Copyright (C) 2014, 2015  Richard Meadows <richardeoin>
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
#include "hw_config.h"

#ifdef GPS_TYPE_OSP

#include "system/system.h"
#include "system/port.h"
#include "system/interrupt.h"
#include "sercom/sercom.h"
#include "sercom/usart.h"
#include "util/dbuffer.h"
#include "osp_messages.h"
#include "gps.h"
#include "watchdog.h"

/**
 * Internal buffers and things
 */
#define OSP_BUFFER_LEN	0x406   /* Payload up to 1023 bytes, plus len, check, efd */
#define SFD1	0xA0
#define SFD2	0xA2
#define EFD1	0xB0
#define EFD2	0xB3

enum sfd_state {
  SFD_WAITING = -1,
  SFD_GOT1,
} sfd_state = SFD_WAITING;

int32_t osp_index = SFD_WAITING;
uint16_t osp_payload_length = 0;
uint8_t osp_irq_buffer[OSP_BUFFER_LEN];

volatile enum gps_error_t gps_error_state;

/* Temp??? */
uint8_t gps_make_oktosend = 0;

/**
 * OSP Output Ack/Nack
 */
#define OSP_OUT_ACK_ID  11      /* Command Acknowledgement */
#define OSP_OUT_NACK_ID 12      /* Command Negative Acknowledgement */

/**
 * OSP Output Messages
 */
volatile struct osp_out_measure_navigation_data_out osp_out_measure_navigation_data_out	= {
  .id		= OSP_OUT_MEASURE_NAVIGATION_DATA_OUT_ID,
  .state	= OSP_PACKET_WAITING,
  .max_payload_size = sizeof(osp_out_measure_navigation_data_out.payload)
};
volatile struct osp_out_clock_status_data osp_out_clock_status_data = {
  .id		= OSP_OUT_CLOCK_STATUS_DATA_ID,
  .state	= OSP_PACKET_WAITING,
  .max_payload_size = sizeof(osp_out_clock_status_data.payload)
};
volatile struct osp_out_ephemeris_data osp_out_ephemeris_data = {
  .id		= OSP_OUT_EPHEMERIS_DATA_ID,
  .state	= OSP_PACKET_WAITING,
  .max_payload_size = sizeof(osp_out_ephemeris_data.payload)
};
volatile struct osp_out_oktosend osp_out_oktosend = {
  .id		= OSP_OUT_OKTOSEND_ID,
  .state	= OSP_PACKET_WAITING,
  .max_payload_size = sizeof(osp_out_oktosend.payload)
};
volatile struct osp_out_geodetic_navigation_data osp_out_geodetic_navigation_data = {
  .id		= OSP_OUT_GEODETIC_NAVIGATION_DATA_ID,
  .state	= OSP_PACKET_WAITING,
  .max_payload_size = sizeof(osp_out_geodetic_navigation_data.payload)
};
volatile struct osp_out_1pps_time osp_out_1pps_time = {
  .id		= OSP_OUT_1PPS_TIME_ID,
  .state	= OSP_PACKET_WAITING,
  .max_payload_size = sizeof(osp_out_1pps_time.payload)
};
volatile struct osp_out_gpio_state osp_out_gpio_state = {
  .id		= OSP_OUT_GPIO_STATE_ID,
  .state	= OSP_PACKET_WAITING,
  .max_payload_size = sizeof(osp_out_gpio_state.payload)
};
volatile struct osp_out_hw_config_req osp_out_hw_config_req = {
  .id		= OSP_OUT_HW_CONFIG_REQ_ID,
  .state	= OSP_PACKET_WAITING,
  .max_payload_size = sizeof(osp_out_hw_config_req.payload)
};
volatile struct osp_out_aiding_request osp_out_aiding_request = {
  .id		= OSP_OUT_AIDING_REQUEST_ID,
  .state	= OSP_PACKET_WAITING,
  .max_payload_size = sizeof(osp_out_aiding_request.payload)
};
volatile struct osp_out_cw_controller_output osp_out_cw_controller_output = {
  .id		= OSP_OUT_CW_CONTROLLER_OUTPUT_ID,
  .state	= OSP_PACKET_WAITING,
  .max_payload_size = sizeof(osp_out_cw_controller_output.payload)
};

/**
 * OSP Output Messages
 */
volatile osp_message_t* const osp_out_messages[] = {
  (osp_message_t*)&osp_out_measure_navigation_data_out,
  (osp_message_t*)&osp_out_clock_status_data,
  (osp_message_t*)&osp_out_ephemeris_data,
  (osp_message_t*)&osp_out_oktosend,
  (osp_message_t*)&osp_out_geodetic_navigation_data,
  (osp_message_t*)&osp_out_1pps_time,
  (osp_message_t*)&osp_out_gpio_state,
  (osp_message_t*)&osp_out_hw_config_req,
  (osp_message_t*)&osp_out_aiding_request,
  (osp_message_t*)&osp_out_cw_controller_output,
};

/**
 * OSP Input Messages
 *
 * Instances are of the generic type (no payload) to save ram
 */
volatile osp_message_t osp_in_advanced_power_measurement = {
  .id		= OSP_IN_ADVANCED_POWER_MEASUREMENT_ID,
  .state	= OSP_PACKET_WAITING
};
volatile osp_message_t osp_in_initialise_data_source = {
  .id		= OSP_IN_INITIALISE_DATA_SOURCE_ID,
  .state	= OSP_PACKET_WAITING
};
volatile osp_message_t osp_in_mode_control = {
  .id		= OSP_IN_MODE_CONTROL_ID,
  .state	= OSP_PACKET_WAITING
};
volatile osp_message_t osp_in_elevation_mask = {
  .id		= OSP_IN_ELEVATION_MASK_ID,
  .state	= OSP_PACKET_WAITING
};
volatile osp_message_t osp_in_power_mask = {
  .id		= OSP_IN_POWER_MASK_ID,
  .state	= OSP_PACKET_WAITING
};
volatile osp_message_t osp_in_set_tricklepower_parameters = {
  .id		= OSP_IN_SET_TRICKLEPOWER_PARAMETERS_ID,
  .state	= OSP_PACKET_WAITING
};
volatile osp_message_t osp_in_set_message_rate = {
  .id		= OSP_IN_SET_MESSAGE_RATE_ID,
  .state	= OSP_PACKET_WAITING
};
volatile osp_message_t osp_in_set_low_power_acquisition_parameters = {
  .id		= OSP_IN_SET_LOW_POWER_ACQUISITION_PARAMETERS_ID,
  .state	= OSP_PACKET_WAITING
};
volatile osp_message_t osp_in_session_request = {
  .id		= OSP_IN_SESSION_REQUEST_ID,
  .state	= OSP_PACKET_WAITING
};
volatile osp_message_t osp_in_hardware_configuration_response = {
  .id		= OSP_IN_HARDWARE_CONFIGURATION_RESPONSE_ID,
  .state	= OSP_PACKET_WAITING
};
volatile osp_message_t osp_in_approximate_ms_position_response = {
  .id		= OSP_IN_APPROXIMATE_MS_POSITION_RESPONSE_ID,
  .state	= OSP_PACKET_WAITING
};
volatile osp_message_t osp_in_power_mode_request = {
  .id		= OSP_IN_POWER_MODE_REQUEST_ID,
  .state	= OSP_PACKET_WAITING
};


/**
 * OSP Input Messages
 */
volatile osp_message_t* const osp_in_messages[] = {
  &osp_in_advanced_power_measurement,
  &osp_in_initialise_data_source,
  &osp_in_mode_control,
  &osp_in_elevation_mask,
  &osp_in_power_mask,
  &osp_in_set_tricklepower_parameters,
  &osp_in_set_message_rate,
  &osp_in_set_low_power_acquisition_parameters,
  &osp_in_session_request,
  &osp_in_hardware_configuration_response,
  &osp_in_approximate_ms_position_response,
  &osp_in_power_mode_request,
};

/**
 * Platform specific handlers
 */
#define _send_buffer(tx_data, length)			\
  do { usart_write_buffer_wait(GPS_SERCOM, tx_data, length); } while (0)
#define _error_handler(error_type)			\
  do { gps_error_state = error_type; } while (0)


/**
 * Harware ON_OFF pulse
 */
void gps_se_on_off_pulse(void)
{
  port_pin_set_config(GPS_SE_ON_OFF_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */
  port_pin_set_output_level(GPS_SE_ON_OFF_PIN, 1);	/* pulse high */
  for (int i = 0; i < 90*4; i++);
  port_pin_set_output_level(GPS_SE_ON_OFF_PIN, 0);
}



/**
 * Calculate a OSP checksum
 */
uint16_t _osp_checksum(uint8_t* data, uint8_t len)
{
  uint16_t checksum = 0;

  for (uint8_t i = 0; i < len; i++) {
    /* Sum */
    checksum += *data;
    data++;

    /* Mask */
    checksum &= 0x7FFF;
  }

  return checksum;
}

/**
 * Processes OSP ack/nack packets
 */
void osp_process_ack(osp_message_id_t message_id, enum osp_packet_state state)
{
  for (uint32_t i = 0; i < sizeof(osp_in_messages)/sizeof(osp_message_t*); i++) {
    if (message_id == osp_in_messages[i]->id) { /* Match! */
      /* Set the message state */
      osp_in_messages[i]->state = state;
    }
  }
}
#include "init.h"
/**
 * Process a single osp frame. Runs in the IRQ so should be short and sweet.
 */
void osp_process_frame(uint8_t* frame, uint16_t payload_length)
{
  uint8_t message_id = frame[2];
  uint16_t payload_length_less_message_id = payload_length-1;

  uint16_t checksum = (frame[2+payload_length] << 8) | frame[2+payload_length+1];
  uint16_t calculated_checksum = _osp_checksum(frame+2, payload_length);

  /* Checksum.. */
  if (calculated_checksum != checksum) {
    _error_handler(GPS_ERROR_BAD_CHECKSUM);
    return;
  }

  /** Parse the message ID */
  if (message_id == OSP_OUT_ACK_ID) {
    /* Ack */
    osp_process_ack(frame[3], OSP_PACKET_ACK);
    return;

  } else if (message_id == OSP_OUT_NACK_ID) {
    /* Nack */
    osp_process_ack(frame[3], OSP_PACKET_NACK);
    return;

  } else {
    /* Search for a frame we recognise */
    for (uint32_t i = 0; i < sizeof(osp_out_messages)/sizeof(osp_message_t*); i++) {

      if (message_id == osp_out_messages[i]->id) { // Match!

        /* Check that frame len is LESS THAN or equal to expected packet size */
        /* (less than for various subfields which have different lengths) */
        if (payload_length_less_message_id <= osp_out_messages[i]->max_payload_size) {

          /* Populate struct. Ignore message ID */
          memcpy((void*)(osp_out_messages[i]+1), frame+2+1, payload_length_less_message_id);

          /* Set the message state */
          osp_out_messages[i]->state = OSP_PACKET_UPDATED;

          if(message_id==41){led_toggle();}

        }

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
      osp_index = 0;
      osp_payload_length = 0;
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
  if (osp_index != SFD_WAITING &&
      osp_index < (2 + osp_payload_length + 2 + 2) &&
      osp_index < OSP_BUFFER_LEN) {

    /* Data in */
    osp_irq_buffer[osp_index++] = data;

    /* Extract length from first two bytes */
    if (osp_index == 2) {
      osp_payload_length = (osp_irq_buffer[0] << 8) | osp_irq_buffer[1];
    }

    /* Is frame complete? (len + payload + checksum + end) */
    if ((osp_index == (2 + osp_payload_length + 2 + 2)) &&
        (osp_irq_buffer[osp_index-2] == EFD1) &&
        (osp_irq_buffer[osp_index-1] == EFD2)) {
      /* Process it */
      osp_process_frame(osp_irq_buffer, osp_payload_length);
    }
  }
}

/**
 * Sends a standard OSP message
 */
void _osp_send_message(osp_message_t* message, uint8_t* payload, uint16_t length)
{
  uint8_t osp[OSP_BUFFER_LEN];

  /* Packet waiting state */
  message->state = OSP_PACKET_WAITING;

  /* Increase length to include message id */
  length++;

  /* Start of Frame */
  osp[0] = SFD1;
  osp[1] = SFD2;
  /* Length, Big Endian */
  osp[2] = (length >> 8) & 0xFF;
  osp[3] = (length >> 0) & 0xFF;

  /* Message ID	*/
  osp[4] = message->id;
  /* Payload */
  memcpy(&osp[5], payload, length-1);

  /* Checksum, Big Endian */
  uint16_t checksum = _osp_checksum(osp+2+2, length);
  osp[4+length+0] = (checksum >> 8) & 0xFF;
  osp[4+length+1] = (checksum >> 0) & 0xFF;
  /* End of Frame */
  osp[4+length+2] = EFD1;
  osp[4+length+3] = EFD2;

  _send_buffer(osp, 4 + length + 4);
}


/**
 * =============================================================================
 * Getters   ===================================================================
 * =============================================================================
 */



/**
 * Gets the current error state of the GPS to check validity of last
 * request
 */
enum gps_error_t gps_get_error_state(void)
{
  return gps_error_state;
}

/**
 * Returns the most recent datapint from the gps
 *
 * Uses the Geodetic Navigation Data frame.
 */
struct gps_data_t gps_get_data(void)
{
  struct gps_data_t data;

  data.is_locked = (osp_out_geodetic_navigation_data.payload.nav_type == 0)?1:0; /* nav_valid = 0? */
  data.latitude = osp_out_geodetic_navigation_data.payload.latitude; /* hndeg */
  data.longitude = osp_out_geodetic_navigation_data.payload.longitude; /* hndeg */
  data.altitude = osp_out_geodetic_navigation_data.payload.altitude_from_msl*10; /* cm -> mm */
  data.satillite_count = osp_out_geodetic_navigation_data.payload.svs_in_fix;

  return data;
}


/**
 * =============================================================================
 * Commands  ===================================================================
 * =============================================================================
 */


/**
 * Disable messages we're not interested in
 */
void osp_set_messages(void)
{
  struct osp_in_set_message_rate config;
  memset(&config, 0, sizeof(struct osp_in_set_message_rate));
  config.id = OSP_IN_SET_MESSAGE_RATE_ID;

  /* Disable default nav (2, 4)*/
  config.payload.mode = OSP_SET_MESSAGE_RATE_DEFAULT_NAV;
  config.payload.update_rate = 0; /* No updates */
  _osp_send_message((osp_message_t*)&config,
                    (uint8_t*)&config.payload,
                    sizeof(config.payload));

  /* Disable CW controller messages (92) */
  config.payload.mode = OSP_SET_MESSAGE_RATE_ONE;
  config.payload.message_id = OSP_OUT_CW_CONTROLLER_OUTPUT_ID;
  config.payload.update_rate = 0; /* No updates */
  _osp_send_message((osp_message_t*)&config,
                    (uint8_t*)&config.payload,
                    sizeof(config.payload));

  /* Enable geodetic nav */
  config.payload.mode = OSP_SET_MESSAGE_RATE_ONE;
  config.payload.message_id = OSP_OUT_GEODETIC_NAVIGATION_DATA_ID;
  config.payload.update_rate = 1; /* Once per second */
  _osp_send_message((osp_message_t*)&config,
                    (uint8_t*)&config.payload,
                    sizeof(config.payload));
}

/**
 * Respond to hardware configuration request
 */
void osp_send_hw_config_resp(void)
{
  /* hw config resp */
  struct osp_in_hardware_configuration_response config;
  memset(&config, 0, sizeof(struct osp_in_hardware_configuration_response));
  config.id = OSP_IN_HARDWARE_CONFIGURATION_RESPONSE_ID;

  /* hw config options */
  config.payload.hw_config = (
    OSP_HW_CONFIG_PRECISE_TIME_TRANSFER_NO | /* Time */
    OSP_HW_CONFIG_PRECISE_TIME_CP_TO_SLC |
    OSP_HW_CONFIG_FREQUENCY_TRANSFER_NO | /* Frequency */
    OSP_HW_CONFIG_FREQUENCY_TRANSFER_NO_COUNTER |
    OSP_HW_CONFIG_RTC_AVAILABLE_YES | /* RTC */
    OSP_HW_CONFIG_RTC_INTERNAL |
    OSP_HW_CONFIG_COARSE_TIME_TRANSFER_NO |
    OSP_HW_CONFIG_FREQUENCY_REFCLK_OFF
    );
  config.payload.nominal_frequency_high = 0;
  config.payload.nominal_frequency_low = __REV(1*1000); /* 1Hz?? */
  config.payload.network_enhancement_type = 0;   /* No enhancement */

  _osp_send_message((osp_message_t*)&config,
                    (uint8_t*)&config.payload,
                    sizeof(config.payload));
}

/**
 * Respond to position aiding request
 */
void osp_send_approximate_ms_postion(void)
{
  /* approximate ms position */
  struct osp_in_approximate_ms_position_response config;
  memset(&config, 0, sizeof(struct osp_in_approximate_ms_position_response));
  config.id = OSP_IN_APPROXIMATE_MS_POSITION_RESPONSE_ID;

  /* values */
  config.payload.sub_id = 1;    /* approx position */
  config.payload.lat = 0;       /* no idea */
  config.payload.lon = 0;       /* no idea */
  config.payload.alt = (2000+500)*10; /* no idea. say 2000m */
  config.payload.est_hor_er = 0xFF;   /* maximum error */
  config.payload.est_ver_er = 0xFFFF; /* maximum error */
  config.payload.use_alt_aiding = 0;  /* don't even use the altitude */

  /* send */
  _osp_send_message((osp_message_t*)&config,
                    (uint8_t*)&config.payload,
                    sizeof(config.payload));

}


/**
 * =============================================================================
 * Serivce   ===================================================================
 * =============================================================================
 */

/**
 * Handle connection-orientated messages from the GPS that need servicing
 */
void gps_service(void)
{
  /* We received the hw config req */
  if (osp_out_hw_config_req.state == OSP_PACKET_UPDATED) {
    /* send response */
    osp_send_hw_config_resp();

    /* clear state */
    osp_out_hw_config_req.state = OSP_PACKET_WAITING;
  }

  /* aiding request */
  if (osp_out_aiding_request.state == OSP_PACKET_UPDATED) {

    if (osp_out_aiding_request.payload.subid ==
        OSP_AIDING_REQUEST_SUBID_APPROXIMATE_MS_POSITION) {
      /* send response to approximate ms position request */
      osp_send_approximate_ms_postion();

      osp_set_messages();

      gps_make_oktosend = 0;  /* all done */
    }

    /* clear state */
    osp_out_aiding_request.state = OSP_PACKET_WAITING;
  }


  /* We're trying to make it oktosend */
  if (gps_make_oktosend == 1) {
    /* And we've had an oktosend packet */
    if (osp_out_oktosend.state == OSP_PACKET_UPDATED) {

      /* was this an enable or disable packet? */
      if (osp_out_oktosend.payload.oktosend == OSP_OKTOSEND_YES) {
        /* all done */
      } else {
        gps_se_on_off_pulse();  /* send another on_off pulse */
      }

      /* clear state */
      osp_out_oktosend.state = OSP_PACKET_WAITING;
    }
  }

}

/**
 * =============================================================================
 * Setup      ==================================================================
 * =============================================================================
 */

/**
 * Init + enable for the usart at the given baud rate
 */
void gps_usart_init_enable(uint32_t baud_rate)
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
	     baud_rate,      /** USART Baud rate */
	     true,	     /** Enable receiver */
	     true,	     /** Enable transmitter */
	     false,	     /** Sample on the rising edge of XLCK */
	     false,	     /** Use the external clock applied to the XCK pin. */
	     0,		     /** External clock frequency in synchronous mode. */
	     true,	     /** Run in standby */
	     GPS_GCLK,			/** GCLK generator source */
	     GPS_SERCOM_MOGI_PINMUX, 		/** PAD0 pinmux */
	     GPS_SERCOM_MIGO_PINMUX,		/** PAD1 pinmux */
	     PINMUX_UNUSED,			/** PAD2 pinmux */
	     PINMUX_UNUSED);			/** PAD3 pinmux */

  usart_enable(GPS_SERCOM);
}

/**
 * GPS reset pin
 */
void gps_reset_on(void)
{
#ifdef GPS_RESET_PIN
  port_pin_set_output_level(GPS_RESET_PIN, 0); /* active low */
#endif
}
void gps_reset_off(void)
{
#ifdef GPS_RESET_PIN
  port_pin_set_output_level(GPS_RESET_PIN, 1); /* active low */
#endif
}
/**
 * Reset. Places the GPS in a RESET state
 */
void gps_reset(void)
{
#ifdef GPS_RESET_PIN
  port_pin_set_config(GPS_RESET_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */
  port_pin_set_output_level(GPS_RESET_PIN, 0);	/* active low */
#endif
}

/**
 * Init
 */
void gps_init(void)
{
  /* Bring GPS out of reset */
  gps_reset_off();

  /* Enable usart */
  gps_usart_init_enable(GPS_BAUD_RATE);

  /* Incoming osp messages are handled in an irq */
  usart_register_rx_callback(GPS_SERCOM, gps_rx_callback, GPS_SERCOM_INT_PRIO);

  /* ---- GPS Configuration ---- */

  /* Bring up the GPS */
  gps_make_oktosend = 1;
  gps_se_on_off_pulse();

  while (gps_make_oktosend == 1) {
    gps_service();
  }
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
	     GPS_BAUD_RATE,  /** USART Baud rate */
	     true,	     /** Enable receiver */
	     true,	     /** Enable transmitter */
	     false,	     /** Sample on the rising edge of XLCK */
	     false,	     /** Use the external clock applied to the XCK pin. */
	     0,		     /** External clock frequency in synchronous mode. */
	     false,	     /** Run in standby */
	     GPS_GCLK,		/** GCLK generator source */
	     GPS_SERCOM_MOGI_PINMUX, 	/** PAD0 pinmux */
	     GPS_SERCOM_MIGO_PINMUX,	/** PAD1 pinmux */
	     PINMUX_UNUSED,		/** PAD2 pinmux */
	     PINMUX_UNUSED);		/** PAD3 pinmux */

  usart_enable(GPS_SERCOM);

  usart_write_wait(GPS_SERCOM, 0x34);
  usart_read_wait(GPS_SERCOM, &data);

  usart_disable(GPS_SERCOM);
}

#endif /* GPS_TYPE_OSP */
