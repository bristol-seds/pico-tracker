#ifndef __verification__
#define __verification__
#endif

/****************************//* analogue_read_tc *//****************************/
/**
 * Write a description of your test case here
 */
#include "analogue.h"
#include "thermistor.h"

/* Parameters in */
struct analogue_read_tc_params {

  /* Input paramters to your test case go here */
  uint32_t dummy;

} analogue_read_tc_params;
/* Results out */
struct analogue_read_tc_results {

  /* Result values should be populated here */
  float battery;
  float thermistor;
  float solar;

} analogue_read_tc_results;
/* Function */
__verification__ void analogue_read_tc(void) {

  /**
   * The main body of the test case goes here.
   *
   * Use the input parameters to run the test case. Populate the
   * results structure at the end
   */

  start_adc_sequence();
  while (!is_adc_sequence_done());

  analogue_read_tc_results.battery = get_battery();
  analogue_read_tc_results.thermistor = thermistor_ratio_to_temperature(get_thermistor());
  analogue_read_tc_results.solar = get_solar();
}
