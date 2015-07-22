#ifndef __verification__
#define __verification__
#endif


/****************************//* location_telemetry_tc *//****************************/
/* Checks the location is returning the right things etc. etc                 */

#include "location.h"

/* Parameters in */
struct location_telemetry_tc_params {
  float lat;
  float lon;
} location_telemetry_tc_params;
/* Results out */
struct location_telemetry_tc_results {
  bool tx_allow;
} location_telemetry_tc_results;
/* Function */
__verification__ void location_telemetry_tc(void) {

  telemetry_location_update(location_telemetry_tc_params.lon, location_telemetry_tc_params.lat);

  location_telemetry_tc_results.tx_allow = telemetry_location_tx_allow();
}
