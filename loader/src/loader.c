/*
 * bootloader functions
 * Copyright (C) 2016  Richard Meadows <richardeoin>
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

#define APPLICATION_BASE	(0x00004000) /* 16K */

/**
 * transfers control to application
 */
void transfer_to_application(void)
{
  void (*application_ptr)(void);

  /* new stack pointer */
  __set_MSP(*(uint32_t *)APPLICATION_BASE);

  /* new vector table */
  SCB->VTOR = ((uint32_t)APPLICATION_BASE & SCB_VTOR_TBLOFF_Msk);

  /* Jump to application reset handler */
  application_ptr = (void (*)(void))(unsigned *)(*(unsigned *)(APPLICATION_BASE + 4));
  application_ptr();
}
