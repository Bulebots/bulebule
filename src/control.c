#include "control.h"

static volatile float target_linear_speed;
static volatile float target_angular_speed;
static volatile float ideal_linear_speed;
static volatile float ideal_angular_speed;

static volatile float kp_linear = 800.;
static volatile float kd_linear = 1600.;
static volatile float kp_angular = 5.;
static volatile float kd_angular = 100.;
static volatile float ki_angular_front = 200.;
static volatile float ki_angular_side = 400.;
static volatile float kp_angular_front = 50.;
static volatile float kp_angular_side = 200.;
static volatile float linear_error;
static volatile float angular_error;
static volatile float last_linear_error;
static volatile float last_angular_error;

static volatile int32_t pwm_left;
static volatile int32_t pwm_right;

static volatile bool collision_detected_signal;
static volatile bool motor_control_enabled_signal;
static volatile bool side_sensors_control_enabled;
static volatile bool front_sensors_control_enabled;
static volatile float side_sensors_integral;
static volatile float front_sensors_integral;

float get_kp_linear(void)
{
	return kp_linear;
}

void set_kp_linear(float value)
{
	kp_linear = value;
}

float get_kd_linear(void)
{
	return kd_linear;
}
void set_kd_linear(float value)
{
	kd_linear = value;
}

float get_kp_angular(void)
{
	return kp_angular;
}

void set_kp_angular(float value)
{
	kp_angular = value;
}

float get_kd_angular(void)
{
	return kd_angular;
}

void set_kd_angular(float value)
{
	kd_angular = value;
}

float get_kp_angular_side(void)
{
	return kp_angular_side;
}

void set_kp_angular_side(float value)
{
	kp_angular_side = value;
}

float get_kp_angular_front(void)
{
	return kp_angular_front;
}

void set_kp_angular_front(float value)
{
	kp_angular_front = value;
}

float get_ki_angular_side(void)
{
	return ki_angular_side;
}

void set_ki_angular_side(float value)
{
	ki_angular_side = value;
}

float get_ki_angular_front(void)
{
	return ki_angular_front;
}

void set_ki_angular_front(float value)
{
	ki_angular_front = value;
}

/**
 * @brief Enable or disable the side sensors control.
 */
void side_sensors_control(bool value)
{
	side_sensors_control_enabled = value;
}

/**
 * @brief Enable or disable the front sensors control.
 */
void front_sensors_control(bool value)
{
	front_sensors_control_enabled = value;
}

/**
 * @brief Set collision detected signal.
 *
 * It also automatically disables the motor control.
 */
static void set_collision_detected(void)
{
	collision_detected_signal = true;
	motor_control_enabled_signal = false;
}

/**
 * @brief Returns true if a collision was detected.
 */
bool collision_detected(void)
{
	return collision_detected_signal;
}

/**
 * @brief Reset the collision detection signal.
 *
 * This will also reset the PWM saturation counters, used for collision
 * detection.
 */
void reset_collision_detection(void)
{
	collision_detected_signal = false;
	reset_pwm_saturation();
}

/**
 * @brief Reset control error variables.
 */
void reset_control_errors(void)
{
	side_sensors_integral = 0;
	front_sensors_integral = 0;
	linear_error = 0;
	angular_error = 0;
	last_linear_error = 0;
	last_angular_error = 0;
}

/**
 * @brief Reset control speed variables.
 */
void reset_control_speed(void)
{
	target_linear_speed = 0.;
	target_angular_speed = 0.;
	ideal_linear_speed = 0.;
	ideal_angular_speed = 0.;
}

/**
 * @brief Reset all control variables.
 *
 * In particular:
 *
 * - Reset control errors.
 * - Reset control speed.
 * - Reset collision detection.
 */
void reset_control_all(void)
{
	reset_control_errors();
	reset_control_speed();
	reset_collision_detection();
}

/**
 * @brief Enable the motor control.
 *
 * This means the motor control function will be executed the PWM output will be
 * generated.
 */
void enable_motor_control(void)
{
	motor_control_enabled_signal = true;
}

/**
 * @brief Disable the motor control.
 *
 * This means the motor control function will not be executed and no PWM output
 * will be generated.
 */
void disable_motor_control(void)
{
	motor_control_enabled_signal = false;
}

/**
 * @brief Return the current PWM duty for the left motor.
 */
float get_left_pwm(void)
{
	return pwm_left;
}

/**
 * @brief Return the current PWM duty for the right motor.
 */
float get_right_pwm(void)
{
	return pwm_right;
}

/**
 * @brief Return the current target linear speed in meters per second.
 */
float get_target_linear_speed(void)
{
	return target_linear_speed;
}

/**
 * @brief Return the current target angular speed in degrees per second.
 */
float get_target_angular_speed(void)
{
	return target_angular_speed;
}

/**
 * @brief Return the current ideal linear speed in meters per second.
 */
float get_ideal_linear_speed(void)
{
	return ideal_linear_speed;
}

/**
 * @brief Return the current ideal angular speed in degrees per second.
 */
float get_ideal_angular_speed(void)
{
	return ideal_angular_speed;
}

/**
 * @brief Return the current measured linear speed in meters per second.
 */
float get_measured_linear_speed(void)
{
	return (get_encoder_left_speed() + get_encoder_right_speed()) / 2.;
}

/**
 * @brief Return the current measured angular speed in radians per second.
 */
float get_measured_angular_speed(void)
{
	return -get_gyro_z_radps();
}

/**
 * @brief Set target linear speed in meters per second.
 */
void set_target_linear_speed(float speed)
{
	target_linear_speed = speed;
}

/**
 * @brief Set target angular speed in degrees per second.
 */
void set_target_angular_speed(float speed)
{
	target_angular_speed = speed;
}

/**
 * @brief Update ideal speed according to the defined speed profile.
 *
 * Current ideal speed is increased or decreased according to the target speed
 * and the defined maximum acceleration and deceleration.
 */
void update_ideal_speed(void)
{
	if (ideal_linear_speed < target_linear_speed) {
		ideal_linear_speed +=
		    get_linear_acceleration() / SYSTICK_FREQUENCY_HZ;
		if (ideal_linear_speed > target_linear_speed)
			ideal_linear_speed = target_linear_speed;
	} else if (ideal_linear_speed > target_linear_speed) {
		ideal_linear_speed -=
		    get_linear_deceleration() / SYSTICK_FREQUENCY_HZ;
		if (ideal_linear_speed < target_linear_speed)
			ideal_linear_speed = target_linear_speed;
	}
	if (ideal_angular_speed < target_angular_speed) {
		ideal_angular_speed +=
		    get_angular_acceleration() / SYSTICK_FREQUENCY_HZ;
		if (ideal_angular_speed > target_angular_speed)
			ideal_angular_speed = target_angular_speed;
	} else if (ideal_angular_speed > target_angular_speed) {
		ideal_angular_speed -=
		    get_angular_acceleration() / SYSTICK_FREQUENCY_HZ;
		if (ideal_angular_speed < target_angular_speed)
			ideal_angular_speed = target_angular_speed;
	}
}

/**
 * @brief Execute the robot motor control.
 *
 * Set the motors power to try to follow a defined speed profile.
 *
 * This function also implements collision detection by checking PWM output
 * saturation. If collision is detected it sets the `collision_detected_signal`
 * variable to `true`.
 */
void motor_control(void)
{
	float linear_pwm;
	float angular_pwm;
	float side_sensors_feedback;
	float front_sensors_feedback;

	if (!motor_control_enabled_signal)
		return;

	if (side_sensors_control_enabled) {
		side_sensors_feedback = get_side_sensors_error();
		side_sensors_integral += side_sensors_feedback;
	} else {
		side_sensors_feedback = 0;
	}

	if (front_sensors_control_enabled) {
		front_sensors_feedback = get_front_sensors_error();
		front_sensors_integral += front_sensors_feedback;

	} else {
		front_sensors_feedback = 0;
	}

	linear_error += ideal_linear_speed - get_measured_linear_speed();
	angular_error += ideal_angular_speed - get_measured_angular_speed();

	linear_pwm = kp_linear * linear_error +
		     kd_linear * (linear_error - last_linear_error);
	angular_pwm = kp_angular * angular_error +
		      kd_angular * (angular_error - last_angular_error) +
		      kp_angular_side * side_sensors_feedback +
		      kp_angular_front * front_sensors_feedback +
		      ki_angular_side * side_sensors_integral +
		      ki_angular_front * front_sensors_integral;

	pwm_left = (int32_t)(linear_pwm + angular_pwm);
	pwm_right = (int32_t)(linear_pwm - angular_pwm);

	power_left(pwm_left);
	power_right(pwm_right);

	last_linear_error = linear_error;
	last_angular_error = angular_error;

	if (pwm_saturation() > MAX_PWM_SATURATION_PERIOD * SYSTICK_FREQUENCY_HZ)
		set_collision_detected();
}
