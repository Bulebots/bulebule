#ifndef __MOVE_H
#define __MOVE_H

#include "search.h"

#include <stdbool.h>

bool collision_detected(void);
void move(enum step_direction direction);
void move_front_many(int cells);
void move_side(enum step_direction direction);
void stop_middle(void);

#endif /* __MOVE_H */
