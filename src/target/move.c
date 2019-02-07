#include "move.h"

/* Assume the mouse tail is initially touching a wall */
static int32_t current_cell_start_micrometers;

/**
 * @brief Return the current robot shift inside the cell, in meters.
 *
 * The shift is the traveled distance since the start of the cell.
 */
float current_cell_shift(void)
{
	return (float)(get_encoder_average_micrometers() -
		       current_cell_start_micrometers) /
	       MICROMETERS_PER_METER;
}

void set_starting_position(void)
{
	current_cell_start_micrometers =
	    get_encoder_average_micrometers() -
	    MOUSE_START_SHIFT * MICROMETERS_PER_METER;
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
 *
 * @param[in] speed Target speed.

 * @return The required number of micrometers to reach the target speed.
 */
int32_t required_micrometers_to_speed(float speed)
{
	float acceleration;
	float current_speed = get_ideal_linear_speed();

	acceleration = (current_speed > speed) ? -get_linear_deceleration()
					       : get_linear_acceleration();

	return (int32_t)((speed * speed - current_speed * current_speed) /
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

	set_ideal_angular_speed(0.);

	target_distance = start + (int32_t)(distance * MICROMETERS_PER_METER);
	if (distance > 0) {
		set_target_linear_speed(get_max_linear_speed());
		while (get_encoder_average_micrometers() <
		       target_distance - required_micrometers_to_speed(speed))
			;
	} else {
		set_target_linear_speed(-get_max_linear_speed());
		while (get_encoder_average_micrometers() >
		       target_distance - required_micrometers_to_speed(speed))
			;
	}
	set_target_linear_speed(speed);
	if (speed == 0.) {
		while (get_ideal_linear_speed() != speed)
			;
	} else {
		while (get_encoder_average_micrometers() < target_distance)
			;
	}
}

/**
 * @brief Wait until the robot is perpendicular with respect to the front wall.
 *
 * @param[in] error Allowed error, in meters.
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
 */
void keep_front_wall_distance(float distance)
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

		wait_front_perpendicular(KEEP_FRONT_DISTANCE_TOLERANCE);

		front_wall_distance = 0.;
		for (i = 0; i < 20; i++) {
			front_wall_distance += get_front_wall_distance();
			sleep_ticks(2);
		}
		front_wall_distance /= 20;
		diff = front_wall_distance - distance;
		if (fabsf(diff) < KEEP_FRONT_DISTANCE_TOLERANCE)
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
 * @brief Turn back (180-degree turn) and correct with front walls if possible.
 *
 * @param[in] force Maximum force to apply on the tires.
 */
void turn_back(float force)
{
	int direction;

	if (get_front_wall_distance() < CELL_DIMENSION)
		keep_front_wall_distance(CELL_DIMENSION / 2.);
	disable_walls_control();
	direction = (int)(rand() % 2) * 2 - 1;
	inplace_turn(direction * PI, force);

	current_cell_start_micrometers =
	    get_encoder_average_micrometers() -
	    (CELL_DIMENSION / 2. + SHIFT_AFTER_180_DEG_TURN) *
		MICROMETERS_PER_METER;
}

/**
 * @brief Turn back (180-degree turn) to a starting position.
 *
 * @param[in] force Maximum force to apply on the tires.
 */
void turn_to_start_position(float force)
{
	float distance;

	set_linear_acceleration(get_linear_acceleration() / 4.);
	set_linear_deceleration(get_linear_deceleration() / 4.);

	turn_back(force);
	distance = MOUSE_START_SHIFT - current_cell_shift();
	target_straight(get_encoder_average_micrometers(), distance, 0.);

	set_linear_acceleration(get_linear_acceleration() * 4.);
	set_linear_deceleration(get_linear_deceleration() * 4.);

	disable_walls_control();
	reset_control_all();
	enable_motor_control();
	drive_break();
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
 * @brief Move front many cells.
 *
 * @param[in] cells Number of cells to move front consecutively.
 */
void move_front_many(int cells)
{
	side_sensors_control(true);
	target_straight(current_cell_start_micrometers, CELL_DIMENSION * cells,
			get_max_linear_speed());
	entered_next_cell();
}

/**
 * @brief Move front a defined distance ending at a defined speed.
 *
 * @param[in] distance Distance to travel.
 * @param[in] end_linear_speed Speed at which to end the movement.
 */
void parametric_move_front(float distance, float end_linear_speed)
{
	target_straight(get_encoder_average_micrometers(), distance,
			end_linear_speed);
}

/**
 * @brief Move left or right into the next cell.
 *
 * @param[in] movement Turn direction (left or right).
 * @param[in] force Maximum force to apply on the tires.
 */
void move_side(enum movement turn, float force)
{
	enable_walls_control();
	target_straight(current_cell_start_micrometers,
			get_move_turn_before(turn),
			get_move_turn_linear_speed(turn, force));
	disable_walls_control();
	speed_turn(turn, force);
	enable_walls_control();
	target_straight(get_encoder_average_micrometers(),
			get_move_turn_after(turn), get_max_linear_speed());
	entered_next_cell();
}

/**
 * @brief Move back into the previous cell.
 *
 * @param[in] force Maximum force to apply on the tires.
 */
void move_back(float force)
{
	stop_middle();
	turn_back(force);
	move_front();
}

/**
 * @brief Move into the next cell according to a movement direction.
 *
 * @param[in] direction Movement direction.
 * @param[in] force Maximum force to apply on the tires.
 */
void move(enum step_direction direction, float force)
{
	if (direction == LEFT)
		move_side(MOVE_LEFT, force);
	else if (direction == RIGHT)
		move_side(MOVE_RIGHT, force);
	else if (direction == FRONT)
		move_front();
	else if (direction == BACK)
		move_back(force);
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
