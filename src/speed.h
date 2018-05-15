#ifndef __SPEED_H
#define __SPEED_H

#include "setup.h"

/* Speed modes*/
#define SLOW_MODE 0
#define NORMAL_MODE 1
#define FAST_MODE 2
#define NUM_MODES 3

float get_linear_acceleration(void);
void set_linear_acceleration(float value);
float get_linear_deceleration(void);
void set_linear_deceleration(float value);
float get_angular_acceleration(void);
void set_angular_acceleration(float value);
float get_max_linear_speed(void);
void set_max_linear_speed(float value);
float get_max_angular_speed(void);
void set_max_angular_speed(float value);
float get_turn_linear_speed(void);
void set_turn_linear_speed(float value);
float get_turn_radius(void);
void set_turn_radius(float value);
float get_turn_t0(void);
void set_turn_t0(float value);
float get_turn_t1(void);
void set_turn_t1(float value);
void set_speed_mode(uint8_t mode);

#endif /* __SPEED_H */
