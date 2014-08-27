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

#ifndef GPS_H
#define GPS_H

/* We use the packed attribute so we can copy direct to structs */
#define __PACKED__	__attribute__((packed))

/**
 * UBX CFG ANT Antenna Control Settings
 */
volatile struct ubx_cfg_ant {
  uint16_t flags;
  uint16_t pins;
} __PACKED__ ubx_cfg_ant;
/**
 * UBX CFG NAV5 Navigation Engine Settings
 */
volatile struct ubx_cfg_nav5 {
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
} __PACKED__ ubx_cfg_nav5;
/**
 * UBX CFG TP TimePulse Parameters
 */
volatile struct ubx_cfg_tp {
  uint32_t interval;
  uint32_t length;
  int8_t status;
  uint8_t timeRef;
  uint8_t flags;
  uint8_t res;
  int16_t antennaCableDelay;
  int16_t rfGroupDelay;
  int32_t userDelay;
} __PACKED__ ubx_cfg_tp;
/**
 * UBX CFG TP5 TimePulse Parameters
 */
volatile struct ubx_cfg_tp5 {
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
} __PACKED__ ubx_cfg_tp5;
/**
 * UBX NAV POSLLH Geodetic Position Solution
 */
volatile struct ubx_nav_posllh {
  uint32_t iTOW;
  int32_t lon;
  int32_t lat;
  int32_t height;
  int32_t hMSL;
  uint32_t hAcc;
  uint32_t vAcc;
} __PACKED__ ubx_nav_posllh;
/**
 * UBX NAV TIMEUTC
 */
volatile struct ubx_nav_timeutc {
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
} __PACKED__ ubx_nav_timeutc;
/**
 * UBX NAV SOL Navigation Solution Information
 */
volatile struct ubx_nav_sol {
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
} __PACKED__ ubx_nav_sol;

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


void usart_loopback_test(void);
char* gps_get_frame(void);
void gps_init(void);

#endif /* GPS_H */
