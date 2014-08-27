/**
 * SAM D20/D21/R21 TC - Timer Counter Driver
 *
 * Copyright (C) 2013-2014 Atmel Corporation. All rights reserved.
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

#ifndef TC_H_INCLUDED
#define TC_H_INCLUDED

/**
 * SAM D20/D21/R21 Timer/Counter Driver (TC)
 *
 * Module Overview
 *
 * The Timer/Counter (TC) module provides a set of timing and counting related
 * functionality, such as the generation of periodic waveforms, the capturing
 * of a periodic waveform's frequency/duty cycle, and software timekeeping for
 * periodic operations. TC modules can be configured to use an 8-, 16-, or
 * 32-bit counter size.
 *
 * This TC module for the SAM D20/D21/R21 is capable of the following functions:
 *
 * - Generation of PWM signals
 * - Generation of timestamps for events
 * - General time counting
 * - Waveform period capture
 * - Waveform frequency capture
 *
 * Functional Description
 *
 * Independent of the configured counter size, each TC module can be set up
 * in one of two different modes; capture and compare.
 *
 * In capture mode, the counter value is stored when a configurable event
 * occurs. This mode can be used to generate timestamps used in event capture,
 * or it can be used for the measurement of a periodic input signal's
 * frequency/duty cycle.
 *
 * In compare mode, the counter value is compared against one or more of the
 * configured channel compare values. When the counter value coincides with a
 * compare value an action can be taken automatically by the module, such as
 * generating an output event or toggling a pin when used for frequency or PWM
 * signal generation.
 *
 * Note: The connection of events between modules requires the use of
 *       the SAM D20/D21/R21 Event System Driver (EVENTS) to route
 *       output event of one module to the the input event of another.
 *       For more information on event routing, refer to the event
 *       driver documentation.
 *
 * Timer/Counter Size
 * Each timer module can be configured in one of three different counter
 * sizes; 8-, 16-, and 32-bits. The size of the counter determines the maximum
 * value it can count to before an overflow occurs and the count is reset back
 * to zero.
 *
 * When using the counter in 16- or 32-bit count mode, Compare Capture
 * register 0 (CC0) is used to store the period value when running in PWM
 * generation match mode.
 *
 * When using 32-bit counter size, two 16-bit counters are chained together
 * in a cascade formation. Even numbered TC modules (e.g. TC0, TC2) can be
 * configured as 32-bit counters. The odd numbered counters will act as slaves
 * to the even numbered masters, and will not be reconfigurable until the
 * master timer is disabled.
 *
 * Clock Settings
 *
 * Clock Selection
 * Each TC peripheral is clocked asynchronously to the
 * system clock by a GCLK (Generic Clock) channel. The GCLK channel
 * connects to any of the GCLK generators. The GCLK generators are
 * configured to use one of the available clock sources on the system
 * such as internal oscillator, external crystals etc.
 *
 * Prescaler
 * Each TC module in the SAM D20/D21/R21 has its own
 * individual clock prescaler, which can be used to divide the input
 * clock frequency used in the counter. This prescaler only scales the
 * clock used to provide clock pulses for the counter to count, and
 * does not affect the digital register interface portion of the
 * module, thus the timer registers will synchronized to the raw GCLK
 * frequency input to the module.
 *
 * As a result of this, when selecting a GCLK frequency and timer prescaler
 * value the user application should consider both the timer resolution
 * required and the synchronization frequency, to avoid lengthy
 * synchronization times of the module if a very slow GCLK frequency is fed
 * into the TC module. It is preferable to use a higher module GCLK frequency
 * as the input to the timer and prescale this down as much as possible to
 * obtain a suitable counter frequency in latency-sensitive applications.
 *
 * Reloading
 * Timer modules also contain a configurable reload action, used when a
 * re-trigger event occurs. Examples of a re-trigger event are the counter
 * reaching the max value when counting up, or when an event from the event
 * system tells the counter to re-trigger. The reload action determines if the
 * prescaler should be reset, and when this should happen. The counter will
 * always be reloaded with the value it is set to start counting from.
 *
 * <table>
 *   <caption>TC module reload actions</caption>
 *   <tr>
 *     <th>Reload Action</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>\ref TC_RELOAD_ACTION_GCLK </td>
 *     <td>Reload TC counter value on next GCLK cycle. Leave prescaler
 *         as-is.</td>
 *   </tr>
 *   <tr>
 *     <td>\ref TC_RELOAD_ACTION_PRESC </td>
 *     <td>Reloads TC counter value on next prescaler clock. Leave prescaler
 *         as-is.</td>
 *   </tr>
 *  <tr>
 *    <td> \ref TC_RELOAD_ACTION_RESYNC </td>
 *    <td>Reload TC counter value on next GCLK cycle. Clear prescaler to
 *        zero.</td>
 *  </tr>
 * </table>
 *
 * The reload action to use will depend on the specific application being
 * implemented. One example is when an external trigger for a reload occurs; if
 * the TC uses the prescaler, the counter in the prescaler should not have a
 * value between zero and the division factor. The TC counter and the counter
 * in the prescaler should both start at zero. When the counter is set to
 * re-trigger when it reaches the max value on the other hand, this is not the
 * right option to use. In such a case it would be better if the prescaler is
 * left unaltered when the re-trigger happens, letting the counter reset on the
 * next GCLK cycle.
 *
 * Compare Match Operations
 * In compare match operation, Compare/Capture registers are used in comparison
 * with the counter value. When the timer's count value matches the value of a
 * compare channel, a user defined action can be taken.
 *
 * Basic Timer
 *
 * A Basic Timer is a simple application where compare match operations is used
 * to determine when a specific period has elapsed. In Basic Timer operations,
 * one or more values in the module's Compare/Capture registers are used to
 * specify the time (as a number of prescaled GCLK cycles) when an action should
 * be taken by the microcontroller. This can be an Interrupt Service Routine
 * (ISR), event generator via the event system, or a software flag that is
 * polled via the user application.
 *
 * Waveform Generation
 *
 * Waveform generation enables the TC module to generate square waves, or if
 * combined with an external passive low-pass filter, analog waveforms.
 *
 * Waveform Generation - PWM
 *
 * Pulse width modulation is a form of waveform generation and a signalling
 * technique that can be useful in many situations. When PWM mode is used,
 * a digital pulse train with a configurable frequency and duty cycle can be
 * generated by the TC module and output to a GPIO pin of the device.
 *
 * Often PWM is used to communicate a control or information parameter to an
 * external circuit or component. Differing impedances of the source generator
 * and sink receiver circuits is less of an issue when using PWM compared to
 * using an analog voltage value, as noise will not generally affect the
 * signal's integrity to a meaningful extent.
 *
 * Frequency Generation mode is in many ways identical to PWM
 * generation. However, in Frequency Generation a toggle only occurs
 * on the output when a match on a capture channels occurs. When the
 * match is made, the timer value is reset, resulting in a variable
 * frequency square wave with a fixed 50% duty cycle.
 *
 * Capture Operations
 *
 * In capture operations, any event from the event system or a pin change can
 * trigger a capture of the counter value. This captured counter value can be
 * used as a timestamp for the event, or it can be used in frequency and pulse
 * width capture.
 *
 * Capture Operations - Event
 *
 * Event capture is a simple use of the capture functionality,
 * designed to create timestamps for specific events. When the TC
 * module's input capture pin is externally toggled, the current timer
 * count value is copied into a buffered register which can then be
 * read out by the user application.
 *
 * Note that when performing any capture operation, there is a risk that the
 * counter reaches its top value (MAX) when counting up, or the bottom value
 * (zero) when counting down, before the capture event occurs. This can distort
 * the result, making event timestamps to appear shorter than reality; the
 * user application should check for timer overflow when reading a capture
 * result in order to detect this situation and perform an appropriate
 * adjustment.
 *
 * Before checking for a new capture, TC_STATUS_COUNT_OVERFLOW should
 * be checked. The response to an overflow error is left to the user
 * application, however it may be necessary to clear both the capture
 * overflow flag and the capture flag upon each capture reading.
 *
 * Capture Operations - Pulse Width
 *
 * Pulse Width Capture mode makes it possible to measure the pulse width and
 * period of PWM signals. This mode uses two capture channels of the counter.
 * This means that the counter module used for Pulse Width Capture can not be
 * used for any other purpose. There are two modes for pulse width capture;
 * Pulse Width Period (PWP) and Period Pulse Width (PPW). In PWP mode, capture
 * channel 0 is used for storing the pulse width and capture channel 1 stores
 * the observed period. While in PPW mode, the roles of the two capture channels
 * is reversed.
 *
 * As in the above example it is necessary to poll on interrupt flags to see
 * if a new capture has happened and check that a capture overflow error has
 * not occurred.
 *
 * One-shot Mode
 *
 * TC modules can be configured into a one-shot mode. When configured in this
 * manner, starting the timer will cause it to count until the next overflow
 * or underflow condition before automatically halting, waiting to be manually
 * triggered by the user application software or an event signal from the event
 * system.
 *
 * Wave Generation Output Inversion
 *
 * The output of the wave generation can be inverted by hardware if desired,
 * resulting in the logically inverted value being output to the configured
 * device GPIO pin.
 *
 *
 * Special Considerations
 *
 * The number of capture compare registers in each TC module is dependent on
 * the specific SAM D20/D21/R21 device being used, and in some cases the counter size.
 *
 * The maximum amount of capture compare registers available in any SAM D20/D21/R21
 * device is two when running in 32-bit mode and four in 8-, and 16-bit modes.
 */

#include "samd20.h"
#include "tc.h"
#include <stdbool.h>

/* Same number for 8-, 16- and 32-bit TC and all TC instances */
#define NUMBER_OF_COMPARE_CAPTURE_CHANNELS TC0_CC8_NUM

/**
 * TC status type
 */
enum tc_status_t {
  TC_STATUS_OK = 0,
  TC_STATUS_BUSY,
  TC_STATUS_DENIED
};

/**
 * TC error type
 */
enum tc_error_t {
  TC_ERROR_INVALID_ARG = 1,
  TC_ERROR_INVALID_STATE,
  TC_ERROR_INVALID_DEVICE,
  TC_ERROR_NO_32BIT_SLAVE_EXISTS
};

/**
 * Module status flags
 */

/** Timer channel 0 has matched against its compare value, or has captured a
 *  new value.
 */
#define TC_STATUS_CHANNEL_0_MATCH    (1UL << 0)

/** Timer channel 1 has matched against its compare value, or has captured a
 *  new value.
 */
#define TC_STATUS_CHANNEL_1_MATCH    (1UL << 1)

/** Timer register synchronization has completed, and the synchronized count
 *  value may be read.
 */
#define TC_STATUS_SYNC_READY         (1UL << 2)

/** A new value was captured before the previous value was read, resulting in
 *  lost data.
 */
#define TC_STATUS_CAPTURE_OVERFLOW   (1UL << 3)

/** The timer count value has overflowed from its maximum value to its minimum
 *  when counting upward, or from its minimum value to its maximum when
 *  counting downward.
 */
#define TC_STATUS_COUNT_OVERFLOW     (1UL << 4)

/**
 * Index of the compare capture channels
 *
 * This enum is used to specify which capture/compare channel to do
 * operations on.
 */
enum tc_compare_capture_channel {
  /** Index of compare capture channel 0 */
  TC_COMPARE_CAPTURE_CHANNEL_0,
  /** Index of compare capture channel 1 */
  TC_COMPARE_CAPTURE_CHANNEL_1,
};

/**
 * TC wave generation mode enum
 *
 * This enum is used to select which mode to run the wave
 * generation in.
 */
enum tc_wave_generation {
  /** Top is max, except in 8-bit counter size where it is the PER
   * register
   */
  TC_WAVE_GENERATION_NORMAL_FREQ      = TC_CTRLA_WAVEGEN_NFRQ,

  /** Top is CC0, except in 8-bit counter size where it is the PER
   * register
   */
  TC_WAVE_GENERATION_MATCH_FREQ       = TC_CTRLA_WAVEGEN_MFRQ,

  /** Top is max, except in 8-bit counter size where it is the PER
   * register
   */
  TC_WAVE_GENERATION_NORMAL_PWM       = TC_CTRLA_WAVEGEN_NPWM,

  /** Top is CC0, except in 8-bit counter size where it is the PER
   * register
   */
  TC_WAVE_GENERATION_MATCH_PWM        = TC_CTRLA_WAVEGEN_MPWM,
};

/**
 * Specifies if the counter is 8-, 16-, or 32-bits.
 *
 * This enum specifies the maximum value it is possible to count to.
 */
enum tc_counter_size {
  /** The counter's max value is 0xFF, the period register is
   * available to be used as top value.
   */
  TC_COUNTER_SIZE_8BIT                = TC_CTRLA_MODE_COUNT8,

  /** The counter's max value is 0xFFFF. There is no separate
   * period register, to modify top one of the capture compare
   * registers has to be used. This limits the amount of
   * available channels.
   */
  TC_COUNTER_SIZE_16BIT               = TC_CTRLA_MODE_COUNT16,

  /** The counter's max value is 0xFFFFFFFF. There is no separate
   * period register, to modify top one of the capture compare
   * registers has to be used. This limits the amount of
   * available channels.
   */
  TC_COUNTER_SIZE_32BIT               = TC_CTRLA_MODE_COUNT32,
};

/**
 * TC Counter reload action enum
 *
 * This enum specify how the counter and prescaler should reload.
 */
enum tc_reload_action {
  /** The counter is reloaded/reset on the next GCLK and starts
   * counting on the prescaler clock.
   */
  TC_RELOAD_ACTION_GCLK               = TC_CTRLA_PRESCSYNC_GCLK,

  /** The counter is reloaded/reset on the next prescaler clock
   */
  TC_RELOAD_ACTION_PRESC              = TC_CTRLA_PRESCSYNC_PRESC,

  /** The counter is reloaded/reset on the next GCLK, and the
   * prescaler is restarted as well.
   */
  TC_RELOAD_ACTION_RESYNC             = TC_CTRLA_PRESCSYNC_RESYNC,
};

/**
 * TC clock prescaler values
 */
enum tc_clock_prescaler {
  /** Divide clock by 1 */
  TC_CLOCK_PRESCALER_DIV1             = TC_CTRLA_PRESCALER(0),
  /** Divide clock by 2 */
  TC_CLOCK_PRESCALER_DIV2             = TC_CTRLA_PRESCALER(1),
  /** Divide clock by 4 */
  TC_CLOCK_PRESCALER_DIV4             = TC_CTRLA_PRESCALER(2),
  /** Divide clock by 8 */
  TC_CLOCK_PRESCALER_DIV8             = TC_CTRLA_PRESCALER(3),
  /** Divide clock by 16 */
  TC_CLOCK_PRESCALER_DIV16            = TC_CTRLA_PRESCALER(4),
  /** Divide clock by 64 */
  TC_CLOCK_PRESCALER_DIV64            = TC_CTRLA_PRESCALER(5),
  /** Divide clock by 256 */
  TC_CLOCK_PRESCALER_DIV256           = TC_CTRLA_PRESCALER(6),
  /** Divide clock by 1024 */
  TC_CLOCK_PRESCALER_DIV1024          = TC_CTRLA_PRESCALER(7),
};

/**
 * TC module count direction.
 */
enum tc_count_direction {
  /** Timer should count upward from zero to MAX. */
  TC_COUNT_DIRECTION_UP,

  /** Timer should count downward to zero from MAX. */
  TC_COUNT_DIRECTION_DOWN,
};

/**
 * Waveform inversion mode.
 */
enum tc_waveform_invert_output {
  /** No inversion of the waveform output. */
  TC_WAVEFORM_INVERT_OUTPUT_NONE      = 0,
  /** Invert output from compare channel 0. */
  TC_WAVEFORM_INVERT_OUTPUT_CHANNEL_0 = TC_CTRLC_INVEN(1),
  /** Invert output from compare channel 1. */
  TC_WAVEFORM_INVERT_OUTPUT_CHANNEL_1 = TC_CTRLC_INVEN(2),
};

/**
 * Action to perform when the TC module is triggered by an event.
 */
enum tc_event_action {
  /** No event action. */
  TC_EVENT_ACTION_OFF                 = TC_EVCTRL_EVACT_OFF,
  /** Re-trigger on event. */
  TC_EVENT_ACTION_RETRIGGER           = TC_EVCTRL_EVACT_RETRIGGER,
  /** Increment counter on event. */
  TC_EVENT_ACTION_INCREMENT_COUNTER   = TC_EVCTRL_EVACT_COUNT,
  /** Start counter on event. */
  TC_EVENT_ACTION_START               = TC_EVCTRL_EVACT_START,

  /** Store period in capture register 0, pulse width in capture
   *  register 1.
   */
  TC_EVENT_ACTION_PPW                 = TC_EVCTRL_EVACT_PPW,

  /** Store pulse width in capture register 0, period in capture
   *  register 1.
   */
  TC_EVENT_ACTION_PWP                 = TC_EVCTRL_EVACT_PWP,
};

/**
 * TC event enable/disable structure.
 */
struct tc_events {
  /** Generate an output event on a compare channel match. */
  bool generate_event_on_compare_channel
  [NUMBER_OF_COMPARE_CAPTURE_CHANNELS];
  /** Generate an output event on counter overflow. */
  bool generate_event_on_overflow;
  /** Perform the configured event action when an incoming event is signalled. */
  bool on_event_perform_action;
  /** Specifies if the input event source is inverted, when used in PWP or
   *  PPW event action modes.
   */
  bool invert_event_input;
  /** Specifies which event to trigger if an event is triggered. */
  enum tc_event_action event_action;
};


/**
 * \brief Configuration struct for TC module in 32-bit size counter mode.
 */
struct tc_pwm_channel {
  /** When \c true, PWM output for the given channel is enabled. */
  bool enabled;
  /** Specifies pin output for each channel. */
  uint32_t pin_out;
  /** Specifies MUX setting for each output channel pin. */
  uint32_t pin_mux;
};


void tc_enable(Tc* const hw);
void tc_disable(Tc* const hw);

void tc_start_counter(Tc* const hw);
void tc_stop_counter(Tc* const hw);

uint32_t tc_get_status(Tc* const hw);
void tc_clear_status(Tc* const hw,
		     const uint32_t status_flags);

void tc_set_count_value(Tc* const hw, const uint32_t count);
uint32_t tc_get_count_value(Tc* const hw);

uint32_t tc_get_capture_value(Tc* const hw,
			      const enum tc_compare_capture_channel channel_index);
void tc_set_compare_value(Tc* const hw,
			  const enum tc_compare_capture_channel channel_index,
			  const uint32_t compare);

const Tc* tc_get_slave_instance(Tc* const hw);
void tc_reset(Tc* const hw);
void tc_set_top_value (Tc* const hw,
		       const uint32_t top_value);

void tc_enable_events(Tc* const hw,
				    struct tc_events *const events);
void tc_disable_events(Tc* const hw,
				     struct tc_events *const events);

enum tc_status_t tc_init(Tc* const hw,
			 enum gclk_generator source_clock,
			 enum tc_counter_size counter_size, /* 8, 16 or 32bit */
			 enum tc_clock_prescaler clock_prescaler,
			 enum tc_wave_generation wave_generation,
			 enum tc_reload_action reload_action,
			 enum tc_count_direction count_direction,
			 uint8_t waveform_invert_output,
			 bool oneshot,
			 bool run_in_standby,
			 uint32_t value,
			 uint32_t top_value,
			 bool* enable_capture_channels,
			 uint32_t* compare_channel_values);

#endif /* TC_H_INCLUDED */
