#ifndef __verification__
#define __verification__
#endif

/****************************//* mem_write_all_tc *//****************************/
/**
 * Erases and writes the entire memory chip
 */
#include "memory.h"

/* Parameters in */
struct mem_write_all_tc_params {

  /* Input paramters to your test case go here */
  uint8_t page_data[256];

} mem_write_all_tc_params;
/* Results out */
struct mem_write_all_tc_results {

  /* Result values should be populated here */
  uint8_t all_good;
  uint32_t fail_address;
  uint8_t fail_wrote, fail_read;

} mem_write_all_tc_results;
/* Function */
__verification__ void mem_write_all_tc(void) {

  /**
   * The main body of the test case goes here.
   *
   * Use the input parameters to run the test case. Populate the
   * results structure at the end
   */
  uint8_t page_read[0x100];
  uint32_t i, j;

  mem_power_on();
  mem_chip_erase();

  for (i = 0; i < TOTAL_PAGES; i++) {
    mem_write_page(i * 0x100, mem_write_all_tc_params.page_data, 0x100);
  }
  for (i = 0; i < TOTAL_PAGES; i++) {
    mem_read_memory(i * 0x100, page_read, 0x100);

    for (j = 0; j < 0x100; j++) {
      if (page_read[j] != mem_write_all_tc_params.page_data[j]) {
        /* Error */
        mem_write_all_tc_results.all_good = 0;
        mem_write_all_tc_results.fail_address = (i * 0x100) + j;
        mem_write_all_tc_results.fail_wrote =
          mem_write_all_tc_params.page_data[i];
        mem_write_all_tc_results.fail_read = page_read[i];

        return;
      }
    }
  }

  /* All good */
  mem_write_all_tc_results.all_good = 1;
}
