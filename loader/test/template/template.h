#ifndef __verification__
#define __verification__
#endif

/****************************//* [template]_tc *//****************************/
/**
 * Write a description of your test case here
 */

/* Parameters in */
struct [template]_tc_params {

  /* Input paramters to your test case go here */

} [template]_tc_params;
/* Results out */
struct [template]_tc_results {

  /* Result values should be populated here */

} [template]_tc_results;
/* Function */
__verification__ void [template]_tc(void) {

  /**
   * The main body of the test case goes here.
   *
   * Use the input parameters to run the test case. Populate the
   * results structure at the end
   */

  [template]_tc_results.result = 2 * [template]_tc_params.input;
}
