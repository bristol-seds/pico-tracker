/*
 * Macros for defining and operating on double buffers
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

#ifndef DBUFFER_H
#define DBUFFER_H

/**
 * Double buffer implementation for single types
 */
#define DBUFFER_T(t)				\
  struct {					\
    t buffers[2];				\
    uint8_t index;				\
  }
#define DBUFFER_INIT(db_ptr) do{		\
    (db_ptr)->index = 0; } while (0)
#define DBUFFER_WRITE(db_ptr, val) do {		\
    (db_ptr)->buffers[(db_ptr)->index] = val; } while (0)
#define DBUFFER_SWAP(db_ptr) do {		\
    (db_ptr)->index ^= 0x1; } while (0)
#define DBUFFER_READ(db_ptr)			\
    (db_ptr)->buffers[(db_ptr)->index ^ 0x1]


/**
 * Double buffer implementation for arrays
 */
#define ARRAY_DBUFFER_T(t, n)			\
  struct {					\
    t buffers[2][n];				\
    uint8_t index;				\
  }
#define ARRAY_DBUFFER_INIT	DBUFFER_INIT
/**
 * Functions for the data source. The data source owns the pointer
 * returned by ARRAY_DBUFFER_WRITE_PTR until it calls
 * ARRAY_DBUFFER_SWAP.
 */
#define ARRAY_DBUFFER_WRITE_PTR(db_ptr)		\
    (db_ptr)->buffers[(db_ptr)->index]
#define ARRAY_DBUFFER_SWAP	DBUFFER_SWAP
/**
 * Functions for the data sink. The pointer returned by
 * ARRAY_DBUFFER_READ_PTR is immediately invalid unless called in a
 * critical section. This is as the data source could be operating in
 * a higher IRQ context.
 */
#define ARRAY_DBUFFER_READ_PTR(db_ptr)		\
    (db_ptr)->buffers[(db_ptr)->index ^ 0x1]


#endif /* DBUFFER_H */
