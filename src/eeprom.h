#ifndef __EEPROM_H
#define __EEPROM_H


#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/flash.h>


#include "setup.h"

#define SEND_BUFFER_SIZE 256
#define FLASH_OPERATION_ADDRESS ((uint32_t)0x0800f000)
#define FLASH_PAGE_NUM_MAX 127
#define FLASH_PAGE_SIZE 0x800
#define FLASH_WRONG_DATA_WRITTEN 0x80
#define RESULT_OK 0

uint32_t flash_program_data(uint32_t start_address, uint8_t *input_data, uint16_t num_elements);

void flash_read_data(uint32_t start_address, uint16_t num_elements, uint8_t *output_data);

void local_ltoa_hex(uint32_t value, uint8_t *out_string);

#endif /* __EEPROM_H */
