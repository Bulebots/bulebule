#ifndef __MOVE_H
#define __MOVE_H

#include "clock.h"
#include "control.h"
#include "hmi.h"
#include "search.h"
#include "setup.h"

void set_starting_position(void);
float get_max_linear_speed(void);
void set_max_linear_speed(float value);
void accelerate(int32_t start, float distance);
void decelerate(int32_t start, float distance, float speed);
void stop_end(void);
void stop_head_front_wall(void);
void stop_middle(void);
void turn_left(void);
void turn_right(void);
void move_out(void);
void move_front(void);
void move_left(void);
void move_right(void);
void move_back(void);
void move(enum step_direction direction);

#endif /* __MOVE_H */
