#ifndef __MOVE_H
#define __MOVE_H

#include "clock.h"
#include "control.h"
#include "hmi.h"
#include "setup.h"

float get_linear_speed(void);
void set_linear_speed(float value);
void move_straight_distance(float distance);
void move_straight_out_of_cell(void);
void move_straight_stop_end(void);
void move_straight_stop_head_front_wall(void);
void move_straight_stop_middle(void);
void move_stop(void);
void turn_left_static(void);
void turn_right_static(void);
void move_front(void);
void move_left(void);
void move_right(void);

#endif /* __MOVE_H */
