#include "calibration.h"
#include "clock.h"
#include "control.h"
#include "detection.h"
#include "encoder.h"
#include "hmi.h"
#include "logging.h"
#include "motor.h"
#include "move.h"
#include "mpu.h"
#include "search.h"
#include "serial.h"
#include "setup.h"
#include "solve.h"
#include "speed.h"

static bool solved;
static void competition(void);

/**
 * @brief Handle the SysTick interruptions.
 */
void sys_tick_handler(void)
{
	clock_tick();
	update_ideal_speed();
	update_distance_readings();
	update_gyro_readings();
	update_encoder_readings();
	motor_control();
}

/**
 * @brief Solve an unknown maze.
 */
static void solve(void)
{
	if (!solved)
		initialize_search();

	reset_motion();
	enable_motor_control();
	set_starting_position();

	initialize_search();
	/* set_goal_classic(); */
	add_goal(3, 5);
	set_target_goal();

	explore();

	stop_middle();
	solved = true;
}

/**
 * @brief Includes the user configurations.
 */
static void user_configuration(void)
{
	uint8_t mode;

	mode = speed_mode_configuration();
	set_speed_mode(mode);
}

/**
 * @brief Includes the functions to be executed before robot starts to move.
 */
static void before_moving(void)
{
	reset_motion();
	disable_walls_control();
	repeat_blink(10, 100);
	sleep_us(5000000);
	led_left_on();
	led_right_on();
	wait_front_sensor_close_signal(0.12);
	led_left_off();
	led_right_off();
	sleep_us(2000000);
	calibrate();
	enable_motor_control();
}

/**
 * @brief Functions to be executed when the solve phase is finished.
 */
static void after_moving(void)
{
	if (collision_detected()) {
		reset_motion();
		blink_collision();
	} else {
		repeat_blink(10, 100);
	}
	reset_motion();
}

/**
 * @brief Includes the functions to be executed during exploration phase.
 */
static void exploration(void)
{
	user_configuration();
	before_moving();
	solve();
	after_moving();
}

/**
 * @brief Includes the functions to be executed during the run phase.
 */
static void run(void)
{
	user_configuration();
	before_moving();
	solve();
	after_moving();
}

/**
 * @brief Competition routine.
 */
static void competition(void)
{
	setup();
	systick_interrupt_enable();
	exploration();
	while (1)
		run();
}

/**
 * @brief Initial setup and infinite wait.
 */
int main(void)
{
	setup();
	systick_interrupt_enable();
	initialize_solver_direction();
	set_speed_mode(0);
	while (1) {
		if (button_left_read_consecutive(500)) {
			before_moving();
			solve();
			after_moving();
		}
		execute_commands();
	}

	return 0;
}
