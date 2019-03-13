#ifndef __PLATFORM_H
#define __PLATFORM_H

#include <stdint.h>

#include <libopencm3/stm32/timer.h>

#include "setup.h"

uint16_t read_encoder_left(void);
uint16_t read_encoder_right(void);
void speaker_on(float hz);
void speaker_off(void);

#endif /* __PLATFORM_H */
