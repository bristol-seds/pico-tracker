#ifndef __verification__
#define __verification__
#endif

/****************************//* backlog_read_tc *//****************************/
/**
 * Write a description of your test case here
 */
#include <stddef.h>

#include "backlog.h"
#include "data.h"
#include "aprs.h"

/* Parameters in */
struct backlog_read_tc_params {

  uint32_t dummy;

} backlog_read_tc_params;
/* Results out */
struct backlog_read_tc_results {

  /* Result values should be populated here */
  char aprs_backlog_str[256];
  uint8_t returned_null;

} backlog_read_tc_results;
/* Function */
extern uint16_t backlog_index;
__verification__ void backlog_read_tc(void) {

  struct tracker_datapoint* dp_ptr;

  /**
   * The main body of the test case goes here.
   *
   * Use the input parameters to run the test case. Populate the
   * results structure at the end
   */

  /* Read and format backlog */
  dp_ptr = get_backlog();

  if (dp_ptr != NULL) {
    encode_backlog(backlog_read_tc_results.aprs_backlog_str, dp_ptr);
    backlog_read_tc_results.returned_null = 0;
  } else {
    backlog_read_tc_results.returned_null = 1;
  }
}
