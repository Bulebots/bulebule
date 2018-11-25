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
static volatile float max_end_linear_speed;
static volatile float max_linear_speed;

const float linear_acceleration_defaults[NUM_MODES] = {4., 4., 5., 5.};
const float linear_deceleration_defaults[NUM_MODES] = {4., 4., 5., 5.};
const float angular_acceleration_defaults[NUM_MODES] = {32. * PI, 32. * PI,
							48. * PI, 64. * PI};
const float max_end_linear_speed_defaults[NUM_MODES] = {.5, .55, 0.61, 0.72};
const float max_linear_speed_defaults[NUM_MODES] = {.5, .55, 0.61, 0.72};
const float max_linear_speed_run_defaults[NUM_MODES] = {1., 1.5, 2., 2.5};

uint8_t speed_configuration;

/**
 * Parameters that define a turn.
 *
 * - Meters to travel in straight line before turning
 * - Meters to travel in straight line after turning
 * - Linear speed, in meters per second, at which to turn
 * - Duration, in milliseconds, of the positive angular acceleration phase
 * - Duration, in milliseconds, of the whole turn
 * - Maximum angular speed at which to turn, in radians per second
 */
struct turn_parameters {
	float before;
	float after;
	float linear_speed;
	int rise_time;
	int elapsed_time;
	float max_angular_speed;
};

// clang-format off
struct turn_parameters turns[][NUM_MODES] = {
    [MOVE_LEFT] = {
	{0.02, 0.02, .400, 219, 291, -2.35 * PI},
	{0.02, 0.02, .448, 166, 260, -3. * PI},
	{0.02, 0.02, .566, 124, 208, -4. * PI},
	{0.02, 0.02, .670, 88, 176, -6. * PI},
    },
    [MOVE_RIGHT] = {
	{0.02, 0.02, .400, 219, 291, 2.35 * PI},
	{0.02, 0.02, .448, 166, 260, 3. * PI},
	{0.02, 0.02, .566, 124, 208, 4. * PI},
	{0.02, 0.02, .670, 88, 176, 6. * PI},
    },
    [MOVE_LEFT_90] = {
	{-0.05, -0.05, .8, 219, 291, -2.3 * PI},
	{-0.05, -0.05, .8, 219, 291, -2.3 * PI},
	{-0.05, -0.05, .8, 219, 291, -2.3 * PI},
	{-0.05, -0.05, .8, 219, 291, -2.3 * PI},
    },
    [MOVE_RIGHT_90] = {
	{-0.05, -0.05, .8, 219, 291, 2.3 * PI},
	{-0.05, -0.05, .8, 219, 291, 2.3 * PI},
	{-0.05, -0.05, .8, 219, 291, 2.3 * PI},
	{-0.05, -0.05, .8, 219, 291, 2.3 * PI},
    },
    [MOVE_LEFT_180] = {
	{-0.04, -0.04, .7, 400, 479, -2.5 * PI},
	{-0.04, -0.04, .7, 400, 479, -2.5 * PI},
	{-0.04, -0.04, .7, 400, 479, -2.5 * PI},
	{-0.04, -0.04, .7, 400, 479, -2.5 * PI},
    },
    [MOVE_RIGHT_180] = {
	{-0.04, -0.04, .7, 400, 479, 2.5 * PI},
	{-0.04, -0.04, .7, 400, 479, 2.5 * PI},
	{-0.04, -0.04, .7, 400, 479, 2.5 * PI},
	{-0.04, -0.04, .7, 400, 479, 2.5 * PI},
    },
    [MOVE_LEFT_TO_45] = {
	{-0.06846, 0.05688, .71, 125, 188, -2. * PI},
	{-0.06846, 0.05688, .71, 125, 188, -2. * PI},
	{-0.06846, 0.05688, .71, 125, 188, -2. * PI},
	{-0.06846, 0.05688, .71, 125, 188, -2. * PI},
    },
    [MOVE_RIGHT_TO_45] = {
	{-0.06846, 0.05688, .71, 125, 188, 2. * PI},
	{-0.06846, 0.05688, .71, 125, 188, 2. * PI},
	{-0.06846, 0.05688, .71, 125, 188, 2. * PI},
	{-0.06846, 0.05688, .71, 125, 188, 2. * PI},
    },
    [MOVE_LEFT_FROM_45] = {
	{0.05688, -0.06846, .71, 125, 188, -2. * PI},
	{0.05688, -0.06846, .71, 125, 188, -2. * PI},
	{0.05688, -0.06846, .71, 125, 188, -2. * PI},
	{0.05688, -0.06846, .71, 125, 188, -2. * PI},
    },
    [MOVE_RIGHT_FROM_45] = {
	{0.05688, -0.06846, .71, 125, 188, 2. * PI},
	{0.05688, -0.06846, .71, 125, 188, 2. * PI},
	{0.05688, -0.06846, .71, 125, 188, 2. * PI},
	{0.05688, -0.06846, .71, 125, 188, 2. * PI},
    },
    [MOVE_LEFT_TO_135] = {
	{-0.03011, 0.04333, .6, 299, 378, -2.5 * PI},
	{-0.03011, 0.04333, .6, 299, 378, -2.5 * PI},
	{-0.03011, 0.04333, .6, 299, 378, -2.5 * PI},
	{-0.03011, 0.04333, .6, 299, 378, -2.5 * PI},
    },
    [MOVE_RIGHT_TO_135] = {
	{-0.03011, 0.04333, .6, 299, 378, 2.5 * PI},
	{-0.03011, 0.04333, .6, 299, 378, 2.5 * PI},
	{-0.03011, 0.04333, .6, 299, 378, 2.5 * PI},
	{-0.03011, 0.04333, .6, 299, 378, 2.5 * PI},
    },
    [MOVE_LEFT_FROM_135] = {
	{0.04333, -0.03011, .6, 299, 378, -2.5 * PI},
	{0.04333, -0.03011, .6, 299, 378, -2.5 * PI},
	{0.04333, -0.03011, .6, 299, 378, -2.5 * PI},
	{0.04333, -0.03011, .6, 299, 378, -2.5 * PI},
    },
    [MOVE_RIGHT_FROM_135] = {
	{0.04333, -0.03011, .6, 299, 378, 2.5 * PI},
	{0.04333, -0.03011, .6, 299, 378, 2.5 * PI},
	{0.04333, -0.03011, .6, 299, 378, 2.5 * PI},
	{0.04333, -0.03011, .6, 299, 378, 2.5 * PI},
    },
    [MOVE_LEFT_DIAGONAL] = {
	{0.0262, 0.0262, .6, 199, 278, -2.5 * PI},
	{0.0262, 0.0262, .6, 199, 278, -2.5 * PI},
	{0.0262, 0.0262, .6, 199, 278, -2.5 * PI},
	{0.0262, 0.0262, .6, 199, 278, -2.5 * PI},
    },
    [MOVE_RIGHT_DIAGONAL] = {
	{0.0262, 0.0262, .6, 199, 278, 2.5 * PI},
	{0.0262, 0.0262, .6, 199, 278, 2.5 * PI},
	{0.0262, 0.0262, .6, 199, 278, 2.5 * PI},
	{0.0262, 0.0262, .6, 199, 278, 2.5 * PI},
    },
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

float get_max_end_linear_speed(void)
{
	return max_end_linear_speed;
}

void set_max_end_linear_speed(float value)
{
	max_end_linear_speed = value;
}

/**
 * @brief Set speed parameters with default values from a predefined mode.
 *
 * @param[in] mode Speed mode from 0 to (NUM_MODES - 1).
 * @param[in] run Whether or not to use run speed (as opposed to exploration).
 */
void set_speed_mode(uint8_t mode, bool run)
{
	speed_configuration = mode;
	linear_acceleration = linear_acceleration_defaults[mode];
	linear_deceleration = linear_deceleration_defaults[mode];
	angular_acceleration = angular_acceleration_defaults[mode];
	if (run)
		max_linear_speed = max_linear_speed_run_defaults[mode];
	else
		max_linear_speed = max_linear_speed_defaults[mode];
	max_end_linear_speed = max_end_linear_speed_defaults[mode];
}

/**
 * @brief Turn at a defined speed level.
 *
 * @param[in] turn Turn type.
 * @param[in] speed Configured speed level.
 */
void speed_turn(enum movement turn, uint8_t speed)
{
	parametric_turn(turns[turn][speed].max_angular_speed,
			turns[turn][speed].rise_time,
			turns[turn][speed].elapsed_time);
}

/**
 * @brief Get the straight distance that a turn adds before a straight movement.
 *
 * @param[in] turn Turn type.
 * @param[in] speed Configured speed level.
 *
 * @return The added distance.
 */
float get_move_turn_before(enum movement move, uint8_t speed)
{
	return turns[move][speed].before;
}

/**
 * @brief Get the straight distance that a turn adds after a straight movement.
 *
 * @param[in] turn Turn type.
 * @param[in] speed Configured speed level.
 *
 * @return The added distance.
 */
float get_move_turn_after(enum movement move, uint8_t speed)
{
	return turns[move][speed].after;
}

/**
 * @brief Get the expected linear speed at which to turn.
 *
 * @param[in] turn Turn type.
 * @param[in] speed Configured speed level.
 *
 * @return The expected speed.
 */
float get_move_turn_linear_speed(enum movement move, uint8_t speed)
{
	return turns[move][speed].linear_speed;
}
