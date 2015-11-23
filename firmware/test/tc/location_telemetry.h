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

  /* Supply co-ords in units of 100 nanodeg */
  location_telemetry_update(
    (int32_t)(location_telemetry_tc_params.lat * 10 * 1000 * 1000),
    (int32_t)(location_telemetry_tc_params.lon * 10 * 1000 * 1000)
    );

  location_telemetry_tc_results.tx_allow = location_telemetry_active();
}
