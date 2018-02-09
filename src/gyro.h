#ifndef __GYRO_H
#define __GYRO_H

#include <stdint.h>

#include <libopencm3/stm32/timer.h>

#include "setup.h"

uint8_t gyro_read_register(uint8_t address);
void gyro_write_register(uint8_t address, uint8_t value);

uint8_t gyro_who_am_i(void);

#endif /* __GYRO_H */
