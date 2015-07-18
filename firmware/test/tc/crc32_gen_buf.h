#ifndef __verification__
#define __verification__
#endif

/****************************//* crc32_gen_buf_tc *//****************************/
/**
 * Checks crc32 generation and uint8_t buffer put/get
 */
#include "crc.h"

#define TEST_BUF_LEN 32

/* Parameters in */
struct crc32_gen_buf_tc_params {

  /* Input paramters to your test case go here */
  uint8_t test_buffer[TEST_BUF_LEN];

} crc32_gen_buf_tc_params;
/* Results out */
struct crc32_gen_buf_tc_results {

  /* Result values should be populated here */
  uint32_t calculated_crc32;
  uint32_t extracted_crc32;

} crc32_gen_buf_tc_results;
/* Function */
__verification__ void crc32_gen_buf_tc(void) {

  /**
   * The main body of the test case goes here.
   *
   * Use the input parameters to run the test case. Populate the
   * results structure at the end
   */
  uint32_t test;
  uint8_t* uint8t_buffer = (uint8_t*)&test;

  crc32_gen_buf_tc_results.calculated_crc32 = calculate_crc32(crc32_gen_buf_tc_params.test_buffer,
                                                      TEST_BUF_LEN);

  /* uint8_t buffer put/get */
  put_crc32(uint8t_buffer, crc32_gen_buf_tc_results.calculated_crc32);
  crc32_gen_buf_tc_results.extracted_crc32 = get_crc32(uint8t_buffer);
}
