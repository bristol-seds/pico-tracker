/*
 * Function related to the flash memory
 * Copyright (C) 2016  Richard Meadows <richardeoin>
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

#include <stdlib.h>

#include "samd20.h"
#include "memory.h"
#include "flash.h"

/**
 * we put our checksum at the last address in flash
 */
volatile unsigned int* flash_checksum =
  (unsigned int*)(FLASH_ADDR + FLASH_SIZE - 4);


// ---------------------------- crc32cx --------------------------------

/* This is crc32b modified to load the message a fullword at a time.
   It assumes the message is word aligned and consists of an integral
   number of words before the 0-byte that marks the end of the message.
   This works only on a little-endian machine.
   Not counting the table setup (which would probably be a separate
   function), this function should be doable in 3 + 22w instructions, where
   w is the number of fullwords in the input message. This is equivalent to
   3 + 5.5n instructions, where n is the number of bytes. 1.25 of those 5.5
   instructions are loads.
   This is Exercise 1 in the text. C.f. Christopher Dannemiller,
   who got it from Linux Source base,
   www.gelato.unsw.edu.au/lxr/source/lib/crc32.c, lines 105-111. */

unsigned int crc32cx(unsigned int *ptr, size_t length)
{
  int j;
  unsigned int byte, crc, mask;
  unsigned int table[256];

  length &= ~0x3;               /* must be mutiple of 4 */

  /* Set up the table */
  for (byte = 0; byte <= 255; byte++) {
    crc = byte;
    for (j = 7; j >= 0; j--) {    // Do eight times.
      mask = -(crc & 1);
      crc = (crc >> 1) ^ (0xEDB88320 & mask);
    }
    table[byte] = crc;
  }

  /* Through with table setup, now calculate the CRC. */
  crc = 0xFFFFFFFF;
  while (length != 0) {
    crc = crc ^ *ptr;
    crc = (crc >> 8) ^ table[crc & 0xFF];
    crc = (crc >> 8) ^ table[crc & 0xFF];
    crc = (crc >> 8) ^ table[crc & 0xFF];
    crc = (crc >> 8) ^ table[crc & 0xFF];

    ptr++; length -= 4;
  }
  return ~crc;
}

/**
 * 32 bit checksum for the whole memory space
 */
unsigned int checksum_memory(void)
{
  /* do crc  */
  return crc32cx((unsigned int)FLASH_ADDR, /* start */
                 FLASH_SIZE - 4);          /* length */
}

/**
 * checks if memory checksum is good
 */
enum flash_state check_flash_state(void)
{
  unsigned int calculated = checksum_memory();

  if (*flash_checksum == 0xFFFFFFFF) { /* not written */
    /* write it */
    mem_write_word((uint32_t)flash_checksum, calculated);

    return FLASH_GOOD;

  } else {                      /* written */
    /* check it */
    if (calculated == *flash_checksum) {
      return FLASH_GOOD;
    } else {
      return FLASH_BAD_CSUM;
    }
  }
}
