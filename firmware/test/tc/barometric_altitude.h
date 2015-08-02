#ifndef __verification__
#define __verification__
#endif

/****************************//* barometric_altitude_tc *//****************************/
/**
 * Write a description of your test case here
 */
#include "altitude.h"

/* Parameters in */
struct barometric_altitude_tc_params {

  /* Input paramters to your test case go here */
  float pressure;            /* Pascals */

} barometric_altitude_tc_params;
/* Results out */
struct barometric_altitude_tc_results {

  /* Result values should be populated here */
  double altitude;              /* meters */

} barometric_altitude_tc_results;
/* Function */
__verification__ void barometric_altitude_tc(void) {

  /**
   * The main body of the test case goes here.
   *
   * Use the input parameters to run the test case. Populate the
   * results structure at the end
   */

  barometric_altitude_tc_results.altitude =
    pressure_to_altitude(barometric_altitude_tc_params.pressure);
}
