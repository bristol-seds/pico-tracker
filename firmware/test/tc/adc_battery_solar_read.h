#ifndef __verification__
#define __verification__
#endif

/****************************//* adc_battery_solar_read_tc *//****************************/
/**
 * Write a description of your test case here
 */
#include "analogue.h"

/* Parameters in */
struct adc_battery_solar_read_tc_params {

  /* Input paramters to your test case go here */
  uint32_t dummy;

} adc_battery_solar_read_tc_params;
/* Results out */
struct adc_battery_solar_read_tc_results {

  /* Result values should be populated here */
  float battery;
  float solar;

} adc_battery_solar_read_tc_results;
/* Function */
__verification__ void adc_battery_solar_read_tc(void) {

  /**
   * The main body of the test case goes here.
   *
   * Use the input parameters to run the test case. Populate the
   * results structure at the end
   */

  start_adc_sequence();
  while (!is_adc_sequence_done());

  adc_battery_solar_read_tc_results.battery = get_battery();
  adc_battery_solar_read_tc_results.solar = get_solar();
}
