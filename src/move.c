#include "move.h"

static volatile float linear_speed = .6;

/* Assume the mouse tail is initially touching a wall */
static int32_t cell_shift_micrometers =
    (WALL_WIDTH / 2 + MOUSE_TAIL) * MICROMETERS_PER_METER;
static int32_t current_cell_start_micrometers;

float get_linear_speed(void)
{
	return linear_speed;
}

void set_linear_speed(float value)
{
	linear_speed = value;
}

/**
 * @brief Mark the beginning of a new cell.
 *
 * This should be executed right after entering a new cell.
 */
static void entered_next_cell(void)
{
	current_cell_start_micrometers = get_encoder_average_micrometers();
	led_left_toggle();
}

/**
 * @brief Calculate the required micrometers to stop.
 *
 * This functions assumes the current speed is the target speed and takes into
 * account the configured linear deceleration.
 */
static int32_t required_micrometers_to_stop(void)
{
	float target_speed = get_target_linear_speed();

	return (int32_t)((target_speed * target_speed) /
			 (2 * get_linear_deceleration()) *
			 MICROMETERS_PER_METER);
}

/**
 * @brief Calculate the required time to stop, in seconds.
 *
 * This functions assumes the current speed is the target speed and takes into
 * account the configured linear deceleration.
 */
static float required_time_to_stop(void)
{
	return get_target_linear_speed() / get_linear_deceleration();
}

/**
 * @brief Calculate the required ticks to stop, in system ticks.
 *
 * This functions assumes the current speed is the target speed and takes into
 * account the configured linear deceleration.
 */
static uint32_t required_ticks_to_stop(void)
{
	float required_seconds = required_time_to_stop();

	return (uint32_t)(required_seconds * SYSTICK_FREQUENCY_HZ);
}

/**
 * @brief Move straight to get out of the current cell.
 *
 * This function takes into account the value of the `cell_shift_micrometers`
 * variable which is basically used to track the exact position within a cell.
 */
void move_straight_out_of_cell(void)
{
	int32_t target_distance;

	target_distance = get_encoder_average_micrometers() +
			  (int32_t)(CELL_DIMENSION * MICROMETERS_PER_METER) -
			  cell_shift_micrometers;
	set_target_angular_speed(0.);
	set_target_linear_speed(linear_speed);
	while (get_encoder_average_micrometers() < target_distance)
		;
	entered_next_cell();
}

/**
 * @brief Move straight and stop at the end of the current cell.
 */
void move_straight_stop_end(void)
{
	int32_t target_distance;
	uint32_t target_ticks;

	target_distance = current_cell_start_micrometers +
			  (int32_t)(CELL_DIMENSION * MICROMETERS_PER_METER) -
			  required_micrometers_to_stop();
	set_target_angular_speed(0.);
	set_target_linear_speed(linear_speed);
	target_ticks = required_ticks_to_stop();
	while (get_encoder_average_micrometers() < target_distance)
		;
	set_target_linear_speed(.0);
	target_ticks += get_clock_ticks();
	while (get_clock_ticks() < target_ticks)
		;
	entered_next_cell();
}

/**
 * @brief Move straight and stop when the head would touch the front wall.
 */
void move_straight_stop_head_front_wall(void)
{
	int32_t target_distance;
	uint32_t target_ticks;

	target_distance = current_cell_start_micrometers +
			  (int32_t)(CELL_DIMENSION * MICROMETERS_PER_METER) -
			  required_micrometers_to_stop() -
			  MOUSE_HEAD * MICROMETERS_PER_METER;
	set_target_angular_speed(0.);
	set_target_linear_speed(linear_speed);
	target_ticks = required_ticks_to_stop();
	while (get_encoder_average_micrometers() < target_distance)
		;
	set_target_linear_speed(.0);
	target_ticks += get_clock_ticks();
	while (get_clock_ticks() < target_ticks)
		;
}

/**
 * @brief Move straight and stop at the middle of the current cell.
 */
void move_straight_stop_middle(void)
{
	int32_t target_distance;
	uint32_t target_ticks;

	target_distance =
	    current_cell_start_micrometers +
	    (int32_t)(CELL_DIMENSION / 2. * MICROMETERS_PER_METER) -
	    required_micrometers_to_stop();
	set_target_angular_speed(0.);
	set_target_linear_speed(linear_speed);
	target_ticks = required_ticks_to_stop();
	while (get_encoder_average_micrometers() < target_distance)
		;
	set_target_linear_speed(.0);
	target_ticks += get_clock_ticks();
	while (get_clock_ticks() < target_ticks)
		;
	cell_shift_micrometers = (CELL_DIMENSION / 2) * MICROMETERS_PER_METER;
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
 * @brief Turn left statically (90-degree turn with zero linear speed).
 */
void turn_left_static(void)
{
	uint32_t starting_time = get_clock_ticks();

	set_target_angular_speed(-4 * PI);
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
 * @brief Move front into the next cell.
 */
void move_front(void)
{
	int32_t target_distance;

	target_distance = current_cell_start_micrometers +
			  (int32_t)(CELL_DIMENSION * MICROMETERS_PER_METER);
	set_target_angular_speed(0.);
	set_target_linear_speed(linear_speed);
	while (get_encoder_average_micrometers() < target_distance)
		;
	entered_next_cell();
}

/**
 * @brief Move left into the next cell.
 */
void move_left(void)
{
	move_straight_stop_middle();
	turn_left_static();
	move_straight_out_of_cell();
}

/**
 * @brief Move right into the next cell.
 */
void move_right(void)
{
	move_straight_stop_middle();
	turn_right_static();
	move_straight_out_of_cell();
}
