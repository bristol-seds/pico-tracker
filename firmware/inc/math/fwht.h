/*
 * Fast Hadamard Transform
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

#ifndef FHT_H
#define FHT_H

#ifdef FWHT_TEST
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#else
#include "samd20.h"
#endif

/**
 * Resources:
 * http://en.wikipedia.org/wiki/Fast_Walsh%E2%80%93Hadamard_transform
 *
 * The Fast Walsh-Hadamard Transform is a divide and conquer algorithm
 * with a complexity of NlogN.
 */

/**
 * Fast Walsh-Hadamard Transform
 *
 * `data` an array that is the input vector. It is modifed in-place to
 * become the output vector.
 * `length` is the number of elements in `data`. This must be a power of two
 *
 * The result is returned in "hadamard" order
 */
void fwht(int8_t *data, size_t length)
{
  size_t step, decomp_index, index;
  int8_t lvalue, rvalue;

  /**
   * The transform is decomposed into smaller WHTs.
   * We ignore the normalisation factors.
   */

  /* Iterate through the decompositions of size length --> 8, 4, 2 */
  for (step = length / 2; step; step /= 2) {

    /* Iterate through each decomposition for this step size */
    for(decomp_index = 0; decomp_index < length; decomp_index += (step*2)) {

      /* Interate through each DFT in the decomposition */
      for(index = 0; index < step; index++) {

        /* Compute a two-point Discrete Fourier Transform (DFT) */
        lvalue = data[decomp_index + index];
        rvalue = data[decomp_index + index + step];

        /* Sum */
        data[decomp_index + index]        = lvalue + rvalue;
        /* Difference */
        data[decomp_index + index + step] = lvalue - rvalue;
      }
    }
  }
}

/**
 * Inverse Fast Walsh-Hadamard Transform
 *
 * `data` an array that is the input vector. It is modifed in-place to
 * become the output vector.
 * `len` is the number of elements in `data`.
 *
 * The result is returned in "hadamard" order
 */
void ifwht(int8_t *data, size_t length)
{
  size_t step, decomp_index, index;
  int8_t lvalue, rvalue;

  /**
   * The transform is decomposed into smaller WHTs.
   * We ignore the normalisation factors.
   */

  /* Iterate through the decompositions of size length --> 8, 4, 2 */
  for (step = length / 2; step; step /= 2) {

    /* Iterate through each decomposition for this step size */
    for(decomp_index = 0; decomp_index < length; decomp_index += (step*2)) {

      /* Interate through each IDFT in the decomposition */
      for(index = 0; index < step; index++) {

        /* Compute a two-point Inverse Discrete Fourier Transform (IDFT) */
        lvalue = data[decomp_index + index];
        rvalue = data[decomp_index + index + step];

        /* Difference */
        data[decomp_index + index]        = lvalue - rvalue;
        /* Sum */
        data[decomp_index + index + step] = lvalue + rvalue;
      }
    }
  }
}

#ifdef FWHT_TEST

#include "string.h"

int main(void)
{
  int8_t fwht_test[] = {1,0,1,0,0,1,1,0};
  const int fwht_test_len = 8;

  fwht(fwht_test, fwht_test_len);

  printf("Fast Walsh-Hadamard Transform: [");
  for (int i; i < 8; i++) { printf("%d,", fwht_test[i]); }
  printf("]\n");



  int8_t ifwht_test[] = {1,0,1,0,0,1,1,0};
  const int ifwht_test_len = 8;

  ifwht(ifwht_test, ifwht_test_len);

  printf("Inverse Fast Walsh-Hadamard Transform: [");
  for (int i; i < 8; i++) { printf("%d,", ifwht_test[i]); }
  printf("]\n");

  return 0;
}
#endif


#endif /* FHT_H */
