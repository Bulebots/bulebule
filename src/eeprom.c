#include "eeprom.h"

#define BYTES_PER_WORD 4

/**
 * @brief Function to get the words of a number of bytes.
 *
 * It includes garbage bytes if needed.
 *
 * @param[in] num_bytes Number of bytes.
 * @return Number of words.
 */
static uint16_t bytes_to_words(uint16_t num_bytes)
{
	uint16_t num_words = num_bytes / BYTES_PER_WORD;

	if ((num_bytes % BYTES_PER_WORD) > 0)
		++num_words;

	return num_words;
}

/**
 * @brief Function to read EEPROM data from a specific address.
 *
 * @param[in] start_address Address to read from.
 * @param[in] num_elements Number of bytes to be read.
 * @param[out] output_data Pointer to a buffer to save the read data.
 */
void eeprom_read_data(uint32_t start_address, uint16_t num_bytes,
		      uint8_t *output_data)
{
	uint16_t iter;
	uint32_t *memory_ptr = (uint32_t *)start_address;

	for (iter = 0; iter < bytes_to_words(num_bytes); iter++) {
		*(uint32_t *)output_data = *(memory_ptr + iter);
		output_data += BYTES_PER_WORD;
	}
}

/**
 * @brief Function to save data on a page of EEPROM.
 *
 * - Unlock flash.
 * - Erase page.
 * - Program flash memory word by word (32-bits) and verify that it is
 * written.
 *
 * @param[in] page_address Address of a EEPROM page to flash on.
 * @param[in] input_data Pointer to the data to be flashed.
 * @param[in] num_bytes Number of bytes to be flashed.
 * @return Flash state.
 */
uint32_t eeprom_flash_page(uint32_t page_address, uint8_t *input_data,
			   uint16_t num_bytes)
{
	uint16_t iter;
	uint32_t flash_status = 0;

	flash_unlock();

	flash_erase_page(page_address);
	flash_status = flash_get_status_flags();
	if (flash_status != FLASH_SR_EOP)
		return flash_status;

	for (iter = 0; iter < bytes_to_words(num_bytes) * BYTES_PER_WORD;
	     iter += BYTES_PER_WORD) {
		flash_program_word(page_address + iter,
				   *((uint32_t *)(input_data + iter)));
		flash_status = flash_get_status_flags();
		if (flash_status != FLASH_SR_EOP)
			return flash_status;

		if (*((uint32_t *)(page_address + iter)) !=
		    *((uint32_t *)(input_data + iter)))
			return FLASH_WRONG_DATA_WRITTEN;
	}

	return RESULT_OK;
}

/**
 * @brief Function to erase a page of EEPROM.
 *
 * @param[in] page_address Address of the EEPROM page to erase.
 * @return Erase state.
 */
uint32_t eeprom_erase_page(uint32_t page_address)
{
	uint32_t erase_status = 0;

	flash_unlock();

	flash_erase_page(page_address);
	erase_status = flash_get_status_flags();
	if (erase_status != FLASH_SR_EOP)
		return erase_status;

	return RESULT_OK;
}
