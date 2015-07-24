#ifndef __verification__
#define __verification__
#endif

/****************************//* location_aprs_file_tc *//****************************/
/* Checks the location is returning the right things etc. etc                 */

#include "location.h"

/* Parameters in */
struct location_aprs_file_tc_params {

  /* Input paramters to your test case go here */
  float lat;
  float lon;

} location_aprs_file_tc_params;
/* Results out */
struct location_aprs_file_tc_results {

  /* Result values should be populated here */
  bool tx_allow;
  float frequency;

} location_aprs_file_tc_results;
/* Function */
__verification__ void location_aprs_file_tc(void) {

  /* Supply co-ords in units of 100 nanodeg */
  aprs_location_update(
    (int32_t)(location_aprs_file_tc_params.lon * 10 * 1000 * 1000),
    (int32_t)(location_aprs_file_tc_params.lat * 10 * 1000 * 1000)
    );

  location_aprs_file_tc_results.tx_allow = aprs_location_tx_allow();
  location_aprs_file_tc_results.frequency = aprs_location_frequency();
}
