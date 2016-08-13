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
#include "init.h"               /* leds etc */

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

volatile enum gps_error gps_error_state;

/**
 * GPS Active?
 */
enum gps_power_state {
  GPS_HIBERNATE	= 0,
  GPS_ACTIVE	= 1,
};
enum gps_power_state gps_power_state = GPS_HIBERNATE;

/**
 * Flight State
 */
enum gps_flight_state gps_flight_state = GPS_FLIGHT_STATE_LAUNCH;

/**
 * Lock State
 */
enum gps_lock_state gps_is_locked_priv = GPS_NO_LOCK;

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
volatile struct osp_out_position_response osp_out_position_response = {
  .id		= OSP_OUT_POSITION_RESPONSE_ID,
  .state	= OSP_PACKET_WAITING,
  .max_payload_size = sizeof(osp_out_position_response.payload)
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
  (osp_message_t*)&osp_out_position_response,
  (osp_message_t*)&osp_out_hw_config_req,
  (osp_message_t*)&osp_out_aiding_request,
  (osp_message_t*)&osp_out_cw_controller_output,
};
/**
 * OSP Output Post Processing functions
 * ** same order as above **
 */
const osp_post_func osp_out_post_functions[] = {
  osp_out_measure_navigation_data_out_post,
  osp_out_clock_status_data_post,
  osp_out_ephemeris_data_post,
  osp_out_oktosend_post,
  osp_out_geodetic_navigation_data_post,
  osp_out_1pps_time_post,
  osp_out_gpio_state_post,
  osp_out_position_response_post,
  osp_out_hw_config_req_post,
  osp_out_aiding_request_post,
  osp_out_cw_controller_output_post,
};


/**
 * OSP Input Messages
 *
 * Instances are of the generic type (no payload) to save ram
 */
volatile osp_message_t osp_in_advanced_power_management = {
  .id		= OSP_IN_ADVANCED_POWER_MANAGEMENT_ID,
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
volatile osp_message_t osp_in_set_position_request = {
  .id		= OSP_IN_POSITION_REQUEST_ID,
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
volatile osp_message_t osp_in_reject = {
  .id		= OSP_IN_REJECT_ID,
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
  &osp_in_advanced_power_management,
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
  for (int i = 0; i < 90*48; i++) { __NOP(); }       /* > 90us */
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

          /* Reject measurement response for the moment */
          if (message_id == OSP_OUT_POSITION_RESPONSE_ID && (frame+2+1)[0] != 1) {
            break;
          }

          /* Populate struct. Ignore message ID */
          memcpy((void*)(osp_out_messages[i]+1), frame+2+1, payload_length_less_message_id);

          /* Set the message state */
          osp_out_messages[i]->state = OSP_PACKET_UPDATED;

          /* Call post function for this message */
          osp_out_post_functions[i](osp_out_messages[i]);
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
 * Flight State  ================================================================
 * =============================================================================
 */

/**
 * Returns current flight state
 */
enum gps_flight_state gps_get_flight_state(void)
{
  return gps_flight_state;
}

/**
 * Sets flight state
 *
 * altitude in mm
 */
void gps_set_flight_state(int32_t altitude)
{
  if (altitude > GPS_FLIGHT_STATE_THREASHOLD_M*1000) {
    gps_flight_state = GPS_FLIGHT_STATE_FLOAT;
  } else {
    gps_flight_state = GPS_FLIGHT_STATE_LAUNCH;
  }
}


/**
 * =============================================================================
 * Power State  ================================================================
 * =============================================================================
 */

/**
 * Puts the GPS into hibernate state
 */
void gps_make_active(void)
{
  if (gps_power_state == GPS_HIBERNATE) {
    gps_se_on_off_pulse();
    gps_power_state = GPS_ACTIVE;
  }
}
/**
 * Puts the GPS into hibernate state
 */
void gps_make_hibernate(void)
{
  if (gps_power_state == GPS_ACTIVE) {
    gps_se_on_off_pulse();
    gps_power_state = GPS_HIBERNATE;
  }
}

/**
 * =============================================================================
 * Lock State ==================================================================
 * =============================================================================
 */

/**
 * Lock state. Set by gps_get_data
 */
enum gps_lock_state gps_is_locked(void)
{
  return gps_is_locked_priv;
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
enum gps_error gps_get_error_state(void)
{
  return gps_error_state;
}

/**
 * Powers up the GPS, waits up to 60 seconds for a fix
 *
 * Uses the Geodetic Navigation Data frame.
 */
struct gps_data_t gps_get_data(void)
{
  struct gps_data_t data;
  uint32_t i;

  /* Clear status */
  osp_out_geodetic_navigation_data.state = OSP_PACKET_WAITING;

  /* Take the GPS out of hibernate*/
  gps_make_active();

  for (i = 0; i < 60; i++) {    /* 60 seconds */

    while (osp_out_geodetic_navigation_data.state != OSP_PACKET_UPDATED) {
      /* idle */
      idle(IDLE_WAIT_FOR_GPS);
    }

    /* Clear status */
    osp_out_geodetic_navigation_data.state = OSP_PACKET_WAITING;

    /* Got packet */
    if (gps_get_flight_state() == GPS_FLIGHT_STATE_LAUNCH) {
      led_toggle();
    }

    /* Check nav */
    if ((osp_out_geodetic_navigation_data.payload.nav_valid == 0) &&        /* Valid fix */
        ((osp_out_geodetic_navigation_data.payload.nav_type & 0x7) != 0) && /* Currently have this fix */
        (osp_out_geodetic_navigation_data.payload.estimated_vertical_position_error < 100*100)) /* < 100m error */
    {
      /* GPS back to hibernate */
      gps_make_hibernate();

      data.is_locked = 1; /* valid fix */
      gps_is_locked_priv = 1;
      data.latitude = osp_out_geodetic_navigation_data.payload.latitude; /* hndeg */
      data.longitude = osp_out_geodetic_navigation_data.payload.longitude; /* hndeg */
      data.altitude = osp_out_geodetic_navigation_data.payload.altitude_from_msl*10; /* cm -> mm */
      data.satillite_count = osp_out_geodetic_navigation_data.payload.svs_in_fix;
      data.time_to_first_fix = i+1; /* number of geo nav packets this took */

      data.year = osp_out_geodetic_navigation_data.payload.utc_year;
      data.month = osp_out_geodetic_navigation_data.payload.utc_month;
      data.day = osp_out_geodetic_navigation_data.payload.utc_day;
      data.hour = osp_out_geodetic_navigation_data.payload.utc_hour;
      data.minute = osp_out_geodetic_navigation_data.payload.utc_minute;
      data.second = (osp_out_geodetic_navigation_data.payload.utc_second+500)/1000; /* ms -> s */

      gps_set_flight_state(data.altitude);

      /* turn the led off! */
      led_off();

      return data;
    }
  }

  /* Leave the GPS on to try to get a lock */

  /* turn the led off! */
  led_off();

  /* invalid */
  memset(&data, 0, sizeof(struct gps_data_t));
  gps_is_locked_priv = 0;       /* not locked */

  /* We can still return some data */
  data.time_to_first_fix = i;
  data.hour = osp_out_geodetic_navigation_data.payload.utc_hour;
  data.minute = osp_out_geodetic_navigation_data.payload.utc_minute;
  data.second = (osp_out_geodetic_navigation_data.payload.utc_second+500)/1000; /* ms -> s */

  return data;
}


/**
 * =============================================================================
 * Wrapped get data ============================================================
 * =============================================================================
 */

/* Number of re-inits made without normal operation */
uint32_t gd_reinit_count = 0;
#define GD_REINIT_COUNT_MAX (3)     /* 3 reinits before we give up and go to the watchdog */

/* Number of times gps_get_data called */
uint32_t gd_count = 0;
#define GD_COUNT_MAX (720) /* GPS is good for about a day @30 per hour */

/* No lock */
uint32_t gd_nolock_count = 0;
#define GD_NOLOCK_COUNT_MAX (10) /* 10 minutes will always be enough to get a lock */

/* Position invalid */
uint8_t gd_invalid_count = 0;
#define GD_INVALID_COUNT_MAX	(5)
/* It may be outside the rannge of altitudes we expect */
#define GD_ALTITUDE_50M		(50*1000)       /* mm */
#define GD_ALTITUDE_20KM	(20*1000*1000)  /* mm */
/* Or it may be _exactly_ the same as the previous, which is unexpected */
int32_t gd_last_latitude, gd_last_longitude;
int32_t gd_last_altitude;

#define GD_INVALID_ALTITUDE_MIN GD_ALTITUDE_50M
#define GD_INVALID_ALTITUDE_MAX GD_ALTITUDE_20KM

/**
 * gps_get_data, but with re-initialisation as required
 */
struct gps_data_t gps_get_data_wrapped(void)
{
  if ((gd_count >= GD_COUNT_MAX) ||
      (gd_nolock_count >= GD_NOLOCK_COUNT_MAX) ||
      (gd_invalid_count >= GD_INVALID_COUNT_MAX)) {

    /* do something to reset the GPS */
    if (gd_reinit_count >= GD_REINIT_COUNT_MAX) {
      while(1);                 /* wait for watchdog */
    } else {
      /* reinitialise */
      gps_reinit();
      gd_reinit_count++;
      /* and reset this */
      gd_count = 0;
      gd_nolock_count = 0;
      gd_invalid_count = 0;
    }
  }

  struct gps_data_t data = gps_get_data();

  /* Always increment count */
  gd_count++;

  /* No lock */
  if (data.is_locked == 0) {
    gd_nolock_count++;
  } else {
    gd_nolock_count = 0;

    /* if we do have a lock, might still be invalid */
    if (((data.altitude < GD_INVALID_ALTITUDE_MIN) ||
         (data.altitude > GD_INVALID_ALTITUDE_MAX)) || /* altitude out of range */
        ((data.latitude  == gd_last_latitude ) &&
         (data.longitude == gd_last_longitude) &&
         (data.altitude  == gd_last_altitude))) { /* OR _all_ position values _exactly_ as before */
      gd_invalid_count++;
    } else {
      /* everything is working well */
      gd_invalid_count = 0;
      gd_reinit_count  = 0;
    }
  }

  /* record values */
  gd_last_latitude  = data.latitude;
  gd_last_longitude = data.longitude;
  gd_last_altitude  = data.altitude;

  return data;
}

/**
 * =============================================================================
 * Low Power ===================================================================
 * =============================================================================
 */

/**
 * Request Full power mode
 */
void osp_session_request_close(void)
{
  /* session request #213 */
  struct osp_in_session_request config;
  memset(&config, 0, sizeof(struct osp_in_session_request));
  config.id = OSP_IN_SESSION_REQUEST_ID;

  /* values */
  config.payload.sub_id = OSP_SESSION_REQUEST_CLOSE;
  config.payload.info = 0;      /* Session close requested */

  /* send */
  _osp_send_message((osp_message_t*)&config,
                    (uint8_t*)&config.payload,
                    sizeof(config.payload));
}

/**
 * Enter APM mode.
 *
 * Enable APM, other parameters are don't care, as per section 7.8.1
 */
void osp_set_apm_mode(void)
{
  /* Advanced power management #53 */
  struct osp_in_advanced_power_management config;
  memset(&config, 0, sizeof(struct osp_in_advanced_power_management));
  config.id = OSP_IN_ADVANCED_POWER_MANAGEMENT_ID;

  config.payload.apm_enabled = 1;
  config.payload.power_duty_cycle = 1;
  config.payload.time_duty_cycle = 1;

  osp_in_advanced_power_management_pre(&config);
  _osp_send_message((osp_message_t*)&config,
                    (uint8_t*)&config.payload,
                    sizeof(config.payload));
}

/**
 * Enter push-to-fix mode
 */
void osp_enter_push_to_fix_mode(void)
{
  /* set tricklepower parameters #151 */
  struct osp_in_set_tricklepower_parameters config;
  memset(&config, 0, sizeof(struct osp_in_set_tricklepower_parameters));
  config.id = OSP_IN_SET_TRICKLEPOWER_PARAMETERS_ID;

  /* values */
  config.payload.pushtofix_mode = 1; /* Enable push-to-fix */
  config.payload.on_time = 200;      /* 200 ms (minimum) */
  config.payload.duty_cycle = 10;    /* 1% */

  /* send */
  osp_in_set_tricklepower_parameters_pre(&config);
  _osp_send_message((osp_message_t*)&config,
                    (uint8_t*)&config.payload,
                    sizeof(config.payload));
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

  /* Enable 1pps time */
  config.payload.mode = OSP_SET_MESSAGE_RATE_ONE;
  config.payload.message_id = OSP_OUT_1PPS_TIME_ID;
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
  config.payload.nominal_frequency_low = 0;//1*1000; /* 1Hz */
  config.payload.network_enhancement_type = 0;   /* No enhancement */

  osp_in_hardware_configuration_response_pre(&config);
  _osp_send_message((osp_message_t*)&config,
                    (uint8_t*)&config.payload,
                    sizeof(config.payload));
}

/**
 * Reset Initialise
 */
void osp_reset_initialise(void)
{
  /* initialise data source #128 */
  struct osp_in_initialise_data_source config;
  memset(&config, 0, sizeof(struct osp_in_initialise_data_source));
  config.id = OSP_IN_INITIALISE_DATA_SOURCE_ID;

  /* values */
  /* All zeros */

  /* send */
  _osp_send_message((osp_message_t*)&config,
                    (uint8_t*)&config.payload,
                    sizeof(config.payload));
}

/**
 * Send reject message NOT AVAILABLE
 */
void osp_send_reject_not_available(uint8_t message_id_to_reject,
                                   uint8_t message_sub_id_to_reject)
{
  /* reject #216 */
  struct osp_in_reject config;
  memset(&config, 0, sizeof(struct osp_in_reject));
  config.id = OSP_IN_REJECT_ID;

  /* values */
  config.payload.sub_id = 2;
  config.payload.rejected_message_id = message_id_to_reject;
  config.payload.rejected_message_sub_id = message_sub_id_to_reject;
  config.payload.rejected_message_reason = OSP_REJECTED_MESSAGE_NOT_AVAILABLE;

  /* send */
  _osp_send_message((osp_message_t*)&config,
                    (uint8_t*)&config.payload,
                    sizeof(config.payload));
}

/**
 * Respond to approximate ms request
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
  osp_in_approximate_ms_position_response_pre(&config);
  _osp_send_message((osp_message_t*)&config,
                    (uint8_t*)&config.payload,
                    sizeof(config.payload));
}

/**
 * Place a position request
 */
void osp_send_position_request(void)
{
  /* approximate ms position */
  struct osp_in_position_request config;
  memset(&config, 0, sizeof(struct osp_in_position_request));
  config.id = OSP_IN_POSITION_REQUEST_ID;

  /* values */
  config.payload.pos_request_id = 0;
  config.payload.num_fixes = 1; /* Just one fix */
  config.payload.time_between_fixes = 0;
  config.payload.horizontal_error_max = 100; /* 100m horizonal error */
  config.payload.vertical_error_max = OSP_MAX_ERROR_80_METER; /* 80m vertical error */
  config.payload.response_time_max = 0;                       /* any time */
  config.payload.time_acc_priority = OSP_TIME_ACC_PRIORITY_NONE;
  config.payload.location_method = 0; /* ???? */

  /* send */
  osp_in_position_request_pre(&config);
  _osp_send_message((osp_message_t*)&config,
                    (uint8_t*)&config.payload,
                    sizeof(config.payload));
}


/**
 * =============================================================================
 * Setup     ===================================================================
 * =============================================================================
 */

/**
 * GPS configuration
 */
void gps_setup(void)
{
  /* clear state. these may have been updated but not serviced since the last time this ran */
  osp_out_hw_config_req.state = OSP_PACKET_WAITING;
  osp_out_aiding_request.state = OSP_PACKET_WAITING;
  osp_out_oktosend.state = OSP_PACKET_WAITING;

  /* Attempt to turn on GPS */
  gps_se_on_off_pulse();

  while (1) {           /* TODO: timeout */

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
        osp_send_reject_not_available(OSP_OUT_AIDING_REQUEST_ID,
                                      OSP_AIDING_REQUEST_SUBID_APPROXIMATE_MS_POSITION);

        /* Set message formats */
        osp_set_messages();

        /* Check these get ack'd? */

        /* Otherwise we're done */
        break;
      }

      /* clear state */
      osp_out_aiding_request.state = OSP_PACKET_WAITING;
    }

    /* oktosend packet */
    if (osp_out_oktosend.state == OSP_PACKET_UPDATED) {

      /* was this an enable or disable packet? */
      if (osp_out_oktosend.payload.oktosend == OSP_OKTOSEND_YES) {
        /* we're now active */
        gps_power_state = GPS_ACTIVE;
      } else {
        gps_se_on_off_pulse();  /* send another on_off pulse */
        gps_power_state = GPS_HIBERNATE;
      }

      /* clear state */
      osp_out_oktosend.state = OSP_PACKET_WAITING;
    }

    idle(IDLE_WAIT_FOR_GPS);
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
	     false,	     /** Run in standby */
	     GPS_GCLK,			/** GCLK generator source */
	     GPS_SERCOM_MOGI_PINMUX, 		/** PAD0 pinmux */
	     GPS_SERCOM_MIGO_PINMUX,		/** PAD1 pinmux */
	     PINMUX_UNUSED,			/** PAD2 pinmux */
	     PINMUX_UNUSED);			/** PAD3 pinmux */

  usart_enable(GPS_SERCOM);
}
void gps_usart_init_disable(void)
{
  usart_disable(GPS_SERCOM);
}

/**
 * GPS reset pin
 */
void gps_reset_exit(void)
{
#ifdef GPS_RESET_PIN
  port_pin_set_output_level(GPS_RESET_PIN, 0); /* gps in operation = low */
#endif
}
void gps_reset_enter(void)
{
#ifdef GPS_RESET_PIN
  port_pin_set_output_level(GPS_RESET_PIN, 1); /* gps in reset = high */
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
  port_pin_set_output_level(GPS_RESET_PIN, 1);	/* gps in reset = high */
#endif
}

void gps_init_timepulse_pin(void) {
#ifdef GPS_TIMEPULSE_PIN
  /* Timepulse_Pin */
  port_pin_set_config(GPS_TIMEPULSE_PIN,
		      PORT_PIN_DIR_INPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */
#endif
}

/**
 * Init
 */
void gps_init(void)
{
  /* Bring GPS out of reset */
  gps_reset_exit();

  /* Timepulse pin - does that go here? */
  gps_init_timepulse_pin();

  /* Enable usart */
  gps_usart_init_enable(GPS_BAUD_RATE);

  /* Incoming osp messages are handled in an irq */
  usart_register_rx_callback(GPS_SERCOM, gps_rx_callback, GPS_SERCOM_INT_PRIO);

  /* ---- GPS Configuration ---- */

  /* We need to wait for the GPS 32kHz clock to start (~300ms). TODO: more robust method for this */
  for (int i = 0; i < 300*1000*16; i++) { __NOP(); }

  /* Close any currently running session. Doesn't do anything unless debugging */
  //osp_reset_initialise();       /* hopefully don't need this now */

  /* Setup sequence */
  gps_setup();
}
/**
 * Re-initialise
 */
void gps_reinit(void)
{
  /* Place GPS in reset */
  gps_reset_enter();

  /* Disable usart */
  gps_usart_init_disable();

  /* Wait for about 3 seconds, kicking the watchdog along the way. TODO: more robust method for this */
  for (int j = 0; j < 10; j++) {
    for (int i = 0; i < 300*1000; i++) { __NOP(); }
    kick_the_watchdog();
  }

  /* Initialise as normal */
  gps_init();
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
