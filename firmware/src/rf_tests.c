/*
 * Radio testing functions. NOT FOR FLIGHT
 * Copyright (C) 2015  Richard Meadows <richardeoin>
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
#include "hw_config.h"
#include "rf_tests.h"
#include "si_trx.h"
#include "si_trx_defs.h"
#include "telemetry.h"
#include "aprs.h"
#include "ax25.h"
#include "watchdog.h"

#define APRS_TEST_LAT 51.47
#define APRS_TEST_LON -2.58
#define APRS_TEST_ALTITUDE	43
#define APRS_TEST_FREQUENCY	145825000
#define APRS_TEST_RF_PATH	SI_RF_PATH_BYPASS

void telemetry_tone(void)
{
  struct si_frequency_configuration fconfig1;
  struct si_frequency_configuration fconfig2;
  si_trx_get_frequency_configuration(&fconfig1, TELEMETRY_FREQUENCY);
  si_trx_get_frequency_configuration(&fconfig2, TELEMETRY_FREQUENCY+1000);

  while (1) {
    si_trx_on(SI_MODEM_MOD_TYPE_CW, &fconfig1, 0, 0x7f, SI_FILTER_DEFAULT, SI_RF_PATH_BYPASS);
    for (int i = 0; i < 50*1000; i++) {
      //idle(IDLE_TELEMETRY_ACTIVE);
    }
    si_trx_off();
    si_trx_on(SI_MODEM_MOD_TYPE_CW, &fconfig2, 0, 0x7f, SI_FILTER_DEFAULT, SI_RF_PATH_BYPASS);
    for (int i = 0; i < 50*1000; i++) {
      //idle(IDLE_TELEMETRY_ACTIVE);
    }
    si_trx_off();
  }
}
void aprs_tone(void)
{
  struct si_frequency_configuration fconfig;
  si_trx_get_frequency_configuration(&fconfig, APRS_TEST_FREQUENCY);

  while (1) {
    si_trx_on(SI_MODEM_MOD_TYPE_CW, &fconfig, 0, 0x7f, SI_FILTER_DEFAULT, APRS_TEST_RF_PATH);

    for (int i = 0; i < 50*1000; i++) {
      idle(IDLE_TELEMETRY_ACTIVE);
    }
    si_trx_off();
  }
}

/**
 * Transmits the high aprs tone (2200Hz)
 */
void aprs_high_fm_tone(void)
{
  struct si_frequency_configuration fconfig;
  si_trx_get_frequency_configuration(&fconfig, APRS_TEST_FREQUENCY);

  ax25_gpio1_pwm_init();

  /* Space */
  system_gclk_gen_set_config(GCLK_GENERATOR_7,
                             GCLK_SOURCE_GCLKGEN1,	/* Source	*/
                             false,		/* High When Disabled	*/
                             AX25_DIVISION_SPACE, /* Division Factor	*/
                             false,		/* Run in standby	*/
                             false);		/* Output Pin Enable	*/

  si_trx_on(SI_MODEM_MOD_TYPE_2GFSK, &fconfig,
            AX25_DEVIATION, APRS_POWER, SI_FILTER_APRS, APRS_TEST_RF_PATH);
  while (1) {
    idle(IDLE_TELEMETRY_ACTIVE);
  }
}

/**
 * APRS function for use during testing. Not for flight
 */
void aprs_test(void)
{
  struct tracker_datapoint dp;
  dp.latitude = APRS_TEST_LAT *10*1000*1000;
  dp.longitude = APRS_TEST_LON *10*1000*1000;
  dp.altitude = APRS_TEST_ALTITUDE *1000;

  while(1) {
    /* Set location */
    aprs_set_datapoint(&dp);

    /* Set frequency */
    telemetry_aprs_set_frequency(APRS_TEST_FREQUENCY);

    /* Transmit packet and wait */
    telemetry_start(TELEMETRY_APRS, 0xFFFF);
    while (telemetry_active()) {

      system_sleep();
      //idle(IDLE_TELEMETRY_ACTIVE);

    }
  }
}

/**
 * RSID function for use during testing. Not for flight
 */
void rsid_test(void)
{
  while (1) {

   telemetry_start_rsid(RSID_CONTESTIA_32_1000);

   // Sleep wait for RSID
   while (telemetry_active()) {
     idle(IDLE_TELEMETRY_ACTIVE);
   }

    for (int i = 3*200*1000; i; i--);
  }
}

void rf_tests(void)
{
  switch(RF_TEST) {
    case RF_TEST_APRS:
      aprs_test();
      break;
    case RF_TEST_APRS_HIGH_FM_TONE:
      aprs_high_fm_tone();
      break;
    case RF_TEST_APRS_TONE:
      aprs_tone();
      break;
   case RF_TEST_TELEMETRY_TONE:
      telemetry_tone();
      break;
    case RF_TEST_RSID:
      rsid_test();
      break;
    default:
      break;
  }
}
