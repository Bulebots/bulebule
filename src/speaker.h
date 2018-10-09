#ifndef __SPEAKER_H
#define __SPEAKER_H

#include <math.h>

#include <libopencm3/stm32/timer.h>

#include "setup.h"

void speaker_play(char note, uint8_t octave, int8_t accidental, float duration);

#endif /* __SPEAKER_H */
