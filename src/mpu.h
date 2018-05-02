#ifndef __MPU_H
#define __MPU_H

#include <stdint.h>

#include <libopencm3/stm32/timer.h>

#include "setup.h"

uint8_t mpu_read_register(uint8_t address);
void mpu_write_register(uint8_t address, uint8_t value);

uint8_t mpu_who_am_i(void);

#endif /* __MPU_H */
