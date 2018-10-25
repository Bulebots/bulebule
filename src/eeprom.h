#ifndef __EEPROM_H
#define __EEPROM_H

#include <libopencm3/stm32/flash.h>

/**
 * Flash module organization
 *
 * The memory organization is based on a main memory block containing 64 pages *
 * of 1 Kbyte (for medium-density devices), and an information block.
 *
 * The linker file was modified to reserve the last memory page for EEPROM.
 * FLASH_EEPROM_ADDRESS = FLASH_BASE + FLASH_EEPROM_PAGE_NUM * FLASH_PAGE_SIZE
 * FLASH_BASE = 0x08000000
 * FLASH_EEPROM_PAGE_NUM = 63
 * FLASH_PAGE_SIZE = 0x400 (1 Kbyte)
 *
 * @see Programming manual (PM0075) "Flash module organization"
 */
#define FLASH_EEPROM_ADDRESS ((uint32_t)(0x0800fc00))
#define BYTES_PER_WORD 4

/** Flash results */
#define RESULT_OK 0
#define FLASH_WRONG_DATA_WRITTEN 0x80

uint32_t flash_eeprom_data(uint8_t *input_data, uint16_t num_elements);

void read_eeprom_data(uint16_t num_elements, uint8_t *output_data);

#endif /* __EEPROM_H */
