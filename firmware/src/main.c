/*
 * Bristol Longshot
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

#include "samd20.h"

#include <stdio.h>
#include <math.h>
#include "semihosting.h"
#include "hw_config.h"
#include "system/system.h"
#include "sercom/usart.h"
#include "gps.h"

int main(void)
{
  SystemInit();

  system_clock_source_osc8m_set_config(SYSTEM_OSC8M_DIV_1, /* Prescaler */
				       false,		   /* Run in Standby */
				       false);		   /* Run on Demand */

  /* Update the value of SystemCoreClock */
  SystemCoreClockUpdate();


  /* Set LED0 as output */
  PORTA.DIRSET.reg = (1 << LED0_PIN);

  /* Configure the SysTick for 50ms interrupts */
  SysTick_Config(SystemCoreClock / 20);

  /* Configure Sleep Mode */
  system_set_sleepmode(SYSTEM_SLEEPMODE_IDLE_0);
  //TODO: system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);

  volatile double d, dd;

  d = 2;
  dd = sqrt(d);

  semihost_printf("Hello World %f\n", dd);

  gps_init();

  while (1) {
    //system_sleep();

    for (int i = 0; i < 1000*1000; i++);
    gps_check_lock();
  }
}

void SysTick_Handler(void)
{
  /* Toggle LED0 */
  PORTA.OUTTGL.reg = (1 << LED0_PIN);
}
