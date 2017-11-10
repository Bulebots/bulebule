#ifndef __CONTROL_H
#define __CONTROL_H

#include "detection.h"
#include "encoder.h"
#include "hmi.h"
#include "motor.h"
#include "setup.h"

float get_linear_acceleration(void);
void set_linear_acceleration(float value);
float get_linear_deceleration(void);
void set_linear_deceleration(float value);
float get_angular_acceleration(void);
void set_angular_acceleration(float value);
float get_side_sensors_error_factor(void);
void set_side_sensors_error_factor(float value);
float get_front_sensors_error_factor(void);
void set_front_sensors_error_factor(float value);
float get_kp_linear(void);
void set_kp_linear(float value);
float get_kd_linear(void);
void set_kd_linear(float value);
float get_kp_angular(void);
void set_kp_angular(float value);
float get_kd_angular(void);
void set_kd_angular(float value);
float get_ki_angular_side(void);
void set_ki_angular_side(float value);
float get_ki_angular_front(void);
void set_ki_angular_front(float value);
void side_sensors_control(bool value);
void front_sensors_control(bool value);
void reset_control(void);
bool collision_detected(void);
float get_left_pwm(void);
float get_right_pwm(void);
float get_target_linear_speed(void);
float get_target_angular_speed(void);
float get_ideal_linear_speed(void);
float get_ideal_angular_speed(void);
void motor_control(void);
void set_target_linear_speed(float speed);
void set_target_angular_speed(float speed);
void update_ideal_speed(void);

#endif /* __CONTROL_H */
