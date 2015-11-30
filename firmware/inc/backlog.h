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

#ifndef BACKLOG_H
#define BACKLOG_H

#include "samd20.h"
#include "data.h"

/**
 * How many things we store in our backlog.
 *
 * Should be a power of two because of fractal retreival algorithm
 * 256 @ 1 / hour = 10 days 16 hours
 */
#define BACKLOG_COUNT_BITS	8
#define BACKLOG_COUNT		256
/**
 * How many bytes each memory record uses
 */
#define BACKLOG_ITEM_SIZE	0x100   /* Currently one backlog per page */
#define BACKLOGS_PER_SECTOR	16
/**
 * Address offset in memory
 */
#define BACKLOG_ADDRESS 0
/**
 * Defines the number of backlogs that should be valid before we start replaying
 */
#define BACKLOG_REPLAY_THRESHOLD	24    /* One day */



/** NOTE: Public methods must power on memory */
void record_backlog(tracker_datapoint* dp);
struct tracker_datapoint* get_backlog(void);

#endif
