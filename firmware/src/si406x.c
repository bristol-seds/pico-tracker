/*
 * Functions for controlling Si406x radios
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
#include "semihosting.h"
#include "system/port.h"
#include "spi_bitbang.h"
#include "si406x_defs.h"
#include "hw_config.h"

#define RADIO_FREQ	434600000
// Quite low power
#define RADIO_PWR	0x7f
#define VCXO_FREQ	SI406X_TCXO_FREQUENCY

uint32_t active_freq       = RADIO_FREQ;
uint8_t active_pwr         = RADIO_PWR;
uint16_t si446x_powerlevel = RADIO_PWR;

/**
 * Generic SPI Send / Receive for the Si406x
 */
void _si406x_transfer(int tx_count, int rx_count, const uint8_t *data)
{
  uint8_t response[100];

  /* Send command */
  _si406x_cs_enable();

  for (int i = 0; i < tx_count; i++) {
    spi_bitbang_transfer(data[i]);
  }

  _si406x_cs_disable();

  /**
   * Poll CTS. From the docs:
   *
   * READ_CMD_BUFF is used to poll the CTS signal via the SPI bus. The
   * NSEL line should be pulled low, followed by sending the
   * READ_CMD_BUFF command on SDI. While NSEL remains asserted low, an
   * additional eight clock pulses are sent on SCLK and the CTS
   * response byte is read on SDO. If the CTS response byte is not
   * 0xFF, the host MCU should pull NSEL high and repeat the polling
   * procedure.
   */

  for (int i = 0; i < 20000; i++); // 20µS
  _si406x_cs_enable();

  do {
    /* Issue READ_CMD_BUFF */
    spi_bitbang_transfer(SI_CMD_READ_CMD_BUFF);
    response[0] = spi_bitbang_transfer(0xFF);

    /* If the reply is 0xFF, read the response */
    if (response[0] == 0xFF) break;

    /* Otherwise repeat the procedure */
    _si406x_cs_disable();
    for (int i = 0; i < 200; i++); // 20µS
    _si406x_cs_enable();

  } while (1); /* TODO: Timeout? */

  /**
   * Read response. From the docs:
   *
   * If the CTS response byte is 0xFF, the host MCU should keep NSEL
   * asserted low and provide additional clock cycles on SCLK to read
   * out as many response bytes (on SDO) as necessary. The host MCU
   * should pull NSEL high upon completion of reading the response
   * stream.
   */
  for (int i = 1; i < rx_count; i++) {
    response[i] = spi_bitbang_transfer(0xFF);
  }

  _si406x_cs_disable();
}

/**
 * Set the Si406x synthesiser to the given frequency
 */
static void si406x_set_frequency(uint32_t frequency)
{
  uint8_t outdiv, band;

  if (frequency < 705000000UL) {
    outdiv = 6;  band = 1;
  }
  if (frequency < 525000000UL) {
    if (VCXO_FREQ < 24000000) { // clock < 24mhz
      outdiv = 4; band = 0;
    } else {
      outdiv = 8; band = 2;
    }
  }
  if (frequency < 353000000UL) {
    outdiv = 12; band = 3;
  }
  if (frequency < 239000000UL) {
    outdiv = 16; band = 4;
  }
  if (frequency < 177000000UL) {
    outdiv = 24; band = 5;
  }

  uint32_t f_pfd = 2 * VCXO_FREQ / outdiv;

  uint16_t n = ((uint16_t)(frequency / f_pfd)) - 1;

  float ratio = (float)frequency / (float)f_pfd;
  float rest  = ratio - (float)n;

  uint32_t m = (uint32_t)(rest * 524288UL); // 2^19

  // set the band parameter
  uint8_t sy_sel = 8;
  uint8_t set_band_property_command[] = {0x11, 0x20, 0x01, 0x51, (band + sy_sel)};
  // send parameters
  _si406x_transfer(5, 1, set_band_property_command);

  // Set the pll parameters
  uint16_t m2 = m / 0x10000;
  uint16_t m1 = (m - m2 * 0x10000) / 0x100;
  uint16_t m0 = (m - m2 * 0x10000 - m1 * 0x100);

  // assemble parameter string
  const uint8_t set_frequency_control_inte[] = {0x11, 0x40, 0x04, 0x00, n, m2, m1, m0};
//  const uint8_t set_frequency_control_inte[] = {
  //  0x11, 0x40, 0x08, 0x00, n, m2, m1, m0, 0x0B, 0x61, 0x20, 0xFA};
  //const uint8_t set_frequency_control_inte[] = {0x11, 0x40, 0x08, 0x00, n, m2, m1, m0, 0x0B, 0x61, 0x20, 0xFA};

  // send parameters
  _si406x_transfer(8, 1, set_frequency_control_inte);

  // Read parameters
  const uint8_t get_frequency_control[] = { 0x12, 0x40, 0x04, 0x00 };
  _si406x_transfer(4, 5, get_frequency_control);
}

void si406x_init(void)
{
  /* Configure the SDN pin */
  port_pin_set_config(SI406X_SDN_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */

  /* Put the SI406x in shutdown */
  _si406x_sdn_enable();

  /* Configure the SDN pin */
  port_pin_set_config(SI406X_SEL_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */

  /* Put the SEL pin in reset */
  _si406x_cs_disable();

  /* Configure the GPIO and IRQ pins */

  /* Configure the serial port */
  spi_bitbang_init(SI406X_SERCOM_MOSI_PIN,
		   SI406X_SERCOM_MISO_PIN,
		   SI406X_SERCOM_SCK_PIN);

  /* Boot */
  for (int i = 0; i < 15*10000; i++); // 15ms
  _si406x_sdn_disable();
  for (int i = 0; i < 15*10000; i++); // 15ms

  const uint8_t PART_INFO_command[] = {0x01}; // Part Info
  _si406x_transfer(1, 9, PART_INFO_command);
}


/**
 * Sets the modem into CW mode
 */
static void si446xSetModem(void)
{
  // Set to CW mode
  const uint8_t set_modem_mod_type_command[] = {0x11, 0x20, 0x01, 0x00, 0x00};
  _si406x_transfer(5, 1, set_modem_mod_type_command);
}

/**
 * Sets the tx power
 */
void si446xSetPower(uint8_t pwr)
{
  // Set the Power
//  uint8_t set_pa_pwr_lvl_property_command[] = {0x11, 0x22, 0x01, 0x01, pwr};
//  _si406x_transfer(5, 1, set_pa_pwr_lvl_property_command);
}

// Config reset ----------------------------------------------------------
static void si446xReset(void)
{
//  _si406x_hf_clock_enable();
  _si406x_sdn_enable();  // active high shutdown = reset

  for (int i = 0; i < 15*10000; i++); // 15ms
  _si406x_sdn_disable();   // booting
  for (int i = 0; i < 15*10000; i++); // 15ms


  const uint8_t PART_INFO_command[] = {0x01}; // Part Info
  _si406x_transfer(1, 9, PART_INFO_command);

  //divide VCXO_FREQ into its bytes; MSB first
  uint16_t x3 = VCXO_FREQ / 0x1000000;
  uint16_t x2 = (VCXO_FREQ - x3 * 0x1000000) / 0x10000;
  uint16_t x1 = (VCXO_FREQ - x3 * 0x1000000 - x2 * 0x10000) / 0x100;
  uint16_t x0 = (VCXO_FREQ - x3 * 0x1000000 - x2 * 0x10000 - x1 * 0x100);

  /* ---- POWER_UP ---- */
  /* no patch, boot main app. img, FREQ_VCXO, return 1 byte */
  /* */
  const uint8_t init_command[] = {0x02, 0x01, 0x01, x3, x2, x1, x0};
  _si406x_transfer(7, 1 , init_command);


  //  Clear all pending interrupts and get the interrupt status back
  const uint8_t get_int_status_command[] = {0x20, 0x00, 0x00, 0x00};
  _si406x_transfer(4, 9, get_int_status_command);
  // cliPrint("Radio ready\n");

  const uint8_t set_int_ctrl_enable[] = {0x11, 0x01, 0x01, 0x00, 0x00};
  _si406x_transfer(5, 1, set_int_ctrl_enable);
  // cliPrint("Setting no Interrupts (see WDS)\n");

  //  Set all GPIOs LOW; Link NIRQ to CTS; Link SDO to MISO; Max drive strength
  const uint8_t gpio_pin_cfg_command[] = {0x13, 0x02, 0x02, 0x02, 0x02, 0x08, 0x0B, 0x00};
  _si406x_transfer(8, 8, gpio_pin_cfg_command);
  // cliPrint("LEDs should be switched off at this point\n");

  //const uint8_t set_global_config1[] = {0x11, 0x00, 0x01, 0x03, 0x60};
  //_si406x_transfer(5, 1, set_global_config1);
  // Sequencer Mode = Fast, Fifo = Half Duplex
  // cliPrint("Setting special global Config 1 changes (see WDS)\n");

  // const uint8_t set_global_xo_tune_command[] = {0x11, 0x00, 0x01, 0x00, 0x00};
  //_si406x_transfer(5, 1, set_global_xo_tune_command);
  // cliPrint("Setting no additional capacitance on VXCO\n");

  si406x_set_frequency(active_freq);

  si446xSetPower(active_pwr);

  si446xSetModem();
  // cliPrint("CW mode set\n");

  si406x_state_tx_tune();
  // cliPrint("TX tune\n");
}

void si446xPTTOn(void)
{
  si446xReset();

  // turn on the LED (GPIO0) to indicate TX
  //  Set GPIOs LOW; Link NIRQ to CTS; Link SDO to MISO; Max drive strength
  const uint8_t gpio_pin_cfg_command2[] = {0x13, 0x02, 0x02, 0x02, 0x02, 0x08, 0x0B, 0x00};
  _si406x_transfer(8, 1, gpio_pin_cfg_command2);

  si406x_state_tx();

  const uint8_t get_state[] = {0x33};
  _si406x_transfer(1, 3, get_state);
}

void si446xPTTOff(void)
{
  si406x_state_ready();

  // turn off the LED (GPIO1)
  //  Set GPIO0 HIGH
  const uint8_t gpio_pin_cfg_command0[] = {0x13, 0x03, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00};
  _si406x_transfer(8, 1, gpio_pin_cfg_command0);

  _si406x_sdn_enable();  // active high = shutdown
  _si406x_hf_clock_disable(); // keep enabled for now
}

/**
 * Quick and dirty loopback test. Should print 0x34
 */
void spi_loopback_test(void)
{
  /* spi_init(SI406X_SERCOM, */
  /* 	   SPI_MODE_MASTER,		/\** SPI mode *\/ */
  /* 	   SPI_DATA_ORDER_MSB,		/\** Data order *\/ */
  /* 	   SPI_TRANSFER_MODE_0,		/\** Transfer mode *\/ */
  /* 	   SI406X_SERCOM_MUX,		/\** Mux setting *\/ */
  /* 	   SPI_CHARACTER_SIZE_8BIT,	/\** SPI character size *\/ */
  /* 	   false,			/\** Enabled in sleep *\/ */
  /* 	   true,				/\** Enable receiver *\/ */
  /* 	   100000,			/\** Master - Baud rate *\/ */
  /* 	   0,				/\** Slave - Frame format *\/ */
  /* 	   0,				/\** Slave - Address mode *\/ */
  /* 	   0,				/\** Slave - Address *\/ */
  /* 	   0,				/\** Slave - Address mask *\/ */
  /* 	   false,			/\** Slave - Preload data  *\/ */
  /* 	   GCLK_GENERATOR_0,		/\** GCLK generator to use *\/ */
  /* 	   SI406X_SERCOM_MOSI_PINMUX,	/\** Pinmux *\/ */
  /* 	   SI406X_SERCOM_MISO_PINMUX,	/\** Pinmux *\/ */
  /* 	   SI406X_SERCOM_SCK_PINMUX,	/\** Pinmux *\/ */
  /* 	   PINMUX_UNUSED);		/\** Pinmux *\/ */

  /* Init loopback */
  spi_bitbang_init(SI406X_SERCOM_MOSI_PIN,
		   SI406X_SERCOM_MOSI_PIN,
		   SI406X_SERCOM_SCK_PIN);

  /* Enable */

  /* Test transfer */
  uint8_t data = spi_bitbang_transfer(0x34);

  /* Print result */
  semihost_printf("Rx'ed: 0x%02x\n", data);
}

