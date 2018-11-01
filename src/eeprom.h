#ifndef __EEPROM_H
#define __EEPROM_H

#include <libopencm3/stm32/flash.h>

/** Flash results */
#define RESULT_OK 0
#define FLASH_WRONG_DATA_WRITTEN 0x80

uint32_t eeprom_flash_page(uint32_t page_address, uint8_t *input_data,
			   uint16_t num_bytes);
void eeprom_read_data(uint32_t start_address, uint16_t num_bytes,
		      uint8_t *output_data);
uint32_t eeprom_erase_page(uint32_t page_address);

#endif /* __EEPROM_H */
