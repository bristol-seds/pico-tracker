/*
 * Outputs ax25 to the si_trx
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

#ifndef AX25_H
#define AX25_H

#include "ax25_sintable.h"

/**
 * Parameters based on the size of the sintable
 */
#define AX25_SINTABLE_SIZE	(AX25_SINTABLE_LENGTH*4)
#define AX25_SINTABLE_LUMASK	(AX25_SINTABLE_LENGTH-1)
#define AX25_SINTABLE_MASK	(AX25_SINTABLE_SIZE-1)
#define AX25_SINTABLE_PART(phase)	((phase >> AX25_SINTABLE_ORDER) & 3)

/**
 * Bell-202
 */
#define AX25_BAUD		2200
#define AX25_MARKFREQ		1
#define AX25_SPACEFREQ		2

#define AX25_BITSTUFFINGCOUNT	5



/**
 * How many points we output in a single symbol-time
 */
#define AX25_OVERSAMPLING	2
#define AX25_TICK_RATE		(AX25_BAUD * AX25_OVERSAMPLING)

/**
 * Define the phase velocities for mark and space
 *
 * This is how many entries in the sin table we step by
 */
#define AX25_MARKPHASEVELOCITY	(AX25_SINTABLE_SIZE/AX25_OVERSAMPLING)
#define AX25_SPACEPHASEVELOCITY	(AX25_MARKPHASEVELOCITY*(2.2f/1.2f))
// TODO ^^





enum ax25_symbol_t {
  AX25_MARK,
  AX25_SPACE,
};


void ax25_start();
uint8_t ax25_tick(void);

#endif /* AX25_H */
