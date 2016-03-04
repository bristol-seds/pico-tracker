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

#include <string.h>

#include "samd20.h"
#include "contestia.h"
#include "telemetry.h"
#include "si_trx.h"
#include "mfsk.h"

#define PREAMBLE_LENGTH 32

/**
 * Current output tones
 */
int8_t contestia_tones[32];
/**
 * Where we are in the current output tones
 */
uint32_t contestia_tone_index = 0xFFFFFFFE;
uint8_t contestia_preamble_index = 0;

/**
 * Starts the transmission of a contestia block
 */
void contestia_start(char* block) {
  /* Start transmission */
  contestia_mfsk_encode_block(block, contestia_tones);
  contestia_tone_index = 0;
}
void contestia_preamble(void) {
  contestia_preamble_index = PREAMBLE_LENGTH;
}

void contestia_set_tone(uint8_t tone) {
  /* Centre the tone */
  int16_t centred_tone = tone - (CONTESTIA_NUMBER_OF_TONES / 2);
  /* Align this to a channel */
  int16_t channel = (centred_tone * CONTESTIA_CHANNEL_SPACING)+(CONTESTIA_CHANNEL_SPACING/2);

  si_trx_switch_channel(channel);
}

/**
 * Called at the baud rate, outputs tones
 *
 * Returns 1 when more work todo, 0 when finished
 */
uint8_t contestia_tick(void) {

  if (contestia_preamble_index) {
    contestia_preamble_index--;

    if (contestia_preamble_index & (PREAMBLE_LENGTH/4)) {
      contestia_set_tone(CONTESTIA_NUMBER_OF_TONES - 1);
    } else {
      contestia_set_tone(0);
    }

    return 1;
  }

  if (contestia_tone_index < 32) {
    uint8_t binary_code;
    uint8_t grey_code;

    /* Output grey code */
    binary_code = contestia_tones[contestia_tone_index];
    grey_code = (binary_code >> 1) ^ binary_code;

    /* Transmit this tone */
    contestia_set_tone(grey_code);

  } else {
    return 0;
  }

  contestia_tone_index++;

  if (contestia_tone_index < 32) {
    return 1;
  }

  return 0;
}



/**
 * Only certain strings are possible in contestia (no lowercase
 * etc.). This function prases a string and fixed anything that would
 * be converted by contestia transmission.
 */
void contestiaize(char* string) {
  for (size_t i = 0; i < strlen(string); i++) {

    /* lowercase => UPPERCASE */
    if (string[i] >= 'a' && string[i] <= 'z') {
      string[i] += 'A' - 'a';
    }

    if ((string[i] < '!' || string[i] > 'Z') && /* Not Printable */
        (string[i] != ' ')  && (string[i] != '\r') &&
        (string[i] != '\n') && (string[i] != 8) &&
        (string[i] != 0)) {
      string[i] = '?'; /* Comes out as question mark */
    }
  }
}
