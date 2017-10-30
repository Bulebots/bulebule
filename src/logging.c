#include "logging.h"

/**
 * @brief Log the current battery voltage.
 */
void log_battery_voltage(void)
{
	LOG_INFO("%f", get_battery_voltage());
}

/**
 * @brief Log information about linear speed relevant variables.
 *
 * These include:
 *
 * - Target linear speed and ideal (expected) linear speed.
 * - Actual speed of both wheels (left and right).
 * - PWM output value for both motors.
 */
void log_linear_speed(void)
{
	float left_speed = get_encoder_left_speed();
	float right_speed = get_encoder_right_speed();
	float target_speed = get_target_linear_speed();
	float ideal_speed = get_ideal_linear_speed();
	int pwm_left = get_left_pwm();
	int pwm_right = get_right_pwm();

	LOG_INFO("%f,%f,%f,%f,%d,%d", target_speed, ideal_speed, left_speed,
		 right_speed, pwm_left, pwm_right);
}
