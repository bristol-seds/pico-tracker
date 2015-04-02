/*
 * Outputs aprs uisng ax25
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

#include <stdio.h>
#include <string.h>

#include "samd20.h"
#include "ax25.h"

/**
 * USEFUL RESOURCES
 * =============================================================================
 *
 * http://www.aprs.org/doc/APRS101.PDF
 * http://k9dci.home.comcast.net/~k9dci/APRS%20Beginner%20Guide%20-%20K9DCI%20Ver%205-1.pdf
 */

char addresses[50];

void aprs_start(void)
{
  sprintf(addresses, "%-6s%c%-6s%c%-6s%c",
          "APRS", 0,
          "M0SBU", 0,
          "WIDE1", 1);

  ax25_start(addresses, 21, "testtest", 8);
}

uint8_t aprs_tick(void)
{
  return ax25_tick();
}
