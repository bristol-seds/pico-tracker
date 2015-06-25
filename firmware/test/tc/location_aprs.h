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

  aprs_location_update(location_aprs_tc_params.lon, location_aprs_tc_params.lat, 0);

  location_aprs_tc_results.tx_allow = aprs_location_tx_allow();
  location_aprs_tc_results.frequency = aprs_location_frequency();
}
