#include "speed.h"

/**
 * Maximum acceleration and deceleration.
 *
 * - Linear acceleration is defined in meters per second squared.
 * - Linear deceleration is defined in meters per second squared.
 * - Angular acceleration is defined in radians per second squared.
 */
static volatile float linear_acceleration;
static volatile float linear_deceleration;
static volatile float angular_acceleration;
static volatile float max_linear_speed;

/**
 * Parameters that define a turn.
 *
 * - Meters to travel in straight line before turning
 * - Meters to travel in straight line after turning
 * - Curve minimum radius
 * - Duration, in meters, of the angular acceleration phase
 * - Duration, in meters, of the constant angular velocity phase
 * - Sign of the turn (left or right)
 */
struct turn_parameters {
	float before;
	float after;
	float radius;
	float transition;
	float arc;
	int sign;
};

/**
 * @brief Calculate the maximum search linear speed.
 *
 * This speed is calculated so that the search speed in long straight lines
 * is always constant.
 *
 * @param[in] force Maximum force to apply while searching.
 *
 * @return The calculated search linear speed.
 */
static float _calculate_search_linear_speed(float force)
{
	float turn_velocity;
	float break_margin;

	turn_velocity = get_move_turn_linear_speed(MOVE_LEFT, force);
	break_margin = get_move_turn_before(MOVE_LEFT);
	break_margin -= turn_velocity * SEARCH_REACTION_TIME;
	return sqrt(turn_velocity * turn_velocity +
		    2 * get_linear_deceleration() * break_margin);
}

/**
 * @brief Set linear acceleration, deceleration and maximum linear speed.
 *
 * @param[in] force Maximum force to apply on the tires.
 * @param[in] run Whether to set speed variables for the run phase or not.
 */
void set_linear_speed_variables(float force, bool run)
{
	linear_acceleration = 2 * force / MOUSE_MASS;
	linear_deceleration = 2 * force / MOUSE_MASS;
	if (run)
		max_linear_speed = 2.5;
	else
		max_linear_speed = _calculate_search_linear_speed(force);
}

// clang-format off
struct turn_parameters turns[] = {
    [MOVE_LEFT] = {0.01700, 0.01700, 0.04921, 0.06042, 0.00037, -1},
    [MOVE_RIGHT] = {0.01700, 0.01700, 0.04921, 0.06042, 0.00037, 1},
    [MOVE_LEFT_90] = {-0.06272, -0.06272, 0.13000, 0.06042, 0.12728, -1},
    [MOVE_RIGHT_90] = {-0.06272, -0.06272, 0.13000, 0.06042, 0.12728, 1},
    [MOVE_LEFT_180] = {-0.04500, -0.04500, 0.08882, 0.06042, 0.20211, -1},
    [MOVE_RIGHT_180] = {-0.04500, -0.04500, 0.08882, 0.06042, 0.20211, 1},
    [MOVE_LEFT_TO_45] = {-0.06374, 0.06354, 0.10000, 0.06042, 0.00161, -1},
    [MOVE_RIGHT_TO_45] = {-0.06374, 0.06354, 0.10000, 0.06042, 0.00161, 1},
    [MOVE_LEFT_FROM_45] = {0.06354, -0.06374, 0.10000, 0.06042, 0.00161, -1},
    [MOVE_RIGHT_FROM_45] = {0.06354, -0.06374, 0.10000, 0.06042, 0.00161, 1},
    [MOVE_LEFT_TO_135] = {-0.03813, 0.03642, 0.08000, 0.06042, 0.11157, -1},
    [MOVE_RIGHT_TO_135] = {-0.03813, 0.03642, 0.08000, 0.06042, 0.11157, 1},
    [MOVE_LEFT_FROM_135] = {0.03642, -0.03813, 0.08000, 0.06042, 0.11157, -1},
    [MOVE_RIGHT_FROM_135] = {0.03642, -0.03813, 0.08000, 0.06042, 0.11157, 1},
    [MOVE_LEFT_DIAGONAL] = {0.03888, 0.03888, 0.06500, 0.06042, 0.02518, -1},
    [MOVE_RIGHT_DIAGONAL] = {0.03888, 0.03888, 0.06500, 0.06042, 0.02518, 1},
};
// clang-format on

float get_linear_acceleration(void)
{
	return linear_acceleration;
}

void set_linear_acceleration(float value)
{
	linear_acceleration = value;
}

float get_linear_deceleration(void)
{
	return linear_deceleration;
}

void set_linear_deceleration(float value)
{
	linear_deceleration = value;
}

float get_angular_acceleration(void)
{
	return angular_acceleration;
}

void set_angular_acceleration(float value)
{
	angular_acceleration = value;
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
 * @brief Execute a speed turn.
 *
 * @param[in] turn_type Turn type.
 * @param[in] force Maximum force to apply while turning.
 */
void speed_turn(enum movement turn_type, float force)
{
	int32_t start;
	int32_t current;
	float travelled;
	float linear_velocity;
	float angular_velocity;
	float max_angular_velocity;
	float factor;
	struct turn_parameters turn = turns[turn_type];

	linear_velocity = get_move_turn_linear_speed(turn_type, force);
	max_angular_velocity = turn.sign * linear_velocity / turn.radius;

	disable_walls_control();
	start = get_encoder_average_micrometers();
	while (true) {
		current = get_encoder_average_micrometers();
		travelled = (float)(current - start) / MICROMETERS_PER_METER;
		if (travelled >= 2 * turn.transition + turn.arc)
			break;
		angular_velocity = max_angular_velocity;
		if (travelled < turn.transition) {
			factor = travelled / turn.transition;
			angular_velocity *= sin(factor * PI / 2);
		} else if (travelled >= turn.transition + turn.arc) {
			factor = (travelled - turn.arc) / turn.transition;
			angular_velocity *= sin(factor * PI / 2);
		}
		set_ideal_angular_speed(angular_velocity);
	}
	set_ideal_angular_speed(0);
}

/**
 * @brief Execute an in-place turn.
 *
 * @param[in] radians Radians to turn (positive means left).
 * @param[in] force Maximum force to apply while turning.
 */
void inplace_turn(float radians, float force)
{
	int turn_sign;
	int32_t start;
	int32_t current;
	float time;
	float angular_velocity;
	float max_angular_velocity;
	float factor;
	float arc;
	float transition;
	float duration;
	float transition_angle;

	turn_sign = sign(radians);
	radians = fabsf(radians);
	angular_acceleration =
	    force * MOUSE_WHEELS_SEPARATION / MOUSE_MOMENT_OF_INERTIA;
	max_angular_velocity = sqrt(radians / 2 * angular_acceleration);
	if (max_angular_velocity > MOUSE_MAX_ANGULAR_VELOCITY)
		max_angular_velocity = MOUSE_MAX_ANGULAR_VELOCITY;

	duration = max_angular_velocity / angular_acceleration * PI;
	transition_angle = duration * max_angular_velocity / PI;
	arc = (radians - 2 * transition_angle) / max_angular_velocity;
	transition = duration / 2;
	max_angular_velocity = turn_sign * max_angular_velocity;

	set_target_linear_speed(get_ideal_linear_speed());
	disable_walls_control();
	start = get_clock_ticks();
	while (true) {
		current = get_clock_ticks();
		time = (float)(current - start) / SYSTICK_FREQUENCY_HZ;
		if (time >= 2 * transition + arc)
			break;
		angular_velocity = max_angular_velocity;
		if (time < transition) {
			factor = time / transition;
			angular_velocity *= sin(factor * PI / 2);
		} else if (time >= transition + arc) {
			factor = (time - arc) / transition;
			angular_velocity *= sin(factor * PI / 2);
		}
		set_ideal_angular_speed(angular_velocity);
	}
	set_ideal_angular_speed(0);
}

/**
 * @brief Get the straight distance that a turn adds before a straight movement.
 *
 * @param[in] turn_type Turn type.
 *
 * @return The added distance.
 */
float get_move_turn_before(enum movement turn_type)
{
	return turns[turn_type].before;
}

/**
 * @brief Get the straight distance that a turn adds after a straight movement.
 *
 * @param[in] turn_type Turn type.
 *
 * @return The added distance.
 */
float get_move_turn_after(enum movement turn_type)
{
	return turns[turn_type].after;
}

/**
 * @brief Get the expected linear speed at which to turn.
 *
 * @param[in] turn_type Turn type.
 * @param[in] force Maximum force to apply while turning.
 *
 * @return The calculated speed.
 */
float get_move_turn_linear_speed(enum movement turn_type, float force)
{
	return sqrt(force * 2 * turns[turn_type].radius / MOUSE_MASS);
}
