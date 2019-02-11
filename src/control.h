#ifndef __CONTROL_H
#define __CONTROL_H

#include "detection.h"
#include "encoder.h"
#include "hmi.h"
#include "motor.h"
#include "setup.h"
#include "speed.h"

float get_kp_linear(void);
void set_kp_linear(float value);
float get_kd_linear(void);
void set_kd_linear(float value);
float get_kp_angular(void);
void set_kp_angular(float value);
float get_kd_angular(void);
void set_kd_angular(float value);
float get_kp_angular_side(void);
void set_kp_angular_side(float value);
float get_kp_angular_front(void);
void set_kp_angular_front(float value);
float get_ki_angular_side(void);
void set_ki_angular_side(float value);
float get_ki_angular_front(void);
void set_ki_angular_front(float value);
void side_sensors_control(bool value);
void front_sensors_control(bool value);
bool collision_detected(void);
void reset_collision_detection(void);
void reset_control_errors(void);
void reset_control_speed(void);
void reset_control_all(void);
void enable_motor_control(void);
void disable_motor_control(void);
int32_t get_left_pwm(void);
int32_t get_right_pwm(void);
float get_target_linear_speed(void);
float get_ideal_linear_speed(void);
float get_ideal_angular_speed(void);
float get_measured_linear_speed(void);
float get_measured_angular_speed(void);
void motor_control(void);
void set_target_linear_speed(float speed);
void set_ideal_angular_speed(float speed);
void update_ideal_linear_speed(void);

#endif /* __CONTROL_H */
