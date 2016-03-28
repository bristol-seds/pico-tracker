/*
 * OSP Message definitions
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

#ifndef OSP_MESSAGES_H
#define OSP_MESSAGES_H

#include "samd20.h"

/** We use the packed attribute so we can copy direct to structs */
#define __PACKED__	__attribute__ ((packed))
/** OSP Message IDs are 8-bit types */
typedef uint8_t	osp_message_id_t;
/** Used for storing the state of each packet */
enum osp_packet_state {
  OSP_PACKET_WAITING,
  OSP_PACKET_ACK,
  OSP_PACKET_NACK,
  OSP_PACKET_UPDATED,
};
/** Generic OSP Message Type. Each message type extended is from this */
typedef struct {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
} osp_message_t;

/** Function typedef for post-processing functions */
typedef void (*osp_post_func)(volatile osp_message_t* const);


/**
 * =============================================================================
 * OSP Input Messages    =======================================================
 * =============================================================================
 */


/**
 * 5.3 OSP Advanced Power Management
 */
#define OSP_IN_ADVANCED_POWER_MANAGEMENT_ID 53
struct osp_in_advanced_power_management {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint8_t apm_enabled;
    uint8_t number_fixes_apm_cycles;
    uint8_t time_between_fixes; /* s */
    uint8_t spare1;
    uint8_t maximum_horizontal_error; /* enum osp_max_error_t */
    uint8_t maximum_vertical_error; /* enum osp_max_error_t */
    uint8_t maximum_response_time;
    uint8_t time_acc_priority;
    uint8_t power_duty_cycle;   /* duty cycle x 0.2 */
    uint8_t time_duty_cycle;
    uint8_t spare2;
  } __PACKED__ payload;
};
static inline void osp_in_advanced_power_management_pre(struct osp_in_advanced_power_management* m)
{
  (void)m;
}
enum osp_max_error_t {
  OSP_MAX_ERROR_1_METER = 0,
  OSP_MAX_ERROR_5_METER,
  OSP_MAX_ERROR_10_METER,
  OSP_MAX_ERROR_20_METER,
  OSP_MAX_ERROR_40_METER,
  OSP_MAX_ERROR_80_METER,
  OSP_MAX_ERROR_160_METER,
  OSP_MAX_ERROR_NO_MAX,
};

/**
 * 5.4 OSP Initialise Data Source
 */
#define OSP_IN_INITIALISE_DATA_SOURCE_ID 128
 struct osp_in_initialise_data_source {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    int32_t x_position;         /* m */
    int32_t y_position;         /* m */
    int32_t z_position;         /* m */
    int32_t clock_drift;        /* Hz */
    uint32_t gps_tow;           /* cs */
    uint16_t gps_week_number;
    uint8_t channels;
    uint8_t reset_config_bitmap;
  } __PACKED__ payload;
};
static inline void osp_in_initialise_data_source_pre(struct osp_in_initialise_data_source* m)
{
  m->payload.x_position = __REV(m->payload.x_position);
  m->payload.y_position = __REV(m->payload.y_position);
  m->payload.z_position = __REV(m->payload.z_position);
  m->payload.clock_drift = __REV(m->payload.clock_drift);
  m->payload.gps_tow = __REV(m->payload.gps_tow);
  m->payload.gps_week_number = __REV16(m->payload.gps_week_number);
}

/**
 * 5.13 Mode Control
 */
#define OSP_IN_MODE_CONTROL_ID 136
 struct osp_in_mode_control {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint16_t res1;
    uint8_t degraded_mode;      /* enum osp_degraded_mode */
    uint8_t position_calc_mode;
    uint8_t res2;
    int16_t altitude;           /* m */
    uint8_t altitude_hold_mode; /* enum osp_altitude_hold_mode */
    uint8_t altitude_hold_source;
    uint8_t res3;
    uint8_t degraded_timeout;  /* s */
    uint8_t dead_reckoning_timeout; /* s */
    uint8_t measurement_and_track_smoothing;
  } __PACKED__ payload;
};
static inline void osp_in_mode_control_pre(struct osp_in_mode_control* m)
{
  m->payload.res1 = __REV16(m->payload.res1);
  m->payload.altitude = __REV16(m->payload.altitude);
}
enum osp_degraded_mode {        /* MUST set DO_NOT_ALLOW for GSW3.2.5 and later */
  OSP_DEGRADED_ALLOW_1SV_FREEZE_DIRECTION = 0,
  OSP_DEGRADED_ALLOW_1SV_FREEZE_CLOCK_DRIFT,
  OSP_DEGRADED_ALLOW_2SV_FREEZE_DIRECTION,
  OSP_DEGRADED_ALLOW_2SV_FREEZE_CLOCK_DRIFT,
  OSP_DEGRADED_DO_NOT_ALLOW,
};
enum osp_altitude_hold_mode {
  OSP_ALTITUDE_HOLD_AUTO = 0,
  OSP_ALTITUDE_HOLD_USER_INPUT,
  OSP_ALTITUDE_HOLD_FORCE_3D_FIX,
};

/**
 * 5.16 OSP Elevation Mask
 */
#define OSP_IN_ELEVATION_MASK_ID 139
 struct osp_in_elevation_mask {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    int16_t tracking_mask;      /* deg NOT IMPLEMENTED */
    int16_t navigation_mask;    /* deg */
  } __PACKED__ payload;
};
static inline void osp_in_elevation_mask_pre(struct osp_in_elevation_mask* m)
{
  m->payload.tracking_mask = __REV16(m->payload.tracking_mask);
  m->payload.navigation_mask = __REV16(m->payload.navigation_mask);
}

/**
 * 5.17 OSP Power Mask
 */
#define OSP_IN_POWER_MASK_ID 140
 struct osp_in_power_mask {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint8_t tracking_mask;      /* dBHz NOT IMPLEMENTED */
    uint8_t navigation_mask;    /* dBHz */
  } __PACKED__ payload;
};
static inline void osp_in_power_mask_pre(struct osp_in_power_mask* m)
{
  (void)m;
}

/**
 * 5.26 OSP Set TricklePower Parameters
 */
#define OSP_IN_SET_TRICKLEPOWER_PARAMETERS_ID 151
struct osp_in_set_tricklepower_parameters {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    int16_t pushtofix_mode;
    int16_t duty_cycle;         /* ‰ */
    int32_t on_time;            /* ms */
  } __PACKED__ payload;
};
static inline void osp_in_set_tricklepower_parameters_pre(struct osp_in_set_tricklepower_parameters* m)
{
  m->payload.pushtofix_mode = __REV16(m->payload.pushtofix_mode);
  m->payload.duty_cycle = __REV16(m->payload.duty_cycle);
  m->payload.on_time = __REV(m->payload.on_time);
}

/**
 * 5.30 OSP Set Message Rate
 */
#define OSP_IN_SET_MESSAGE_RATE_ID 166
 struct osp_in_set_message_rate {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint8_t mode;               /* enum osp_message_rate */
    uint8_t message_id;
    uint8_t update_rate;        /* s */
    uint8_t res1;
    uint8_t res2;
    uint8_t res3;
    uint8_t res4;
  } __PACKED__ payload;
};
static inline void osp_in_set_message_rate_pre(struct osp_in_set_message_rate m)
{
  (void)m;
}
enum osp_message_rate {
  OSP_SET_MESSAGE_RATE_ONE = 0,
  OSP_SET_MESSAGE_RATE_POLL_ONE,
  OSP_SET_MESSAGE_RATE_DEFAULT_NAV,
  OSP_SET_MESSAGE_RATE_DEFAULT_DEBUG,
  OSP_SET_MESSAGE_RATE_DEFAULT_NAV_DEBUG,
};

/**
 * 5.31.1 OSP Set Low Power Acquisition Parameters
 */
#define OSP_IN_SET_LOW_POWER_ACQUISITION_PARAMETERS_ID 167
 struct osp_in_set_low_power_acquisition_parameters {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint32_t max_off_time;      /* ms */
    uint32_t max_search_time;   /* ms */
    uint32_t pushtofix_period;  /* s */
    uint16_t adaptive_tricklepower;
  } __PACKED__ payload;
};
static inline void osp_in_set_low_power_acquisition_parameters_pre(struct osp_in_set_low_power_acquisition_parameters* m)
{
  m->payload.max_off_time = __REV(m->payload.max_off_time);
  m->payload.max_search_time = __REV(m->payload.max_search_time);
  m->payload.pushtofix_period = __REV(m->payload.pushtofix_period);
  m->payload.adaptive_tricklepower = __REV16(m->payload.adaptive_tricklepower);
}

/**
 * 5.47 OSP Position Request
 */
#define OSP_IN_POSITION_REQUEST_ID 210
 struct osp_in_position_request {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint8_t pos_request_id;
    uint8_t num_fixes;
    uint8_t time_between_fixes;   /* s */
    uint8_t horizontal_error_max; /* m */
    uint8_t vertical_error_max;   /* enum osp_max_error_t */
    uint8_t response_time_max;    /* s */
    uint8_t time_acc_priority;    /* enum osp_time_acc_priority */
    uint8_t location_method;      /* enum osp_location_method */
  } __PACKED__ payload;
};
static inline void osp_in_position_request_pre(struct osp_in_position_request* m)
{
  (void)m;
}
enum osp_time_acc_priority {
  OSP_TIME_ACC_PRIORITY_NONE		= 0,
  OSP_TIME_ACC_PRIORITY_TIME		= 1,
  OSP_TIME_ACC_PRIORITY_ACCURACY	= 2,
  OSP_TIME_ACC_PRIORITY_USE_FULL_TIME	= 3
};
enum osp_location_method {
  OSP_LOCATION_METHOD_MS_ASSISTED		= 0,
  OSP_LOCATION_METHOD_MS_BASED			= 1,
  OSP_LOCATION_METHOD_MS_BASED_ALLOW_ASSISTED	= 2,
  OSP_LOCATION_METHOD_MS_ASSISTED_ALLOW_BASED	= 3,
  OSP_LOCATION_METHOD_SIMULTANEOUS		= 4,
};


/**
 * 5.65 OSP Session Request
 */
#define OSP_IN_SESSION_REQUEST_ID 213
 struct osp_in_session_request {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint8_t sub_id;             /* enum osp_session_request */
    uint8_t info;
  } __PACKED__ payload;
};
static inline void osp_in_session_request_pre(struct osp_in_session_request* m)
{
  (void)m;
}
enum osp_session_request {
  OSP_SESSION_REQUEST_OPEN = 1,
  OSP_SESSION_REQUEST_CLOSE = 2,
};

/**
 * 5.67 OSP Hardware Configuration Response
 */
#define OSP_IN_HARDWARE_CONFIGURATION_RESPONSE_ID 214
struct osp_in_hardware_configuration_response {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint8_t hw_config;          /* enum osp_hw_config */
    uint8_t nominal_frequency_high;
    uint32_t nominal_frequency_low; /* mHz */
    uint8_t network_enhancement_type;
  } __PACKED__ payload;
};
static inline void osp_in_hardware_configuration_response_pre(struct osp_in_hardware_configuration_response* m)
{
  m->payload.nominal_frequency_low = __REV(m->payload.nominal_frequency_low);
}
enum osp_hw_config {
  OSP_HW_CONFIG_PRECISE_TIME_TRANSFER_NO	= (0<<0),
  OSP_HW_CONFIG_PRECISE_TIME_TRANSFER_YES	= (1<<0),
  OSP_HW_CONFIG_PRECISE_TIME_CP_TO_SLC		= (0<<1),
  OSP_HW_CONFIG_PRECISE_TIME_BIDIRECTIONAL	= (1<<1),
  OSP_HW_CONFIG_FREQUENCY_TRANSFER_NO		= (0<<2),
  OSP_HW_CONFIG_FREQUENCY_TRANSFER_YES		= (1<<2),
  OSP_HW_CONFIG_FREQUENCY_TRANSFER_COUNTER	= (0<<3),
  OSP_HW_CONFIG_FREQUENCY_TRANSFER_NO_COUNTER	= (1<<3),
  OSP_HW_CONFIG_RTC_AVAILABLE_NO		= (0<<4),
  OSP_HW_CONFIG_RTC_AVAILABLE_YES		= (1<<4),
  OSP_HW_CONFIG_RTC_EXTERNAL			= (0<<5),
  OSP_HW_CONFIG_RTC_INTERNAL			= (1<<5),
  OSP_HW_CONFIG_COARSE_TIME_TRANSFER_NO		= (0<<6),
  OSP_HW_CONFIG_COARSE_TIME_TRANSFER_YES	= (1<<6),
  OSP_HW_CONFIG_FREQUENCY_REFCLK_ON		= (0<<7),
  OSP_HW_CONFIG_FREQUENCY_REFCLK_OFF		= (1<<7),
};


/**
 * 5.68 Approximate MS Position Response
 */
#define OSP_IN_APPROXIMATE_MS_POSITION_RESPONSE_ID 215
struct osp_in_approximate_ms_position_response {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint8_t sub_id;             /* 1 */
    int32_t lat;               /* 180*2^-32 deg */
    int32_t lon;               /* 180*2^-32 deg */
    uint16_t alt;               /* dm + 500 */
    uint8_t est_hor_er;
    uint16_t est_ver_er;
    uint8_t use_alt_aiding;
  } __PACKED__ payload;
};
static inline void osp_in_approximate_ms_position_response_pre(struct osp_in_approximate_ms_position_response* m)
{
  m->payload.lat = __REV(m->payload.lat);
  m->payload.lon = __REV(m->payload.lon);
  m->payload.alt = __REV16(m->payload.alt);
  m->payload.est_ver_er = __REV16(m->payload.est_ver_er);
}

/**
 * 5.74 OSP Reject
 */
#define OSP_IN_REJECT_ID 216
 struct osp_in_reject {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint8_t sub_id;
    uint8_t rejected_message_id;
    uint8_t rejected_message_sub_id;
    uint8_t rejected_message_reason; /* enum osp_rejected_message_reason */
  } __PACKED__ payload;
};
enum osp_rejected_message_reason {
  OSP_REJECTED_MESSAGE_NOT_READY	= (1<<1),
  OSP_REJECTED_MESSAGE_NOT_AVAILABLE	= (1<<2),
  OSP_REJECTED_MESSAGE_BAD_FORMATTING	= (1<<3),
  OSP_REJECTED_MESSAGE_NO_TIME_PULSE	= (1<<4),
};

/**
 * 5.76 OSP Power Mode Request
 */
#define OSP_IN_POWER_MODE_REQUEST_ID 218
 struct osp_in_power_mode_request_Full_Power {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint8_t sub_id;
  } __PACKED__ payload;
};
 struct osp_in_power_mode_request_Micro_Power {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint8_t sub_id;
    uint32_t reserved;
  } __PACKED__ payload;
};



/**
 * =============================================================================
 * OSP Output Messages   =======================================================
 * =============================================================================
 */

/**
 * 6.2 OSP Measure Naviagation Data Out
 */
#define OSP_OUT_MEASURE_NAVIGATION_DATA_OUT_ID 2
struct osp_out_measure_navigation_data_out {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    int32_t x_position;         /* m */
    int32_t y_position;         /* m */
    int32_t z_position;         /* m */
    int16_t x_velocity;         /* ms-1 */
    int16_t y_velocity;         /* ms-1 */
    int16_t z_velocity;         /* ms-1 */
    uint8_t mode1;
    uint8_t hdop;
    uint8_t mode2;
    uint16_t gps_week;
    uint32_t gps_tow;           /* cs */
    uint8_t svs_in_fix;
    uint8_t ch1_prn;
    uint8_t ch2_prn;
    uint8_t ch3_prn;
    uint8_t ch4_prn;
    uint8_t ch5_prn;
    uint8_t ch6_prn;
    uint8_t ch7_prn;
    uint8_t ch8_prn;
    uint8_t ch9_prn;
    uint8_t ch10_prn;
    uint8_t ch11_prn;
    uint8_t ch12_prn;
  } __PACKED__ payload;
};
static void osp_out_measure_navigation_data_out_post(volatile osp_message_t* o)
{
  struct osp_out_measure_navigation_data_out* m = (struct osp_out_measure_navigation_data_out*)o;

  m->payload.x_position = __REV(m->payload.x_position);
  m->payload.y_position = __REV(m->payload.y_position);
  m->payload.z_position = __REV(m->payload.z_position);
  m->payload.x_velocity = __REV16(m->payload.x_velocity);
  m->payload.y_velocity = __REV16(m->payload.y_velocity);
  m->payload.z_velocity = __REV16(m->payload.z_velocity);
  m->payload.gps_week = __REV16(m->payload.gps_week);
  m->payload.gps_tow = __REV(m->payload.gps_tow);
}

/**
 * 6.7 OSP Clock Status Data
 */
#define OSP_OUT_CLOCK_STATUS_DATA_ID 7
struct osp_out_clock_status_data {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint16_t extended_gps_week;
    uint32_t gps_tow;           /* s */
    uint8_t svs;
    uint32_t clock_drift;       /* Hz */
    uint32_t clock_bias;        /* ns */
    uint32_t extimated_gps_time; /* ms */
  } __PACKED__ payload;
};
static void osp_out_clock_status_data_post(volatile osp_message_t* o)
{
  struct osp_out_clock_status_data* m = (struct osp_out_clock_status_data*)o;

  m->payload.extended_gps_week = __REV16(m->payload.extended_gps_week);
  m->payload.gps_tow = __REV(m->payload.gps_tow);
  m->payload.clock_drift = __REV(m->payload.clock_drift);
  m->payload.clock_bias = __REV(m->payload.clock_bias);
  m->payload.extimated_gps_time = __REV(m->payload.extimated_gps_time);
}

/**
 * 6.15 OSP Ephemeris Data
 */
#define OSP_OUT_EPHEMERIS_DATA_ID 15
struct osp_out_ephemeris_data {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint8_t sv_id;
    uint16_t data[45];
  } __PACKED__ payload;
};
static void osp_out_ephemeris_data_post(volatile osp_message_t* o)
{
  struct osp_out_ephemeris_data* m = (struct osp_out_ephemeris_data*)o;

  for (int i = 0; i < 45; i++) {
    m->payload.data[i] = __REV16(m->payload.data[i]);
  }
}

/**
 * 6.18 OSP OkToSend
 */
#define OSP_OUT_OKTOSEND_ID 18
struct osp_out_oktosend {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint8_t oktosend;
  } __PACKED__ payload;
};
static void osp_out_oktosend_post(volatile osp_message_t* o)
{
  (void)o;
}
enum osp_oktosend {
  OSP_OKTOSEND_NO = 0,
  OSP_OKTOSEND_YES = 1
};

/**
 * 6.27 OSP Geodetic Navigation Data
 */
#define OSP_OUT_GEODETIC_NAVIGATION_DATA_ID 41
struct osp_out_geodetic_navigation_data {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint16_t nav_valid;
    uint16_t nav_type;
    uint16_t extended_week_number;
    uint32_t tow;               /* s */
    uint16_t utc_year;          /* years */
    uint8_t utc_month;          /* months */
    uint8_t utc_day;            /* days */
    uint8_t utc_hour;           /* hours */
    uint8_t utc_minute;         /* minutes */
    uint16_t utc_second;        /* ms */
    uint32_t satellite_id_list;
    int32_t latitude;           /* hndeg */
    int32_t longitude;           /* hndeg */
    int32_t altitude_from_ellipsoid; /* cm */
    int32_t altitude_from_msl;       /* cm */
    int8_t map_datum;
    uint16_t speed_over_ground;      /* cms-1 */
    uint16_t course_over_ground;     /* cdeg true north */
    int16_t magnetic_variation;
    int16_t climb_rate;         /* cms-1 */
    int16_t heading_rate;       /* cdegs-1 */
    uint32_t estimated_horizontal_position_error; /* cm */
    uint32_t estimated_vertical_position_error;   /* cm */
    uint32_t estimated_time_error;                /* cs */
    uint16_t estimated_horizontal_velocity_error; /* cms-1 */
    int32_t clock_bias;                           /* cm */
    uint32_t clock_bias_error;                    /* cm */
    int32_t clock_drift;                           /* cms-1 */
    uint32_t clock_drift_error;                   /* cms-1 */
    uint32_t distance;                            /* m */
    uint16_t distance_error;                      /* m */
    uint16_t heading_error;                       /* cdeg */
    uint8_t svs_in_fix;
    uint8_t hdop_x5;
    uint8_t additional_mode_info;
  } __PACKED__ payload;
};
static void osp_out_geodetic_navigation_data_post(volatile osp_message_t* o)
{
  struct osp_out_geodetic_navigation_data* m = (struct osp_out_geodetic_navigation_data*)o;

  m->payload.nav_valid = __REV16(m->payload.nav_valid);
  m->payload.nav_type = __REV16(m->payload.nav_type);
  m->payload.extended_week_number = __REV16(m->payload.extended_week_number);
  m->payload.tow = __REV(m->payload.tow);
  m->payload.utc_year = __REV16(m->payload.utc_year);
  m->payload.utc_second = __REV16(m->payload.utc_second);
  m->payload.satellite_id_list = __REV(m->payload.satellite_id_list);

  m->payload.latitude = __REV(m->payload.latitude);
  m->payload.longitude = __REV(m->payload.longitude);
  m->payload.altitude_from_ellipsoid = __REV(m->payload.altitude_from_ellipsoid);
  m->payload.altitude_from_msl = __REV(m->payload.altitude_from_msl);

  m->payload.speed_over_ground = __REV16(m->payload.speed_over_ground);
  m->payload.course_over_ground = __REV16(m->payload.course_over_ground);
  m->payload.magnetic_variation = __REV16(m->payload.magnetic_variation);
  m->payload.climb_rate = __REV16(m->payload.climb_rate);
  m->payload.heading_rate = __REV16(m->payload.heading_rate);

  m->payload.estimated_horizontal_position_error = __REV(m->payload.estimated_horizontal_position_error);
  m->payload.estimated_vertical_position_error = __REV(m->payload.estimated_vertical_position_error);
  m->payload.estimated_time_error = __REV(m->payload.estimated_time_error);
  m->payload.estimated_horizontal_velocity_error = __REV16(m->payload.estimated_horizontal_velocity_error);

  m->payload.clock_bias = __REV(m->payload.clock_bias);
  m->payload.clock_bias_error = __REV(m->payload.clock_bias_error);
  m->payload.clock_drift = __REV(m->payload.clock_drift);
  m->payload.clock_drift_error = __REV(m->payload.clock_drift_error);
  m->payload.distance = __REV(m->payload.distance);
  m->payload.distance_error = __REV16(m->payload.distance_error);
  m->payload.heading_error = __REV16(m->payload.heading_error);
}


/**
 * 6.42 OSP 1PPS Time
 */
#define OSP_OUT_1PPS_TIME_ID 52
struct osp_out_1pps_time {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t day;
    uint8_t month;
    uint16_t year;
    int16_t utc_offset_int;     /* s. gps offset from utc  */
    uint32_t utc_offset_frac;   /* ns */
    uint8_t status;             /* enum osp_1pps_status */
  } __PACKED__ payload;
};
static void osp_out_1pps_time_post(volatile osp_message_t* o)
{
  struct osp_out_1pps_time* m = (struct osp_out_1pps_time*)o;

  m->payload.year = __REV16(m->payload.year);
  m->payload.utc_offset_int = __REV16(m->payload.utc_offset_int);
  m->payload.utc_offset_frac = __REV(m->payload.utc_offset_frac);
}
enum osp_1pps_status {
  OSP_1PPS_VALID	= (1<<0),
  OSP_1PPS_IS_UTCTIME	= (1<<1), /* otherwise gps time */
  OSP_1PPS_OFFSET_VALID	= (1<<2),
};


/**
 * 6.55 OSP GPIO State
 */
#define OSP_OUT_GPIO_STATE_ID 65
struct osp_out_gpio_state {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint8_t subid;              /* always 192 it seems */
    uint16_t gpio_state;        /* bitmapped */
  } __PACKED__ payload;
};
static void osp_out_gpio_state_post(volatile osp_message_t* o)
{
  struct osp_out_gpio_state* m = (struct osp_out_gpio_state*)o;

  m->payload.gpio_state = __REV16(m->payload.gpio_state);
}


/**
 * 6.58 OSP Position Response
 */
#define OSP_OUT_POSITION_RESPONSE_ID 69
struct osp_out_position_response {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint8_t subid;              /* ensure always 1 for position response */
    uint8_t position_request_id;
    uint8_t position_results_flag;
    uint8_t position_error_status; /* enum osp_position_error_status */
    uint8_t position_accuracy_met;
    uint8_t position_type;
    uint8_t dgps_correction;    /* enum os_dgps_correction_type */
    uint16_t gps_week;
    uint32_t gps_seconds;       /* ms */
    int32_t latitude;          /* 180/2^32 degrees */
    int32_t longitude;         /* 360/2^32 degrees */
    uint8_t other_sections;
    /* Horizontal error */
    uint8_t er_el_angle;        /* 180/2^8 degrees */
    uint8_t major_std_error;
    uint8_t minor_std_error;
    /* Vertical error */
    uint16_t height;            /* dm+500m.. ONLY SUPPORTS TO +6km */
    uint8_t height_std_error;
    /* Velocity section */
    uint16_t horizonal_velocity; /* 0.0625 ms-1 */
    uint16_t heading;            /* 360/2^16 degrees */
    uint8_t vertical_velocity;   /* 0.5ms-1 */
    uint8_t velocity_er_el_angle; /* 0.75 degrees */
    uint8_t velocity_major_std_error;
    uint8_t velicity_minor_std_error;
    uint8_t vertical_velocity_std_error;
    /* Clock correction section */
    uint8_t time_reference;
    uint16_t clock_bias;
    uint16_t clock_drift;
    uint8_t clock_std_error;
    uint8_t utc_offset;         /* s */
    /* Position Correction Section */
    uint8_t number_svs;
    struct {
      uint8_t sv_prn;
      uint8_t c_n0;             /* dBHz */
    } svs[16];
  } __PACKED__ payload;
};
static void osp_out_position_response_post(volatile osp_message_t* o)
{
  struct osp_out_position_response* m = (struct osp_out_position_response*)o;

  m->payload.gps_week = __REV16(m->payload.gps_week);
  m->payload.gps_seconds = __REV(m->payload.gps_seconds);
  m->payload.latitude = __REV(m->payload.latitude);
  m->payload.longitude = __REV(m->payload.longitude);
  m->payload.height = __REV16(m->payload.height);
  m->payload.horizonal_velocity = __REV16(m->payload.horizonal_velocity);
  m->payload.heading = __REV16(m->payload.heading);
  m->payload.clock_bias = __REV16(m->payload.clock_bias);
  m->payload.clock_drift = __REV16(m->payload.clock_drift);
}
enum osp_position_error_status {
  OSP_POSITION_ERROR_STATUS_VALID		= 0,
  OSP_POSITION_ERROR_STATUS_NOT_ENOUGH_SATS	= 1,
  OSP_POSITION_ERROR_STATUS_GPS_AIDING_MISSING	= 2,
  OSP_POSITION_ERROR_STATUS_NEED_MORE_TIME	= 3,
  OSP_POSITION_ERROR_STATUS_NO_AFTER_FULL_SEARCH= 4,
  OSP_POSITION_ERROR_STATUS_POS_REPORT_DISABLED = 5,
  OSP_POSITION_ERROR_STATUS_REJECTED_FOR_QOP	= 6,
};
enum osp_dgps_correction_type {
  OSP_DGPS_CORRECTION_NO	= 0,
  OSP_DGPS_CORRECTION_LOCAL	= 1,
  OSP_DGPS_CORRECTION_WAAS	= 2,
};

/**
 * OSP HW_CONFIG_REQ
 */
#define OSP_OUT_HW_CONFIG_REQ_ID 71
struct osp_out_hw_config_req {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
  } __PACKED__ payload;
};
static void osp_out_hw_config_req_post(volatile osp_message_t* o)
{
  (void)o;
}
/**
 * 6.70 OSP Various Aiding Requests
 */
#define OSP_OUT_AIDING_REQUEST_ID 73
struct osp_out_aiding_request {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint8_t subid;
    union {
      struct {} approximate_ms_position;
      struct {
        uint8_t todo[8];
      } nav_bit_aiding;
    };
  } __PACKED__ payload;
};
static void osp_out_aiding_request_post(volatile osp_message_t* o)
{
  (void)o;
}
enum {
  OSP_AIDING_REQUEST_SUBID_APPROXIMATE_MS_POSITION = 1
};


/**
 * 6.84 OSP CW Controller Output
 */
#define OSP_OUT_CW_CONTROLLER_OUTPUT_ID 92
struct osp_out_cw_controller_output {
  osp_message_id_t id;
  enum osp_packet_state state;
  uint16_t max_payload_size;
  struct {
    uint8_t subid;
    union {
      struct {
        uint32_t frequency_peak[8]; /* Hz */
        uint16_t c_n0[8];           /* cdBHz */
      } interference_report;
      struct {
        uint8_t sampling_mode;
        uint8_t ad_mode;
        struct {
          int8_t centre_freq_bin;
          uint8_t number_of_bins;
        } filters[8];
      } mitigation_report;
    };
  } __PACKED__ payload;
};
static void osp_out_cw_controller_output_post(volatile osp_message_t* o)
{
  (void)o;
}


/* /\** */
/*  * */
/*  *\/ */
/* #define OSP__ID
struct osp_ { */
/*   osp_message_id_t id; */
/*   enum osp_packet_state state;
  uint16_t max_payload_size; */
/*   struct { */

/*   } __PACKED__ payload; */
/* };
static void osp__post(volatile osp_message_t* o)
{
  struct osp_* m = (struct osp_*)o;

} */



#endif /* OSP_MESSAGES_H */
