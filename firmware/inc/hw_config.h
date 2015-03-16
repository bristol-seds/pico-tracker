/*
 * Hardware definitions and configuations
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

#ifndef HW_CONFIG_H
#define HW_CONFIG_H

#include "port.h"
#include "samd20.h"

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
#define LED0_PIN		PIN_PA15
#endif

/**
 * GPS
 */
#define GPS_SERCOM		(SercomUsart*)SERCOM1
#define GPS_SERCOM_MOGI_PIN	PIN_PA00
#define GPS_SERCOM_MOGI_PINMUX	PINMUX_PA00D_SERCOM1_PAD0
#define GPS_SERCOM_MIGO_PIN	PIN_PA01
#define GPS_SERCOM_MIGO_PINMUX	PINMUX_PA01D_SERCOM1_PAD1
#define GPS_SERCOM_MUX		USART_RX_1_TX_0_XCK_1
#define GPS_PLATFORM_MODEL	UBX_PLATFORM_MODEL_AIRBORNE_1G
#define GPS_TIMEPULSE_PIN	PIN_PA05
#define GPS_TIMEPULSE_PINMUX	PINMUX_PA05A_EIC_EXTINT5
#define GPS_TIMEPULSE_FREQ      2
#define GPS_TIMEPULSE_EXTINT	5

/**
 * USART Loopback Testing
 */
#define USART_MUX_LOOPBACK	USART_RX_0_TX_0_XCK_1

/**
 * Flash Memory
 */
#define FLASH_SERCOM		(SercomSpi*)SERCOM0
#define FLASH_SERCOM_MOSI_PIN	PIN_PA06
#define FLASH_SERCOM_MOSI_PINMUX PINMUX_PA06D_SERCOM0_PAD2
#define FLASH_SERCOM_MISO_PIN	PIN_PA09
#define FLASH_SERCOM_MISO_PINMUX PINMUX_PA09C_SERCOM0_PAD1
#define FLASH_SERCOM_SCK_PIN	PIN_PA07
#define FLASH_SERCOM_SCK_PINMUX PINMUX_PA07D_SERCOM0_PAD3
#define FLASH_CSN_PIN		PIN_PA10
#define FLASH_WPN_PIN		PIN_PA08

/**
 * Battery ADC
 */
#define BATTERY_ADC_PIN		PIN_PA11
#define BATTERY_ADC_PINMUX	PINMUX_PA11B_ADC_AIN19

/**
 * Radio
 */
#define SI406X_SERCOM		(SercomSpi*)SERCOM3
#define SI406X_SERCOM_MOSI_PIN	PIN_PA19
#define SI406X_SERCOM_MOSI_PINMUX PINMUX_PA19D_SERCOM3_PAD3
#define SI406X_SERCOM_MISO_PIN	PIN_PA22
#define SI406X_SERCOM_MISO_PINMUX PINMUX_PA22C_SERCOM3_PAD0
#define SI406X_SERCOM_SCK_PIN	PIN_PA23
#define SI406X_SERCOM_SCK_PINMUX	PINMUX_PA23C_SERCOM3_PAD1
#define SI406X_SERCOM_MUX	SPI_SIGNAL_MUX_SETTING_D
#define SI406X_SEL_PIN		PIN_PA18
#define SI406X_IRQ_PIN		PIN_PA24
#define SI406X_IRQ_PINMUX	PINMUX_PA24A_EIC_EXTINT12
#define SI406X_SDN_PIN		PIN_PA16
#define SI406X_GPIO0_PIN	PIN_PA27
#define SI406X_GPIO1_PIN	PIN_PA25
#define SI406X_GPIO1_PINMUX	PINMUX_PA25F_TC5_WO1
#define SI406X_TCXO_FREQUENCY	16369000

/**
 * XOSC
 */
#define XOSC_FREQUENCY		16369000
#define XOSC_GCLK1_DIVIDE	4

/**
 * Timings
 */
#define TELEMETRY_INTERVAL	30

/**
 * Watchdog Timer
 */
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
 * NC - Pins not connected externally
 */
#define NC1_PIN			PIN_PA02
#define NC2_PIN			PIN_PA03
#define NC3_PIN			PIN_PA04
#define NC4_PIN			PIN_PA14

/**
 * Interrupt Priority
 */
/* Telemetry Tick Timer */
#define TC0_INT_PRIO		0
/* XOSC Measure Timer */
#define TC2_INT_PRIO		2
/* GPS USART Rx */
#define GPS_SERCOM_INT_PRIO	0
/* Timepulse, telemetry */
#define EIC_INT_PRIO		0


#endif /* HW_CONFIG_H */
