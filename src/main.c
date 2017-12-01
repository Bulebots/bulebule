#include "calibration.h"
#include "clock.h"
#include "control.h"
#include "detection.h"
#include "encoder.h"
#include "hmi.h"
#include "logging.h"
#include "motor.h"
#include "move.h"
#include "search.h"
#include "serial.h"
#include "setup.h"
#include "solve.h"

static void standard_run(void);

/**
 * @brief Handle the SysTick interruptions.
 */
void sys_tick_handler(void)
{
	clock_tick();
	update_ideal_speed();
	update_encoder_readings();
	update_distance_readings();
	motor_control();
}

/**
 * @brief Competition behaviour.
 */
static void standard_run(void)
{
	initialize_solver_direction();
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
}

/**
 * @brief Initial setup and infinite wait.
 */
int main(void)
{
	setup();
	while (1) {
		log_gyro_raw();
		execute_commands();
	}
	standard_run();
	return 0;
}
