#ifndef __verification__
#define __verification__
#endif

#include "samd20.h"
#include "bmp180.h"

/****************************//* pressure_temperature_tc *//****************************/
/**
 * Write a description of your test case here
 */

/* Parameters in */
struct pressure_temperature_tc_params {

  /* Input paramters to your test case go here */
  uint32_t dummy;

} pressure_temperature_tc_params;
/* Results out */
struct pressure_temperature_tc_results {

  /* Result values should be populated here */
  float pressure, temperature;

} pressure_temperature_tc_results;
/* Function */
__verification__ void pressure_temperature_tc(void) {

  /**
   * The main body of the test case goes here.
   *
   * Use the input parameters to run the test case. Populate the
   * results structure at the end
   */
  struct barometer* b = get_barometer();

  pressure_temperature_tc_results.pressure    = (float)b->pressure;
  pressure_temperature_tc_results.temperature = (float)b->temperature;
}
