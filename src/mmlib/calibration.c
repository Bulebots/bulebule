#include "calibration.h"

/**
 * @brief Calibrate side sensors and gyroscope's Z axis.
 *
 * Should be executed only when the robot is static in the middle of a cell.
 */
void calibrate(void)
{
	side_sensors_calibration();
	systick_interrupt_disable();
	gyro_z_calibration();
	systick_interrupt_enable();
}

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

	calibrate();
	disable_walls_control();
	enable_motor_control();
	each(10, log_linear_speed, 1000);
	set_ideal_angular_speed(0.);
	set_target_linear_speed(get_max_linear_speed());
	start_micrometers = get_encoder_average_micrometers();
	while (get_encoder_average_micrometers() - start_micrometers < 500000) {
		log_linear_speed();
		sleep_ticks(1);
	}
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
	calibrate();
	disable_walls_control();
	enable_motor_control();
	start_data_logging(log_angular_speed);
	sleep_seconds(.1);
	inplace_turn(3 * PI, 0.25);
	sleep_seconds(.1);
	stop_data_logging();
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
	calibrate();
	disable_walls_control();
	enable_motor_control();
	start_data_logging(log_angular_speed);
	sleep_seconds(.1);
	inplace_turn(PI / 2, 0.25);
	sleep_seconds(.1);
	stop_data_logging();
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
 * - 's': to stop now.
 * - 'k': keep half cell front distance.
 * - 'j': keep one cell front distance.
 */
void run_movement_sequence(const char *sequence)
{
	char movement;
	uint8_t force = 0.25;

	calibrate();
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
			move_side(LEFT, force);
			break;
		case 'R':
			move_side(RIGHT, force);
			break;
		case 'B':
			move_back(force);
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
			speed_turn(MOVE_LEFT, force);
			break;
		case 'r':
			speed_turn(MOVE_RIGHT, force);
			break;
		case 'b':
			turn_back(force);
			break;
		case 's':
			set_target_linear_speed(0.);
			sleep_seconds(1);
			break;
		case 'k':
			keep_front_wall_distance(CELL_DIMENSION / 2.);
			break;
		case 'j':
			keep_front_wall_distance(CELL_DIMENSION);
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
	float linear_acceleration = get_linear_acceleration();
	float linear_deceleration = get_linear_deceleration();
	float distance;

	calibrate();
	disable_walls_control();
	enable_motor_control();

	set_linear_acceleration(2.);
	set_linear_deceleration(2.);

	distance = 2 * CELL_DIMENSION - MOUSE_LENGTH - WALL_WIDTH;

	start_data_logging(log_data_front_sensors_calibration);
	target_straight(get_encoder_average_micrometers(), distance, 0.);
	stop_data_logging();

	reset_motion();

	set_linear_acceleration(linear_acceleration);
	set_linear_deceleration(linear_deceleration);

	repeat_blink(10, 100);
}
