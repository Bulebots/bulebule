#include "battery.h"
#include "calibration.h"
#include "clock.h"
#include "control.h"
#include "detection.h"
#include "encoder.h"
#include "hmi.h"
#include "logging.h"
#include "motor.h"
#include "serial.h"
#include "setup.h"

/**
 * @brief Handle the SysTick interruptions.
 */
void sys_tick_handler(void)
{
	clock_tick();
	if (!collision_detected()) {
		update_ideal_speed();
		update_encoder_readings();
		motor_control();
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
			run_linear_speed_profile();
			led_left_off();
		}
		if (button_right_read()) {
			log_battery_voltage();
			sleep_ticks(500);
		}
		execute_commands();
	}

	return 0;
}
