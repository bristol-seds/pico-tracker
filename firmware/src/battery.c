/*
 * Functions related to the battery
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
#include "hw_config.h"
#include "system/port.h"
#include "battery.h"
#include "data.h"

/* We wait for n cycles of continous power before charging */
uint32_t excess_count = 0;
#define EXCESS_COUNT_MAX	(60)

/* Internal state */
enum battery_use_state battery_use_state;
enum battery_charge_state battery_charge_state;

/**
 * Battery cold signals 'do-not-use'
 * so we can be charging, but battery shouldn't be used
 *
 * Only want to charge when the bus voltage is higher than 2.6V too.
 * We don't want to charge battery with li-ion cap
 */

/**
 * use state
 */
void update_use_state(struct tracker_datapoint *dp)
{
#ifdef RECHARGABLE_MIN_V
  if (dp->battery < RECHARGABLE_MIN_V) { /* voltage too low? */
    battery_use_state = BATTERY_VOLTAGE_TOO_LOW;
    return;
  }
#endif
  battery_use_state = BATTERY_GOOD;
}

/**
 * charge state
 * also sets external charge pin if used
 */
void update_charge_state(struct tracker_datapoint *dp)
{
#if RECHARGABLE_BATTERY
  if (dp->solar > dp->battery) { /* power coming in */
    /* excess / charging */
    if (excess_count >= EXCESS_COUNT_MAX) { /* charge */
      battery_charge_state = BATTERY_CHARGING;
    } else {                    /* excess */
      battery_charge_state = BATTERY_EXCESS;
      excess_count++;           /* count until we can charge */
    }
  } else {                      /* no power coming in */
    /* discharging */
    battery_charge_state = BATTERY_DISCHARGING;
    excess_count = 0;
  }

#else  /* non-rechargable */
  battery_charge_state = BATTERY_DISCHARGING;
#endif

#ifdef CHG_ENABLE_PIN           /* Set external pin */
  if (battery_charge_state == BATTERY_CHARGING) {
    port_pin_set_output_level(CHG_ENABLE_PIN, 1);	/* pin is active high */
  } else {
    port_pin_set_output_level(CHG_ENABLE_PIN, 0);	/* pin is active high */
  }
#endif
}

/**
 * Returns use state
 */
enum battery_use_state get_battery_use_state(void)
{
  return battery_use_state;
}
/**
 * Returns charge state
 */
enum battery_charge_state get_battery_charge_state(void)
{
  return battery_charge_state;
}

/**
 * Updates internal battery states.
 * Also sets external charge pin if used
 */
void update_battery(struct tracker_datapoint *dp)
{
  update_use_state(dp);
  update_charge_state(dp);
}

/**
 * Setup battery control pins
 */
void battery_init(void)
{
#ifdef CHG_ENABLE_PIN
  port_pin_set_config(CHG_ENABLE_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */
  port_pin_set_output_level(CHG_ENABLE_PIN, 0);	/* pin is active high */
#endif
}
