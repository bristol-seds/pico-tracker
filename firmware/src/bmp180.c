/*
 * BMP180 (also BMP085)
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

#include <stdbool.h>
#include <stdint.h>

#include "samd20.h"
#include "hw_config.h"

#if BAROMETER_TYPE_BMP180

#include "barometer.h"
#include "sercom/i2c.h"

#define BMP180_ADDRESS		0xEE
#define BMP180_REG_ID		0xD0
#define BMP180_REG_SOFTRESET	0xE0
#define BMP180_REG_CTRLMEAS	0xF4
#define BMP180_REG_DATA		0xF6

/**
 * Control Register Values
 */
typedef enum {
  TEMPERATURE			= 0x2E,
  PRESSURE_ULTRALOW		= 0x34, //  4500µS Delay
  PRESSURE_STANDARD		= 0x74, //  7500µS Delay
  PRESSURE_HIGHRES		= 0xB4, // 13500µS Delay
  PRESSURE_ULTRAHIGHRES		= 0xF4  // 22500µS Delay
} bmp085_command;

/**
 * The mode of pressure measurement
 */
#define PRESSURE_MODE		PRESSURE_HIGHRES// PRESSURE_STANDARD
#define PRESSURE_DELAY		13500//7500

#define TEMPERATURE_DELAY	4500

/**
 * Barometer data structure
 */
struct barometer barometer;

/**
 * Calibration Values
 */
struct calibration {
  int16_t AC1, AC2, AC3, B1, B2, MB, MC, MD;
  uint16_t AC4, AC5, AC6;
} calibration;

/**
 * Lookup table for oversampling values.
 */
uint8_t oversampling(void) {
  switch(PRESSURE_MODE) {
    case PRESSURE_ULTRALOW:
      return 0;
    case PRESSURE_HIGHRES:
      return 2;
    case PRESSURE_ULTRAHIGHRES:
      return 3;
    default: // PRESSURE_STANDARD
      return 1;
  }
}

/**
 * Implements a microsecond delay
 */
void delay_us(uint16_t microseconds) {
  int32_t i = microseconds * 16;

  while(i--);
}

/**
 * Utility function to read from a 8-bit register
 */
uint8_t read_8(char address) {
  uint8_t buffer[3];
  buffer[0] = address;

  /* Set regsiter to read */
  i2c_master_write(BMP180_ADDRESS, buffer, 1);

  /* Read it */
  i2c_master_read(BMP180_ADDRESS, buffer, 1);

  return buffer[0];
}
/**
 * Utility function to read a 16-bit register.
 */
uint16_t read_16(char address) {
  uint8_t buffer[3];
  buffer[0] = address;

  /* Set regsiter to read */
  i2c_master_write(BMP180_ADDRESS, buffer, 1);

  /* Read it */
  i2c_master_read(BMP180_ADDRESS, buffer, 2);

  return (buffer[0] << 8) | buffer[1];
}
/**
 * Reads off the BMP180's calibration values.
 */
void get_cal_param(struct calibration *c) {
  c->AC1 = read_16(0xAA);
  c->AC2 = read_16(0xAC);
  c->AC3 = read_16(0xAE);
  c->AC4 = read_16(0xB0);
  c->AC5 = read_16(0xB2);
  c->AC6 = read_16(0xB4);
  c->B1  = read_16(0xB6);
  c->B2  = read_16(0xB8);
  c->MB  = read_16(0xBA);
  c->MC  = read_16(0xBC);
  c->MD  = read_16(0xBE);
}
/**
 * Writes a command to the BMP085's control register.
 */
void write_command(bmp085_command command) {

  uint8_t buffer[3];
  buffer[0] = BMP180_REG_CTRLMEAS;
  buffer[1] = command;

  /* Write to command register */
  i2c_master_write(BMP180_ADDRESS, buffer, 2);
}


/**
 * Takes a temperature measurement and returns the uncompensated value.
 */
int32_t get_ut(void) {
  write_command(TEMPERATURE);

  delay_us(TEMPERATURE_DELAY);

  return read_16(BMP180_REG_DATA);
}
/**
 * Takes a pressure measurement and returns the uncompenstated value.
 */
int32_t get_up(void) {
  uint8_t buffer[3];
  write_command(PRESSURE_MODE);

  delay_us(PRESSURE_DELAY);

  buffer[0] = BMP180_REG_DATA;

  /* Set regsiter to read */
  i2c_master_write(BMP180_ADDRESS, buffer, 1);

  /* Read it */
  i2c_master_read(BMP180_ADDRESS | 1, buffer, 3);

  return ((buffer[0] << 16) | (buffer[1] << 8) |
          buffer[2]) >> (8 - oversampling());

}

/* -----------------------------------------------------------------------------
 * Calculations
 */


/**
 * Returns the variable B5, which is used for both temperature and
 * pressure calculations.
 */
int32_t get_b5(struct calibration *c, int32_t ut) {
  int32_t x1, x2;

  x1 = ((ut - c->AC6) * c->AC5) >> 15;
  x2 = (double)(c->MC << 11) / (x1 + c->MD);
  return x1 + x2; // B5
}
/**
 * Returns the temperature in °C using variable B5
 */
double get_temperature(int32_t B5) {
  return (double)((B5 + 8) >> 4) / 10;
}
/**
 * Returns the pressure in pascals using the calibration and variable
 * B5.
 */
int32_t get_pressure(struct calibration *c, int32_t B5, int32_t up) {
  int64_t B6, X1, X2, X3, B3, pressure;
  uint64_t B4, B7;

  B6 = B5 - 4000;
  X1 = (c->B2 * ((B6 * B6) >> 12)) >> 11;
  X2 = (c->AC2 * B6) >> 11;
  X3 = X1 + X2;
  B3 = ((((((int64_t)(c->AC1) * 4) + X3) << oversampling()) + 2) >> 2);
  X1 = (c->AC3 * B6) >> 13;
  X2 = (c->B1 * ((B6 * B6) >> 12)) >> 16;
  X3 = ((X1 + X2) + 2) >> 2;
  B4 = (c->AC4 * (uint64_t)(X3 + 32768)) >> 15;
  B7 = ((uint64_t)(up - B3) * (50000 >> oversampling()));

  if (B7 < 0x80000000) {
    pressure = (B7 << 1) / B4;
  } else {
    pressure = (B7 / B4) << 1;
  }

  X1 = (pressure >> 8);
  X1 *= X1;
  X1 = (X1 * 3038) >> 16;
  X2 = (-7357 * pressure) >> 16;
  pressure += (X1 + X2 + 3791) >> 4;

  return pressure;
}

struct barometer* get_barometer(void)
{
  int32_t ut = get_ut();
  int32_t up = get_up();
  int32_t b5 = get_b5(&calibration, ut);

  barometer.temperature = get_temperature(b5);
  barometer.pressure = get_pressure(&calibration, b5, up);

  return &barometer;
}

/**
 * Assume twi_master_init has already been called
 */
void bmp180_init(void)
{
  /* Read ID */
  uint8_t id = read_8(BMP180_REG_ID);

  /* Check ID */
  switch(id) {
    case 0x85:                  /* BMP085 */
    case 0x55:                  /* BMP180 */
      break;
    default:                    /* ?????? */
      while(1);
  }

  /* Get the calibration parameters */
  get_cal_param(&calibration);
}

#endif
