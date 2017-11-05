#include "calibration.h"
#include "clock.h"
#include "control.h"
#include "detection.h"
#include "encoder.h"
#include "hmi.h"
#include "logging.h"
#include "motor.h"
#include "move.h"
#include "serial.h"
#include "setup.h"

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
	while (1) {
		if (button_left_read()) {
			led_left_on();
			sleep_ticks(2000);
			side_sensors_control(false);
			front_sensors_control(true);
			motor_control_enable = true;
			sleep_ticks(200);
			led_left_off();
		}
		if (button_right_read()) {
			sleep_ticks(5000);
			move_straight_out_of_cell();
			move_right();
			move_left();
			move_right();
			move_left();
			move_straight_stop_head_front_wall();
		}
		execute_commands();
	}

	return 0;
}
