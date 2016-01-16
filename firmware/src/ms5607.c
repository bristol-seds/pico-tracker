/*
 * MS5607
 * Copyright (C) 2015  Richard Meadows <richardeoin>
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

#include <stdbool.h>
#include <stdint.h>

#include "samd20.h"
#include "hw_config.h"

#if (BAROMETER_TYPE_MS5607 || BAROMETER_TYPE_MS5611)

#include "barometer.h"
#include "i2c_bb.h"

#define MS5607_ADDRESS		0xEE

/**
 * MS5607 commands
 */
typedef enum {
  MS5607_RESET			= 0x1E,
  MS5607_CONVERT_D1_OSR_256	= 0x40, /* (delay   900us) */
  MS5607_CONVERT_D1_OSR_512	= 0x42, /* (delay  3000us) */
  MS5607_CONVERT_D1_OSR_1024	= 0x44, /* (delay  4000us) */
  MS5607_CONVERT_D1_OSR_2048	= 0x46, /* (delay  6000us) */
  MS5607_CONVERT_D1_OSR_4096	= 0x48, /* (delay 10000us) */
  MS5607_CONVERT_D2_OSR_256	= 0x50, /* (delay   900us) */
  MS5607_CONVERT_D2_OSR_512	= 0x52, /* (delay  3000us) */
  MS5607_CONVERT_D2_OSR_1024	= 0x54, /* (delay  4000us) */
  MS5607_CONVERT_D2_OSR_2048	= 0x56, /* (delay  6000us) */
  MS5607_CONVERT_D2_OSR_4096	= 0x58, /* (delay 10000us) */
  MS5607_ADC_READ		= 0x00,
  MS5607_PROM_READ_BASE	= 0xA0,
} ms5607_command;

/**
 * Commands to use
 */
#define D1_COMMAND	MS5607_CONVERT_D1_OSR_4096
#define D1_DELAY_US	10000
#define D2_COMMAND	MS5607_CONVERT_D2_OSR_4096
#define D2_DELAY_US	10000


/**
 * Barometer data structure
 */
struct barometer barometer;

/**
 * Check initialised correctly
 */
uint8_t ms5607_init_success = 0;

/**
 * Calibration Values read from PROM
 */
struct calibration {
  uint16_t manufacturer_data;
  uint16_t C1, C2, C3, C4, C5, C6;
  uint16_t checksum;            /* need to read all 16-bits for checksum */
} calibration;


/**
 * Implements a microsecond delay
 */
void delay_us(uint16_t microseconds) {
  int32_t i = microseconds * 16;

  while(i--);
}

/**
 * Writes a command
 */
void command(uint8_t command) {
  uint8_t buffer[1];
  buffer[0] = command;

  /* Write command */
  i2c_bb_write(MS5607_ADDRESS, buffer, 1);
}
/**
 * Reads an 8-bit value
 */
uint8_t read_8(uint8_t command) {
  uint8_t buffer[1];
  buffer[0] = command;

  /* Write command */
  i2c_bb_write(MS5607_ADDRESS, buffer, 1);

  /* Read it */
  i2c_bb_read(MS5607_ADDRESS, buffer, 1);

  return buffer[0];
}
/**
 * Read a 16-bit value
 */
uint16_t read_16(uint8_t command) {
  uint8_t buffer[2];
  buffer[0] = command;

  /* Write command */
  i2c_bb_write(MS5607_ADDRESS, buffer, 1);

  /* Read it */
  i2c_bb_read(MS5607_ADDRESS, buffer, 2);

  return (buffer[0] << 8) | buffer[1];
}
/**
 * Read a 24-bit value
 */
uint32_t read_24(uint8_t command) {
  uint8_t buffer[3];
  buffer[0] = command;

  /* Write command */
  i2c_bb_write(MS5607_ADDRESS, buffer, 1);

  /* Read it */
  i2c_bb_read(MS5607_ADDRESS, buffer, 3);

  return (buffer[0] << 16) | (buffer[1] << 8) | buffer[2];
}

/**
 * Reads off the MS5607 calibration values from PROM
 */
void get_cal_param(struct calibration *c) {

  /* Manufacturer Data */
  c->manufacturer_data = read_16(MS5607_PROM_READ_BASE);

  /* Calibration */
  c->C1 = read_16(MS5607_PROM_READ_BASE+2);
  c->C2 = read_16(MS5607_PROM_READ_BASE+4);
  c->C3 = read_16(MS5607_PROM_READ_BASE+6);
  c->C4 = read_16(MS5607_PROM_READ_BASE+8);
  c->C5 = read_16(MS5607_PROM_READ_BASE+10);
  c->C6 = read_16(MS5607_PROM_READ_BASE+12);

  /* Checksum */
  c->checksum = read_16(MS5607_PROM_READ_BASE+14);
}


/**
 * Calculates the checksum of a calibration struct
 */
uint16_t checksum_nrem_part(uint16_t n_rem, uint16_t data)
{
  n_rem ^= (data >> 8);
  for (uint8_t n_bit = 8; n_bit > 0; n_bit--) {
    if (n_rem & (0x8000)) {
      n_rem = (n_rem << 1) ^ 0x3000;
    } else {
      n_rem = (n_rem << 1);
    }
  }

  n_rem ^= (data & 0x00FF);
  for (uint8_t n_bit = 8; n_bit > 0; n_bit--) {
    if (n_rem & (0x8000)) {
      n_rem = (n_rem << 1) ^ 0x3000;
    } else {
      n_rem = (n_rem << 1);
    }
  }

  return n_rem;
}
uint8_t calibration_checksum(struct calibration *c)
{
  uint16_t n_rem = 0;

  n_rem = checksum_nrem_part(n_rem, c->manufacturer_data);
  n_rem = checksum_nrem_part(n_rem, c->C1);
  n_rem = checksum_nrem_part(n_rem, c->C2);
  n_rem = checksum_nrem_part(n_rem, c->C3);
  n_rem = checksum_nrem_part(n_rem, c->C4);
  n_rem = checksum_nrem_part(n_rem, c->C5);
  n_rem = checksum_nrem_part(n_rem, c->C6);
  n_rem = checksum_nrem_part(n_rem, c->checksum & 0xFF00);

  return (0x000F & (n_rem >> 12));
}


/**
 * Takes a temperature measurement and returns the uncompensated value.
 */
uint32_t get_d1(void) {

  /* Write command */
  command(D1_COMMAND);

  /* Wait */
  delay_us(D1_DELAY_US);

  /* Read value */
  return read_24(MS5607_ADC_READ);
}
/**
 * Takes a pressure measurement and returns the uncompenstated value.
 */
uint32_t get_d2(void) {

  /* Write command */
  command(D2_COMMAND);

  /* Wait */
  delay_us(D2_DELAY_US);

  /* Read value */
  return read_24(MS5607_ADC_READ);
}

/* -----------------------------------------------------------------------------
 * Calculations
 */

struct barometer* get_barometer(void)
{
  if (ms5607_init_success) {

    uint32_t D1 = get_d1();
    uint32_t D2 = get_d2();
    struct calibration* c = &calibration;


    /* Difference between actual and reference temperature */
    int32_t dT = D2 - (c->C5 * (1 << 8));

    /* Actual temperature (-40 85°C with 0.01°C resolution) */
    int32_t TEMP = 2000 + (int32_t)(dT * ((double)c->C6 / (1<<23)));


    /* Offset at actual temperature */
    double OFF = ((double)c->C2 * (1<<17)) + (((double)c->C4*dT) / (1<<6));

    /* Sensitivity at actual temperature */
    double SENS = ((double)c->C1 * (1<<16)) + (((double)c->C3*dT) / (1<<7));

    /* Second order temperature correction */
    if (TEMP < 2000) {          /* Less than 20ºC */

      TEMP -= ((uint64_t)dT*dT / (1<<31));
      OFF  -= 61 * ((double)(TEMP - 2000)*(TEMP - 2000)) / (1<<4);
      SENS -= 2  * ((double)(TEMP - 2000)*(TEMP - 2000));

      if (TEMP < -1500) {         /* Less than -15ºC */

        OFF  -= 15 * ((double)(TEMP + 1500)*(TEMP + 1500));
        SENS -=  8 * ((double)(TEMP + 1500)*(TEMP + 1500));
      }
    }

    /* Temperature compensated pressure (10 1200mbar with 0.01mbar resolution) */
    double P = (((D1 * SENS) / (1<<21)) - OFF) / (1<<15);

    barometer.temperature	= (double)TEMP / 100; /* degC */
#if BAROMETER_TYPE_MS5611
    barometer.pressure	= P/2;                        /* Pa. DIVIDE BY 2 FOR MS5611 ONLY */
#else
    barometer.pressure  = P;
#endif
    barometer.valid	= 1;

  } else {                      /* Barometer not initialised correctly */

    barometer.temperature = 0;
    barometer.pressure = 0;
    barometer.valid = 0;
  }

  return &barometer;
}

/**
 * Assume i2c_bb_init has already been called
 */
void barometer_init(void)
{
  /* Get the calibration parameters */
  get_cal_param(&calibration);

  /* Confirm checksum */
  if ((calibration.checksum & 0xF) == calibration_checksum(&calibration)) {
    ms5607_init_success = 1;
  }
}

#endif
