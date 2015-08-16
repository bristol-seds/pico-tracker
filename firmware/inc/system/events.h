/**
 * SAM D20/D21/R21 Event System Driver
 *
 * Copyright (C) 2012-2014 Atmel Corporation. All rights reserved.
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
#ifndef EVENTS_H_INCLUDED
#define EVENTS_H_INCLUDED

#include "samd20.h"
#include "system/system.h"

/**
 * Event System Driver (EVENTS)
 *
 * This driver for SAM D20/D21/R21 devices provides an interface for the configuration
 * and management of the device's peripheral event resources and users within
 * the device, including enabling and disabling of peripheral source selection
 * and synchronization of clock domains between various modules. The following API
 * modes is covered by this manual:
 *  - Polled API
 *
 * The following peripherals are used by this module:
 *
 * - EVSYS (Event System Management)
 *
 * Module Overview
 *
 * Peripherals within the SAM D20/D21/R21 devices are capable of generating two types of
 * actions in response to given stimulus: set a register flag for later
 * intervention by the CPU (using interrupt or polling methods), or generate
 * event signals which can be internally routed directly to other
 * peripherals within the device. The use of events allows for direct actions
 * to be performed in one peripheral in response to a stimulus in another
 * without CPU intervention. This can lower the overall power consumption of the
 * system if the CPU is able to remain in sleep modes for longer periods (SleepWalking&trade;), and
 * lowers the latency of the system response.
 *
 * The event system is comprised of a number of freely configurable Event
 * resources, plus a number of fixed Event Users. Each Event resource can be
 * configured to select the input peripheral that will generate the events
 * signal, as well as the synchronization path and edge detection mode.
 * The fixed-function Event Users, connected to peripherals within the device,
 * can then subscribe to an Event resource in a one-to-many relationship in order
 * to receive events as they are generated. An overview of the event system
 * chain is shown in
 *
 * There are many different events that can be routed in the device, which can
 * then trigger many different actions. For example, an Analog Comparator module
 * could be configured to generate an event when the input signal rises above
 * the compare threshold, which then triggers a Timer Counter module to capture
 * the current count value for later use.
 *
 * Event Channels
 *
 * The Event module in each device consists of several channels, which can be
 * freely linked to an event generator (i.e. a peripheral within the device
 * that is capable of generating events). Each channel can be individually
 * configured to select the generator peripheral, signal path and edge detection
 * applied to the input event signal, before being passed to any event user(s).
 *
 * Event channels can support multiple users within the device in a standardized
 * manner; when an Event User is linked to an Event Channel, the channel will
 * automatically handshake with all attached users to ensure that all modules
 * correctly receive and acknowledge the event.
 *
 * Event Users
 *
 * Event Users are able to subscribe to an Event Channel, once it has been
 * configured. Each Event User consists of a fixed connection to one of the
 * peripherals within the device (for example, an ADC module or Timer module)
 * and is capable of being connected to a single Event Channel.
 *
 * Edge Detection
 *
 * For asynchronous events, edge detection on the event input is not possible,
 * and the event signal must be passed directly between the event generator and
 * event user. For synchronous and re-synchronous events, the input signal from
 * the event generator must pass through an edge detection unit, so that only
 * the rising, falling or both edges of the event signal triggers an action in
 * the event user.
 *
 * Path Selection
 *
 * The event system in the SAM D20/D21/R21 devices supports three signal path types from
 * the event generator to event users: asynchronous, synchronous and
 * re-synchronous events.
 *
 * Asynchronous Paths
 *
 * Asynchronous event paths allow for an asynchronous connection between the
 * event generator and event user(s), when the source and destination
 * peripherals share the same "Generic Clock"
 * channel. In this mode the event is propagated between the source and
 * destination directly to reduce the event latency, thus no edge detection is
 * possible.
 *
 * Synchronous Paths
 *
 * The Synchronous event path should be used when edge detection or interrupts
 * from the event channel are required, and the source event generator and the
 * event channel shares the same Generic Clock channel.
 *
 * Re-synchronous Paths
 *
 * Re-synchronous event paths are a special form of synchronous events, where
 * when edge detection or interrupts from the event channel are required, but
 * the event generator and the event channel use different Generic Clock
 * channels. The re-synchronous path allows the Event System to synchronize the
 * incoming event signal from the Event Generator to the clock of the Event
 * System module to avoid missed events, at the cost of a higher latency due to
 * the re-synchronization process.
 *
 *
 * Configuring Events
 *
 * For SAM D20/D21/R21 devices, several steps are required to properly configure an
 * event chain, so that hardware peripherals can respond to events generated by
 * each other, listed below.
 *
 * Source Peripheral
 *  -# The source peripheral (that will generate events) must be configured and
 *     enabled.
 *  -# The source peripheral (that will generate events) must have an output
 *     event enabled.
 *
 * Event System
 *  -# An event system channel must be allocated and configured with the
 *     correct source peripheral selected as the channel's event generator.
 *  -# The event system user must be configured and enabled, and attached to
 #     event channel previously allocated.
 *
 * Destination Peripheral
 *  -# The destination peripheral (that will receive events) must be configured
 *     and enabled.
 *  -# The destination peripheral (that will receive events) must have an input
 *     event enabled.
 *
 */


enum status_code {
  STATUS_OK,
  STATUS_BUSY,
  STATUS_ERR_INVALID_ARG,
  STATUS_ERR_OVERFLOW,
  STATUS_ERR_DENIED,
  STATUS_ERR_BAUDRATE_UNAVAILABLE,
  STATUS_ERR_PACKET_COLLISION,
  STATUS_ERR_BAD_ADDRESS,
STATUS_ERR_TIMEOUT
};


/**
 * Edge detect enum
 *
 * Event channel edge detect setting
 *
 */
enum events_edge_detect {
  /** No event output */
  EVENTS_EDGE_DETECT_NONE,
  /** Event on rising edge */
  EVENTS_EDGE_DETECT_RISING,
  /** Event on falling edge */
  EVENTS_EDGE_DETECT_FALLING,
  /** Event on both edges */
  EVENTS_EDGE_DETECT_BOTH,
};

/**
 * Path selection enum
 *
 * Event channel path selection
 *
 */
enum events_path_selection {
  /** Select the synchronous path for this event channel */
  EVENTS_PATH_SYNCHRONOUS,
  /** Select the resynchronizer path for this event channel */
  EVENTS_PATH_RESYNCHRONIZED,
  /** Select the asynchronous path for this event channel */
  EVENTS_PATH_ASYNCHRONOUS,
};

/** Definition for no generator selection */
#define EVSYS_ID_GEN_NONE   0
/** Definition for no user selection */
#define EVSYS_ID_USER_NONE  0



void system_events_init(void);

/**
 * Allocate an event channel and set configuration
 *
 * Allocates an event channel from the event channel pool and sets
 * the channel configuration.
 *
 * \param[out] resource Pointer to a \ref events_resource struct instance
 * \param[in]  config   Pointer to a \ref events_config struct
 *
 * \return Status of the configuration procedure
 * \retval STATUS_OK            Allocation and configuration went successful
 * \retval STATUS_ERR_NOT_FOUND No free event channel found
 *
 */
enum status_code events_allocate(uint8_t channel,
                                 enum events_edge_detect edge_detect,	/** edge detection mode */
                                 enum events_path_selection path,	/** events channel path */
                                 uint8_t generator,			/** event generator for the channel */
                                 uint8_t clock_source);			/** clock source for the event channel */

/**
 * Attach user to the event channel
 *
 * Attach a user peripheral to the event channel to receive events.
 *
 * \param[in] resource Pointer to an \ref events_resource struct instance
 * \param[in] user_id  A number identifying the user peripheral found in the device header file.
 *
 * \return Status of the user attach procedure
 * \retval STATUS_OK No errors detected when attaching the event user
 */
enum status_code events_attach_user(uint8_t channel, uint8_t user_id);

/**
 * Check if a channel is busy
 *
 * Check if a channel is busy, a channels stays busy until all users connected to the channel
 * has handled an event
 *
 * \param[in] resource Pointer to a \ref events_resource struct instance
 *
 * \return Status of the channels busy state
 * \retval true   One or more users connected to the channel has not handled the last event
 * \retval false  All users are ready handle new events
 */
bool events_is_busy(uint8_t channel);

/**
 * Trigger software event
 *
 * Trigger an event by software
 *
 * \param[in] resource Pointer to an \ref events_resource struct
 *
 * \return Status of the event software procedure
 * \retval STATUS_OK No error was detected when software tigger signal was issued
 * \retval STATUS_ERR_UNSUPPORTED_DEV If the channel path is asynchronous and/or the
 *                                    edge detection is not set to RISING
 */
enum status_code events_trigger(uint8_t channel);

/**
 * Check if all users connected to the channel is ready
 *
 * Check if all users connected to the channel is ready to handle incomming events
 *
 * \param[in] resource Pointer to an \ref events_resource struct
 *
 * \return The ready status of users connected to an event channel
 * \retval true  All users connect to event channel is ready handle incomming events
 * \retval false One or more users connect to event channel is not ready to handle incomming events
 */
bool events_is_users_ready(uint8_t channel);

/**
 * Check if event is detected on event channel
 *
 * Check if an event has been detected on the channel
 *
 * \note This function will clear the event detected interrupt flag
 *
 * \param[in] resource Pointer to an \ref events_resource struct
 *
 * \return Status of the event detection interrupt flag
 * \retval true  Event has been detected
 * \retval false Event has not been detected
 */
bool events_is_detected(uint8_t channel);

/**
 * Check if there has been an overrun situation on this channel
 *
 * Check if there has been an overrun situation on this channel
 *
 * \note This function will clear the event overrun detected interrupt flag
 *
 * \param[in] resource Pointer to an \ref events_resource struct
 *
 * \return Status of the event overrun interrupt flag
 * \retval true  Event overrun has been detected
 * \retval false Event overrun has not been detected
 */
bool events_is_overrun(uint8_t channel);


#endif /* EVENTS_H_INCLUDED */
