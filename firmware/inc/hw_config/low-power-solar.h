/*
 * Hardware definitions and configuations
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

#ifndef HW_CONFIG_H
#define HW_CONFIG_H

#include "port.h"
#include "samd20.h"
#include "adc/adc.h"

#define V0986
#define V0987

/*
 * Convenience definitions for available GPIO modules
 */
#if (PORT_GROUPS > 0)
#define PORTA		PORT->Group[0]
#endif
#if (PORT_GROUPS > 1)
#define PORTB		PORT->Group[1]
#endif
#if (PORT_GROUPS > 2)
#define PORTC		PORT->Group[2]
#endif
#if (PORT_GROUPS > 3)
#define PORTD		PORT->Group[3]
#endif


//#define XPLAINED

/**
 * LED
 */
#ifdef XPLAINED
#define LED0_PIN		PIN_PA14
#else
#ifdef V0986
#define LED0_PIN		PIN_PA15
#else
#define LED0_PIN		PIN_PA16
#endif
#endif

/**
 * GPS
 */
#define GPS_SERCOM		(SercomUsart*)SERCOM0
#define GPS_SERCOM_MOGI_PIN	PIN_PA06
#define GPS_SERCOM_MOGI_PINMUX	PINMUX_PA06D_SERCOM0_PAD2
#define GPS_SERCOM_MIGO_PIN	PIN_PA07
#define GPS_SERCOM_MIGO_PINMUX	PINMUX_PA07D_SERCOM0_PAD3
#define GPS_SERCOM_MUX		USART_RX_3_TX_2_XCK_3
#define GPS_GCLK		GCLK_GENERATOR_0

#define GPS_TYPE_OSP
#define GPS_BAUD_RATE		115200
#define GPS_SE_ON_OFF_PIN	PIN_PA05
#define GPS_RESET_PIN		PIN_PA01

#define GPS_FLIGHT_STATE_THREASHOLD_M	8000 /* 8km altitude */


/**
 * USART Loopback Testing
 */
#define USART_MUX_LOOPBACK	USART_RX_2_TX_2_XCK_3

/**
 * Flash Memory
 */
#define FLASH_SERCOM		(SercomSpi*)SERCOM2
#define FLASH_SERCOM_MOSI_PIN	PIN_PA08
#define FLASH_SERCOM_MOSI_PINMUX PINMUX_PA08D_SERCOM2_PAD0
#define FLASH_SERCOM_MISO_PIN	PIN_PA11
#define FLASH_SERCOM_MISO_PINMUX PINMUX_PA11D_SERCOM2_PAD3
#define FLASH_SERCOM_SCK_PIN	PIN_PA09
#define FLASH_SERCOM_SCK_PINMUX PINMUX_PA09D_SERCOM2_PAD1
#define FLASH_SERCOM_MUX	SPI_SIGNAL_MUX_SETTING_D
#define FLASH_CSN_PIN		PIN_PA10

/**
 * I2C Bus - NOT USED
 */

/**
 * Barometer
 */
#define USE_BAROMETER		0
#define BAROMETER_TYPE_BMP180	0
#define BAROMETER_TYPE_MS5607	1
#define BAROMETER_TYPE_MS5611	0

/**
 * Battery ADC
 */
#define BATTERY_ADC		1
#define BATTERY_ADC_PIN		PIN_PA04
#define BATTERY_ADC_PINMUX	PINMUX_PA04B_ADC_AIN4
#define BATTERY_ADC_CHANNEL	ADC_POSITIVE_INPUT_PIN4
#define BATTERY_ADC_CHANNEL_DIV	0.23256 /* 33k,10k divider */
#define BATTERY_ADC_REFERENCE	ADC_REFERENCE_INT1V

/**
 * External Thermistor ADC
 */
#define THERMISTOR_ADC		1
#define THERMISTOR_ADC_PIN	PIN_PA02
#define THERMISTOR_ADC_PINMUX	PINMUX_PA02B_ADC_AIN0
#define THERMISTOR_ADC_CHANNEL	ADC_POSITIVE_INPUT_PIN0
#define THERMISTOR_ADC_CHANNEL_DIV 1.48
#define THERMISTOR_ADC_REFERENCE ADC_REFERENCE_INTVCC0 /* internal 1/1.48 VCC reference */

/**
 * Solar ADC
 */
#define SOLAR_ADC		1
#define SOLAR_ADC_PIN		PIN_PA03
#define SOLAR_ADC_PINMUX	PINMUX_PA03B_ADC_AIN1
#define SOLAR_ADC_CHANNEL	ADC_POSITIVE_INPUT_PIN1
#define SOLAR_ADC_CHANNEL_DIV	0.23256 /* 33k,10k divider */
#define SOLAR_ADC_REFERENCE	ADC_REFERENCE_INT1V

/**
 * Battery
 */
#define RECHARGABLE_BATTERY	1
#define CHG_ENABLE_PIN		PIN_PA27
#define RECHARGABLE_MIN_V	(2.6)

/**
 * Cold out
 */
#define COLD_OUT_TEMPERATURE	(-58.0)
#define COLD_OUT_SECONDS	(15*60) /* 15 minutes */
#define COLD_OUT_COUNT_MAX	(16*4)  /* up to 16 hours*/

/**
 * Cycle Times
 */
#define CYCLE_TIME_FAST		(60)
#define CYCLE_TIME_SLOW		(120)

/**
 * Radio
 */
#define SI4xxx_SERCOM		(SercomSpi*)SERCOM3
#define SI4xxx_SERCOM_MOSI_PIN	PIN_PA19
#define SI4xxx_SERCOM_MOSI_PINMUX PINMUX_PA19D_SERCOM3_PAD3
#define SI4xxx_SERCOM_MISO_PIN	PIN_PA22
#define SI4xxx_SERCOM_MISO_PINMUX PINMUX_PA22C_SERCOM3_PAD0
#define SI4xxx_SERCOM_SCK_PIN	PIN_PA23
#define SI4xxx_SERCOM_SCK_PINMUX	PINMUX_PA23C_SERCOM3_PAD1
#define SI4xxx_SERCOM_MUX	SPI_SIGNAL_MUX_SETTING_M
#define SI4xxx_SEL_PIN		PIN_PA18
#define SI4xxx_IRQ_PIN		PIN_PA24
#define SI4xxx_IRQ_PINMUX	PINMUX_PA24A_EIC_EXTINT12
#define SI4xxx_SDN_PIN		PIN_PA17
#define SI4xxx_GPIO1_PIN	PIN_PA25
#define SI4xxx_GPIO1_PINMUX	PINMUX_PA25F_TC5_WO1
#define SI4xxx_TCXO_FREQUENCY	16369000
#define SI4xxx_TCXO_REG_EN_PIN	PIN_PA16

/**
 * RF Power @ 1.8V
 *
 * These values are taken from the datasheet, but the
 * difference between them has been measured as 6dB
 */
#define RF_POWER_14dBm		0x7f
#define RF_POWER_8dBm		0x7f /* tested in lab (36) */


/**
 * RF Enable
 *
 * Disable this when there's no antenna connected
 */
#define RF_TX_ENABLE		1

/**
 * LF Clock
 */
#define USE_LFTIMER			1

/**
 * HF Clock
 */
#define USE_XOSC				1
#define XOSC_TCXO_SHUTDOWN_EN	0
#define XOSC_FREQUENCY			16369000
#define XOSC_GCLK_DIVIDE		2         /* ~8MHz on GCLK */
#define OSC8M_GCLK_DIVIDE		1         /* ~8MHz on GCLK */

/**
 * Telemetry
 */
#define TELEMETRY_ENABLE		1
#define TELEMETRY_USE_GEOFENCE	1
#define TELEMETRY_FREQUENCY		434600000
#define TELEMETRY_POWER			RF_POWER_8dBm

/**
 * APRS
 */
#define APRS_ENABLE			1
#define APRS_USE_GEOFENCE	1
#define APRS_POWER			RF_POWER_14dBm

/**
 * ARISS APRS
 * Geofence used is only "no aprs"
 */
#define ARISS_ENABLE		1
#define ARISS_USE_PREDICT	0
#define ARISS_USE_GEOFENCE	0
#define ARISS_FREQUENCY		145825000

/**
 * Prefix
 */
#define GEOFENCE_USE_PREFIX	1

/**
 * Watchdog Timer
 *
 * There is a performance penalty to using the watchdog - you must
 * wait up to 1/16kHz when kicking to sync with it. Disable when not in use
 */
//#define DEBUG_USE_INTWATCHDOG	0
#define WDT_GCLK		GCLK_GENERATOR_4

/**
 * External Watchdog Timer
 */
#define WDT_WDI_PIN		PIN_PA28

/**
 * SWD
 */
#define SWD_SWDCLK_PIN		PIN_PA30
#define SWD_SWDCLK_PINMUX	PINMUX_PA30G_CORE_SWCLK
#define SWD_SWDIO_PIN		PIN_PA31


/**
 * Interrupt Priority
 */
#define WDT_INT_PRIO		0 /* Early warning interrupt */

#define TC0_INT_PRIO		1 /* Telemetry Tick Timer */

#define GPS_SERCOM_INT_PRIO	2 /* GPS USART Rx */

#define EIC_INT_PRIO		3 /* Timepulse, telemetry */

#define TC2_INT_PRIO		3 /* XOSC Measure Timer */

#define TC4_INT_PRIO		3 /* LF timer */
#define RTC_INT_PRIO		3 /* RTC */

#define ADC_INT_PRIO		3 /* ADC */

#endif /* HW_CONFIG_H */
