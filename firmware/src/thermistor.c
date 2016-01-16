/*
 * Thermistor equations
 * Copyright (C) 2015 Adam Cately, Richard Meadows
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

#include <stdio.h>
#include <math.h>

#define SERIES_RESISTOR 100000  /* 100k */

/**
 * Calculate the temperature corresponding to the voltage read from the potential divider.
 * Note: Uses full Steinhart–Hart equation
 * Parameters: ratio - divider voltage over supply voltage, 0 - 1
 */
float thermistor_ratio_to_temperature(float ratio)
{
  float resistance;

  /* Coefficients precomputed (can also use http://www.rusefi.com/Steinhart-Hart.html ) */
  float A = 0.0011293919788944533;
  float B = 0.00023410122017736258;
  float C = 0.00000008763737806081768;

  /* convert the value to resistance */
  resistance = SERIES_RESISTOR / ratio;
  resistance -= SERIES_RESISTOR;

  float logR = log(resistance);
  float temperature = A + B*logR + C*pow(logR, 3);
  temperature = 1.0 / temperature;

  return (float)temperature - 273.15;
}

/* void CalculateSteinhartCoefficients(){ */
/*   //Reference temperatures and corresponding resistances to tune algorithm */
/*   float sampleTemperatures[] = {-55.0, -15.0, 25.0}; */
/*   float sampleResistances[] = {963000, 72930, 10000}; */

/*   float L1 = log(sampleResistances[0]); */
/*   float L2 = log(sampleResistances[1]); */
/*   float L3 = log(sampleResistances[2]); */

/*   float Y1 = 1.0 / (sampleTemperatures[0] + 273.15); */
/*   float Y2 = 1.0 / (sampleTemperatures[1] + 273.15); */
/*   float Y3 = 1.0 / (sampleTemperatures[2] + 273.15); */

/*   float G2 = (Y2 - Y1) / (L2 - L1); */
/*   float G3 = (Y3 - Y1) / (L3 - L1); */

/*   float C = (G3 - G2) / (L3 - L2); */
/*   C /= (L1 + L2 + L3); */

/*   float B = G2; */
/*   B -= C*( (L1 * L1) + (L1 * L2) + (L2 * L2) ); */

/*   float A = Y1 - ( B + (L1*L1)*C )*L1; */

/*   printf("A: %g, B: %g, C: %g\n", A, B, C); */
/* } */
