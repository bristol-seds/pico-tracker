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

#include <string.h>

#include "samd20.h"
#include "system/gclk.h"
#include "system/pinmux.h"
#include "tc/tc_driver.h"
#include "hw_config.h"
#include "ax25.h"
#include "telemetry.h"
#include "si_trx.h"

enum ax25_symbol_t next_symbol;
uint8_t bit_index;
struct ax25_byte_t current_byte;
uint8_t current_bit;
uint8_t one_count;
uint32_t byte_index;

enum ax25_state_t ax25_state;
uint8_t ax25_frame[AX25_MAX_FRAME_LEN];
uint32_t ax25_index, ax25_frame_length;

void ax25_gpio1_pwm_init(void);

/**
 * Frame Check Sequence (FCS)
 * =============================================================================
 */

/**
 * CRC Function for CCITT-16. Poly = 0x8408
 * http://www.nongnu.org/avr-libc/user-manual/group__util__crc.html#ga1c1d3ad875310cbc58000e24d981ad20
 */
uint16_t crc_ccitt_update (uint16_t crc, uint8_t data)
{
  data ^= (crc & 0xff);
  data ^= data << 4;
  return ((((uint16_t)data << 8) | ((crc >> 8) & 0xff)) ^ (uint8_t)(data >> 4)
          ^ ((uint16_t)data << 3));
}

/**
 * Calculates the Frame Check Sequence (FCS) using the CRC algorithm
 */
uint16_t crc_fcs(uint8_t *string, uint32_t length)
{
  size_t i;
  uint16_t crc;
  uint8_t c;

  crc = 0xFFFF;

  for (i = 0; i < length; i++) {
    c = string[i];
    crc = crc_ccitt_update(crc, c);
  }

  return crc ^ 0xFFFF;
}


/**
 * Starts the transmission of an ax25 frame
 */
void ax25_start(char* addresses, uint32_t addresses_len,
                char* information, uint32_t information_len)
{
  uint32_t i, j;
  uint16_t fcs;

  /* Process addresses */
  for (i = 0; i < addresses_len; i++) {

    if ((i % 7) == 6) {         /* Secondary Station ID */
      ax25_frame[i] = ((addresses[i] << 1) & 0x1F) | 0x60;
    } else {
      ax25_frame[i] = (addresses[i] << 1);
    }
  }
  ax25_frame[i-1] |= 0x1;     /* Set HLDC bit */

  ax25_frame[i++] = AX25_CONTROL_WORD;
  ax25_frame[i++] = AX25_PROTOCOL_ID;

  /* Process information */
  memcpy(ax25_frame+i, information, information_len);
  i += information_len;

  /* Frame Check Sequence (FCS) */
  fcs = crc_fcs(ax25_frame, i);
  ax25_frame[i++] = (fcs >> 0) & 0xFF;
  ax25_frame[i++] = (fcs >> 8) & 0xFF;

  /* Length */
  ax25_frame_length = i;

  /* Init */
  next_symbol = AX25_MARK;
  bit_index = 8;
  current_bit = 1;
  one_count = 0;

  ax25_state = AX25_PREAMBLE;
  ax25_index = 0;

  /* Hardware init */
  ax25_gpio1_pwm_init();
}



/**
 * Sets up gpio1 for the afsk pwm output. Uses gclk 7
 */
void ax25_gpio1_pwm_init(void)
{
  float gclk1_frequency = (float)system_gclk_gen_get_hz(1);

  uint32_t top = 38;//(uint32_t)(gclk1_frequency / 13200.0*4);// & ~0x1;
  uint32_t capture = top >> 1;  /* 50% duty cycle */

  if (top > 0xFF) while (1); // It's only an 8-bit counter

  /* Setup GCLK genertor 7 */
  system_gclk_gen_set_config(GCLK_GENERATOR_7,
                             GCLK_SOURCE_GCLKGEN1,	/* Source	*/
        		     false,		/* High When Disabled	*/
        		     11, /* Division Factor	*/// TODO
        		     false,		/* Run in standby	*/
        		     false);		/* Output Pin Enable	*/
  system_gclk_gen_enable(GCLK_GENERATOR_7);

  /* Configure timer */
  bool capture_channel_enables[]    = {false, true};
  uint32_t compare_channel_values[] = {0x0000, capture};

  tc_init(TC5,
	  GCLK_GENERATOR_7,
	  TC_COUNTER_SIZE_8BIT,
	  TC_CLOCK_PRESCALER_DIV4,
	  TC_WAVE_GENERATION_NORMAL_PWM,
	  TC_RELOAD_ACTION_GCLK,
	  TC_COUNT_DIRECTION_UP,
	  TC_WAVEFORM_INVERT_OUTPUT_NONE,
	  false,			/* Oneshot = false */
	  false,			/* Run in standby = false */
	  0x0000,			/* Initial value */
	  top,				/* Top value */
	  capture_channel_enables,	/* Capture Channel Enables */
	  compare_channel_values);	/* Compare Channels Values */


  /* Enable the output pin */
  system_pinmux_pin_set_config(SI406X_GPIO1_PINMUX >> 16,	/* GPIO Pin	*/
 			       SI406X_GPIO1_PINMUX & 0xFFFF,	/* Mux Position */
 			       SYSTEM_PINMUX_PIN_DIR_INPUT,	/* Direction	*/
 			       SYSTEM_PINMUX_PIN_PULL_NONE,	/* Pull		*/
 			       false);    			/* Powersave	*/

  tc_enable(TC5);
  tc_start_counter(TC5);
}


/**
 * Returns the next byte to transmit
 */
struct ax25_byte_t ax25_get_next_byte(void) {

  /* Return HLDC flag by default */
  struct ax25_byte_t next = { .val = 0x7E, .stuff = 0 };

  switch (ax25_state) {
  case AX25_PREAMBLE:           /* Preamble */
    /* Return flag by default*/

    /* Check for next state */
    ax25_index++;
    if (ax25_index >= AX25_PREAMBLE_FLAGS) {
      /* Next state */
      ax25_state = AX25_FRAME;
      ax25_index = 0;
    }
    break;


  case AX25_FRAME:              /* Frame */
    /* Return data */
    next.val = ax25_frame[ax25_index];
    next.stuff = 1;

    /* Check for next state */
    ax25_index++;
    if (ax25_index >= ax25_frame_length) {
      /* Next state */
      ax25_state = AX25_POSTAMBLE;
      ax25_index = 0;
    }
    break;


  case AX25_POSTAMBLE:          /* Postamble */
    /* Return flag by default */

    /* Check for next state */
    ax25_index++;
    if (ax25_index >= AX25_POSTAMBLE_FLAGS) {
      /* Next state */
      ax25_state = AX25_PREAMBLE;
      ax25_index = 0;
    }
    break;


  default:
    break;
  }

  return next;
}


/**
 * Returns the next symbol to transmit
 */
enum ax25_symbol_t ax25_get_next_symbol(void) {

  uint8_t bit;

  if (bit_index >= 8) {
    current_byte = ax25_get_next_byte();
    bit_index = 0;
  }

  /* transmit bits lsb first */
  bit = current_byte.val & 0x01;

  if (bit) {                    /* One */

    one_count++;

    /* Check if we need to stuff this bit */
    if (one_count >= AX25_BITSTUFFINGCOUNT && current_byte.stuff) {
      current_byte.val &= ~0x01;/* Next bit is zero */
      one_count = 0;

    } else {
      current_byte.val >>= 1;   /* Move along one bit */
      bit_index++;
    }
  } else {                      /* Zero */

    one_count = 0;              /* Clear concecutive ones */
    current_byte.val >>= 1;     /* Move along one bit */
    bit_index++;

    /* NRZI encoding */
    current_bit ^= 0x01;
  }

  return current_bit;
}


/**
 * Called at our tick rate, controls the pwm gclk
 *
 * Returns 1 when more work todo, 0 when finished
 */
uint8_t ax25_tick(void)
{

  if (next_symbol == AX25_SPACE) {


    system_gclk_gen_set_config(GCLK_GENERATOR_7,
                               GCLK_SOURCE_GCLKGEN1,	/* Source	*/
                               false,		/* High When Disabled	*/
                               6, /* Division Factor	*/// TODO
                               false,		/* Run in standby	*/
                               false);		/* Output Pin Enable	*/

  } else {


    system_gclk_gen_set_config(GCLK_GENERATOR_7,
                               GCLK_SOURCE_GCLKGEN1,	/* Source	*/
                               false,		/* High When Disabled	*/
                               11, /* Division Factor	*/// TODO
                               false,		/* Run in standby	*/
                               false);		/* Output Pin Enable	*/

  }

  next_symbol = ax25_get_next_symbol();

  return 1;
}
