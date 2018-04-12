#ifndef __ENCODER_H
#define __ENCODER_H

#include <stdint.h>

#include <libopencm3/stm32/timer.h>

#include "config.h"
#include "setup.h"

float get_micrometers_per_count(void);
void set_micrometers_per_count(float value);
float get_wheels_separation(void);
void set_wheels_separation(float value);
int32_t get_encoder_left_diff_count(void);
int32_t get_encoder_right_diff_count(void);
int32_t get_encoder_left_total_count(void);
int32_t get_encoder_right_total_count(void);
int32_t get_encoder_left_micrometers(void);
int32_t get_encoder_right_micrometers(void);
int32_t get_encoder_average_micrometers(void);
float get_encoder_left_speed(void);
float get_encoder_right_speed(void);
float get_encoder_angular_speed(void);

int32_t max_likelihood_counter_diff(uint16_t now, uint16_t before);
void update_encoder_readings(void);

#endif /* __ENCODER_H */
