#include "logging.h"

/**
 * @brief Log the current battery voltage.
 */
void log_battery_voltage(void)
{
	LOG_INFO("%f", get_battery_voltage());
}

/**
 * @brief Log all the control variables.
 */
void log_control_variables(void)
{
	float kp_linear = get_kp_linear();
	float kd_linear = get_kd_linear();
	float kp_angular = get_kp_angular();
	float kd_angular = get_kd_angular();

	LOG_INFO("%f,%f,%f,%f", kp_linear, kd_linear, kp_angular, kd_angular);
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

/**
 * @brief Log information about angular speed relevant variables.
 *
 * These include:
 *
 * - Target angular speed and ideal (expected) angular speed.
 * - Actual calculated angular speed.
 * - PWM output value for both motors.
 */
void log_angular_speed(void)
{
	float angular_speed = get_encoder_angular_speed();
	float target_speed = get_target_angular_speed();
	float ideal_speed = get_ideal_angular_speed();
	int pwm_left = get_left_pwm();
	int pwm_right = get_right_pwm();

	LOG_INFO("%f,%f,%f,%d,%d", target_speed, ideal_speed, angular_speed,
		 pwm_left, pwm_right);
}

/**
 * @brief Log all sensor distance readings.
 */
void log_sensors_distance(void)
{
	float sl_dist = get_side_left_distance();
	float sr_dist = get_side_right_distance();
	float fl_dist = get_front_left_distance();
	float fr_dist = get_front_right_distance();

	LOG_INFO("%f,%f,%f,%f", sl_dist, sr_dist, fl_dist, fr_dist);
}
