#ifndef __MOVE_H
#define __MOVE_H

#include <math.h>

#include "clock.h"
#include "control.h"
#include "hmi.h"
#include "motor.h"
#include "search.h"
#include "setup.h"
#include "speed.h"

void set_starting_position(void);
uint32_t required_micrometers_to_speed(float speed);
float required_time_to_speed(float speed);
uint32_t required_ticks_to_speed(float speed);
void disable_walls_control(void);
void target_straight(int32_t start, float distance, float speed);
void keep_front_wall_distance(float distance, float error);
void stop_end(void);
void stop_head_front_wall(void);
void stop_middle(void);
void turn_side(enum step_direction side);
void turn_left(void);
void turn_right(void);
void turn_back(void);
void move_front(void);
void move_left(void);
void move_right(void);
void move_back(void);
void move(enum step_direction direction);
void reset_motion(void);

#endif /* __MOVE_H */
