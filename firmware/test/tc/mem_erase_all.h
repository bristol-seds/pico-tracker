#ifndef __verification__
#define __verification__
#endif

/****************************//* mem_erase_all_tc *//****************************/
/**
 * Write a description of your test case here
 */
#include "memory.h"

/* Parameters in */
struct mem_erase_all_tc_params {

  /* Input paramters to your test case go here */
  uint32_t dummy;

} mem_erase_all_tc_params;
/* Results out */
struct mem_erase_all_tc_results {

  /* Result values should be populated here */
  uint32_t dummy;

} mem_erase_all_tc_results;
/* Function */
__verification__ void mem_erase_all_tc(void) {

  /**
   * The main body of the test case goes here.
   *
   * Use the input parameters to run the test case. Populate the
   * results structure at the end
   */

  mem_power_on();

  mem_chip_erase();
}
