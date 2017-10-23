#ifndef __CONTROL_H
#define __CONTROL_H

#include "encoder.h"
#include "motor.h"
#include "setup.h"

float get_left_pwm(void);
float get_right_pwm(void);
float get_target_linear_speed(void);
float get_ideal_linear_speed(void);
void motor_control(void);
void set_target_linear_speed(float speed);
void update_ideal_speed(void);

#endif /* __CONTROL_H */
