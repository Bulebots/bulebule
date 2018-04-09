#ifndef __MOVE_H
#define __MOVE_H

#include "search.h"

#include <stdbool.h>

bool collision_detected(void);
void move(enum step_direction direction);

#endif /* __MOVE_H */
