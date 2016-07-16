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
 * 64-byte pages
 * 256-byte rows (erase) - 4 pages
 */

#define TOTAL_PAGES	0x100
#define TOTAL_ROWS	0x40

#define PAGE_MASK	0x7FFC0
#define ROW_MASK	0x7FF00

#define PAGE_SIZE	0x00040
#define ROW_SIZE	0x00100

/**
 * Pages assigned to backlog. Currently 256 records
 */
#define BACKLOG_START_PAGE	0x00
#define BACKLOG_END_PAGE	0xff


void mem_chip_erase(void);
void mem_read_memory(uint32_t address, uint8_t* buffer, uint32_t length);
void mem_write_page(uint32_t address, uint8_t* buffer, uint16_t length);
void mem_erase_sector(uint32_t address);

uint8_t mem_power_on();
void mem_power_off();

#endif
