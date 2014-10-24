/*
 * UBX Message definitions
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

#ifndef UBX_MESSAGES_H
#define UBX_MESSAGES_H

#include "samd20.h"

/** We use the packed attribute so we can copy direct to structs */
#define __PACKED__	__attribute__((packed))
/** UBX Message IDs are 16-bit types */
typedef uint16_t	ubx_message_id_t;
/** Used for storing the state of each packet */
enum ubx_packet_state {
  UBX_PACKET_WAITING,
  UBX_PACKET_ACK,
  UBX_PACKET_NACK,
};
/** Generic UBX Message Type. Each message type extended is from this */
typedef struct {
  ubx_message_id_t id;
  enum ubx_packet_state state;
} ubx_message_t;

/** UBX Class Types */
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
 * =============================================================================
 * UBX CFG Message Types =======================================================
 * =============================================================================
 */

/**
 * UBX CFG ANT Antenna Control Settings
 */
__PACKED__ struct ubx_cfg_ant {
  ubx_message_id_t id;
  enum ubx_packet_state state;
  struct {
    uint16_t flags;
    uint16_t pins;
  } payload;
};
/**
 * UBX CFG GNSS
 */
__PACKED__ struct ubx_cfg_gnss {
  ubx_message_id_t id;
  enum ubx_packet_state state;
  struct {
    uint8_t msgVer;
    uint8_t numTrkChHw;
    uint8_t numTrkChUse;
    uint8_t numConfigBlocks;
    struct {
      uint8_t gnssID;
      uint8_t resTrkCh;
      uint8_t maxTrkCh;
      uint8_t reserved1;
      int32_t flags;
    } block[8];
  } payload;
};
/**
 * UBX CFG NAV5 Navigation Engine Settings
 */
__PACKED__ struct ubx_cfg_nav5 {
  ubx_message_id_t id;
  enum ubx_packet_state state;
  struct {
    uint16_t mask;
    uint8_t dynModel;
    uint8_t fixMode;
    int32_t fixedAlt;
    uint32_t fixedAltVar;
    int8_t minElev;
    uint8_t drLimit;
    uint16_t pDop;
    uint16_t tDop;
    uint16_t pAcc;
    uint16_t tAcc;
    uint8_t staticHoldThresh;
    uint8_t dgpsTimeOut;
    uint32_t res2;
    uint32_t res3;
    uint32_t res4;
  } payload;
};
/**
 * UBX CFG TP5 TimePulse Parameters
 */
__PACKED__ struct ubx_cfg_tp5 {
  ubx_message_id_t id;
  enum ubx_packet_state state;
  struct {
    uint8_t tpIdx;
    uint8_t res0;
    uint16_t res1;
    int16_t antCableDelay;
    int16_t rfGroupDelay;
    uint32_t freqPeriod;
    uint32_t freqPeriodLoc;
    uint32_t pulseLenRatio;
    uint32_t pulseLenRatioLock;
    int32_t userConfigDelay;
    uint32_t flags;
  } payload;
};
/**
 * UBX CFG PRT Polls the configuration for one I/O Port
 */
__PACKED__ struct ubx_cfg_prt {
  ubx_message_id_t id;
  enum ubx_packet_state state;
  struct {
    uint8_t portID;
    uint8_t res0;
    uint16_t txReady;
    uint32_t mode;
    uint32_t baudRate;
    uint16_t inProtoMask;
    uint16_t outProtoMask;
    uint16_t flags;
    uint16_t res3;
  } payload;
};

/**
 * UBX Dynamic Platform Model
 */
enum {
  UBX_PLATFORM_MODEL_PORTABLE		= 0,
  UBX_PLATFORM_MODEL_STATIONARY		= 2,
  UBX_PLATFORM_MODEL_PEDESTRIAN		= 3,
  UBX_PLATFORM_MODEL_AUTOMOTIVE		= 4,
  UBX_PLATFORM_MODEL_SEA		= 5,
  UBX_PLATFORM_MODEL_AIRBORNE_1G	= 6,
  UBX_PLATFORM_MODEL_AIRBORNE_2G	= 7,
  UBX_PLATFORM_MODEL_AIRBORNE_4G	= 8,
};
/**
 * UBX GNSS Systems
 */
enum {
  UBX_GNSS_GPS		= 0,
  UBX_GNSS_SBAS		= 1,
  UBX_GNSS_GALILEO	= 2,
  UBX_GNSS_BEIDOU	= 3,
  UBX_GNSS_QZSS		= 5,
  UBX_GNSS_GLONASS	= 6,
};

/**
 * =============================================================================
 * UBX NAV Message Types =======================================================
 * =============================================================================
 */

/**
 * UBX NAV POSLLH Geodetic Position Solution
 */
__PACKED__ struct ubx_nav_posllh {
  ubx_message_id_t id;
  enum ubx_packet_state state;
  struct {
    uint32_t iTOW;
    int32_t lon;
    int32_t lat;
    int32_t height;
    int32_t hMSL;
    uint32_t hAcc;
    uint32_t vAcc;
  } payload;
};
/**
 * UBX NAV TIMEUTC
 */
__PACKED__ struct ubx_nav_timeutc {
  ubx_message_id_t id;
  enum ubx_packet_state state;
  struct {
    uint32_t iTOW;
    uint32_t tAcc;
    int32_t nano;
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t valid;
  } payload;
};
/**
 * UBX NAV SOL Navigation Solution Information
 */
__PACKED__ struct ubx_nav_sol {
  ubx_message_id_t id;
  enum ubx_packet_state state;
  struct {
    uint32_t iTOW;
    int32_t fTOW;
    uint16_t week;
    uint8_t gpsFix;
    uint8_t flags;
    int32_t ecefX;
    int32_t ecefY;
    int32_t ecefZ;
    uint32_t pAcc;
    int32_t ecefVX;
    int32_t ecefVY;
    int32_t ecefVZ;
    uint32_t sAcc;
    uint16_t pDOP;
    uint8_t res1;
    uint8_t numSV;
    uint32_t res2;
  } payload;
};
/**
 * UBX NAV STATUS Receiver Navigation Status
 */
__PACKED__ struct ubx_nav_status {
  ubx_message_id_t id;
  enum ubx_packet_state state;
  struct {
    uint32_t iTOW;
    uint8_t gpsFix;
    int8_t flags;
    int8_t fixStat;
    int8_t flags2;
    uint32_t ttff;
    uint32_t msss;
  } payload;
};

#endif /* UBX_MESSAGES_H */
