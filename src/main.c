#include "battery.h"
#include "clock.h"
#include "control.h"
#include "detection.h"
#include "encoder.h"
#include "hmi.h"
#include "logging.h"
#include "motor.h"
#include "setup.h"

/**
 * @brief Handle the SysTick interruptions.
 */
void sys_tick_handler(void)
{
	clock_tick();
	update_ideal_speed();
	update_encoder_readings();
	motor_control();
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

			/* Speed profile test */
			each(20, log_linear_speed, 1000);
			set_target_angular_speed(0.);
			set_target_linear_speed(.5);
			each(20, log_linear_speed, 3000);
			set_target_angular_speed(0.);
			set_target_linear_speed(0.);
			each(20, log_linear_speed, 2000);

			led_left_off();
		}
		if (button_right_read()) {
			LOG_INFO("%f", get_battery_voltage());
			sleep_ticks(500);
		}
	}

	return 0;
}
