#ifndef __MOVE_H
#define __MOVE_H

#include "clock.h"
#include "control.h"
#include "hmi.h"
#include "logging.h"

void move_straight_distance(float distance);
void move_straight(void);
void move_straight_out_of_cell(void);
void move_stop(void);
void turn_right_static(void);
void move_right(void);

#endif /* __MOVE_H */
