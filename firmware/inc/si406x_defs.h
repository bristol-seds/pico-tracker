/*
 * Definitions and macros for the Si406x
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

#ifndef SI406X_DEFS_H
#define SI406X_DEFS_H

/**
 * Si406x Boot Commands
 */
enum {
  SI_CMD_POWER_UP		= 0x02,
};
/**
 * Si406x Common Commands
 */
enum {
  SI_CMD_NOP			= 0x00,
  SI_CMD_PART_INFO		= 0x01,
  SI_CMD_FUNC_INFO		= 0x10,
  SI_CMD_SET_PROPERTY		= 0x11,
  SI_CMD_GET_PROPERTY		= 0x12,
  SI_CMD_GPIO_PIN_CFG		= 0x13,
  SI_CMD_FIFO_INFO		= 0x15,
  SI_CMD_GET_INT_STATUS		= 0x20,
  SI_CMD_REQUEST_DEVICE_STATE	= 0x33,
  SI_CMD_CHANGE_STATE		= 0x34,
  SI_CMD_READ_CMD_BUFF		= 0x44,
  SI_CMD_FRR_A_READ		= 0x50,
  SI_CMD_FRR_B_READ		= 0x51,
  SI_CMD_FRR_C_READ		= 0x53,
  SI_CMD_FRR_D_READ		= 0x57,
};
/**
 * Si406x Tx Commands
 */
enum {
  SI_CMD_START_TX		= 0x31,
  SI_CMD_WRITE_TX_FIFO		= 0x66,
};
/**
 * Si406x Rx Commands
 */
enum {
  SI_CMD_PACKET_INFO		= 0x16,
  SI_CMD_GET_MODEM_STATUS	= 0x22,
  SI_CMD_START_RX		= 0x32,
  SI_CMD_RX_HOP			= 0x36,
  SI_CMD_READ_RX_FIFO		= 0x77,
};
/**
 * Si406x Advanced Commands
 */
enum {
  SI_CMD_GET_ADC_READING	= 0x14,
  SI_CMD_PROTOCOL_CFG		= 0x18,
  SI_CMD_GET_PH_STATUS		= 0x21,
  SI_CMD_GET_CHIP_STATUS	= 0x23,
};

/**
 * Si406x State Change Commands
 */
enum {
  SI_STATE_CHANGE_NOCHANGE	= (0 << 8) | SI_CMD_CHANGE_STATE,
  SI_STATE_CHANGE_SLEEP		= (1 << 8) | SI_CMD_CHANGE_STATE,
  SI_STATE_CHANGE_SPI_ACTIVE	= (2 << 8) | SI_CMD_CHANGE_STATE,
  SI_STATE_CHANGE_READY		= (3 << 8) | SI_CMD_CHANGE_STATE,
  SI_STATE_CHANGE_TX_TUNE	= (5 << 8) | SI_CMD_CHANGE_STATE,
  SI_STATE_CHANGE_RX_TUNE	= (6 << 8) | SI_CMD_CHANGE_STATE,
  SI_STATE_CHANGE_TX		= (7 << 8) | SI_CMD_CHANGE_STATE,
  SI_STATE_CHANGE_RX		= (8 << 8) | SI_CMD_CHANGE_STATE,
};

/**
 * Generic SPI Send / Receive for the Si406x
 */
void _si406x_transfer(int tx_count, int rx_count, const uint8_t *data);

/**
 * Chip Select. Active Low (High = Inactive, Low = Active)
 */
#define _si406x_cs_enable()			\
  port_pin_set_output_level(SI406X_SEL_PIN, 0)
#define _si406x_cs_disable()			\
  port_pin_set_output_level(SI406X_SEL_PIN, 1)

/**
 * Shutdown. Active High (High = Shutdown, Low = Run)
 */
#define _si406x_sdn_enable()			\
  port_pin_set_output_level(SI406X_SDN_PIN, 1)
#define _si406x_sdn_disable()			\
  port_pin_set_output_level(SI406X_SDN_PIN, 0)

/**
 * HF Clock
 */
#define _si406x_hf_clock_enable(void)		\
  /* TODO: Clock is always enabled */
#define _si406x_hf_clock_disable(void)		\
  /* TODO: Clock is always enabled */


/**
 * Convenience transfer functions
 */
static void _si406x_transfer_uint16(uint16_t value)
{
  _si406x_transfer(2, 0, (uint8_t*)&value);
}

/**
 * State changes
 */
#define si406x_state_ready()			\
  _si406x_transfer_uint16(SI_STATE_CHANGE_READY)
/**
 * Change to TX tune state
 */
#define si406x_state_tx_tune()				\
  _si406x_transfer_uint16(SI_STATE_CHANGE_TX_TUNE)
/**
 * Change to RX tune state
 */
#define si406x_state_rx_tune()				\
  _si406x_transfer_uint16(SI_STATE_CHANGE_RX_TUNE)
/**
 * Change to TX state
 */
#define si406x_state_tx()				\
  _si406x_transfer_uint16(SI_STATE_CHANGE_TX)
/**
 * Change to RX state
 */
#define si406x_state_rx()			\
  _si406x_transfer_uint16(SI_STATE_CHANGE_RX)



#endif /* SI406X_DEFS_H */
