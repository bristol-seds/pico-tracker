#ifndef __verification__
#define __verification__
#endif

/****************************//* location_aprs_tc *//****************************/
/* Checks the location is returning the right things etc. etc                 */

/* Parameters in */
struct location_aprs_tc_params {
  int input;
} location_aprs_tc_params;
/* Results out */
struct location_aprs_tc_results {
  int result;
} location_aprs_tc_results;
/* Function */
__verification__ void location_aprs_tc(void) {

  location_aprs_tc_results.result = 2 * location_aprs_tc_params.input;
}
