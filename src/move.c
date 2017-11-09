#include "move.h"

static volatile float max_linear_speed = .6;

/* Assume the mouse tail is initially touching a wall */
static float cell_shift = WALL_WIDTH / 2 + MOUSE_TAIL;
static int32_t current_cell_start_micrometers;

void set_starting_position(void)
{
	cell_shift = WALL_WIDTH / 2 + MOUSE_TAIL;
}

float get_max_linear_speed(void)
{
	return max_linear_speed;
}

void set_max_linear_speed(float value)
{
	max_linear_speed = value;
}

/**
 * @brief Mark the beginning of a new cell.
 *
 * This should be executed right after entering a new cell.
 */
static void entered_next_cell(void)
{
	current_cell_start_micrometers = get_encoder_average_micrometers();
	cell_shift = 0.;
	led_left_toggle();
}

/**
 * @brief Calculate the required micrometers to reach a given speed.
 *
 * This functions assumes the current speed is the target speed and takes into
 * account the configured linear deceleration.
 */
static uint32_t required_micrometers_to_speed(float speed)
{
	float acceleration;
	float target_speed = get_target_linear_speed();

	acceleration = (target_speed > speed) ? -get_linear_deceleration()
					      : get_linear_acceleration();

	return (uint32_t)((speed * speed - target_speed * target_speed) /
			  (2 * acceleration) * MICROMETERS_PER_METER);
}

/**
 * @brief Calculate the required time to reach a given speed, in seconds.
 *
 * This functions assumes the current speed is the target speed and takes into
 * account the configured linear deceleration.
 */
static float required_time_to_speed(float speed)
{
	float acceleration;
	float target_speed = get_target_linear_speed();

	acceleration = (target_speed > speed) ? -get_linear_deceleration()
					      : get_linear_acceleration();

	return (speed - target_speed) / acceleration;
}

/**
 * @brief Calculate the required ticks to reach a given speed, in ticks.
 *
 * This functions assumes the current speed is the target speed and takes into
 * account the configured linear deceleration.
 */
static uint32_t required_ticks_to_speed(float speed)
{
	float required_seconds = required_time_to_speed(speed);

	return (uint32_t)(required_seconds * SYSTICK_FREQUENCY_HZ);
}

/**
 * @brief Accelerate from a starting point and travel a defined distance.
 *
 * The acceleration will always try to reach `max_linear_speed`.
 *
 * @param[in] start Starting point, in micrometers.
 * @param[in] distance Distance to travel, in meters.
 */
void accelerate(int32_t start, float distance)
{
	int32_t target_distance;

	target_distance = start + (int32_t)(distance * MICROMETERS_PER_METER);
	set_target_angular_speed(0.);
	set_target_linear_speed(max_linear_speed);
	while (get_encoder_average_micrometers() < target_distance)
		;
}

/**
 * @brief Decelerate from a starting point and travel a defined distance.
 *
 * @param[in] start Starting point, in micrometers.
 * @param[in] distance Distance to travel, in meters.
 * @param[in] speed Target speed after decelerating, in meters per second.
 */
void decelerate(int32_t start, float distance, float speed)
{
	int32_t target_distance;
	uint32_t target_ticks;

	target_distance = start + (int32_t)(distance * MICROMETERS_PER_METER) -
			  (int32_t)required_micrometers_to_speed(speed);
	set_target_angular_speed(0.);
	set_target_linear_speed(max_linear_speed);
	target_ticks = required_ticks_to_speed(speed);
	while (get_encoder_average_micrometers() < target_distance)
		;
	set_target_linear_speed(speed);
	target_ticks += get_clock_ticks();
	while (get_clock_ticks() < target_ticks)
		;
}

/**
 * @brief Move straight and stop at the end of the current cell.
 */
void stop_end(void)
{
	decelerate(current_cell_start_micrometers, CELL_DIMENSION, 0.);
	entered_next_cell();
}

/**
 * @brief Move straight and stop when the head would touch the front wall.
 */
void stop_head_front_wall(void)
{
	float distance = CELL_DIMENSION - WALL_WIDTH / 2. - MOUSE_HEAD;

	decelerate(current_cell_start_micrometers, distance, 0.);
	cell_shift = distance;
}

/**
 * @brief Move straight and stop at the middle of the current cell.
 */
void stop_middle(void)
{
	float distance = CELL_DIMENSION / 2.;

	decelerate(current_cell_start_micrometers, distance, 0.);
	cell_shift = distance;
}

/**
 * @brief Turn left (90-degree turn with zero linear speed).
 */
void turn_left(void)
{
	uint32_t starting_time = get_clock_ticks();

	set_target_angular_speed(-4 * PI);
	while (get_clock_ticks() - starting_time <= 125)
		;
	set_target_angular_speed(0);
	while (get_clock_ticks() - starting_time <= 250)
		;
}

/**
 * @brief Turn right (90-degree turn with zero linear speed).
 */
void turn_right(void)
{
	uint32_t starting_time = get_clock_ticks();

	set_target_angular_speed(4 * PI);
	while (get_clock_ticks() - starting_time <= 125)
		;
	set_target_angular_speed(0);
	while (get_clock_ticks() - starting_time <= 250)
		;
}

/**
 * @brief Move out of the current cell and into the next cell.
 *
 * This function takes into account the value of the `cell_shift` variable wich
 * is basically used to track the exact position within a cell.
 */
void move_out(void)
{
	accelerate(get_encoder_average_micrometers(),
		   CELL_DIMENSION - cell_shift);
	entered_next_cell();
}

/**
 * @brief Move front into the next cell.
 */
void move_front(void)
{
	accelerate(current_cell_start_micrometers, CELL_DIMENSION);
	entered_next_cell();
}

/**
 * @brief Move left into the next cell.
 */
void move_left(void)
{
	decelerate(current_cell_start_micrometers, 0.03, 0.404);
	turn_left();
	accelerate(get_encoder_average_micrometers(), 0.03);
	entered_next_cell();
}

/**
 * @brief Move right into the next cell.
 */
void move_right(void)
{
	decelerate(current_cell_start_micrometers, 0.03, 0.404);
	turn_right();
	accelerate(get_encoder_average_micrometers(), 0.03);
	entered_next_cell();
}

/**
 * @brief Move back into the previous cell.
 */
void move_back(void)
{
	stop_middle();
	turn_right();
	turn_right();
	move_out();
}

/**
 * @brief Move into the next cell according to a movement direction.
 *
 * @param[in] direction Movement direction.
 */
void move(enum step_direction direction)
{
	if (direction == LEFT)
		move_left();
	else if (direction == RIGHT)
		move_right();
	else if (direction == FRONT)
		move_front();
	else if (direction == BACK)
		move_back();
	else
		stop_middle();
}
