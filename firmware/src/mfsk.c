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
#include "mfsk.h"
#include "math/fwht.h"

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





size_t BitsPerSymbol = 5;
size_t Symbols = 32;

size_t BitsPerCharacter = 7;
size_t SymbolsPerBlock = 64;

uint8_t bContestia = 0;

int8_t FHT_Buffer[64]; /* SymbolsPerBlock */
uint8_t OutputBlock[64]; /* SymbolsPerBlock */

static const uint64_t ScramblingCode = 0xE257E6D0291574ECLL;

void EncodeCharacter(uint8_t Char) {
  size_t TimeBit;
  uint8_t Mask = (SymbolsPerBlock << 1) - 1;

  if (bContestia) {
    if (Char >= 'a' && Char <= 'z')
      Char += 'A' - 'a';
    if (Char == ' ')
      Char = 59;
    else if (Char == '\r')
      Char = 60;
    else if (Char == '\n')
      Char = 0;
    else if (Char >= 33 && Char <= 90)
      Char -= 32;
    else if (Char == 8)
      Char = 61;
    else if (Char == 0)
      Char = 0;
    else
      Char = '?' - 32;
//} else if (bRTTYM) {
  } else {
    Char &= Mask;
  }

  for (TimeBit = 0; TimeBit < SymbolsPerBlock; TimeBit++)
    FHT_Buffer[TimeBit] = 0;
  if (Char<SymbolsPerBlock)
    FHT_Buffer[Char] = 1;
  else
    FHT_Buffer[Char-SymbolsPerBlock] = (-1);
  ifwht(FHT_Buffer, SymbolsPerBlock);
}

void ScrambleFHT(size_t CodeOffset)
{ size_t TimeBit;
  size_t CodeWrap=(SymbolsPerBlock-1);
  size_t CodeBit=CodeOffset&CodeWrap;
  for (TimeBit=0; TimeBit<SymbolsPerBlock; TimeBit++)
  { uint64_t CodeMask=1; CodeMask<<=CodeBit;
    if (ScramblingCode&CodeMask)
      FHT_Buffer[TimeBit] = (-FHT_Buffer[TimeBit]);
    CodeBit+=1; CodeBit&=CodeWrap; }
}

void EncodeBlock(uint8_t *InputBlock) {
  size_t FreqBit;
  size_t TimeBit;
  size_t nShift;

  nShift = (bContestia) ? 5 : 13; // Contestia/RTTYM or Olivia

  for (TimeBit = 0; TimeBit < SymbolsPerBlock; TimeBit ++)
    OutputBlock[TimeBit] = 0;

  for (FreqBit = 0; FreqBit < BitsPerSymbol; FreqBit++) {
    EncodeCharacter(InputBlock[FreqBit]);
    ScrambleFHT(FreqBit * nShift);
    size_t Rotate = 0;
    for (TimeBit = 0; TimeBit < SymbolsPerBlock; TimeBit++) {
      if (FHT_Buffer[TimeBit] < 0) {
        size_t Bit = FreqBit+Rotate;
        if (Bit >= BitsPerSymbol) Bit -= BitsPerSymbol;
        uint8_t Mask = 1;
        Mask <<= Bit;
        OutputBlock[TimeBit] |= Mask;
      }
      Rotate += 1;
      if (Rotate >= BitsPerSymbol)
        Rotate -= BitsPerSymbol;
    }
  }
}







/**
 * This function encodes a single block of Olivia MFSK
 *
 * It takes a buffer of ASCII-encoded text and returns an array of
 * tones to transmit.
 */
void olivia_mfsk_encode_block(char* buffer, uint8_t* tones)
{
  size_t bits_per_character = 7;

  size_t bits_per_symbol = 5; /* That is, there are 2^5=32 tones */
  size_t symbols_per_block = 64;

  /* For the moment do this */
  EncodeBlock((uint8_t*)buffer);
  memcpy(tones, OutputBlock, symbols_per_block);
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
