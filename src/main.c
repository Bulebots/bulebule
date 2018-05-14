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
static void solve(void);

/**
 * @brief Handle the SysTick interruptions.
 */
void sys_tick_handler(void)
{
	clock_tick();
	update_ideal_speed();
	update_encoder_readings();
	update_distance_readings();
	update_gyro_readings();
	motor_control();
}

/**
 * @brief Solve an unknown maze.
 */
void solve(void)
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
 * @brief Initial setup and infinite wait.
 */
int main(void)
{
	setup();
	systick_interrupt_disable();
	gyro_z_calibration();
	systick_interrupt_enable();
	initialize_solver_direction();
	set_speed_mode(0);
	while (1) {
		if (button_left_read_consecutive(500)) {
			reset_motion();
			disable_walls_control();
			enable_motor_control();
			blink_burst();
			sleep_ticks(5000);
			led_left_on();
			led_right_on();
			wait_front_sensor_close_signal(0.12);
			led_left_off();
			led_right_off();
			sleep_ticks(2000);
			side_sensors_calibration();
			solve();
			if (collision_detected()) {
				reset_motion();
				blink_collision();
			} else {
				blink_burst();
			}
			reset_motion();
		}
		execute_commands();
	}

	return 0;
}
