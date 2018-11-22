#include "calibration.h"
#include "clock.h"
#include "control.h"
#include "detection.h"
#include "eeprom.h"
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
#include "speaker.h"
#include "speed.h"

static void competition(void);

/**
 * @brief Handle the SysTick interruptions.
 */
void sys_tick_handler(void)
{
	clock_tick();
	update_ideal_speed();
	update_distance_readings();
	update_gyro_readings();
	update_encoder_readings();
	motor_control();
}

/**
 * @brief Let the user configure the mouse before exploring or running.
 *
 * The user selects the speed mode with the buttons. Then, depending on the
 * phase (exploration or run), speed configurations will be selected according
 * to the speed mode.
 *
 * @param[in] run Whether the user is configuring for the run phase or not.
 *
 * @return The selected speed mode.
 */
static uint8_t user_configuration(bool run)
{
	uint8_t mode;

	mode = speed_mode_configuration();
	set_speed_mode(mode, run);
	return mode;
}

/**
 * @brief Check battery voltage and warn if the voltage is getting too low.
 */
static void check_battery_voltage(void)
{
	float voltage;

	voltage = get_battery_voltage();
	if (voltage < 3.6)
		speaker_warn_low_battery();
	if (voltage < 3.5)
		speaker_warn_low_battery();
	if (voltage < 3.4)
		speaker_warn_low_battery();
	if (voltage < 3.3)
		speaker_play_error();
}

/**
 * @brief Includes the functions to be executed before robot starts to move.
 */
static void before_moving(void)
{
	reset_motion();
	disable_walls_control();
	repeat_blink(10, 100);
	sleep_us(5000000);
	check_battery_voltage();
	led_left_on();
	led_right_on();
	wait_front_sensor_close_signal(0.12);
	srand(read_cycle_counter());
	led_left_off();
	led_right_off();
	sleep_us(2000000);
	calibrate();
	enable_motor_control();
	set_starting_position();
}

/**
 * @brief Functions to be executed when the solve phase is finished.
 */
static void after_moving(void)
{
	reset_motion();
	if (collision_detected())
		blink_collision();
	else
		speaker_play_success();
	check_battery_voltage();
}

/**
 * @brief Includes the functions to be executed during exploration phase.
 */
static void exploration_phase(void)
{
	uint8_t speed;

	speed = user_configuration(false);
	before_moving();
	explore(speed);
	after_moving();
}

/**
 * @brief Includes the functions to be executed during the running phase.
 */
static void running_phase(void)
{
	uint8_t speed;

	speed = user_configuration(true);
	before_moving();
	run(speed);
	run_back(speed);
	after_moving();
}

/**
 * @brief Competition routine.
 */
static void competition(void)
{
	if (!reuse_maze()) {
		initialize_solver_direction();
		set_goal_classic();
		set_target_goal();
		exploration_phase();
		set_run_sequence();
		save_maze();
		led_bluepill_on();
	}
	while (1)
		running_phase();
}

/**
 * @brief Training routine.
 */
static void training(void)
{
	initialize_solver_direction();
	add_goal(3, 5);
	set_target_goal();
	exploration_phase();
	set_run_sequence();
	while (1)
		running_phase();
}

/**
 * @brief Initial setup and infinite wait.
 */
int main(void)
{
	setup();
	set_speed_mode(0, false);
	systick_interrupt_enable();
	competition();
	while (1) {
		if (button_left_read_consecutive(500))
			training();
		execute_commands();
	}

	return 0;
}
