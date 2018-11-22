#ifndef __MOVE_H
#define __MOVE_H

#include <math.h>

#include "clock.h"
#include "control.h"
#include "hmi.h"
#include "motor.h"
#include "path.h"
#include "search.h"
#include "setup.h"
#include "speed.h"

float current_cell_shift(void);
void set_starting_position(void);
int32_t required_micrometers_to_speed(float speed);
float required_time_to_speed(float speed);
uint32_t required_ticks_to_speed(float speed);
void disable_walls_control(void);
void target_straight(int32_t start, float distance, float speed);
void keep_front_wall_distance(float distance);
void stop_end(void);
void stop_head_front_wall(void);
void stop_middle(void);
void parametric_turn(float angular_speed, float rise_time, float elapsed_time);
void turn_back(uint8_t speed);
void turn_to_start_position(uint8_t speed);
void move_front(void);
void move_front_many(int cells);
void parametric_move_front(float distance, float end_linear_speed);
void move_side(enum movement turn, uint8_t speed);
void move_back(uint8_t speed);
void move(enum step_direction direction, uint8_t speed);
void reset_motion(void);

#endif /* __MOVE_H */
