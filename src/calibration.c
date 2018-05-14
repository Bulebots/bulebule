#include "calibration.h"

/**
 * @brief Run a full linear profile test.
 *
 * The robot will accelerate and maintain the target speed for 0.5 m and then
 * decelerate back to zero speed. There is no angular speed in this test, only
 * linear movement. During all this test information about the relevant linear
 * speed variables is logged periodically for later analysis.
 */
void run_linear_speed_profile(void)
{
	int32_t start_micrometers;

	disable_walls_control();
	enable_motor_control();
	each(10, log_linear_speed, 1000);
	set_target_angular_speed(0.);
	set_target_linear_speed(get_max_linear_speed());
	start_micrometers = get_encoder_average_micrometers();
	while (get_encoder_average_micrometers() - start_micrometers < 500000) {
		log_linear_speed();
		sleep_ticks(1);
	}
	set_target_angular_speed(0.);
	set_target_linear_speed(0.);
	each(1, log_linear_speed, 2000);
	reset_motion();
}

/**
 * @brief Run a full angular profile test.
 *
 * The robot will accelerate, maintain the target speed for a while and then
 * decelerate back to zero speed. There is no linear speed in this test, only
 * rotational movement. During all this test information about the relevant
 * angular speed variables is logged periodically for later analysis.
 *
 * The target turn is 3 * PI radians.
 */
void run_angular_speed_profile(void)
{
	float target_angular_speed = 4 * PI;

	disable_walls_control();
	enable_motor_control();
	each(10, log_angular_speed, 1000);
	set_target_angular_speed(target_angular_speed);
	set_target_linear_speed(0.);
	each(10, log_angular_speed, 1000 * (3 * PI) / target_angular_speed);
	set_target_angular_speed(0.);
	set_target_linear_speed(0.);
	each(10, log_angular_speed, 2000);
	reset_motion();
}

/**
 * @brief Run a static 90-degree right turn speed profile.
 *
 * The robot will accelerate to reach the target angular speed and decelerate
 * just in time to complete the 90 degree turn.
 */
void run_static_turn_right_profile(void)
{
	float target_angular_speed = 4 * PI;

	disable_walls_control();
	enable_motor_control();
	each(10, log_angular_speed, 1000);
	set_target_angular_speed(target_angular_speed);
	set_target_linear_speed(0.);
	each(10, log_angular_speed, 1000 * (PI / 2.) / target_angular_speed);
	set_target_angular_speed(0);
	set_target_linear_speed(0.);
	each(10, log_angular_speed, 200);
	reset_motion();
}

/**
 * Execute simple movement command sequences.
 *
 * - 'O': to get out of the starting cell.
 * - 'F': to move front.
 * - 'L': to move left.
 * - 'R': to move right.
 * - 'B': to move back.
 * - 'M': to stop at the middle of the cell.
 * - 'H': to stop touching the front wall of the cell.
 * - 'E': to stop at the end of the cell.
 * - 'l': to turn left (in place).
 * - 'r': to turn right (in place).
 * - 'b': to turn back (in place).
 */
void run_movement_sequence(const char *sequence)
{
	char movement;

	reset_motion();
	enable_motor_control();
	while (true) {
		movement = *sequence++;
		if (!movement)
			break;
		switch (movement) {
		case 'O':
			set_starting_position();
			move_front();
			break;
		case 'F':
			move_front();
			break;
		case 'L':
			move_left();
			break;
		case 'R':
			move_right();
			break;
		case 'B':
			move_back();
			break;
		case 'M':
			stop_middle();
			break;
		case 'H':
			stop_head_front_wall();
			break;
		case 'E':
			stop_end();
			break;
		case 'l':
			turn_left();
			break;
		case 'r':
			turn_right();
			break;
		case 'b':
			turn_back();
			break;
		default:
			break;
		}
	}
	reset_motion();
}

/**
 * @brief Front sensors calibration funtion.
 *
 * Assumes the robot is positioned at the start of a cell, with its tail
 * touching the back wall. It will accelerate in a straight line and will stop
 * touching its nose with the next cell's front wall.
 *
 * During this movement the robot will be logging information about the front
 * sensors.
 */
void run_front_sensors_calibration(void)
{
	float calibration_linear_speed = .3;
	float linear_acceleration = get_linear_acceleration();
	float linear_deceleration = get_linear_deceleration();
	float required_deceleration;
	int32_t target_micrometers;
	int32_t micrometers_to_stop;
	uint32_t ticks_to_stop;

	disable_walls_control();
	enable_motor_control();

	set_linear_acceleration(3.);

	target_micrometers =
	    get_encoder_average_micrometers() +
	    (2 * CELL_DIMENSION - MOUSE_TAIL - MOUSE_HEAD - WALL_WIDTH) *
		MICROMETERS_PER_METER;
	set_target_angular_speed(0.);
	set_target_linear_speed(calibration_linear_speed);
	micrometers_to_stop = 15000;
	while (get_encoder_average_micrometers() <
	       target_micrometers - micrometers_to_stop) {
		log_front_sensors_calibration();
		sleep_us(1000);
	}
	required_deceleration =
	    (calibration_linear_speed * calibration_linear_speed) /
	    (2 *
	     (float)(target_micrometers - get_encoder_average_micrometers()) /
	     MICROMETERS_PER_METER);
	set_linear_deceleration(required_deceleration);
	ticks_to_stop = (uint32_t)(required_time_to_speed(0.) * 1000);
	set_target_linear_speed(0.);
	each(2, log_front_sensors_calibration, ticks_to_stop);

	reset_motion();

	set_linear_acceleration(linear_acceleration);
	set_linear_deceleration(linear_deceleration);

	blink_burst();
}
