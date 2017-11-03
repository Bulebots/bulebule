#include "move.h"

static float cell_shift_micrometers = 42000;

/**
 * @brief Move straight to get out of the current cell.
 *
 * This function takes into account the value of the `cell_shift_micrometers`
 * variable which is basically used to track the exact position within a cell.
 */
void move_straight_out_of_cell(void)
{
	int32_t starting_position = get_encoder_average_micrometers();

	set_target_angular_speed(0.);
	set_target_linear_speed(.5);
	while (get_encoder_average_micrometers() - starting_position <
	       CELL_DIMENSION * MICROMETERS_PER_METER - cell_shift_micrometers)
		;
	led_left_toggle();
}

/**
 * @brief Move straight into the next cell.
 */
void move_straight(void)
{
	int32_t starting_position = get_encoder_average_micrometers();

	set_target_angular_speed(0.);
	set_target_linear_speed(.5);
	while (get_encoder_average_micrometers() - starting_position <
	       CELL_DIMENSION * MICROMETERS_PER_METER)
		;
	led_left_toggle();
}

/**
 * @brief Stop now, setting both target linear and angular speeds to zero.
 */
void move_stop(void)
{
	set_target_angular_speed(0.);
	set_target_linear_speed(0.);
}

/**
 * @brief Turn right statically (90-degree turn with zero linear speed).
 */
void turn_right_static(void)
{
	uint32_t starting_time = get_clock_ticks();

	set_target_angular_speed(4 * PI);
	set_target_linear_speed(0.);
	while (get_clock_ticks() - starting_time < 125)
		;
	set_target_angular_speed(0);
	set_target_linear_speed(0.);
	while (get_clock_ticks() - starting_time < 125)
		;
	led_left_toggle();
}

/**
 * @brief Move right into the next cell.
 */
void move_right(void)
{
	uint32_t starting_time = get_clock_ticks();

	set_target_angular_speed(4 * PI);
	while (get_clock_ticks() - starting_time < 125)
		;
	set_target_angular_speed(0);
	while (get_clock_ticks() - starting_time < 125)
		;
}
