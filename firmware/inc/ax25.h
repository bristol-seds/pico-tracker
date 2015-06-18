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


/**
 * Bell-202
 */
#define AX25_BAUD		1200
#define AX25_MARK_FREQ		1200
#define AX25_SPACE_FREQ		2200

/**
 * GCLK division factors
 */
#define AX25_DIVISION_MARK	11
#define AX25_DIVISION_SPACE	6

/**
 * How often our handler gets called
 */
#define AX25_TICK_RATE		AX25_BAUD

/**
 * AX25 packet parameters
 */
#define AX25_BITSTUFFINGCOUNT	5
#define AX25_PREAMBLE_FLAGS	30 /* 200 ms of flags (normally 100ms?) */
#define AX25_POSTAMBLE_FLAGS	1
#define AX25_MAX_FRAME_LEN	0x100

/**
 * Hard-coded AX25 words
 */
#define AX25_CONTROL_WORD	0x03 /* Use Unnumbered Information (UI) frames */
#define AX25_PROTOCOL_ID	0xF0 /* No third level protocol */
#define AX25_HDLC_FLAG		0x7E

enum ax25_symbol_t {
  AX25_MARK,
  AX25_SPACE,
  AX25_NONE,
};
struct ax25_byte_t {
  uint8_t val;
  uint8_t stuff;
};

/**
 * State machine
 */
enum ax25_state_t {
  AX25_PREAMBLE,
  AX25_FRAME,
  AX25_POSTAMBLE,
  AX25_IDLE,
};


void ax25_start(char* addresses, uint32_t addresses_len,
                char* information, uint32_t information_len);
uint8_t ax25_tick(void);

#endif /* AX25_H */
