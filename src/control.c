#include "control.h"

static volatile float target_linear_speed;
static volatile float ideal_linear_speed;

static volatile float kp_linear = 10.;
static volatile float kd_linear = 0.;

static volatile int32_t pwm_left;
static volatile int32_t pwm_right;

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
 * @brief Return the current ideal linear speed in meters per second.
 */
float get_ideal_linear_speed(void)
{
	return ideal_linear_speed;
}

/**
 * @brief Set target linear speed in meters per second.
 */
void set_target_linear_speed(float speed)
{
	target_linear_speed = speed;
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
		ideal_linear_speed += MAX_ACCELERATION / SYSTICK_FREQUENCY_HZ;
		if (ideal_linear_speed > target_linear_speed)
			ideal_linear_speed = target_linear_speed;
	} else if (ideal_linear_speed > target_linear_speed) {
		ideal_linear_speed -= MAX_DECELERATION / SYSTICK_FREQUENCY_HZ;
		if (ideal_linear_speed < target_linear_speed)
			ideal_linear_speed = target_linear_speed;
	}
}

/**
 * @brief Execute the robot motor control.
 *
 * Set the motors power to try to follow a defined speed profile.
 */
void motor_control(void)
{
	static float linear_error;
	static float last_linear_error;

	float encoder_feedback_linear;
	float linear_pwm;

	encoder_feedback_linear =
	    (get_encoder_left_speed() + get_encoder_right_speed()) / 2.;

	linear_error += ideal_linear_speed - encoder_feedback_linear;

	linear_pwm = kp_linear * linear_error +
		     kd_linear * (linear_error - last_linear_error);

	pwm_left = (int32_t)linear_pwm;
	pwm_right = (int32_t)linear_pwm;

	power_left(pwm_left);
	power_right(pwm_right);

	last_linear_error = linear_error;
}
