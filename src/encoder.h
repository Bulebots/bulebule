#ifndef __ENCODER_H
#define __ENCODER_H

#include <stdint.h>

#include <libopencm3/stm32/timer.h>

#include "setup.h"

int32_t get_encoder_left_diff_count(void);
int32_t get_encoder_right_diff_count(void);
int32_t get_encoder_left_total_count(void);
int32_t get_encoder_right_total_count(void);
int32_t get_encoder_left_micrometers(void);
int32_t get_encoder_right_micrometers(void);
float get_encoder_left_speed(void);
float get_encoder_right_speed(void);

int32_t max_likelihood_counter_diff(uint16_t now, uint16_t before);
void update_encoder_readings(void);

#endif /* __ENCODER_H */
