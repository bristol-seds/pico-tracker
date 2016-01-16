#ifndef __verification__
#define __verification__
#endif

/****************************//* thermistor_equation_tc *//****************************/
/**
 * Write a description of your test case here
 */

#include "thermistor.h"

/* Parameters in */
struct thermistor_equation_tc_params {

  /* Input paramters to your test case go here */
  float value;

} thermistor_equation_tc_params;
/* Results out */
struct thermistor_equation_tc_results {

  /* Result values should be populated here */
  float temperature;

} thermistor_equation_tc_results;
/* Function */
__verification__ void thermistor_equation_tc(void) {

  /**
   * The main body of the test case goes here.
   *
   * Use the input parameters to run the test case. Populate the
   * results structure at the end
   */

  thermistor_equation_tc_results.temperature =
    thermistor_ratio_to_temperature(thermistor_equation_tc_params.value);
}
