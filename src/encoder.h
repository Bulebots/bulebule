#ifndef __ENCODER_H
#define __ENCODER_H

#include <stdint.h>

#include <libopencm3/stm32/timer.h>

#include "setup.h"

int32_t get_encoder_left_diff_count(void);
int32_t get_encoder_right_diff_count(void);
uint16_t get_encoder_left_total_count(void);
uint16_t get_encoder_right_total_count(void);
float get_encoder_left_distance(void);
float get_encoder_right_distance(void);

void update_encoder_readings(void);

#endif /* __ENCODER_H */
