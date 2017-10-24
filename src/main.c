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

void my_log(void)
{
	float left_speed = get_encoder_left_speed();
	float right_speed = get_encoder_right_speed();
	float target_speed = get_target_linear_speed();
	float ideal_speed = get_ideal_linear_speed();
	int32_t pwm_left = get_left_pwm();
	int32_t pwm_right = get_right_pwm();
	int32_t left_diff_count = get_encoder_left_diff_count();
	int32_t right_diff_count = get_encoder_right_diff_count();

	LOG_INFO("%f,%f,%f,%f,%d,%d,%d,%d", target_speed, ideal_speed,
		 left_speed, right_speed, pwm_left, pwm_right, left_diff_count,
		 right_diff_count);
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
			each(20, my_log, 1000);
			set_target_angular_speed(0.);
			set_target_linear_speed(.5);
			each(20, my_log, 3000);
			set_target_angular_speed(0.);
			set_target_linear_speed(0.);
			each(20, my_log, 2000);

			led_left_off();
		}
		if (button_right_read()) {
			LOG_INFO("%f", get_battery_voltage());
			sleep_ticks(500);
		}
	}

	return 0;
}
