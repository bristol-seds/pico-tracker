/*
 * Bristol SEDS pico-tracker
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

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "samd20.h"
#include "semihosting.h"
#include "hw_config.h"
#include "system/system.h"
#include "sercom/usart.h"
#include "system/port.h"
#include "tc/tc_driver.h"
#include "gps.h"
#include "system/wdt.h"
#include "timepulse.h"
#include "telemetry.h"
//#include "si406x.h"
#include "si4060.h"
#include "spi_bitbang.h"
#include "rtty.h"
#include "system/interrupt.h"

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

  /* Configure the serial port */
  spi_bitbang_init(SI406X_SERCOM_MOSI_PIN,
		   SI406X_SERCOM_MISO_PIN,
		   SI406X_SERCOM_SCK_PIN);
}
void si4060_gpio_init()
{
  /* Configure the GPIO and IRQ pins */
  port_pin_set_config(SI406X_GPIO0_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */
  port_pin_set_output_level(SI406X_GPIO0_PIN, 0);
  /* Configure the GPIO and IRQ pins */
  port_pin_set_config(SI406X_GPIO1_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */
  port_pin_set_output_level(SI406X_GPIO1_PIN, 0);
}



/**
 * Initialises the status LED
 */
static inline void led_init(void)
{
  port_pin_set_config(LED0_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */
  port_pin_set_output_level(LED0_PIN, 1);	/* LED is active low */
}
/**
 * Turns the status LED on
 */
static inline void led_on(void)
{
  port_pin_set_output_level(LED0_PIN, 0);	/* LED is active low */
}
/**
 * Turns the status lED off
 */
static inline void led_off(void)
{
  port_pin_set_output_level(LED0_PIN, 1);	/* LED is active low */
}



void set_timer(uint32_t time)
{
  bool capture_channel_enables[]    = {false, false};
  uint32_t compare_channel_values[] = {time, 0x0000};

  tc_init(TC2,
	  GCLK_GENERATOR_0,
	  TC_COUNTER_SIZE_32BIT,
	  TC_CLOCK_PRESCALER_DIV1,
	  TC_WAVE_GENERATION_NORMAL_FREQ,
	  TC_RELOAD_ACTION_GCLK,
	  TC_COUNT_DIRECTION_UP,
	  TC_WAVEFORM_INVERT_OUTPUT_NONE,
	  false,			/* Oneshot = false */
	  false,			/* Run in standby = false */
	  0x0000,			/* Initial value */
	  time+1,			/* Top value */
	  capture_channel_enables,	/* Capture Channel Enables */
	  compare_channel_values);	/* Compare Channels Values */

  struct tc_events ev;
  memset(&ev, 0, sizeof(ev));
  ev.generate_event_on_compare_channel[0] = true;
  ev.event_action = TC_EVENT_ACTION_RETRIGGER;

  tc_enable_events(TC2, &ev);

  irq_register_handler(TC2_IRQn, 3);

  tc_enable(TC2);
  tc_start_counter(TC2);
}

/* void wdt_init() { */
/*   /\* 64 seconds timeout. So 2^(15+6) cycles of the wdt clock *\/ */
/*   system_gclk_gen_set_config(WDT_GCLK, */
/* 			     GCLK_SOURCE_OSCULP32K, /\* Source 		*\/ */
/* 			     false,		/\* High When Disabled	*\/ */
/* 			     128,		/\* Division Factor	*\/ */
/* 			     false,		/\* Run in standby	*\/ */
/* 			     true);		/\* Output Pin Enable	*\/ */
/*   system_gclk_gen_enable(WDT_GCLK); */

/*   /\* Set the watchdog timer. On 256Hz gclk 4  *\/ */
/*   wdt_set_config(true,			/\* Lock WDT		*\/ */
/*   		 true,			/\* Enable WDT		*\/ */
/*   		 GCLK_GENERATOR_4,	/\* Clock Source		*\/ */
/*   		 WDT_PERIOD_16384CLK,	/\* Timeout Period	*\/ */
/*   		 WDT_PERIOD_NONE,	/\* Window Period	*\/ */
/*   		 WDT_PERIOD_NONE);	/\* Early Warning Period	*\/ */
/* } */

int main(void)
{
  /**
   * Internal initialisation
   * ---------------------------------------------------------------------------
   */

  /* Clock up to 14MHz with 0 wait states */
  system_flash_set_waitstates(SYSTEM_WAIT_STATE_1_8V_14MHZ);

  /* Up the clock rate to 4MHz */
  system_clock_source_osc8m_set_config(SYSTEM_OSC8M_DIV_2, /* Prescaler */
				       false,		   /* Run in Standby */
				       false);		   /* Run on Demand */

  /* Restart the GCLK Module */
  system_gclk_init();

  /* Get the current CPU Clock */
  SystemCoreClock = system_cpu_clock_get_hz();

  /* Configure Sleep Mode */
  system_set_sleepmode(SYSTEM_SLEEPMODE_IDLE_0);
  //TODO: system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);

  /* Configure the SysTick for 50Hz triggering */
  SysTick_Config(SystemCoreClock / 50);


  /**
   * System initialisation
   * ---------------------------------------------------------------------------
   */

  led_init();
  gps_init();

  /* Initialise Si4060 */
  si4060_hw_init();
  si4060_gpio_init();
  si4060_reset();

  /* check radio communication */
  int i = si4060_part_info();
  if (i != 0x4063) {
    while(1);
  }

  si4060_power_up();
  si4060_setup(MOD_TYPE_2FSK);

  si4060_gpio_init();
  si4060_start_tx(0);

  while (1) {
    /* Send the last packet */
    while (rtty_active());

    port_pin_set_output_level(SI406X_GPIO0_PIN, 0);

    /* Send requests to the gps */
    gps_update();

    /* Wait between frames */
    led_on();
    for (int i = 0; i < 100*1000; i++);
    led_off();
    for (int i = 0; i < 100*1000; i++);

    /* Set the next packet */
    set_telemetry_string();

    port_pin_set_output_level(SI406X_GPIO0_PIN, 1);

    //system_sleep();
  }
}

/**
 * Called at 50Hz
 */
void SysTick_Handler(void)
{
  /* Output RTTY */
  rtty_tick();
}
