#include "calibration.h"

void run_linear_speed_profile(void)
{
	each(20, log_linear_speed, 1000);
	set_target_angular_speed(0.);
	set_target_linear_speed(.5);
	each(20, log_linear_speed, 2000);
	set_target_angular_speed(0.);
	set_target_linear_speed(0.);
	each(20, log_linear_speed, 2000);
}
