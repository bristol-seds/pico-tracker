/*
 * Reed-Solomon Identification (RSID) functions
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

#include <string.h>

#include "samd20.h"
#include "rsid.h"
#include "telemetry.h"
#include "si_trx.h"

/**
 * USEFUL RESOURCES ============================================================
 *
 * http://www.w1hkj.com/RSID_description.html
 *
 * dl-fldigi source:
 * https://github.com/jamescoxon/dl-fldigi/blob/master/src/rsid/rsid.cxx#L180
 */


/**
 * ENCODING
 * =============================================================================
 */


/**
 * Constants used in the Reed-Solomon encoding
 */
const int rsid_squares[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
  0, 2, 4, 6, 8,10,12,14, 9,11,13,15, 1, 3, 5, 7,
  0, 3, 6, 5,12,15,10, 9, 1, 2, 7, 4,13,14,11, 8,
  0, 4, 8,12, 9,13, 1, 5,11,15, 3, 7, 2, 6,10,14,
  0, 5,10,15,13, 8, 7, 2, 3, 6, 9,12,14,11, 4, 1,
  0, 6,12,10, 1, 7,13,11, 2, 4,14, 8, 3, 5,15, 9,
  0, 7,14, 9, 5, 2,11,12,10,13, 4, 3,15, 8, 1, 6,
  0, 8, 9, 1,11, 3, 2,10,15, 7, 6,14, 4,12,13, 5,
  0, 9,11, 2,15, 6, 4,13, 7,14,12, 5, 8, 1, 3,10,
  0,10,13, 7, 3, 9,14, 4, 6,12,11, 1, 5,15, 8, 2,
  0,11,15, 4, 7,12, 8, 3,14, 5, 1,10, 9, 2, 6,13,
  0,12, 1,13, 2,14, 3,15, 4, 8, 5, 9, 6,10, 7,11,
  0,13, 3,14, 6,11, 5, 8,12, 1,15, 2,10, 7, 9, 4,
  0,14, 5,11,10, 4,15, 1,13, 3, 8, 6, 7, 9, 2,12,
  0,15, 7, 8,14, 1, 9, 6, 5,10, 2,13,11, 4,12, 3
};
const int rsid_indices[] = {
  2, 4, 8, 9, 11, 15, 7, 14, 5, 10, 13, 3
};

/**
 * This function populates the RSID_NSYMBOLS tones needed to transmit
 * a RSID for the given rsid_code
 */
void rsid_encode(rsid_code_t rsid_code, int8_t* rsid)
{
  memset(rsid, 0, RSID_NSYMBOLS * sizeof(int8_t));

  /* Encode the 12-bit code into the first 3 nibbles */
  rsid[0] = (rsid_code >> 8) & 0x0F;
  rsid[1] = (rsid_code >> 4) & 0x0F;
  rsid[2] = (rsid_code >> 0) & 0x0F;


  for (int i = 0; i < 12; i++) {
    for (int j = RSID_NSYMBOLS - 1; j > 0; j--) {
      rsid[j] = rsid[j - 1] ^ rsid_squares[(rsid[j] << 4) + rsid_indices[i]];
    }

    rsid[0] = rsid_squares[(rsid[0] << 4) + rsid_indices[i]];
  }
}


/**
 * TRANSMISSION
 * =============================================================================
 */

uint8_t rsid_index = 0xFE;
int8_t rsid[RSID_NSYMBOLS];

#define MODEM_TONE_SPACING 7.805

#define RSID_LOOKUP
/* Lookup tables designed for a modem tone spacing of 7.805 Hz */
int8_t rsid_tones_channels[] = { 0, 1, 3, 4, 6, 7, 8, 10, 11 };
float rsid_tones_deviations[] = {
  0, 0.379, -0.241, 0.138, -0.482, -0.103, 0.276, -0.344, 0.035 };


/**
 * Setup a rsid transmission
 */
void rsid_start(rsid_code_t rsid_code)
{
  /* Start transmission */
  rsid_index = 0;
  rsid_encode(rsid_code, rsid);
}

/**
 * Outputs one of the 16 RSID tones
 */
void rsid_tone(uint8_t tone)
{
  int16_t channel;
  float deviation;
  int8_t air_tone = tone - 7;

#ifdef RSID_LOOKUP
  if (air_tone >= 0) {
    channel = rsid_tones_channels[air_tone];
    deviation = rsid_tones_deviations[air_tone];
  } else {
    channel = -rsid_tones_channels[-air_tone];
    deviation = -rsid_tones_deviations[-air_tone];
  }
#else
  /* Calcuate the offset of this tone in channels */
  float tone_offset = ((float)air_tone * (float)RSID_SYMBOL_RATE) / (float)MODEM_TONE_SPACING;
  /* Centre channel */
  channel = (int16_t)round(tone_offset);
  /* And the deviation from this we need, in channels */
  deviation = tone_offset - (float)channel;
#endif

  float duty_cycle = 0.5 + (deviation / 2); // FSK only provides a marginal improvement in performance!

  si_trx_switch_channel(channel);
  telemetry_gpio1_pwm_duty(duty_cycle);
}

/**
 * Called at the rsid baud rate
 */
uint8_t rsid_tick(void)
{

  if (rsid_index < RSID_NSYMBOLS) {

    /* Transmit this tone */
    rsid_tone(rsid[rsid_index]);

    rsid_index++;
    if (rsid_index < RSID_NSYMBOLS) {
      return 1;
    }
  }

  return 0;
}
