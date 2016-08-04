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

/**
 * See Errata 10804 for more details, d/s §35.3.16
 * Silicon Revision C
 */
#define FIX_ERRATA_REV_C_FLASH_10804

#define MEM_SIZE 16384          /* 16 KB */
/**
 * Allocate a 16KB section of flash memory, aligned to an NVM row
 */
const uint8_t nvm_section[MEM_SIZE]
  __attribute__ ((aligned (256)))
  __attribute__ ((section (".eeprom")))
  = { 0xFF };

/**
 * Poll the status register until the busy bit is cleared
 */
void _mem_wait_for_done(void)
{
  while ((NVMCTRL->INTFLAG.reg & NVMCTRL_INTFLAG_READY) == 0);
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
#ifdef FIX_ERRATA_REV_C_FLASH_10804
  /* save CTRLB and disable cache */
  uint32_t temp = NVMCTRL->CTRLB.reg;
  NVMCTRL->CTRLB.reg |= NVMCTRL_CTRLB_CACHEDIS;
#endif

  /* erase each row */
  for (int n = 0; n < TOTAL_ROWS; n++) {
    /* write address */
    NVMCTRL->ADDR.reg  = (uint32_t)(nvm_section + (n*ROW_SIZE)) >> 1;
    /* unlock */
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_UR;
    /* erase */
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER;
    _mem_wait_for_done();
    /* lock */
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_LR;
  }

#ifdef FIX_ERRATA_REV_C_FLASH_10804
  /* restore CTRLB */
  NVMCTRL->CTRLB.reg = temp;
#endif
}

/**
 * Read memory
 */
void mem_read_memory(uint32_t address, uint8_t* buffer, uint32_t length)
{
  memcpy(buffer, nvm_section + address, length);
}
/**
 * Write single word
 */
void mem_write_word(uint32_t address, uint32_t word)
{
#ifdef FIX_ERRATA_REV_C_FLASH_10804
  /* save CTRLB and disable cache */
  uint32_t temp = NVMCTRL->CTRLB.reg;
  NVMCTRL->CTRLB.reg |= NVMCTRL_CTRLB_CACHEDIS;
#endif

  /* write address */
  NVMCTRL->ADDR.reg = (uint32_t)(address) >> 1;
  /* write data. length must be multiple of two */
  *(uint32_t*)address = word;
  /* unlock */
  NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_UR;
  /* write page */
  NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;
  _mem_wait_for_done();
  /* lock */
  NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_LR;

#ifdef FIX_ERRATA_REV_C_FLASH_10804
  /* restore CTRLB */
  NVMCTRL->CTRLB.reg = temp;
#endif
}
/**
 * Write 64-byte page. Address should be page aligned
 */
void mem_write_page(uint32_t address, uint8_t* buffer, uint16_t length)
{
#ifdef FIX_ERRATA_REV_C_FLASH_10804
  /* save CTRLB and disable cache */
  uint32_t temp = NVMCTRL->CTRLB.reg;
  NVMCTRL->CTRLB.reg |= NVMCTRL_CTRLB_CACHEDIS;
#endif

  if ((address < MEM_SIZE) && (length <= PAGE_SIZE)) {
    /* write address */
    NVMCTRL->ADDR.reg = (uint32_t)(nvm_section + address) >> 1;
    /* write data. length must be multiple of two */
    memcpy((void*)(nvm_section + address), buffer, length & ~0x1);
    /* unlock */
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_UR;
    /* write page */
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;
    _mem_wait_for_done();
    /* lock */
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_LR;
  }

#ifdef FIX_ERRATA_REV_C_FLASH_10804
  /* restore CTRLB */
  NVMCTRL->CTRLB.reg = temp;
#endif
}
/**
 * Erase 256-byte sector.
 */
void mem_erase_sector(uint32_t address)
{
#ifdef FIX_ERRATA_REV_C_FLASH_10804
  /* save CTRLB and disable cache */
  uint32_t temp = NVMCTRL->CTRLB.reg;
  NVMCTRL->CTRLB.reg |= NVMCTRL_CTRLB_CACHEDIS;
#endif

  if (address < MEM_SIZE) {
    /* write address */
    NVMCTRL->ADDR.reg  = (uint32_t)(nvm_section + address) >> 1;
    /* unlock */
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_UR;
    _mem_wait_for_done();
    /* erase row */
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER;
    _mem_wait_for_done();
    /* lock */
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_LR;
  }

#ifdef FIX_ERRATA_REV_C_FLASH_10804
  /* restore CTRLB */
  NVMCTRL->CTRLB.reg = temp;
#endif
}

/**
 * Initialise and Power on Memory Interface
 *
 * Returns 1 on success, 0 on failure
 */
uint8_t mem_power_on(void)
{
  /* NVMCTRL is enabled by default */
  return 1;
}
/**
 * Return memory to lowest power state
 */
void mem_power_off(void)
{
}
