/*
 * Provides functions for using the external flash memory
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
#include "memory.h"
#include "sercom/spi.h"
#include "hw_config.h"

enum memory_opcodes {
  MEM_OP_READ			= 0x03,
  MEM_OP_ERASE_4KB_SECTOR	= 0x20,
  MEM_OP_ERASE_64KB_BLOCK	= 0xD8,
  MEM_OP_CHIP_ERASE		= 0x60,
  MEM_OP_PAGE_PROGRAM		= 0x02,
  MEM_OP_READ_STATUS_REGISTER	= 0x05,
  MEM_OP_WRITE_STATUS_REGISTER	= 0x01,
  MEM_OP_WRITE_ENABLE		= 0x06,
  MEM_OP_WRITE_DISABLE		= 0x04,
  MEM_OP_READ_ID		= 0xAB,
  MEM_OP_READ_JEDEC_ID		= 0x9F,
  MEM_OP_POWER_DOWN		= 0xB9,
  MEM_OP_POWER_UP		= 0xAB,
};

enum memory_status_register {
  MEM_STATUS_BUSY		= (1 << 0),
  MEM_STATUS_WRITES_ENABLED	= (1 << 1),
};

#define SST25WF040B_JEDEC_ID	0x00131662

/**
 * Check initialised correctly
 */
uint8_t memory_init_success = 0;

/**
 * Chip Select. Active Low (High = Inactive, Low = Active)
 */
#define _mem_cs_enable()			\
  port_pin_set_output_level(FLASH_CSN_PIN, 0)
#define _mem_cs_disable()			\
  port_pin_set_output_level(FLASH_CSN_PIN, 1)

/**
 * Transfers `length` bytes
 */
#define _mem_transfer(tx_data, rx_data, length)                         \
  spi_transceive_buffer_wait(FLASH_SERCOM, tx_data, rx_data, length)
/**
 * Reads `length` bytes
 */
#define _mem_read(rx_data, length)                              \
  spi_read_buffer_wait(FLASH_SERCOM, rx_data, length, 0xFF)
/**
 * Writes `length` bytes
 */
#define _mem_write(tx_data, length)			\
  spi_write_buffer_wait(FLASH_SERCOM, tx_data, length)


/**
 * Write a single command with no data
 */
void _mem_single_command(uint8_t command)
{
  _mem_cs_enable();
  _mem_write(&command, 1);
  _mem_cs_disable();
}
/**
 * Read the status register until the busy bit is cleared
 */
void _mem_wait_for_done(void)
{
  uint8_t tx_data = MEM_OP_READ_STATUS_REGISTER;
  uint8_t status_register;

  _mem_cs_enable();
  _mem_write(&tx_data, 1);

  do {
    _mem_read(&status_register, 1);

    // Sleep for a few hundred microseconds??
    for (int i = 0; i < 100; i++);

  } while (status_register & MEM_STATUS_BUSY);

  _mem_cs_disable();
}

/**
 * JEDEC ID
 */
uint32_t mem_read_jedec_id(void)
{
  uint8_t tx_data;
  uint32_t rx_data_32;
  uint8_t* rx_data = (uint8_t*)&rx_data_32;

  tx_data = MEM_OP_READ_JEDEC_ID;

  _mem_cs_enable();
  _mem_write(&tx_data, 1); _mem_read(rx_data, 4);
  _mem_cs_disable();

  return rx_data_32;
}

/**
 * Enter Deep Power Down
 */
void mem_enter_deep_power_down(void)
{
  _mem_single_command(MEM_OP_POWER_DOWN);
}
/**
 * Exit Deep Power Down
 */
void  mem_exit_deep_power_down(void)
{
  _mem_single_command(MEM_OP_POWER_UP);
}


/**
 * =============================================================================
 * Public Methods ==============================================================
 * =============================================================================
 */

/**
 * Simple Commands
 */
void mem_chip_erase(void)
{
  if (memory_init_success) {
    _mem_single_command(MEM_OP_WRITE_ENABLE);

    _mem_single_command(MEM_OP_CHIP_ERASE);

    /* Wait */
    _mem_wait_for_done();

    _mem_single_command(MEM_OP_WRITE_DISABLE);
  }
}

/**
 * Read memory
 */
void mem_read_memory(uint32_t address, uint8_t* buffer, uint32_t length)
{
  uint8_t tx_data[4];

  if (memory_init_success) {
    address &= MEMORY_MASK;

    tx_data[0] = MEM_OP_READ;
    tx_data[1] = (address >> 16) & 0xFF;
    tx_data[2] = (address >> 8) & 0xFF;
    tx_data[3] = (address >> 0) & 0xFF;

    _mem_cs_enable();
    _mem_write(tx_data, 4); _mem_read(buffer, length);
    _mem_cs_disable();
  }
}
/**
 * Write 256-byte page. Address should be page aligned
 */
void mem_write_page(uint32_t address, uint8_t* buffer, uint16_t length)
{
  uint8_t tx_data[4];

  if (memory_init_success) {
    _mem_single_command(MEM_OP_WRITE_ENABLE);

    address &= PAGE_MASK;

    tx_data[0] = MEM_OP_PAGE_PROGRAM;
    tx_data[1] = (address >> 16) & 0xFF;
    tx_data[2] = (address >> 8) & 0xFF;
    tx_data[3] = (address >> 0) & 0xFF;

    _mem_cs_enable();
    _mem_write(tx_data, 4); _mem_write(buffer, length);
    _mem_cs_disable();

    /* Wait */
    _mem_wait_for_done();

    _mem_single_command(MEM_OP_WRITE_DISABLE);
  }
}
/**
 * Erase sector
 */
void mem_erase_sector(uint32_t address)
{
  uint8_t tx_data[4];

  if (memory_init_success) {
    _mem_single_command(MEM_OP_WRITE_ENABLE);

    address &= SECTOR_MASK;

    tx_data[0] = MEM_OP_ERASE_4KB_SECTOR;
    tx_data[1] = (address >> 16) & 0xFF;
    tx_data[2] = (address >> 8) & 0xFF;
    tx_data[3] = (address >> 0) & 0xFF;

    _mem_cs_enable();
    _mem_write(tx_data, 4);
    _mem_cs_disable();

    /* Wait */
    _mem_wait_for_done();

    _mem_single_command(MEM_OP_WRITE_DISABLE);
  }
}

/**
 * Initialise and Power on Memory Interface
 *
 * Returns 1 on success, 0 on failute
 */
uint8_t mem_power_on(void)
{
  /* Configure the SPI select pin */
  port_pin_set_config(FLASH_CSN_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */
  /* Disable the SEL pin */
  _mem_cs_disable();

  spi_init(FLASH_SERCOM,
  	   SPI_MODE_MASTER,		/** SPI mode */
  	   SPI_DATA_ORDER_MSB,		/** Data order */
  	   SPI_TRANSFER_MODE_0,		/** Transfer mode */
  	   FLASH_SERCOM_MUX,		/** Mux setting */
  	   SPI_CHARACTER_SIZE_8BIT,	/** SPI character size */
  	   false,			/** Enabled in sleep */
  	   true,			/** Enable receiver */
  	   1000*1000,			/** Master - Baud rate */
  	   0,				/** Slave - Frame format */
  	   0,				/** Slave - Address mode */
  	   0,				/** Slave - Address */
  	   0,				/** Slave - Address mask */
  	   false,			/** Slave - Preload data  */
  	   GCLK_GENERATOR_0,		/** GCLK generator to use */
  	   FLASH_SERCOM_MOSI_PINMUX,	/** Pinmux */
  	   FLASH_SERCOM_MISO_PINMUX,	/** Pinmux */
  	   FLASH_SERCOM_SCK_PINMUX,	/** Pinmux */
  	   PINMUX_UNUSED);		/** Pinmux */

  spi_enable(FLASH_SERCOM);

  /* Release from power down */
  mem_exit_deep_power_down();

  /* Make 3 attempts to read JEDEC chip ID */
  for (int i = 0; i < 3; i++) {

    uint32_t jedec = mem_read_jedec_id();

    /* Check it's the chip we're expecting */
    if (jedec == SST25WF040B_JEDEC_ID) {
      /* Correct ID */
      memory_init_success = 1;
      return 1;
    }
  }

  /* Memory failed to initialise correctly */
  return 0;
}
/**
 * Return memory to lowest power state
 */
void mem_power_off(void)
{
  if (memory_init_success) {
    /* Place memory in deep power down */
    mem_enter_deep_power_down();

    /* Disable SPI interface */
    spi_disable(FLASH_SERCOM);

    /* Return pins to default state (lowest power) */
    system_pinmux_pin_set_config(FLASH_CSN_PIN,
                                 SYSTEM_PINMUX_GPIO,
                                 SYSTEM_PINMUX_PIN_DIR_OUTPUT,
                                 SYSTEM_PINMUX_PIN_PULL_NONE,
                                 true);
    system_pinmux_pin_set_config(FLASH_SERCOM_MOSI_PIN,
                                 SYSTEM_PINMUX_GPIO,
                                 SYSTEM_PINMUX_PIN_DIR_OUTPUT,
                                 SYSTEM_PINMUX_PIN_PULL_NONE,
                                 true);
    system_pinmux_pin_set_config(FLASH_SERCOM_MISO_PIN,
                                 SYSTEM_PINMUX_GPIO,
                                 SYSTEM_PINMUX_PIN_DIR_OUTPUT,
                                 SYSTEM_PINMUX_PIN_PULL_NONE,
                                 true);
    system_pinmux_pin_set_config(FLASH_SERCOM_SCK_PIN,
                                 SYSTEM_PINMUX_GPIO,
                                 SYSTEM_PINMUX_PIN_DIR_OUTPUT,
                                 SYSTEM_PINMUX_PIN_PULL_NONE,
                                 true);

    /* memory is no longer initialised */
    memory_init_success = 0;
  }
}
