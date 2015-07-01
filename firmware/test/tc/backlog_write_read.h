#ifndef __verification__
#define __verification__
#endif

/****************************//* backlog_write_read_tc *//****************************/
/**
 * Write a description of your test case here
 */
#include "backlog.h"
#include "data.h"

/* Parameters in */
struct backlog_write_read_tc_params {

  /* Input paramters to your test case go here */
  uint8_t record_not_get;
  uint32_t epoch_write;

} backlog_write_read_tc_params;
/* Results out */
struct backlog_write_read_tc_results {

  /* Result values should be populated here */
  uint32_t epoch_read;
  uint32_t wptr, rptr;

} backlog_write_read_tc_results;
/* Function */
extern uint16_t backlog_write_index;
__verification__ void backlog_write_read_tc(void) {

  struct tracker_datapoint dp;
  struct tracker_datapoint* dp_ptr;

  /**
   * The main body of the test case goes here.
   *
   * Use the input parameters to run the test case. Populate the
   * results structure at the end
   */

  if (backlog_write_read_tc_params.record_not_get) { /* Record */

    dp.time.epoch = backlog_write_read_tc_params.epoch_write;
    record_backlog(&dp);
    backlog_write_read_tc_results.wptr = backlog_write_index;
  } else {

    dp_ptr = get_backlog();
    backlog_write_read_tc_results.epoch_read = dp_ptr->time.epoch;
  }
}
