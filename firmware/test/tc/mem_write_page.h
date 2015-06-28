#ifndef __verification__
#define __verification__
#endif

/****************************//* mem_write_page_tc *//****************************/
/**
 * Erases sector, Writes a page in memory and reads it back
 */
#include "memory.h"

/* Parameters in */
struct mem_write_page_tc_params {

  /* Input paramters to your test case go here */
  uint32_t address;
  uint8_t page[256];

} mem_write_page_tc_params;
/* Results out */
struct mem_write_page_tc_results {

  /* Result values should be populated here */
  uint8_t page_read[256];

} mem_write_page_tc_results;
/* Function */
__verification__ void mem_write_page_tc(void) {

  /**
   * The main body of the test case goes here.
   *
   * Use the input parameters to run the test case. Populate the
   * results structure at the end
   */

  init_memory();

  mem_erase_sector(mem_write_page_tc_params.address);

  mem_write_page(mem_write_page_tc_params.address,
                 mem_write_page_tc_params.page, 0x100);

  mem_read_memory(mem_write_page_tc_params.address,
                  mem_write_page_tc_results.page_read, 0x100);
}
