#ifndef __SPEED_H
#define __SPEED_H

#include "move.h"
#include "path.h"
#include "setup.h"

#define NUM_MODES 2

float get_linear_acceleration(void);
void set_linear_acceleration(float value);
float get_linear_deceleration(void);
void set_linear_deceleration(float value);
float get_angular_acceleration(void);
void set_angular_acceleration(float value);
float get_max_linear_speed(void);
void set_max_linear_speed(float value);
void set_linear_speed_variables(float force, bool run);
float get_move_turn_before(enum movement move);
float get_move_turn_after(enum movement move);
float get_move_turn_linear_speed(enum movement turn_type, float force);

void speed_turn(enum movement turn_type, float force);
void inplace_turn(float radians, float force);

#endif /* __SPEED_H */
