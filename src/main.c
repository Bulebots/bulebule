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

static bool motor_control_enable;

/**
 * @brief Handle the SysTick interruptions.
 */
void sys_tick_handler(void)
{
	clock_tick();
	if (!collision_detected()) {
		update_ideal_speed();
		update_encoder_readings();
		update_distance_readings();
		if (motor_control_enable)
			motor_control();
	} else {
		drive_off();
		led_left_on();
		led_right_on();
	}
}

/**
 * @brief Initial setup and infinite wait.
 */
int main(void)
{
	setup();
	initialize_solver_direction();
	motor_control_enable = true;
	while (1) {
		if (button_left_read()) {
			led_left_off();
			led_right_off();
			sleep_ticks(5000);
			led_left_on();
			led_right_on();
			sleep_ticks(1000);
			side_sensors_calibration();
			led_left_off();
			led_right_off();
			solve();
			led_left_on();
			led_right_on();
		}
		if (button_right_read()) {
			sleep_ticks(5000);
			move_out();
			move_right();
			move_left();
			move_right();
			move_left();
			stop_head_front_wall();
		}
		execute_commands();
	}

	return 0;
}
