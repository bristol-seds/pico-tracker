#ifndef __verification__
#define __verification__
#endif

/****************************//* epoch_from_time_tc *//****************************/
/**
 * Write a description of your test case here
 */
#include "data.h"

/* Parameters in */
struct epoch_from_time_tc_params {

  /* Input paramters to your test case go here */
  int dummy;

} epoch_from_time_tc_params;
/* Results out */
struct epoch_from_time_tc_results {

  /* Result values should be populated here */
  uint32_t epoch;

} epoch_from_time_tc_results;
/* Function */
__verification__ void epoch_from_time_tc(void) {

  /**
   * The main body of the test case goes here.
   *
   * Use the input parameters to run the test case. Populate the
   * results structure at the end
   */

  struct tracker_time t;
  t.year = 2016;
  t.month = 6;
  t.day = 30;
  t.hour = 18;
  t.minute = 48;
  t.second = 0;

  /* Get epoch */
  epoch_from_time_tc_results.epoch = get_epoch_from_time(&t);
}
