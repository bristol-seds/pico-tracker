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

/***************************** test cases *******************************/

#include "times_two.h"
/* [new_tc] */


/******************************* tc_main ********************************/

#include "init.h"

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

/**
 * Prelude to main loop
 */
__verification__ void tc_prelude(void) {

  /* Initialise the board */
  init(INIT_TESTCASE);

  /* Proceed to main loop */
  tc_main();
}
