/*
 * Functions for controlling Si Labs Transceivers
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
#include "si_trx.h"
#include "semihosting.h"
#include "system/port.h"
#include "spi_bitbang.h"
#include "si_trx_defs.h"
#include "hw_config.h"

#define VCXO_FREQUENCY	SI4xxx_TCXO_FREQUENCY


/**
 * =============================================================================
 *                         Helper Functions
 * =============================================================================
 */

/**
 * Convenience transfer functions
 */
static void _si_trx_set_property_8(uint8_t group, uint8_t property, uint8_t value)
{
  uint8_t buffer[5];

  buffer[0] = SI_CMD_SET_PROPERTY;
  buffer[1] = group;
  buffer[2] = 1;
  buffer[3] = property;
  buffer[4] = value;

  _si_trx_transfer(5, 0, buffer);
}
static void _si_trx_set_property_16(uint8_t group, uint8_t property, uint16_t value)
{
  uint8_t buffer[6];

  buffer[0] = SI_CMD_SET_PROPERTY;
  buffer[1] = group;
  buffer[2] = 2;
  buffer[3] = property;
  buffer[4] = (value >> 8);
  buffer[5] = (value);

  _si_trx_transfer(6, 0, buffer);
}
static void _si_trx_set_property_24(uint8_t group, uint8_t property, uint32_t value)
{
  uint8_t buffer[8];

  buffer[0] = SI_CMD_SET_PROPERTY;
  buffer[1] = group;
  buffer[2] = 3;
  buffer[3] = property;
  buffer[4] = (value >> 16);
  buffer[5] = (value >> 8);
  buffer[6] = (value);

  _si_trx_transfer(7, 0, buffer);
}
static void _si_trx_set_property_32(uint8_t group, uint8_t property, uint32_t value)
{
  uint8_t buffer[8];

  buffer[0] = SI_CMD_SET_PROPERTY;
  buffer[1] = group;
  buffer[2] = 4;
  buffer[3] = property;
  buffer[4] = (value >> 24);
  buffer[5] = (value >> 16);
  buffer[6] = (value >> 8);
  buffer[7] = (value);

  _si_trx_transfer(8, 0, buffer);
}






void _si_trx_transfer_nocts(int tx_count, int rx_count, uint8_t *data)
{
  /* Unused */
  (void)rx_count;

  /* Send command */
  _si_trx_cs_enable();

  for (int i = 0; i < tx_count; i++) {
    spi_bitbang_transfer(data[i]);
  }

  _si_trx_cs_disable();
}

/**
 * Generic SPI Send / Receive
 */
void _si_trx_transfer(int tx_count, int rx_count, uint8_t *data)
{
  uint8_t response;

  /* Send command */
  _si_trx_cs_enable();

  for (int i = 0; i < tx_count; i++) {
    spi_bitbang_transfer(data[i]);
  }

  _si_trx_cs_disable();

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

  do {
    for (int i = 0; i < 200; i++); /* Approx. 20µS */
    _si_trx_cs_enable();

    /* Issue READ_CMD_BUFF */
    spi_bitbang_transfer(SI_CMD_READ_CMD_BUFF);
    response = spi_bitbang_transfer(0xFF);

    /* If the reply is 0xFF, read the response */
    if (response == 0xFF) break;

    /* Otherwise repeat the procedure */
    _si_trx_cs_disable();

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
  for (int i = 0; i < rx_count; i++) {
    data[i] = spi_bitbang_transfer(0xFF);
  }

  _si_trx_cs_disable();
}


/**
 * Issues the POWER_UP command
 */
static void si_trx_power_up(uint8_t clock_source, uint32_t xo_freq)
{
  uint8_t buffer[7];

  buffer[0] = SI_CMD_POWER_UP;
  buffer[1] = SI_POWER_UP_FUNCTION;
  buffer[2] = clock_source;
  buffer[3] = (xo_freq >> 24);
  buffer[4] = (xo_freq >> 16);
  buffer[5] = (xo_freq >> 8);
  buffer[6] = (xo_freq);

  _si_trx_transfer(7, 0, buffer);
}
/**
 * Gets the 16 bit part number
 */
static uint16_t si_trx_get_part_info(void)
{
  uint8_t buffer[3];

  buffer[0] = SI_CMD_PART_INFO;

  _si_trx_transfer(1, 3, buffer);

  return (buffer[1] << 8) | buffer[2];
}
/**
 * Clears pending interrupts. Set the corresponding bit low to clear
 * the interrupt.
 */
static void si_trx_clear_pending_interrupts(uint8_t packet_handler_clear_pending,
                                            uint8_t chip_clear_pending)
{
  uint8_t buffer[4];

  buffer[0] = SI_CMD_GET_INT_STATUS;
  buffer[1] = packet_handler_clear_pending & ((1<<5)|(1<<1)); /* Mask used bits */
  buffer[2] = 0;
  buffer[3] = chip_clear_pending;

  _si_trx_transfer(4, 0, buffer);

  /* This command returns the interrupts status, but we don't use it */
}
/**
 * Sets the GPIO configuration for each pin
 */
static void si_trx_set_gpio_configuration(si_gpio_t gpio0, si_gpio_t gpio1,
                                          si_gpio_t gpio2, si_gpio_t gpio3,
                                          uint8_t drive_strength)
{
  uint8_t buffer[8];
  buffer[0] = SI_CMD_GPIO_PIN_CFG;
  buffer[1] = gpio0;
  buffer[2] = gpio1;
  buffer[3] = gpio2;
  buffer[4] = gpio3;
  buffer[5] = SI_GPIO_PIN_CFG_NIRQ_MODE_DONOTHING;
  buffer[6] = SI_GPIO_PIN_CFG_SDO_MODE_DONOTHING;
  buffer[7] = drive_strength;

  _si_trx_transfer(8, 0, buffer);
}
/**
 * Starts transmitting
 */
static void si_trx_start_tx(uint8_t channel)
{
  uint8_t buffer[5];
  buffer[0] = SI_CMD_START_TX;
  buffer[1] = channel;
  buffer[2] = (1 << 4);
  buffer[3] = 0;
  buffer[4] = 0;

  _si_trx_transfer(5, 0, buffer);
}
/**
 * Gets readings from the auxillary ADC
 */
static void si_trx_get_adc_reading(uint8_t enable, uint8_t configuration,
                                   uint16_t* gpio_value,
                                   uint16_t* battery_value,
                                   uint16_t* temperature_value)
{
  uint8_t buffer[6];
  buffer[0] = SI_CMD_GET_ADC_READING;
  buffer[1] = enable;
  buffer[2] = configuration;

  _si_trx_transfer(3, 6, buffer);

  *gpio_value = ((buffer[0] & 0x7) << 8) | buffer[1];
  *battery_value = ((buffer[2] & 0x7) << 8) | buffer[3];
  *temperature_value = ((buffer[4] & 0x7) << 8) | buffer[5];
}
/**
 * Returns the measured internal die temperature of the radio
 */
float si_trx_get_temperature(void)
{
  uint16_t raw_gpio, raw_battery, raw_temperature;

  /* Get the reading from the adc */
  si_trx_get_adc_reading(SI_GET_ADC_READING_TEMPERATURE, 0,
                         &raw_gpio, &raw_battery, &raw_temperature);

  return (((float)raw_temperature * 568.0) / 2560.0) - 297.0;
}


/**
 * Sets the internal frac-n pll synthesiser divisiors
 */
static void si_trx_frequency_control_set_divider(uint8_t integer_divider,
						 uint32_t fractional_divider)
{
  uint32_t divider = (fractional_divider & 0xFFFFFF) | (integer_divider << 24);

  _si_trx_set_property_32(SI_PROPERTY_GROUP_FREQ_CONTROL,
			  SI_FREQ_CONTROL_INTE,
			  divider);
}
/**
 * Sets the output divider of the frac-n pll synthesiser
 */
static void si_trx_frequency_control_set_band(uint8_t band, uint8_t sy_sel)
{
  _si_trx_set_property_8(SI_PROPERTY_GROUP_MODEM,
			 SI_MODEM_CLKGEN_BAND,
			 sy_sel | (band & 0x7));
}
/**
 * Set the control word of the modem delta-sigma modulator
 */
static void si_trx_modem_set_dsm_ctrl(uint8_t ctrl_word)
{
  _si_trx_set_property_8(SI_PROPERTY_GROUP_MODEM,
			 SI_MODEM_DSM_CTRL,
			 ctrl_word);
}

/**
 * Sets the modem frequency deviation. This is how much the external
 * pin deviates the synthesiser from the centre frequency. In units of
 * the resolution of the frac-n pll synthsiser.
 *
 * This is an unsigned 17-bit value.
 */
void si_trx_modem_set_deviation(uint32_t deviation)
{
  _si_trx_set_property_24(SI_PROPERTY_GROUP_MODEM,
			  SI_MODEM_FREQ_DEV,
			  deviation);
}


static void si_trx_modem_set_tx_datarate(uint32_t rate)
{
  uint32_t nco_max_count = ((float)SI4xxx_TCXO_FREQUENCY / 10);

  /* Set TX_NCO_MODE */
  _si_trx_set_property_32(SI_PROPERTY_GROUP_MODEM,
                          SI_MODEM_TX_NCO_MODE,
                          (SI_MODEM_TX_NCO_TXOSR_10X |
                           (nco_max_count & 0x03FFFFFF)));

  /* Set DATA_RATE */
  _si_trx_set_property_24(SI_PROPERTY_GROUP_MODEM,
                          SI_MODEM_DATA_RATE,
                          rate & 0xFFFFFF);
}
/**
 * Writes the coefficients of the modem tx filter
 *
 */
static void si_trx_modem_tx_filter_coefficients(uint8_t* coeff_array)
{
  uint8_t buffer[4+9];

  buffer[0] = SI_CMD_SET_PROPERTY;
  buffer[1] = SI_PROPERTY_GROUP_MODEM; // group
  buffer[2] = 9;
  buffer[3] = SI_MODEM_TX_FILTER_COEFF8; // prop
  /* Write filter coefficents 8 to 0 */
  for (int i = 0; i < 9; i++) {
    buffer[4+i] = coeff_array[8-i];
  }

  _si_trx_transfer(4+9, 0, buffer);
}


/**
 * Sets the modem frequency offset manually. In units of the
 * resolution of the frac-n pll synthsiser.
 *
 * This is a signed 16-bit value.
 */
void si_trx_modem_set_offset(int16_t offset)
{
  /* _si_trx_set_property_16(SI_PROPERTY_GROUP_MODEM, */
  /*                         SI_MODEM_FREQ_OFFSET, */
  /*                         offset); */

  uint8_t buffer[6];

  buffer[0] = SI_CMD_SET_PROPERTY;
  buffer[1] = SI_PROPERTY_GROUP_MODEM; // group
  buffer[2] = 2;
  buffer[3] = SI_MODEM_FREQ_OFFSET; // prop
  buffer[4] = (offset >> 8);
  buffer[5] = (offset);

  _si_trx_transfer_nocts(6, 0, buffer);
}

/**
 * Sets the modulation mode
 */
static void si_trx_modem_set_modulation(uint8_t tx_direct_mode,
					uint8_t tx_direct_gpio,
					uint8_t tx_modulation_source,
					uint8_t modulation_type)
{
  _si_trx_set_property_8(SI_PROPERTY_GROUP_MODEM, SI_MODEM_MOD_TYPE,
			 tx_direct_mode | tx_direct_gpio |
			 tx_modulation_source | modulation_type);
}
/**
 * Sets the tx power
 */
static void si_trx_set_tx_power(uint8_t tx_power)
{
  _si_trx_set_property_8(SI_PROPERTY_GROUP_PA, SI_PA_PWR_LVL, tx_power);
}
/**
 * Set the duty cycle of the tx power amplifier drive signal
 */
static void si_trx_set_tx_pa_duty_cycle(uint8_t pa_duty_cycle)
{
  _si_trx_set_property_8(SI_PROPERTY_GROUP_PA, SI_PA_BIAS_CLKDUTY, pa_duty_cycle);
}


/**
 * Pre-calculates sythesiser paramters for use with si_trx_set_frequency
 * Might be called before module is initialised, so should just contain math
 *
 * frequency: Floating-point value for the frequency
 */
void si_trx_get_frequency_configuration(struct si_frequency_configuration* config,
                                        uint32_t frequency)
{
  /* Higher frequency resolution, but also higher power (~+200µA) */
  config->nprescaler = 2;

  if (frequency < 705000000UL) {
    config->outdiv = 6;		config->band = SI_MODEM_CLKGEN_FVCO_DIV_6;
  }
  if (frequency < 525000000UL) {
    config->outdiv = 8;		config->band = SI_MODEM_CLKGEN_FVCO_DIV_8;
  }
  if (frequency < 353000000UL) {
    config->outdiv = 12;	config->band = SI_MODEM_CLKGEN_FVCO_DIV_12;
  }
  if (frequency < 239000000UL) {
    config->outdiv = 16;	config->band = SI_MODEM_CLKGEN_FVCO_DIV_16;
  }
  if (frequency < 177000000UL) {
    config->outdiv = 24;	config->band = SI_MODEM_CLKGEN_FVCO_DIV_24;
  }

  float f_pfd = config->nprescaler * VCXO_FREQUENCY / config->outdiv;

  config->n = ((uint16_t)(frequency / f_pfd)) - 1;

  float ratio = (float)frequency / f_pfd;
  float rest  = ratio - (float)config->n;

  config->m = (uint32_t)(rest * (float)(1 << 19));

  /* Check n and m are in valid ranges, halt otherwise */
  if (config->n > 0x7f || config->m > 0xfffff) while (1);

  /* Calculate the LSB tuning resolution of the frac-n pll synthesiser. */
  config->lsb_tuning_resolution = f_pfd / (float)(1 << 19);
}

/**
 * Writes a pre-calculated frequency configuration to the si
 * synthesiser. Also sets deviation
 */
static void si_trx_set_frequency(struct si_frequency_configuration* config,
                                 uint16_t deviation)
{
  /* Set the frac-n PLL output divider */
  if (config->nprescaler == 4) { /* Prescaler */
    si_trx_frequency_control_set_band(config->band, SI_MODEM_CLKGEN_SY_SEL_0);
  } else { /* Default Mode */
    si_trx_frequency_control_set_band(config->band, SI_MODEM_CLKGEN_SY_SEL_1);
  }

  /* Set the frac-n PLL divisior */
  si_trx_frequency_control_set_divider(config->n, config->m);

  /* Set the modem dsm control word. Allow even deviation values */
  si_trx_modem_set_dsm_ctrl(SI_MODEM_DSM_CTRL_NOFORCE_DSM_LSB |
                            SI_MODEM_DSM_CTRL_MASH_1_1_1);

  /* Set the modem frequency deviation (for the external pin)*/
  si_trx_modem_set_deviation(deviation);
}

/**
 * Resets the transceiver
 */
void si_trx_reset(uint8_t modulation_type, struct si_frequency_configuration* fconfig,
                  uint16_t deviation, uint8_t power, enum si_filter_model filter)
{
  /* We expect to already be shutdown  */
  _si_trx_sdn_enable();  /* active high shutdown = reset */

  for (int i = 0; i < 15; i++); /* a few microseconds */

  _si_trx_sdn_disable();   /* booting. expected to take 15ms */

  /* Poll for part number */
  while (si_trx_get_part_info() != 17512);

  /* Power Up */
  si_trx_power_up(SI_POWER_UP_TCXO, VCXO_FREQUENCY);

  /* Clear pending interrupts */
  si_trx_clear_pending_interrupts(0, 0);

  /* Disable all interrupts */
  _si_trx_set_property_8(SI_PROPERTY_GROUP_INT_CTL, SI_INT_CTL_ENABLE, 0);

  /* Configure GPIOs */
  si_trx_set_gpio_configuration(SI_GPIO_PIN_CFG_GPIO_MODE_INPUT,
                                SI_GPIO_PIN_CFG_GPIO_MODE_INPUT | SI_GPIO_PIN_CFG_PULL_ENABLE,
                                SI_GPIO_PIN_CFG_GPIO_MODE_INPUT,
                                SI_GPIO_PIN_CFG_GPIO_MODE_INPUT | SI_GPIO_PIN_CFG_PULL_ENABLE,
                                SI_GPIO_PIN_CFG_DRV_STRENGTH_LOW);

  si_trx_set_frequency(fconfig, deviation);
  si_trx_set_tx_power(power);

  /* Modem tx filter coefficients */
  uint8_t rsid_si_coeff[] = {0x1, 0x4, 0x9, 0x11, 0x19, 0x22, 0x2a, 0x30, 0x31};
  uint8_t p_si_coeff[] = {0x6, 0x8, 0x1, 0xf2, 0xe4, 0xe7, 0xff, 0x1d, 0x2b};

  switch (filter) {
    case SI_FILTER_APRS:
      /**
       * Modem tx filter coefficients for APRS
       * 0dB @ 2.2kHz
       * -6dB @ 1.2kHz (for pre-emphasis)
       * < -30dB from 3.6KHz
       */
      si_trx_modem_set_tx_datarate(1600); /* Filter sampling rate 1600*10 = 16kHz */
      si_trx_modem_tx_filter_coefficients(p_si_coeff);
      break;
    case SI_FILTER_RSID:
      /**
       * Modem tx filter coefficients for RSID
       * -6dB @ dc (for ±0.5 deviation max)
       * < -50dB from 1.8KHz
       */
      si_trx_modem_set_tx_datarate(1600); /* Filter sampling rate 1600*10 = 16kHz */
      si_trx_modem_tx_filter_coefficients(rsid_si_coeff);
      break;
    default:                      /* Just leave the defaults from startup in place */
      break;
  }

  /* Set modulation type */
  si_trx_modem_set_modulation(SI_MODEM_MOD_DIRECT_MODE_SYNC, // ASYNC
                              SI_MODEM_MOD_GPIO_1,
                              SI_MODEM_MOD_SOURCE_DIRECT,
                              modulation_type);

  si_trx_state_tx_tune();
}

/**
 * Enables the radio and starts transmitting
 */
void si_trx_on(uint8_t modulation_type, struct si_frequency_configuration* fconfig,
    uint16_t deviation, uint8_t power, enum si_filter_model filter)
{
  si_trx_reset(modulation_type, fconfig, deviation, power, filter);
  si_trx_start_tx(0);
}
/**
 * Disables the radio and places it in shutdown
 */
void si_trx_off(void)
{
  si_trx_state_ready();

  /* Physical shutdown */
  _si_trx_sdn_enable();
}

/**
 * Resets the radio
 */
void si_trx_shutdown(void)
{
  /* Configure the SDN pin */
  port_pin_set_config(SI4xxx_SDN_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */

  /* Put the transciever in shutdown */
  _si_trx_sdn_enable();
}
/**
 * Initialises the radio interface to the radio
 */
void si_trx_init(void)
{
  si_trx_shutdown();

  /* Configure the SPI select pin */
  port_pin_set_config(SI4xxx_SEL_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */

  /* Put the SEL pin in reset */
  _si_trx_cs_disable();

  /* Configure the GPIO pins */
  port_pin_set_config(SI4xxx_GPIO0_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */
  port_pin_set_output_level(SI4xxx_GPIO0_PIN, 0);
  port_pin_set_config(SI4xxx_GPIO1_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */
  port_pin_set_output_level(SI4xxx_GPIO1_PIN, 0);

  /* Configure the IRQ pin */
  // TODO

  /* Configure the serial port */
  spi_bitbang_init(SI4xxx_SERCOM_MOSI_PIN,
		   SI4xxx_SERCOM_MISO_PIN,
		   SI4xxx_SERCOM_SCK_PIN);
}




/**
 * Quick and dirty loopback test. Should print 0x34
 */
void spi_loopback_test(void)
{
  /* spi_init(SI_TRX_SERCOM, */
  /* 	   SPI_MODE_MASTER,		/\** SPI mode *\/ */
  /* 	   SPI_DATA_ORDER_MSB,		/\** Data order *\/ */
  /* 	   SPI_TRANSFER_MODE_0,		/\** Transfer mode *\/ */
  /* 	   SI4xxx_SERCOM_MUX,		/\** Mux setting *\/ */
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
  /* 	   SI4xxx_SERCOM_MOSI_PINMUX,	/\** Pinmux *\/ */
  /* 	   SI4xxx_SERCOM_MISO_PINMUX,	/\** Pinmux *\/ */
  /* 	   SI4xxx_SERCOM_SCK_PINMUX,	/\** Pinmux *\/ */
  /* 	   PINMUX_UNUSED);		/\** Pinmux *\/ */
  /* spi_enable(SI_TRX_SERCOM); */

  /* Init loopback */
  spi_bitbang_init(SI4xxx_SERCOM_MOSI_PIN,
		   SI4xxx_SERCOM_MOSI_PIN,
		   SI4xxx_SERCOM_SCK_PIN);

  /* Enable */

  /* Test transfer */
  uint8_t data = spi_bitbang_transfer(0x34);

  /* Print result */
  semihost_printf("Rx'ed: 0x%02x\n", data);
}
