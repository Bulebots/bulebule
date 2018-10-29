#ifndef __SPEED_H
#define __SPEED_H

#include "formatting.h"
#include "move.h"
#include "path.h"
#include "setup.h"

#define NUM_MODES 3

float get_linear_acceleration(void);
void set_linear_acceleration(float value);
float get_linear_deceleration(void);
void set_linear_deceleration(float value);
float get_angular_acceleration(void);
void set_angular_acceleration(float value);
float get_max_linear_speed(void);
void set_max_linear_speed(float value);
float get_max_end_linear_speed(void);
void set_max_end_linear_speed(float value);
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
void set_speed_mode(uint8_t mode, bool run);
float get_move_turn_space(enum movement move, uint8_t speed);
float get_move_turn_linear_speed(enum movement move, uint8_t speed);

void speed_turn(enum movement turn, uint8_t speed);

#endif /* __SPEED_H */
