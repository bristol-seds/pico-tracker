/*
 * Provides functions for using the external flash memory
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

#ifndef MEMORY_H
#define MEMORY_H

/**
 * Memory layout:
 *
 * 256-byte pages
 * 4-kbyte sectors (erase) - 16 pages
 * 64-kbyte blocks - 16 sectors
 */

#define TOTAL_PAGES	0x800
#define TOTAL_SECTORS	0x80
#define TOTAL_BLOCKS	0x8

#define MEMORY_MASK	0x7FFFF
#define PAGE_MASK	0x7FF00
#define SECTOR_MASK	0x7F000

/**
 * Pages assigned to backlog. Currently 128 records
 */
#define BACKLOG_START_PAGE	0x00
#define BACKLOG_END_PAGE	0x7f


void mem_read_memory(uint32_t address, uint8_t* buffer, uint32_t length);
void mem_write_page(uint32_t address, uint8_t* buffer, uint16_t length);
void mem_erase_sector(uint32_t address);
void init_memory(void);

#endif
