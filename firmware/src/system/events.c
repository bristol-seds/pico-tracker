/*
 * SAM D20/D21/R21 Event System Controller Driver
 *
 * Copyright (C) 2013-2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include "system/events.h"
#include "system/system.h"
#include "samd20.h"

#define EVENTS_INVALID_CHANNEL                  0xff
#define _EVENTS_START_OFFSET_BUSY_BITS           8
#define _EVENTS_START_OFFSET_USER_READY_BIT      0
#define _EVENTS_START_OFFSET_DETECTION_BIT       8
#define _EVENTS_START_OFFSET_OVERRUN_BIT         0

/**
 * internal
 *
 */
uint32_t _events_find_bit_position(uint8_t channel, uint8_t start_offset)
{
  uint8_t byte_offset = channel >> 3;
  uint32_t pos;

  pos = (((channel % 8) + 1) << start_offset) * ((0xffff * byte_offset) + 1);

  return pos;
}


void system_events_init(void)
{
  /* Enable EVSYS register interface */
  system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC, PM_APBCMASK_EVSYS);

  /* Make sure the EVSYS module is properly reset */
  EVSYS->CTRL.reg = EVSYS_CTRL_SWRST;

  while (EVSYS->CTRL.reg & EVSYS_CTRL_SWRST);
}


enum status_code events_allocate(uint8_t channel,
                                 enum events_edge_detect edge_detect,	/** edge detection mode */
                                 enum events_path_selection path,	/** events channel path */
                                 uint8_t generator,			/** event generator for the channel */
                                 uint8_t clock_source)			/** clock source for the event channel */
{
  if (path != EVENTS_PATH_ASYNCHRONOUS) {
    /* Set up a GLCK channel to use with the specific channel */
    system_gclk_chan_set_config(EVSYS_GCLK_ID_0 + channel, (enum gclk_generator)clock_source);
    system_gclk_chan_enable(EVSYS_GCLK_ID_0 + channel);
  }

  EVSYS->CHANNEL.reg = EVSYS_CHANNEL_CHANNEL(channel)       |
    EVSYS_CHANNEL_EVGEN(generator)   |
    EVSYS_CHANNEL_PATH(path)         |
    EVSYS_CHANNEL_EDGSEL(edge_detect);

  return STATUS_OK;
}

enum status_code events_trigger(uint8_t channel)
{
  /* Because of indirect access the channel must be set first */
  ((uint8_t*)&EVSYS->CHANNEL)[0] = EVSYS_CHANNEL_CHANNEL(channel);

  /* Assert if event path is asynchronous */
  if (EVSYS->CHANNEL.reg & EVSYS_CHANNEL_PATH(EVENTS_PATH_ASYNCHRONOUS)) {
    return STATUS_ERR_DENIED;
  }

  /* Assert if event edge detection is not set to RISING */
  if (!(EVSYS->CHANNEL.reg & EVSYS_CHANNEL_EDGSEL(EVENTS_EDGE_DETECT_RISING))) {
    return STATUS_ERR_DENIED;
  }


  /* The GCLKREQ bit has to be set while triggering the software event */
  EVSYS->CTRL.reg = EVSYS_CTRL_GCLKREQ;

  ((uint16_t*)&EVSYS->CHANNEL)[0] = EVSYS_CHANNEL_CHANNEL(channel) |
    EVSYS_CHANNEL_SWEVT;

  EVSYS->CTRL.reg &= ~EVSYS_CTRL_GCLKREQ;

  return STATUS_OK;
}

bool events_is_busy(uint8_t channel)
{
  return EVSYS->CHSTATUS.reg & (_events_find_bit_position(channel, _EVENTS_START_OFFSET_BUSY_BITS));
}

bool events_is_users_ready(uint8_t channel)
{
  return EVSYS->CHSTATUS.reg & (_events_find_bit_position(channel, _EVENTS_START_OFFSET_USER_READY_BIT));
}

bool events_is_detected(uint8_t channel)
{
  uint32_t flag = _events_find_bit_position(channel, _EVENTS_START_OFFSET_DETECTION_BIT);

  /* Clear flag when read */
  if (EVSYS->INTFLAG.reg & flag) {
    EVSYS->INTFLAG.reg = flag;
    return true;
  }

  return false;
}

bool events_is_overrun(uint8_t channel)
{
  uint32_t flag = _events_find_bit_position(channel, _EVENTS_START_OFFSET_OVERRUN_BIT);

  /* Clear flag when read */
  if (EVSYS->INTFLAG.reg & flag) {
    EVSYS->INTFLAG.reg = flag;
    return true;
  }

  return false;
}

enum status_code events_attach_user(uint8_t channel, uint8_t user_id)
{
  /* Channel number is n + 1 */
  EVSYS->USER.reg = EVSYS_USER_CHANNEL(channel + 1) |
    EVSYS_USER_USER(user_id);

  return STATUS_OK;
}

enum status_code events_detach_user(uint8_t channel, uint8_t user_id)
{
  /* Write 0 to the channel bit field to select no input */
  EVSYS->USER.reg = EVSYS_USER_USER(user_id);

  return STATUS_OK;
}
