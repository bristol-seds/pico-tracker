#ifndef __verification__
#define __verification__
#endif


/****************************//* location_aprs_tc *//****************************/
/* Checks the location is returning the right things etc. etc                 */

#include "location.h"
#include "aprs.h"

/* Parameters in */
struct location_aprs_tc_params {
  float lat;
  float lon;
} location_aprs_tc_params;
/* Results out */
struct location_aprs_tc_results {
  bool tx_allow;
  double frequency;
  char* prefix;
  char* callsign;
} location_aprs_tc_results;
/* Function */
__verification__ void location_aprs_tc(void) {

  /* Supply co-ords in units of 100 nanodeg */
  location_aprs_update(
    (int32_t)(location_aprs_tc_params.lat * 10 * 1000 * 1000),
    (int32_t)(location_aprs_tc_params.lon * 10 * 1000 * 1000)
    );

  location_prefix_update(
    (int32_t)(location_aprs_tc_params.lat * 10 * 1000 * 1000),
    (int32_t)(location_aprs_tc_params.lon * 10 * 1000 * 1000)
    );


  location_aprs_tc_results.tx_allow = location_aprs_could_tx();
  location_aprs_tc_results.frequency = location_aprs_frequency();
  location_aprs_tc_results.prefix = location_prefix();
  location_aprs_tc_results.callsign = aprs_callsign(location_aprs_call());
}
