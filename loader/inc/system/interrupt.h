/**
 * Global interrupt management for SAM D20, SAM3 and SAM4 (NVIC based)
 *
 * Copyright (c) 2012-2014 Atmel Corporation. All rights reserved.
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

#ifndef SYSTEM_INTERRUPT_H
#define SYSTEM_INTERRUPT_H

#include "samd20.h"

/**
 * Registers an interupt with the NVIC
 */
#define irq_register_handler(int_num, int_prio)		\
  do {							\
    NVIC_ClearPendingIRQ((IRQn_Type)int_num);		\
    NVIC_SetPriority(    (IRQn_Type)int_num, int_prio);	\
    NVIC_EnableIRQ(      (IRQn_Type)int_num);		\
  } while (0)

/**
 * Safe global IRQ enable / disable
 */
#define cpu_irq_enable()			\
  do {						\
    __DMB();					\
    __enable_irq();				\
  } while (0)
#define cpu_irq_disable()			\
  do {						\
    __disable_irq();				\
    __DMB();					\
  } while (0)
#define cpu_irq_is_enabled()    (__get_PRIMASK() == 0)

void cpu_irq_enter_critical(void);
void cpu_irq_leave_critical(void);

#endif /* SYSTEM_INTERRUPT_H */
