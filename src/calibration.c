#include "calibration.h"

/**
 * @brief Run a full linear profile test.
 *
 * The robot will accelerate, maintain the target speed for a while and then
 * decelerate back to zero speed. There is no angular speed in this test, only
 * linear movement. During all this test information about the speed of each
 * motor as well as the PWM output is logged periodically for later analysis.
 */
void run_linear_speed_profile(void)
{
	each(10, log_linear_speed, 1000);
	set_target_angular_speed(0.);
	set_target_linear_speed(.5);
	each(10, log_linear_speed, 2000);
	set_target_angular_speed(0.);
	set_target_linear_speed(0.);
	each(10, log_linear_speed, 2000);
}
