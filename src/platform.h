#ifndef __PLATFORM_H
#define __PLATFORM_H

#include <stdint.h>

#include <libopencm3/stm32/timer.h>

uint16_t read_encoder_left(void);
uint16_t read_encoder_right(void);

#endif /* __PLATFORM_H */
