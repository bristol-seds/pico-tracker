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
 *
 *
 */

/**
 * We might want to use this with different types in the future.
 */
#define T int8_t

/**
 * Fast Walsh-Hadamard Transform
 *
 * `data` an array that is the input vector. Modifed in-place
 * `len` is the number of elements in `data`
 *
 * The result is returned in "hadamard" order
 */
void fwht(T *Data, size_t Len)
{
  size_t Step, Ptr, Ptr2; T Bit1, Bit2, NewBit1, NewBit2;
  for(Step=1; Step<Len; Step*=2)
  { for(Ptr=0; Ptr<Len; Ptr+=2*Step)
    { for(Ptr2=Ptr; (Ptr2-Ptr)<Step; Ptr2+=1)
      { Bit1=Data[Ptr2];  Bit2=Data[Ptr2+Step];
        NewBit1=Bit2; NewBit1+=Bit1;
        NewBit2=Bit2; NewBit2-=Bit1;
        Data[Ptr2]=NewBit1; Data[Ptr2+Step]=NewBit2;
      }
    }
  }
}

/**
 * Inverse Fast Hadamard Transform
 *
 * `data` an array that is the input vector. Modifed in-place
 * `len` is the number of elements in `data`
 *
 * The result is returned in "hadamard" order
 */
void ifwht(T *Data, size_t len)
{
  size_t step, Ptr, Ptr2; T Bit1, Bit2, NewBit1, NewBit2;
  for(step=len/2; step; step/=2)
  { for(Ptr=0; Ptr<len; Ptr+=2*step)
    { for(Ptr2=Ptr; (Ptr2-Ptr)<step; Ptr2+=1)
      { Bit1=Data[Ptr2];  Bit2=Data[Ptr2+step];
        NewBit1=Bit1; NewBit1-=Bit2;
        NewBit2=Bit1; NewBit2+=Bit2;
        Data[Ptr2]=NewBit1; Data[Ptr2+step]=NewBit2;
      }
    }
  }
}

#ifdef FWHT_TEST

#include "string.h"

int main(void)
{
  T fwht_test[] = {1,0,1,0,0,1,1,0};
  const int fwht_test_len = 8;

  fwht(fwht_test, fwht_test_len);

  printf("Fast Walsh-Hadamard Transform: [");
  for (int i; i < 8; i++) { printf("%d,", fwht_test[i]); }
  printf("]\n");



  T ifwht_test[] = {1,0,1,0,0,1,1,0};
  const int ifwht_test_len = 8;

  ifwht(ifwht_test, ifwht_test_len);

  printf("Inverse Fast Walsh-Hadamard Transform: [");
  for (int i; i < 8; i++) { printf("%d,", ifwht_test[i]); }
  printf("]\n");

  return 0;
}
#endif


#endif /* FHT_H */
