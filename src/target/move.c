#include "move.h"

/* Assume the mouse tail is initially touching a wall */
static int32_t current_cell_start_micrometers;

void set_starting_position(void)
{
	current_cell_start_micrometers =
	    get_encoder_average_micrometers() -
	    (MOUSE_TAIL + WALL_WIDTH / 2) * MICROMETERS_PER_METER;
}

/**
 * @brief Mark the beginning of a new cell.
 *
 * It should be executed right after entering a new cell.
 *
 * Takes into account a possible front-wall longitudinal correction.
 */
static void entered_next_cell(void)
{
	int32_t front_wall_correction;

	current_cell_start_micrometers = get_encoder_average_micrometers();
	if (front_wall_detection()) {
		front_wall_correction =
		    (int32_t)((get_front_wall_distance() - CELL_DIMENSION) *
			      MICROMETERS_PER_METER);
		current_cell_start_micrometers += front_wall_correction;
	}
	led_left_toggle();
}

/**
 * @brief Calculate the required micrometers to reach a given speed.
 *
 * This functions assumes the current speed is the target speed and takes into
 * account the configured linear deceleration.
 */
uint32_t required_micrometers_to_speed(float speed)
{
	float acceleration;
	float current_speed = get_ideal_linear_speed();

	acceleration = (current_speed > speed) ? -get_linear_deceleration()
					       : get_linear_acceleration();

	return (uint32_t)((speed * speed - current_speed * current_speed) /
			  (2 * acceleration) * MICROMETERS_PER_METER);
}

/**
 * @brief Calculate the required time to reach a given speed, in seconds.
 *
 * This functions assumes the current speed is the target speed and takes into
 * account the configured linear deceleration.
 */
float required_time_to_speed(float speed)
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
uint32_t required_ticks_to_speed(float speed)
{
	float required_seconds = required_time_to_speed(speed);

	return (uint32_t)(required_seconds * SYSTICK_FREQUENCY_HZ);
}

/**
 * @brief Activation of sensors control depending on walls around.
 */
static void enable_walls_control(void)
{
	front_sensors_control(front_wall_detection());
	side_sensors_control((right_wall_detection() || left_wall_detection()));
}

/**
 * @brief Disable sensors control.
 */
void disable_walls_control(void)
{
	side_sensors_control(false);
	front_sensors_control(false);
}

/**
 * @brief Reach a target position at a target speed.
 *
 * @param[in] start Starting point, in micrometers.
 * @param[in] distance Distance to travel, in meters, from the starting point.
 * @param[in] speed Target speed, in meters per second.
 */
void target_straight(int32_t start, float distance, float speed)
{
	int32_t target_distance;

	set_target_angular_speed(0.);

	target_distance = start + (int32_t)(distance * MICROMETERS_PER_METER);
	if (distance > 0) {
		set_target_linear_speed(get_max_linear_speed());
		while (get_encoder_average_micrometers() <
		       target_distance -
			   (int32_t)required_micrometers_to_speed(speed))
			;
	} else {
		set_target_linear_speed(-get_max_linear_speed());
		while (get_encoder_average_micrometers() >
		       target_distance +
			   (int32_t)required_micrometers_to_speed(speed))
			;
	}
	set_target_linear_speed(speed);
	while (get_ideal_linear_speed() != speed)
		;
}

/**
 * @brief Wait until the robot is perpendicular with respect to the front wall.
 */
static void wait_front_perpendicular(float error)
{
	int i;
	float average = 0.;

	while (true) {
		for (i = 0; i < 20; i++) {
			average += get_front_sensors_error();
			sleep_ticks(2);
		}
		average /= 20;
		if (average < error)
			break;
	}
}

/**
 * @brief Keep a specified distance from the front wall.
 *
 * @param[in] distance Distance to keep from the front wall, in meters.
 * @param[in] error Allowed error, in meters.
 */
void keep_front_wall_distance(float distance, float error)
{
	int i;
	float diff;
	float front_wall_distance;

	if (!front_wall_detection())
		return;

	set_linear_acceleration(get_linear_acceleration() / 2.);
	set_linear_deceleration(get_linear_deceleration() / 2.);

	while (true) {
		front_sensors_control(true);
		side_sensors_control(false);

		wait_front_perpendicular(error);

		front_wall_distance = 0.;
		for (i = 0; i < 20; i++) {
			front_wall_distance += get_front_wall_distance();
			sleep_ticks(2);
		}
		front_wall_distance /= 20;
		diff = front_wall_distance - distance;
		if (fabsf(diff) < error)
			break;
		target_straight(get_encoder_average_micrometers(), diff, 0.);
	}

	set_linear_acceleration(get_linear_acceleration() * 2.);
	set_linear_deceleration(get_linear_deceleration() * 2.);

	disable_walls_control();
	reset_control_all();
}

/**
 * @brief Move straight and stop at the end of the current cell.
 */
void stop_end(void)
{
	enable_walls_control();
	target_straight(current_cell_start_micrometers, CELL_DIMENSION, 0.);
	disable_walls_control();
	reset_control_errors();
	entered_next_cell();
}

/**
 * @brief Move straight and stop when the head would touch the front wall.
 */
void stop_head_front_wall(void)
{
	float distance = CELL_DIMENSION - WALL_WIDTH / 2. - MOUSE_HEAD;

	enable_walls_control();
	target_straight(current_cell_start_micrometers, distance, 0.);
	disable_walls_control();
	reset_control_errors();
}

/**
 * @brief Move straight and stop at the middle of the current cell.
 */
void stop_middle(void)
{
	float distance = CELL_DIMENSION / 2.;

	enable_walls_control();
	target_straight(current_cell_start_micrometers, distance, 0.);
	disable_walls_control();
	reset_control_errors();
}

/**
 * @brief Turn left (90-degree turn with zero linear speed).
 */
void turn_left(void)
{
	uint32_t starting_time = get_clock_ticks();

	set_target_angular_speed(-get_max_angular_speed());
	while (get_clock_ticks() - starting_time <= get_turn_t0())
		;
	set_target_angular_speed(0);
	while (get_clock_ticks() - starting_time <= get_turn_t1())
		;
}

/**
 * @brief Turn right (90-degree turn with zero linear speed).
 */
void turn_right(void)
{
	uint32_t starting_time = get_clock_ticks();

	set_target_angular_speed(get_max_angular_speed());
	while (get_clock_ticks() - starting_time <= get_turn_t0())
		;
	set_target_angular_speed(0);
	while (get_clock_ticks() - starting_time <= get_turn_t1())
		;
}

/**
 * @brief Turn left or right in-place (90-degree turn with zero linear speed).
 */
void turn_side(enum step_direction side)
{
	if (side == RIGHT)
		turn_right();
	else
		turn_left();
}

/**
 * @brief Randomly choose between `LEFT` and `RIGHT`.
 */
static enum step_direction choose_random_side(void)
{
	if (read_cycle_counter() % 2)
		return RIGHT;
	return LEFT;
}

/**
 * @brief Turn back (180-degree turn) and correct with front walls if possible.
 */
void turn_back(void)
{
	enum step_direction side;

	if (current_side_wall(LEFT) == current_side_wall(RIGHT))
		side = choose_random_side();
	else if (current_side_wall(RIGHT))
		side = RIGHT;
	else
		side = LEFT;

	keep_front_wall_distance(CELL_DIMENSION / 2., 0.001);
	turn_side(side);
	keep_front_wall_distance(CELL_DIMENSION / 2., 0.001);
	turn_side(side);

	current_cell_start_micrometers =
	    get_encoder_average_micrometers() -
	    (CELL_DIMENSION / 2.) * MICROMETERS_PER_METER;
}

/**
 * @brief Move front into the next cell.
 */
void move_front(void)
{
	enable_walls_control();
	target_straight(current_cell_start_micrometers, CELL_DIMENSION,
			get_max_linear_speed());
	entered_next_cell();
}

/**
 * @brief Move left or right into the next cell.
 */
static void move_side(enum step_direction side)
{
	enable_walls_control();
	target_straight(current_cell_start_micrometers,
			CELL_DIMENSION / 2 - get_turn_radius() -
			    MOUSE_AXIS_SEPARATION / 2.,
			get_turn_linear_speed());
	disable_walls_control();
	turn_side(side);
	enable_walls_control();
	target_straight(get_encoder_average_micrometers(),
			CELL_DIMENSION / 2 - get_turn_radius() +
			    MOUSE_AXIS_SEPARATION / 2.,
			get_max_linear_speed());
	entered_next_cell();
}

/**
 * @brief Move left into the next cell.
 */
void move_left(void)
{
	move_side(LEFT);
}

/**
 * @brief Move right into the next cell.
 */
void move_right(void)
{
	move_side(RIGHT);
}

/**
 * @brief Move back into the previous cell.
 */
void move_back(void)
{
	stop_middle();
	turn_back();
	move_front();
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

/**
 * @brief Reset motion to an iddle state.
 *
 * - Disable motor control.
 * - Disable walls control.
 * - Turn the motor driver off.
 * - Reset control state.
 */
void reset_motion(void)
{
	disable_motor_control();
	disable_walls_control();
	drive_off();
	reset_control_all();
}
