/*
 * C stubs for verification suite test cases
 * Copyright (C) 2014  Richard Meadows <richardeoin>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


/* CTypes just gets confused by function attributes... */
#ifdef CTYPESGEN
#define __verification__

#else
#define __verification__ \
  __attribute__ ((section(".text.verif"))) \
  __attribute__ ((optimize("O0")))	   \

#endif

/****************************//* nmea_decode_tc *//****************************/

/* Parameters in */
struct nmea_decode_tc_params {
  int in;
} nmea_decode_tc_params;
/* Results out */
struct nmea_decode_tc_results {
  int result;
} nmea_decode_tc_results;
/* Function */
__verification__ void nmea_decode_tc(void) {

  nmea_decode_tc_results.result = 2 * nmea_decode_tc_params.in;
}

/****************************//* times_two_tc *//****************************/
/* The simplest test case. ever.   Used to check for sanity                 */

/* Parameters in */
struct times_two_tc_params {
  int input;
} times_two_tc_params;
/* Results out */
struct times_two_tc_results {
  int result;
} times_two_tc_results;
/* Function */
__verification__ void times_two_tc(void) {

  times_two_tc_results.result = 2 * times_two_tc_params.input;
}

/*******************************//* tc_main *//********************************/

/**
 * Called at the start of the test case run
 */
__verification__ void tc_main(void) {
  /* Test enviroment initialisation */

  /* Wait forever while test cases execute */
  while (1);
}
