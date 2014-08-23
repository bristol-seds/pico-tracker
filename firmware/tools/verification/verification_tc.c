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

/* Includes */
#include <nmea/nmea.h>
#include <string.h>

/* Parameters in */
struct nmea_tc_params {
  char buff[2048];
} nmea_tc_params;

/* Results out */
nmeaINFO nmea_tc_results;

/* Function */
__verification__ void nmea_tc(void) {

  nmeaPARSER parser;
  int size;

  /* Get input buffer size */
  size = strlen(nmea_tc_params.buff);

  /* Init nmea */
  nmea_zero_INFO(&nmea_tc_results);
  nmea_parser_init(&parser);

  /* Parse */
  nmea_parse(&parser, nmea_tc_params.buff, size, &nmea_tc_results);

  /* Cleanup */
  nmea_parser_destroy(&parser);
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

typedef void (*tc_ptr_type)(void);
volatile tc_ptr_type tc_ptr;

/**
 * Runs a test case
 */
__verification__ void tc_run() {


   (*tc_ptr)();
}

/**
 * Called to trigger the test case run
 */
__verification__ void tc_main(void) {

  /* Wait forever while test cases execute */
  while (1) {
    tc_run();
  }
}
