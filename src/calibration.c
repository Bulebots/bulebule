#include "calibration.h"

/**
 * @brief Run a full linear profile test.
 *
 * The robot will accelerate, maintain the target speed for a while and then
 * decelerate back to zero speed. There is no angular speed in this test, only
 * linear movement. During all this test information about the relevant linear
 * speed variables is logged periodically for later analysis.
 */
void run_linear_speed_profile(void)
{
	each(10, log_linear_speed, 1000);
	set_target_angular_speed(0.);
	set_target_linear_speed(.5);
	each(10, log_linear_speed, 1000);
	set_target_angular_speed(0.);
	set_target_linear_speed(0.);
	each(10, log_linear_speed, 2000);
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

	each(10, log_angular_speed, 1000);
	set_target_angular_speed(target_angular_speed);
	set_target_linear_speed(0.);
	each(10, log_angular_speed, 1000 * (3 * PI) / target_angular_speed);
	set_target_angular_speed(0.);
	set_target_linear_speed(0.);
	each(10, log_angular_speed, 2000);
}

/**
 * @brief Run a profiling test for sensors distance calculation.
 *
 * This test executes 1000 times the function that reads and transforms sensors
 * input to distance. Then, it logs the total time on clock ticks.
 */
void run_distances_profiling(void)
{
	uint32_t clock_tick_start, clock_tick_end;
	uint16_t i;

	clock_tick_start = get_clock_ticks();
	for (i = 0; i < 1000; i++)
		update_distance_readings();
	clock_tick_end = get_clock_ticks();
	LOG_INFO("Clock ticks %" PRIu32, (clock_tick_end - clock_tick_start));
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

	each(10, log_angular_speed, 1000);
	set_target_angular_speed(target_angular_speed);
	set_target_linear_speed(0.);
	each(10, log_angular_speed, 1000 * (PI / 2.) / target_angular_speed);
	set_target_angular_speed(0);
	set_target_linear_speed(0.);
	each(10, log_angular_speed, 200);
}
