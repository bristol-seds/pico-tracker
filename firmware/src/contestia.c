/*
 * Outputs contestia to the si_trx
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

#include "samd20.h"
#include "contestia.h"
#include "telemetry.h"
#include "si_trx.h"
#include "mfsk.h"

/**
 * Current output tones
 */
int8_t contestia_tones[CONTESTIA_NUMBER_OF_TONES];
/**
 * Where we are in the current output tones
 */
uint32_t contestia_tone_index = 0xFFFFFFFE;

/**
 * Starts the transmission of a contestia block
 */
void contestia_start(char* block) {
  /* Start transmission */
  contestia_mfsk_encode_block(block, contestia_tones);
  contestia_tone_index = 0;
}

/**
 * Called at the baud rate, outputs tones
 */
uint8_t contestia_tick(void) {

  if (contestia_tone_index < CONTESTIA_NUMBER_OF_TONES) {
    uint8_t binary_code;
    uint8_t grey_code;
    int16_t channel;

    /* Output grey code */
    binary_code = contestia_tones[contestia_tone_index];
    grey_code = (binary_code >> 1) ^ binary_code;

    /* Align this to a channel */
    channel = grey_code - (CONTESTIA_NUMBER_OF_TONES / 2);
    si_trx_switch_channel(channel * CONTESTIA_CHANNEL_SPACING);

  } else {
    return 0;
  }

  contestia_tone_index++;

  if (contestia_tone_index < CONTESTIA_NUMBER_OF_TONES) {
    return 1;
  }

  return 0;
}
