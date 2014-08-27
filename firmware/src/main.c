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
#include "system/port.h"
#include "gps.h"
#include "timepulse.h"
//#include "si406x.h"
#include "si4060.h"
#include "spi_bitbang.h"

void si4060_hw_init(void)
{
  /* Configure the SDN pin */
  port_pin_set_config(SI406X_SDN_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */

  /* Put the SI406x in shutdown */
  //_si406x_sdn_enable();
  si4060_shutdown();

  /* Configure the SDN pin */
  port_pin_set_config(SI406X_SEL_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */

  /* Put the SEL pin in reset */
  _si406x_cs_disable();

  /* Configure the GPIO and IRQ pins */
  port_pin_set_config(SI406X_GPIO0_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */
  port_pin_set_output_level(SI406X_GPIO0_PIN, 0);

  /* Configure the serial port */
  spi_bitbang_init(SI406X_SERCOM_MOSI_PIN,
		   SI406X_SERCOM_MISO_PIN,
		   SI406X_SERCOM_SCK_PIN);
}

int main(void)
{
  /* Clock up to 28MHz with 1 wait state */
  system_flash_set_waitstates(1);

  /* Up the clock rate to 4MHz */
  system_clock_source_osc8m_set_config(SYSTEM_OSC8M_DIV_2, /* Prescaler */
				       false,		   /* Run in Standby */
				       false);		   /* Run on Demand */

  /* Restart the GCLK Module */
  system_gclk_init();

  /* Get the current CPU Clock */
  SystemCoreClock = system_cpu_clock_get_hz();

  /* Set LED0 as output */
  //PORTA.DIRSET.reg = (1 << SI406X_HF_CLK_PIN);

  /* Configure the SysTick for cpu/1000 output*//*for 50ms interrupts */
  //SysTick_Config(500); //SystemCoreClock / 20);

  /* Configure Sleep Mode */
  system_set_sleepmode(SYSTEM_SLEEPMODE_IDLE_0);
  //TODO: system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);

  semihost_printf("Hello World %fHz\n", RF_FREQ_HZ);


  /* Initialise GPS */
  gps_init();
  /* Wait for GPS timepulse to stabilise */
  for (int i = 0; i < 1000*100; i++);


  /* For the moment output GCLK_MAIN / 2 on HF CLK */
  switch_gclk_main_to_timepulse();
  //half_glck_main_on_hf_clk();
  /* Wait for HF CLK to stabilise */
  for (int i = 0; i < 1000*100; i++);

  semihost_printf("GCLK_MAIN = %d\n", gclk_main_frequency());

  /* Drop the CPU clock to 1.5Mhz */
  system_cpu_clock_set_divider(SYSTEM_MAIN_CLOCK_DIV_16);

  /* Initialise Si4060 */
  si4060_hw_init();

  /* reset the radio chip from shutdown */
  si4060_reset();

  /* check radio communication */
  int i = si4060_part_info();
  if (i != 0x4063) {
    while(1);
  }

  si4060_power_up();
  si4060_setup(MOD_TYPE_CW);
  si4060_start_tx(0);

  uint32_t ll = 0;

  while (1) {
    semihost_printf("State is %d    %d\n", si4060_get_state(), ll++);
    si4060_get_freq();
    for (int i = 0; i < 1000*10; i++);
    port_pin_set_output_level(SI406X_GPIO0_PIN, 0);
    //si4060_start_tx(0);
    for (int i = 0; i < 1000*10; i++);
    port_pin_set_output_level(SI406X_GPIO0_PIN, 1);




    //system_sleep();
  }
}

void SysTick_Handler(void)
{
  /* Toggle LED0 */
  //PORTA.OUTTGL.reg = (1 << SI406X_HF_CLK_PIN);
}
