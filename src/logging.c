#include "logging.h"

/**
 * @brief Log the current battery voltage.
 */
void log_battery_voltage(void)
{
	LOG_INFO("%f", get_battery_voltage());
}

/**
 * @brief Log all the configuration variables.
 */
void log_configuration_variables(void)
{
	float micrometers_per_count = get_micrometers_per_count();
	float wheels_separation = get_wheels_separation();
	float max_linear_speed = get_max_linear_speed();
	float linear_acceleration = get_linear_acceleration();
	float linear_deceleration = get_linear_deceleration();
	float angular_acceleration = get_angular_acceleration();
	float kp_linear = get_kp_linear();
	float kd_linear = get_kd_linear();
	float kp_angular = get_kp_angular();
	float kd_angular = get_kd_angular();
	float ki_angular_side = get_ki_angular_side();
	float ki_angular_front = get_ki_angular_front();
	float side_sensors_error_factor = get_side_sensors_error_factor();
	float front_sensors_error_factor = get_front_sensors_error_factor();

	LOG_INFO("{\"micrometers_per_count\":%f,"
		 "\"wheels_separation\":%f,"
		 "\"max_linear_speed\":%f,"
		 "\"linear_acceleration\":%f,"
		 "\"linear_deceleration\":%f,"
		 "\"angular_acceleration\":%f,"
		 "\"kp_linear\":%f,"
		 "\"kd_linear\":%f,"
		 "\"kp_angular\":%f,"
		 "\"kd_angular\":%f,"
		 "\"ki_angular_side\":%f,"
		 "\"ki_angular_front\":%f,"
		 "\"side_sensors_error_factor\":%f,"
		 "\"front_sensors_error_factor\":%f}",
		 micrometers_per_count, wheels_separation, max_linear_speed,
		 linear_acceleration, linear_deceleration, angular_acceleration,
		 kp_linear, kd_linear, kp_angular, kd_angular, ki_angular_side,
		 ki_angular_front, side_sensors_error_factor,
		 front_sensors_error_factor);
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

/**
 * @brief Log all sensor distance readings, published for real-time.
 */
void log_sensors_distance_pub(void)
{
	float reading;

	reading = get_side_left_distance();
	LOG_INFO("PUB,line,left-side,%f", reading);
	reading = get_front_left_distance();
	LOG_INFO("PUB,line,left-front,%f", reading);
	reading = get_front_right_distance();
	LOG_INFO("PUB,line,right-front,%f", reading);
	reading = get_side_right_distance();
	LOG_INFO("PUB,line,right-side,%f", reading);
}

/**
 * @brief Log all sensors raw readings.
 */
void log_sensors_raw(void)
{
	uint16_t off[NUM_SENSOR];
	uint16_t on[NUM_SENSOR];

	get_sensors_raw(off, on);

	LOG_INFO("OFF-ON,%d,%d,%d,%d,%d,%d,%d,%d", off[SENSOR_SIDE_LEFT_ID],
		 off[SENSOR_SIDE_RIGHT_ID], off[SENSOR_FRONT_LEFT_ID],
		 off[SENSOR_FRONT_RIGHT_ID], on[SENSOR_SIDE_LEFT_ID],
		 on[SENSOR_SIDE_RIGHT_ID], on[SENSOR_FRONT_LEFT_ID],
		 on[SENSOR_FRONT_RIGHT_ID]);
}

/**
 * @brief Log all sensor raw readings, published for real-time.
 */
void log_sensors_raw_pub(void)
{
	uint16_t off[NUM_SENSOR];
	uint16_t on[NUM_SENSOR];

	get_sensors_raw(off, on);

	LOG_INFO("PUB,line,left-side-raw-on,%u", on[SENSOR_SIDE_LEFT_ID]);
	LOG_INFO("PUB,line,left-front-raw-on,%u", on[SENSOR_FRONT_LEFT_ID]);
	LOG_INFO("PUB,line,right-front-raw-on,%u", on[SENSOR_FRONT_RIGHT_ID]);
	LOG_INFO("PUB,line,right-side-raw-on,%u", on[SENSOR_SIDE_RIGHT_ID]);
	LOG_INFO("PUB,line,left-side-raw-off,%u", off[SENSOR_SIDE_LEFT_ID]);
	LOG_INFO("PUB,line,left-front-raw-off,%u", off[SENSOR_FRONT_LEFT_ID]);
	LOG_INFO("PUB,line,right-front-raw-off,%u", off[SENSOR_FRONT_RIGHT_ID]);
	LOG_INFO("PUB,line,right-side-raw-off,%u", off[SENSOR_SIDE_RIGHT_ID]);
}

/**
 * @brief Log front sensors variables for calibration.
 */
void log_front_sensors_calibration(void)
{
	float sensors_error = get_front_sensors_error();
	float left_distance = get_front_left_distance();
	float right_distance = get_front_right_distance();
	uint16_t off[NUM_SENSOR];
	uint16_t on[NUM_SENSOR];
	int32_t micrometers = get_encoder_average_micrometers();

	get_sensors_raw(off, on);

	LOG_INFO("{\"micrometers\":%" PRId32 ","
		 "\"left_raw_on\":%d,"
		 "\"left_raw_off\":%d,"
		 "\"right_raw_on\":%d,"
		 "\"right_raw_off\":%d,"
		 "\"left_distance\":%f,"
		 "\"right_distance\":%f,"
		 "\"distance_error\":%f}",
		 micrometers, on[SENSOR_FRONT_LEFT_ID],
		 off[SENSOR_FRONT_LEFT_ID], on[SENSOR_FRONT_RIGHT_ID],
		 off[SENSOR_FRONT_RIGHT_ID], left_distance, right_distance,
		 sensors_error);
}

/**
 * @brief Log front sensors distances and error.
 */
void log_front_sensors_error(void)
{
	float sensors_error = get_front_sensors_error();
	float fl_dist = get_front_left_distance();
	float fr_dist = get_front_right_distance();

	LOG_INFO("{\"front_sensors_error\":%f,"
		 "\"front_left_distance\":%f,"
		 "\"front_right_distance\":%f}",
		 sensors_error, fl_dist, fr_dist);
}

/**
 * @brief Log side sensors distances and error.
 */
void log_side_sensors_error(void)
{
	float sensors_error = get_side_sensors_error();
	float sl_dist = get_side_left_distance();
	float sr_dist = get_side_right_distance();

	LOG_INFO("{\"front_sensors_error\":%f,"
		 "\"side_left_distance\":%f,"
		 "\"side_right_distance\":%f}",
		 sensors_error, sl_dist, sr_dist);
}

/**
 * @brief Log the result of walls detection.
 */
void log_walls_detection(void)
{
	bool front_wall = front_wall_detection();
	bool right_wall = right_wall_detection();
	bool left_wall = left_wall_detection();

	LOG_INFO("{\"wall_left\":%d,"
		 "\"wall_right\":%d,"
		 "\"wall_front\":%d}",
		 left_wall, right_wall, front_wall);
}
