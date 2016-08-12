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
#include <string.h>

#include "samd20.h"
#include "memory.h"
#include "flash.h"
#include "watchdog.h"

/* Check these are multiples of 64 */
#if (D1_SECTORS & 0x3F)
#error D1_SECTORS _must_ be a mul 64, so checksums fill integer no. of sectors
#endif
#if (D2_SECTORS & 0x3F)
#error D2_SECTORS _must_ be a mul 64, so checksums fill integer no. of sectors
#endif

#define D1_CHECKSUMS_SECTORS	(D1_SECTORS/64)
#define D2_CHECKSUMS_SECTORS	(D2_SECTORS/64)

/* ROM copy of checksums */
const uint32_t d1_checksums_nvm[D1_SECTORS]
  __attribute__ ((aligned (256)))
  = { 0xFF };
const uint32_t d2_checksums_nvm[D2_SECTORS]
  __attribute__ ((aligned (256)))
  = { 0xFF };

/* RAM copy of checksums */
uint32_t d1_checksums_ram[D1_SECTORS];
uint32_t d2_checksums_ram[D2_SECTORS];

/* crc32 calculation */
uint32_t table[256] = { 0xFFFFFFFF };


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

uint32_t crc32cx(unsigned int *ptr, size_t length)
{
  int j;
  uint32_t byte, crc, mask;

  length &= ~0x3;               /* must be mutiple of 4 */

  /* Set up the table */
  if (table[0] == 0xFFFFFFFF) {
  for (byte = 0; byte <= 255; byte++) {
    crc = byte;
    for (j = 7; j >= 0; j--) {    // Do eight times.
      mask = -(crc & 1);
      crc = (crc >> 1) ^ (0xEDB88320 & mask);
    }
    table[byte] = crc;
  }
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
 * 32 bit checksum for the given sector
 */
uint32_t checksum_sector(unsigned int* sector)
{
  /* do crc  */
  return crc32cx((unsigned int*)sector, /* start */
                 SECTOR_SIZE);          /* length */
}

/**
 * updates checksum records in nvm
 */
void update_checksums(const uint32_t* nvm, uint32_t* ram, int sectors)
{
  int i;

  for (i = 0; i < sectors; i++,
         nvm += 64, ram += 64) {
    mem_erase_sector((unsigned int*)nvm);
    mem_write_sector((unsigned int*)nvm, (unsigned int*)ram);

    kick_the_watchdog();
  }
}


/**
 * Checks and repairs application memory space
 *
 * returns the number of errors successfully corrected
 */
uint32_t check_and_repair_memory(void)
{
  unsigned int* address1 = (unsigned int*)D1_START;
  unsigned int* address2 = (unsigned int*)D2_START;
  unsigned int check1, check2;
  uint8_t update_d1_check = 0, update_d2_check = 0;
  uint32_t errors_found = 0;
  int i;

  /* load checksums */
  memcpy(d1_checksums_ram, d1_checksums_nvm, D1_SECTORS * sizeof(uint32_t));
  memcpy(d2_checksums_ram, d2_checksums_nvm, D2_SECTORS * sizeof(uint32_t));

  /* foreach sector */
  for (i = 0; i < D1_SECTORS; i++,
         address1 += 64, address2 += 64) {
    /* calculate checksums */
    check1 = checksum_sector(address1);
    check2 = checksum_sector(address2);

    if ((check1 == d1_checksums_ram[i]) &&
        (check2 == d2_checksums_ram[i])) {
      /* all good */
      continue;

    } else if ((check1 != d1_checksums_ram[i]) &&
               (check2 == d2_checksums_ram[i])) {
      /* restore d1 */
      mem_erase_sector(address1);
      mem_write_sector(address1, address2);

      /* update d1 checksum */
      d1_checksums_ram[i] = check2;

      /* flag checksum for write */
      update_d1_check = 1;
      errors_found++;

    } else if ((check1 == d1_checksums_ram[i]) &&
               (check2 != d2_checksums_ram[i])) {
      /* restore d2 */
      mem_erase_sector(address2);
      mem_write_sector(address2, address1);

      /* update d2 checksum */
      d2_checksums_ram[i] = check1;

      /* flag checksum for write */
      update_d2_check = 1;
      errors_found++;

    } else {
      /* both bad, restore d2 and calculate both checksums */
      /* restore d2 */
      mem_erase_sector(address2);
      mem_write_sector(address2, address1);

      /* update both checksums */
      d1_checksums_ram[i] = check1;
      d2_checksums_ram[i] = check1;

      /* flag checksums for write */
      update_d1_check = 1;
      update_d2_check = 1;
      /* don't count these as errors, probably happen when
         programming for first time */
    }

    kick_the_watchdog();
  }

  /* update checksums */
  if (update_d1_check) { update_checksums(d1_checksums_nvm,
                                          d1_checksums_ram,
                                          D1_CHECKSUMS_SECTORS); }
  if (update_d2_check) { update_checksums(d2_checksums_nvm,
                                          d2_checksums_ram,
                                          D2_CHECKSUMS_SECTORS); }

  return errors_found;
}
