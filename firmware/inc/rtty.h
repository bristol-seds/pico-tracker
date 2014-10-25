/*
 * Bit-bangs RTTY
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

#ifndef RTTY_H
#define RTTY_H

#include "util/dbuffer.h"

/**
 * Output String
 */
#define RTTY_STRING_MAX	0x200
/**
 * It's actually a double buffer which we swap for mid-string updates
 */
ARRAY_DBUFFER_T(char, RTTY_STRING_MAX) rtty_dbuffer_string;

int rtty_active(void);
int rtty_start(void);
int32_t rtty_get_index(void);
void rtty_set_length(int32_t length);
void rtty_tick(void);

#endif /* RTTY_H */
