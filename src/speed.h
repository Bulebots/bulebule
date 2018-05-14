#ifndef __SPEED_H
#define __SPEED_H

#include "setup.h"

float get_linear_acceleration(void);
void set_linear_acceleration(float value);
float get_linear_deceleration(void);
void set_linear_deceleration(float value);
float get_angular_acceleration(void);
void set_angular_acceleration(float value);

#endif /* __SPEED_H */
