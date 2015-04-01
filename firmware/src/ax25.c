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

#include "samd20.h"
#include "ax25.h"
#include "ax25_sintable.h"
#include "si_trx.h"

#define AX25_MARKSINTABLE	sintable_512_1500hz
#define AX25_SPACESINTABLE	sintable_512_2500hz


uint32_t ax25_phase;
uint32_t ax25_phasevelocity;
uint32_t ax25_oversampling_count;

uint16_t* ax25_sintable;

void ax25_start()
{
  /* Init */
  ax25_phase = 0;
  ax25_phasevelocity = AX25_MARKPHASEVELOCITY;
  ax25_sintable = AX25_MARKSINTABLE;
  ax25_oversampling_count = -1LL;
}


uint32_t toggle = 0;
enum ax25_symbol_t ax25_get_next_symbol(void) {
  return (toggle++ & 1) ? AX25_MARK : AX25_SPACE;
}


void telemetry_gpio1_pwm_duty(float duty_cycle);

/**
 * Called at our tick rate, outputs tones
 *
 * Returns 1 when more work todo, 0 when finished
 */
uint8_t ax25_tick(void)
{
  int16_t deviation;
  uint32_t sintable_phase = ax25_phase & AX25_SINTABLE_LUMASK;

  /* Set the instantaneous fm deviation based on the current phase */
  switch (AX25_SINTABLE_PART(ax25_phase)) {
  case 0: 				/* 0° - 90° */
    deviation = ax25_sintable[sintable_phase];
    break;
  case 1:				/* 90° - 180° */
    deviation = ax25_sintable[AX25_SINTABLE_LUMASK - sintable_phase];
    break;
  case 2:				/* 180° - 270° */
    deviation = -ax25_sintable[sintable_phase];
    break;
  case 3:				/* 270° - 360° */
    deviation = -ax25_sintable[AX25_SINTABLE_LUMASK - sintable_phase];
    break;
  default:
    deviation = 0;
  }

//  si_trx_switch_channel(deviation);

//  float duty = 0.5 + ((float)deviation/192.0) / 2.0;
  float duty = (ax25_get_next_symbol() == AX25_SPACE) ? 1.0 : 0.0;

  telemetry_gpio1_pwm_duty(duty);

  /* Update with next bit */
  if (ax25_oversampling_count++ >= AX25_OVERSAMPLING) {
    ax25_oversampling_count = 0;

    /* Set phase velocity for next symbol */
    if (0) {//ax25_get_next_symbol() == AX25_SPACE) {
      ax25_phasevelocity = AX25_SPACEPHASEVELOCITY;
      ax25_sintable = AX25_SPACESINTABLE;
    } else {
      ax25_phasevelocity = AX25_MARKPHASEVELOCITY;
      ax25_sintable = AX25_MARKSINTABLE;
    }
  }
  /* Update phase */
  ax25_phase += ax25_phasevelocity;


  return 1;
}
