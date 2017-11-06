#include "control.h"

/**
 * Maximum acceleration and deceleration.
 *
 * - Linear acceleration is defined in meters per second squared.
 * - Angular acceleration is defined in radians per second squared.
 */
static volatile float linear_acceleration = 5.;
static volatile float linear_deceleration = 5.;
static volatile float angular_acceleration = 32. * PI;

static volatile float target_linear_speed;
static volatile float target_angular_speed;
static volatile float ideal_linear_speed;
static volatile float ideal_angular_speed;

static volatile float kp_linear = 1600.;
static volatile float kd_linear = 100.;
static volatile float kp_angular = 60.;
static volatile float kd_angular = 50.;
static volatile float ki_angular_front = 50.;
static volatile float ki_angular_side = 40.;
static volatile float side_sensors_error_factor = 50.;
static volatile float front_sensors_error_factor = 20.;

static volatile int32_t pwm_left;
static volatile int32_t pwm_right;

static volatile bool collision_detected_signal;
static volatile bool side_sensors_control_enabled;
static volatile bool front_sensors_control_enabled;
static volatile float side_sensors_integral;
static volatile float front_sensors_integral;

float get_linear_acceleration(void)
{
	return linear_acceleration;
}

void set_linear_acceleration(float value)
{
	linear_acceleration = value;
}

float get_linear_deceleration(void)
{
	return linear_deceleration;
}

void set_linear_deceleration(float value)
{
	linear_deceleration = value;
}

float get_angular_acceleration(void)
{
	return angular_acceleration;
}

void set_angular_acceleration(float value)
{
	angular_acceleration = value;
}

float get_side_sensors_error_factor(void)
{
	return side_sensors_error_factor;
}

void set_side_sensors_error_factor(float value)
{
	side_sensors_error_factor = value;
}

float get_front_sensors_error_factor(void)
{
	return front_sensors_error_factor;
}

void set_front_sensors_error_factor(float value)
{
	front_sensors_error_factor = value;
}

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
 *
 * The integral variable is initialized to 0 on disable.
 */
void side_sensors_control(bool value)
{
	side_sensors_control_enabled = value;
	if (value == false)
		side_sensors_integral = 0;
}

/**
 * @brief Enable or disable the front sensors control.
 *
 * The integral variable is initialized to 0 on disable.
 */
void front_sensors_control(bool value)
{
	front_sensors_control_enabled = value;
	if (value == false)
		front_sensors_integral = 0;
}

/**
 * @brief Returns true if a collision was detected.
 */
bool collision_detected(void)
{
	return collision_detected_signal;
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
		    linear_acceleration / SYSTICK_FREQUENCY_HZ;
		if (ideal_linear_speed > target_linear_speed)
			ideal_linear_speed = target_linear_speed;
	} else if (ideal_linear_speed > target_linear_speed) {
		ideal_linear_speed -=
		    linear_acceleration / SYSTICK_FREQUENCY_HZ;
		if (ideal_linear_speed < target_linear_speed)
			ideal_linear_speed = target_linear_speed;
	}
	if (ideal_angular_speed < target_angular_speed) {
		ideal_angular_speed +=
		    angular_acceleration / SYSTICK_FREQUENCY_HZ;
		if (ideal_angular_speed > target_angular_speed)
			ideal_angular_speed = target_angular_speed;
	} else if (ideal_angular_speed > target_angular_speed) {
		ideal_angular_speed -=
		    angular_acceleration / SYSTICK_FREQUENCY_HZ;
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
	static float linear_error;
	static float angular_error;
	static float last_linear_error;
	static float last_angular_error;

	float left_speed;
	float right_speed;
	float encoder_feedback_linear;
	float encoder_feedback_angular;
	float linear_pwm;
	float angular_pwm;
	float side_sensors_feedback;
	float front_sensors_feedback;

	left_speed = get_encoder_left_speed();
	right_speed = get_encoder_right_speed();
	encoder_feedback_linear = (left_speed + right_speed) / 2.;
	encoder_feedback_angular = get_encoder_angular_speed();

	if (side_sensors_control_enabled) {
		side_sensors_feedback =
		    get_side_sensors_error() * side_sensors_error_factor;
		side_sensors_integral += side_sensors_feedback;
	} else {
		side_sensors_feedback = 0;
		side_sensors_integral = 0;
	}

	if (front_sensors_control_enabled) {
		front_sensors_feedback =
		    get_front_sensors_error() * front_sensors_error_factor;
		front_sensors_integral += front_sensors_feedback;

	} else {
		front_sensors_feedback = 0;
		front_sensors_integral = 0;
	}

	linear_error += ideal_linear_speed - encoder_feedback_linear;
	angular_error += ideal_angular_speed - encoder_feedback_angular;

	linear_pwm = kp_linear * linear_error +
		     kd_linear * (linear_error - last_linear_error);
	angular_pwm = kp_angular * (angular_error + side_sensors_feedback +
				    front_sensors_feedback) +
		      kd_angular * (angular_error - last_angular_error) +
		      ki_angular_side * side_sensors_integral +
		      ki_angular_front * front_sensors_integral;

	pwm_left = (int32_t)(linear_pwm + angular_pwm);
	pwm_right = (int32_t)(linear_pwm - angular_pwm);

	power_left(pwm_left);
	power_right(pwm_right);

	last_linear_error = linear_error;
	last_angular_error = angular_error;

	if (pwm_saturation() > MAX_PWM_SATURATION_PERIOD * SYSTICK_FREQUENCY_HZ)
		collision_detected_signal = true;
}
