#include "eeprom.h"

/**
 * @brief ADC1 and ADC2 interruption routine.
 *
 * - Manage the ADC2 analog watchdog interruption flag.
 * - Send a message.
 * - Toggle a LED.
 * - Disable analog watchdog interruptions on injected channels.
 */
void flash_read_data(uint32_t start_address, uint16_t num_elements, uint8_t *output_data)
 {
 	uint16_t iter;
 	uint32_t *memory_ptr= (uint32_t*)start_address;

 	for(iter=0; iter<num_elements/4; iter++)
 	{
 		*(uint32_t*)output_data = *(memory_ptr + iter);
 		output_data += 4;
 	}
 }

/**
 * @brief Function to get battery voltage.
 *
 * This function reads the voltage of the battery from the register configured
 * on the ADC2. The conversion is triggered on sensors state machine function
 * sm_emitter_adc.
 *
 * The value is converted from bits to voltage taking into account that the
 * battery voltage is read through a voltage divider.
 *
 *@return The battery voltage in volts.
 */
uint32_t flash_program_data(uint32_t start_address, uint8_t *input_data, uint16_t num_elements)
 {
 	uint16_t iter;
 	uint32_t current_address = start_address;
 	uint32_t page_address = start_address;
 	uint32_t flash_status = 0;

 	/*check if start_address is in proper range*/
 	if((start_address - FLASH_BASE) >= (FLASH_PAGE_SIZE * (FLASH_PAGE_NUM_MAX+1)))
 		return 1;

 	/*calculate current page address*/
 	if(start_address % FLASH_PAGE_SIZE)
 		page_address -= (start_address % FLASH_PAGE_SIZE);

 	flash_unlock();

 	/*Erasing page*/
 	flash_erase_page(page_address);
 	flash_status = flash_get_status_flags();
 	if(flash_status != FLASH_SR_EOP)
 		return flash_status;

 	/*programming flash memory*/
 	for(iter=0; iter<num_elements; iter += 4)
 	{
 		/*programming word data*/
 		flash_program_word(current_address+iter, *((uint32_t*)(input_data + iter)));
 		flash_status = flash_get_status_flags();
 		if(flash_status != FLASH_SR_EOP)
 			return flash_status;

 		/*verify if correct data is programmed*/
 		if(*((uint32_t*)(current_address+iter)) != *((uint32_t*)(input_data + iter)))
 			return FLASH_WRONG_DATA_WRITTEN;
 }

 return 0;
}

void local_ltoa_hex(uint32_t value, uint8_t *out_string)
 {
 	uint8_t iter;

 	/*end of string*/
 	out_string += 8;
 	*(out_string--) = 0;

 	for(iter=0; iter<8; iter++)
 	{
 		*(out_string--) = (((value&0xf) > 0x9) ? (0x40 + ((value&0xf) - 0x9)) : (0x30 | (value&0xf)));
 		value >>= 4;
 	}
 }
