/*
 * Functions for MFSK encoding / decoding
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

#include <string.h>

#include "samd20.h"
#include "msfk.h"

//#define OLIVIA_MFSK_ENCODE_TEST
#ifdef OLIVIA_MFSK_ENCODE_TEST
#include <stdint.h>
#endif


static const uint64_t ScramblingCodeOlivia	= 0xE257E6D0291574ECLL;
static const uint64_t ScramblingCodeContestia	= 0xEDB88320LL;

/**
 * USEFUL RESOURES =============================================================
 *
 * http://en.wikipedia.org/wiki/Olivia_MFSK
 * http://www.arrl.org/olivia
 *
 * Also, although without images:
 * http://web.archive.org/web/20070927210543/http://homepage.sunrise.ch/mysunrise/jalocha/fht_coding.htm
 *
 * Useful guide to the differences between olivia and contestia
 * (ultimately we'd like to support both):
 * http://f1ult.free.fr/DIGIMODES/MULTIPSK/contestia_rttym_en.htm
 *
 * Also dl-fldigi's source might be helpful:
 * https://github.com/jamescoxon/dl-fldigi/blob/master/src/include/jalocha/pj_mfsk.h
 */

/**
 * This function encodes a single block of Olivia MFSK
 *
 * It takes a buffer of ASCII-encoded text and returns an array of
 * tones to transmit.
 */
void olivia_mfsk_encode_block(char* buffer, int8_t* tones)
{
  size_t bits_per_character = 7;

  size_t bits_per_symbol = 5; /* That is, there are 2^5=32 tones */
  size_t symbols_per_block = 64;




  /* TODO! */




}

#ifdef OLIVIA_MFSK_ENCODE_TEST
void main(void)
{
  /* Define a test string */
  char test_string[6];
  test_string = "HELLO";

  /* Define a buffer for the tones produced */
  int8_t tones[64];


  olivia_mfsk_encode_block(test_string, tones);


  /* Debug printout */
  for (int i = 0; i < sizeof(tones)/sizeof(int8_t); i++) {
    printf("%d ", tones[i]);
  }
  printf("\n");
}
#endif
