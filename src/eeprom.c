#include "eeprom.h"

/**
 * @brief Function to read eeprom data
 *
 * @param[in] num_elements Number of bytes to be read
 * @param[out] output_data Pointer to a word to save the read data
 */
void read_eeprom_data(uint16_t num_elements, uint8_t *output_data)
{
	uint32_t eeprom_address = FLASH_EEPROM_ADDRESS;
	uint16_t iter;
	uint32_t *memory_ptr = (uint32_t *)eeprom_address;

	for (iter = 0; iter < num_elements / BYTES_PER_WORD; iter++) {
		*(uint32_t *)output_data = *(memory_ptr + iter);
		output_data += BYTES_PER_WORD;
	}
}

/**
 * @brief Function to save data on eeprom
 *
 * - Unlock flash
 * - Erase page
 * - Program flash memory word by word (32-bits )and verify that it is written
 *
 * @param[in] input_data Pointer to the data to be flashed
 * @param[in] num_elements Number of bytes to be flashed
 * @return Flash state.
 */
uint32_t flash_eeprom_data(uint8_t *input_data, uint16_t num_elements)
{
	uint16_t iter;
	uint32_t eeprom_address = FLASH_EEPROM_ADDRESS;
	uint32_t flash_status = 0;

	flash_unlock();

	flash_erase_page(eeprom_address);
	flash_status = flash_get_status_flags();
	if (flash_status != FLASH_SR_EOP)
		return flash_status;

	for (iter = 0; iter < num_elements; iter += BYTES_PER_WORD) {
		flash_program_word(eeprom_address + iter,
				   *((uint32_t *)(input_data + iter)));
		flash_status = flash_get_status_flags();
		if (flash_status != FLASH_SR_EOP)
			return flash_status;

		if (*((uint32_t *)(eeprom_address + iter)) !=
		    *((uint32_t *)(input_data + iter)))
			return FLASH_WRONG_DATA_WRITTEN;
	}

	return RESULT_OK;
}
