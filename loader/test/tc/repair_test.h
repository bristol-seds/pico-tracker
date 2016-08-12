#ifndef __verification__
#define __verification__
#endif

/****************************//* repair_test_tc *//****************************/
/**
 * Write a description of your test case here
 */
#include <string.h>

#include "flash.h"
#include "memory.h"

/* Parameters in */
struct repair_test_tc_params {

  /* Input paramters to your test case go here */
  uint32_t address_to_corrupt;

} repair_test_tc_params;
/* Results out */
struct repair_test_tc_results {

  /* Result values should be populated here */
  uint32_t errors_corrected;
  int memcmp_result;

} repair_test_tc_results;
/* Function */
__verification__ void repair_test_tc(void) {

  /**
   * The main body of the test case goes here.
   *
   * Use the input parameters to run the test case. Populate the
   * results structure at the end
   */

  if (repair_test_tc_params.address_to_corrupt) {
    mem_write_word((unsigned int*)repair_test_tc_params.address_to_corrupt, 0);
  }

  /* repair */
  repair_test_tc_results.errors_corrected = check_and_repair_memory();

  /* check memory */
  repair_test_tc_results.memcmp_result =
    memcmp((void*)D1_START, (void*)D2_START, APPLICATION_LENGTH);
}
