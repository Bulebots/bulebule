#ifndef __SPEED_H
#define __SPEED_H

#include "formatting.h"
#include "move.h"
#include "path.h"
#include "setup.h"

#define NUM_MODES 4

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
void set_speed_mode(uint8_t mode, bool run);
float get_move_turn_before(enum movement move, uint8_t speed);
float get_move_turn_after(enum movement move, uint8_t speed);
float get_move_turn_linear_speed(enum movement move, uint8_t speed);

void speed_turn(enum movement turn, uint8_t speed);

#endif /* __SPEED_H */
