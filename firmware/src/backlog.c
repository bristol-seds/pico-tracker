/*
 * Records and retreives backlog points from memory
 * Copyright (C) 2015  Richard Meadows <richardeoin>
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

#include <stddef.h>
#include <math.h>
#include <string.h>

#include "samd20.h"
#include "backlog.h"
#include "data.h"
#include "memory.h"
#include "crc.h"

uint8_t buffer[BACKLOG_ITEM_SIZE];

#define ACTUAL_BACKLOG_ITEM_SIZE	(sizeof(struct tracker_datapoint) + 4)

/* When erasing a sector sometimes we have to store internally */
uint8_t sector_erase_buffer[BACKLOGS_PER_SECTOR-1][BACKLOG_ITEM_SIZE];

uint8_t is_backlog_valid_loaded = 0;
uint8_t is_backlog_valid[BACKLOG_COUNT] = {};
#define BACKLOG_VALID_FLAG	0x44

/**
 * The index at which we are currently writing backlog
 */
uint16_t backlog_write_index = 0;
uint8_t is_write_index_loaded = 0;
/**
 * The index at which we are currently replaying backlog
 */
uint16_t backlog_read_index = 0;

#define INC_INDEX(idx)	do { idx++; if ((idx) >= BACKLOG_COUNT) { idx = 0; }} while(0)
#define ADDRESS(idx)	(BACKLOG_ADDRESS + ((idx) * BACKLOG_ITEM_SIZE))

/**
 * We reverse the index bit order when reading
 */
uint16_t reverse_index(uint16_t index) {

  /* Swap bit order */
  index = ((index >> 1) & 0x5555) | ((index & 0x5555) << 1);
  index = ((index >> 2) & 0x3333) | ((index & 0x3333) << 2);
  index = ((index >> 4) & 0x0F0F) | ((index & 0x0F0F) << 4);
  index = ((index >> 8) & 0x00FF) | ((index & 0x00FF) << 8);
  index = (index >> (16 - BACKLOG_COUNT_BITS));

  return index & (BACKLOG_COUNT-1);
}
/**
 * Populates the backlog valid array if needed
 */
void load_is_backlog_valid(void)
{
  uint32_t i;
  uint32_t epoch_value;

  for (i = 0; i < BACKLOG_COUNT; i++) {

    mem_read_memory(ADDRESS(i), (uint8_t*)&epoch_value, 4);

    is_backlog_valid[i] = (epoch_value == 0xFFFFFFFF) ?
      0x00 : BACKLOG_VALID_FLAG;
  }

  is_backlog_valid_loaded = BACKLOG_VALID_FLAG;
}


/**
 * WRITE =======================================================================
 */

/**
 * Erases Backlog item
 */
void erase_backlog_item(uint16_t index) {

  uint32_t start_address = ADDRESS(index) & SECTOR_MASK;
  uint32_t end_address = start_address + SECTOR_SIZE;
  uint16_t start_index = (start_address-BACKLOG_ADDRESS) / BACKLOG_ITEM_SIZE;
  uint16_t end_index = ceil((float)(end_address-BACKLOG_ADDRESS) / BACKLOG_ITEM_SIZE);
  uint16_t i;

  if (index >= BACKLOG_COUNT) { while (1); }

  /* When we want to erase an item mid-sector we have to store those before it */
  for (i = 0; (start_index+i) < index; i++) {
    mem_read_memory(ADDRESS(start_index+i), sector_erase_buffer[i], ACTUAL_BACKLOG_ITEM_SIZE);
  }

  /* Erase */
  mem_erase_sector(start_address);

  /* Restore */
  for (i = 0; (start_index+i) < index; i++) {
    do {
      mem_write_page(ADDRESS(start_index+i), sector_erase_buffer[i], ACTUAL_BACKLOG_ITEM_SIZE);
      mem_read_memory(ADDRESS(start_index+i), buffer, ACTUAL_BACKLOG_ITEM_SIZE);
      /* Check identicial */
    } while (memcmp(sector_erase_buffer[i], buffer, ACTUAL_BACKLOG_ITEM_SIZE) != 0);
  }

  /* Clear items in the is_backlog_valid array*/
  for (; index < end_index; index++){
    is_backlog_valid[index] = 0;
  }
}
/**
 * Load write index by finding highest epoch
 */
void load_write_index(void)
{
  uint16_t i, highest_epoch_index = (BACKLOG_COUNT-1);
  uint32_t epoch_value, highest_epoch = 0;

  /* Need to load backlog valid first */
  if (is_backlog_valid_loaded == 0) {
    load_is_backlog_valid();
  }

  for (i = 0; i < BACKLOG_COUNT; i++) {
    if (is_backlog_valid[i] == BACKLOG_VALID_FLAG) {
      mem_read_memory(ADDRESS(i), (uint8_t*)&epoch_value, 4);

      if (epoch_value >= highest_epoch) {
        highest_epoch = epoch_value;
        highest_epoch_index = i;
      }
    }
  }

  backlog_write_index = highest_epoch_index;
  INC_INDEX(backlog_write_index);

  is_write_index_loaded = 0xFF;
}
/**
 * Writes backlog item with checksum
 */
void write_backlog_item(uint16_t index, tracker_datapoint* dp)
{
  uint8_t* buffer_ptr = (uint8_t*)dp;
  uint32_t crc;

  if (index >= BACKLOG_COUNT) { while (1); }

  /* Checksum */
  crc = calculate_crc32(buffer_ptr, sizeof(tracker_datapoint));
  put_crc32(buffer_ptr + sizeof(tracker_datapoint), crc);

  do {
    /* Write */
    mem_write_page(ADDRESS(index), buffer_ptr, ACTUAL_BACKLOG_ITEM_SIZE);

    /* Read back */
    mem_read_memory(ADDRESS(index), buffer, ACTUAL_BACKLOG_ITEM_SIZE);

    /* Check identicial */
  } while (memcmp(buffer_ptr, buffer, ACTUAL_BACKLOG_ITEM_SIZE) != 0);

  /* This item of backlog is now valid */
  is_backlog_valid[index] = BACKLOG_VALID_FLAG;
}

/**
 * Writes a datapoint to memory. Erases oldest records if needs be
 */
void record_backlog(tracker_datapoint* dp)
{
  if (is_backlog_valid_loaded == 0) {
    load_is_backlog_valid();
  }

  /* Load write index by finding highest epoch */
  if (is_write_index_loaded == 0) {
    load_write_index();
  }

  /* Erase old records if need be */
  if (is_backlog_valid[backlog_write_index] == BACKLOG_VALID_FLAG) {
    erase_backlog_item(backlog_write_index);
  }

  /* Write */
  write_backlog_item(backlog_write_index, dp);
  INC_INDEX(backlog_write_index);
}


/**
 * READ ========================================================================
 */

/**
 * Reads and checks a backlog at address
 *
 * Return NULL if the backlog at the address is invalid
 */
struct tracker_datapoint* read_check_backlog_item(uint16_t index)
{
  uint32_t crc_calc, crc_buffer;

  if (index >= BACKLOG_COUNT) { while (1); }

  mem_read_memory(ADDRESS(index), buffer, ACTUAL_BACKLOG_ITEM_SIZE);

  /* Checksum */
  crc_calc = calculate_crc32(buffer, sizeof(tracker_datapoint));
  crc_buffer = get_crc32(buffer + sizeof(tracker_datapoint));

  if (crc_calc == crc_buffer) {
    return (struct tracker_datapoint*)buffer;
  } else {
    return NULL;
  }
}
/**
 * Returns the number of backlogs indicated as valid in is_backlog_valid.
 *
 * Should correspond with the actual number of valid backlogs
 */
uint16_t is_backlog_valid_count(void)
{
  uint16_t i, count = 0;

  for (i = 0; i < BACKLOG_COUNT; i++) {
    if (is_backlog_valid[i] == BACKLOG_VALID_FLAG) {
      count++;
    }
  }

  return count;
}
/**
 * Gets a valid backlog item. Returns NULL if none available.
 */
struct tracker_datapoint* get_backlog(void)
{
  uint32_t i;
  struct tracker_datapoint* dp;

  if (is_backlog_valid_loaded == 0) {
    load_is_backlog_valid();
  }

  /* Return early if we haven't reached our replay threshold */
  if (is_backlog_valid_count() < BACKLOG_REPLAY_THRESHOLD) {
    return NULL;
  }

  for (i = 0; i < BACKLOG_COUNT; i++) {

    /* Find an read index we think is valid */
    for (; (i < BACKLOG_COUNT) &&
           (is_backlog_valid[reverse_index(backlog_read_index)] != BACKLOG_VALID_FLAG);
         i++) {

      INC_INDEX(backlog_read_index);
    }

    /* Couldn't find valid backlog */
    if (i == BACKLOG_COUNT) return NULL;

    /* Check this */
    dp = read_check_backlog_item(reverse_index(backlog_read_index));
    INC_INDEX(backlog_read_index);

    /* Return if it's good */
    if (dp) return dp;
  }

  return NULL;
}
