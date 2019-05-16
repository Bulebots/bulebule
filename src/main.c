#include "mmlib/calibration.h"
#include "mmlib/clock.h"
#include "mmlib/command.h"
#include "mmlib/control.h"
#include "mmlib/encoder.h"
#include "mmlib/hmi.h"
#include "mmlib/logging.h"
#include "mmlib/move.h"
#include "mmlib/search.h"
#include "mmlib/solve.h"
#include "mmlib/speed.h"
#include "mmlib/walls.h"

#include "eeprom.h"
#include "motor.h"
#include "setup.h"
#include "voltage.h"

/**
 * @brief Handle the SysTick interruptions.
 */
void sys_tick_handler(void)
{
	clock_tick();
	update_distance_readings();
	update_gyro_readings();
	update_encoder_readings();
	motor_control();
	log_data();
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
 * @brief Functions to be executed after moving (running or exploring).
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
 * @brief Let the user configure the mouse forces before exploring or running.
 *
 * The user selects the force to apply to the tires while exploring or running.
 *
 * Data logging is always active during movement phase.
 *
 * @param[in] run Whether the robot should be running.
 */
static void movement_phase(bool do_run)
{
	float force;

	force = hmi_configure_force(0.1, 0.05);
	kinematic_configuration(force, do_run);

	start_data_logging(log_data_control);
	before_moving();
	if (!do_run) {
		explore(force);
	} else {
		run(force);
		run_back(force);
	}
	after_moving();
	stop_data_logging();
}

/**
 * @brief Configure the goal for the search phase.
 */
static void configure_goal(void)
{
	switch (button_user_wait_action()) {
	case BUTTON_SHORT:
		add_goal(1, 0);
		break;
	case BUTTON_LONG:
		set_goal_classic();
		speaker_play_competition();
		break;
	}
}

/**
 * @brief Execute the exploration phase.
 */
static void exploration_phase(void)
{
	set_search_initial_direction(NORTH);
	configure_goal();
	set_target_goal();

	movement_phase(false);

	set_run_sequence();
	save_maze();
}

/**
 * @brief Configure the robot and execute the search/run.
 */
static void configure_and_move(void)
{
	if (!maze_is_saved())
		exploration_phase();
	led_bluepill_on();
	switch (button_user_wait_action()) {
	case BUTTON_SHORT:
		load_maze();
		break;
	case BUTTON_LONG:
		reset_maze();
		led_bluepill_off();
		exploration_phase();
		led_bluepill_on();
		break;
	}
	while (1)
		movement_phase(true);
}

/**
 * @brief Initial setup and infinite wait.
 */
int main(void)
{
	setup();
	kinematic_configuration(0.25, false);
	systick_interrupt_enable();
	while (1) {
		switch (button_user_response()) {
		case BUTTON_NONE:
			break;
		default:
			configure_and_move();
			break;
		}
		execute_command();
	}

	return 0;
}
