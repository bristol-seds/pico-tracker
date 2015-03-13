#ifndef __verification__
#define __verification__
#endif

/****************************//* times_two_tc *//****************************/
/* The simplest test case. ever.   Used to check for sanity                 */

/* Parameters in */
struct times_two_tc_params {
  int input;
} times_two_tc_params;
/* Results out */
struct times_two_tc_results {
  int result;
} times_two_tc_results;
/* Function */
__verification__ void times_two_tc(void) {

  times_two_tc_results.result = 2 * times_two_tc_params.input;
}
