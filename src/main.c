#include "battery.h"
#include "clock.h"
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
	update_encoder_readings();
}

/**
 * @brief Initial setup and infinite wait.
 */
int main(void)
{
	setup();

	drive_forward();

	while (1) {
		uint16_t off[4];
		uint16_t on[4];

		sleep_ticks(250);
		get_sensors_data(off, on);
		LOG_INFO("OFF-ON, %d, %d, %d, %d, %d, %d, %d, %d", off[0],
			 off[1], off[2], off[3], on[0], on[1], on[2], on[3]);
	}

	return 0;
}
