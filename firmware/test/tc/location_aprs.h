#ifndef __verification__
#define __verification__
#endif


/****************************//* location_aprs_tc *//****************************/
/* Checks the location is returning the right things etc. etc                 */

#include "location.h"

/* Parameters in */
struct location_aprs_tc_params {
  float lat;
  float lon;
} location_aprs_tc_params;
/* Results out */
struct location_aprs_tc_results {
  bool tx_allow;
  float frequency;
} location_aprs_tc_results;
/* Function */
__verification__ void location_aprs_tc(void) {

  /* Supply co-ords in units of 100 nanodeg */
  location_aprs_update(
    (int32_t)(location_aprs_tc_params.lon * 10 * 1000 * 1000),
    (int32_t)(location_aprs_tc_params.lat * 10 * 1000 * 1000)
    );

  location_aprs_tc_results.tx_allow = location_aprs_active();
  location_aprs_tc_results.frequency = location_aprs_frequency();
}
