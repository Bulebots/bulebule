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
static volatile float max_angular_speed;
static volatile float turn_linear_speed;
static volatile float turn_radius;
static volatile float turn_t0;
static volatile float turn_t1;

const float linear_acceleration_defaults[NUM_MODES] = {4., 5., 5.};
const float linear_deceleration_defaults[NUM_MODES] = {4., 5., 5.};
const float angular_acceleration_defaults[NUM_MODES] = {32. * PI, 48. * PI,
							64. * PI};
const float max_end_linear_speed_defaults[NUM_MODES] = {.5, 0.61, 0.72};
const float max_linear_speed_defaults[NUM_MODES] = {.5, 0.61, 0.72};
const float max_linear_speed_run_defaults[NUM_MODES] = {1., 1.5, 2.};
const float max_angular_speed_defaults[NUM_MODES] = {3. * PI, 4. * PI, 6. * PI};
const float turn_linear_speed_defaults[NUM_MODES] = {0.448, 0.566, 0.670};
const float turn_radius_defaults[NUM_MODES] = {0.07, 0.07, 0.07};
const float turn_t0_defaults[NUM_MODES] = {166, 124., 88.};
const float turn_t1_defaults[NUM_MODES] = {260, 208., 176.};

uint8_t speed_configuration;

struct turn_parameters {
	float space;
	float linear_speed;
	int rise_time;
	int elapsed_time;
	float max_angular_speed;
};

struct turn_parameters turns[][NUM_MODES] = {
    [MOVE_LEFT] =
	{
	    {0.02, .448, 166, 260, -3. * PI},
	    {0.02, .448, 166, 260, -3. * PI},
	    {0.02, .448, 166, 260, -3. * PI},
	},
    [MOVE_RIGHT] =
	{
	    {0.02, .448, 166, 260, 3. * PI},
	    {0.02, .448, 166, 260, 3. * PI},
	    {0.02, .448, 166, 260, 3. * PI},
	},
    [MOVE_LEFT_90] =
	{
	    {-0.05, .8, 219, 291, -2.3 * PI},
	    {-0.05, .8, 219, 291, -2.3 * PI},
	    {-0.05, .8, 219, 291, -2.3 * PI},
	},
    [MOVE_RIGHT_90] =
	{
	    {-0.05, .8, 219, 291, 2.3 * PI},
	    {-0.05, .8, 219, 291, 2.3 * PI},
	    {-0.05, .8, 219, 291, 2.3 * PI},
	},
    [MOVE_LEFT_180] =
	{
	    {-0.04, .7, 400, 479, -2.5 * PI},
	    {-0.04, .7, 400, 479, -2.5 * PI},
	    {-0.04, .7, 400, 479, -2.5 * PI},
	},
    [MOVE_RIGHT_180] =
	{
	    {-0.04, .7, 400, 479, 2.5 * PI},
	    {-0.04, .7, 400, 479, 2.5 * PI},
	    {-0.04, .7, 400, 479, 2.5 * PI},
	},
};

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

float get_max_angular_speed(void)
{
	return max_angular_speed;
}

void set_max_angular_speed(float value)
{
	max_angular_speed = value;
}

float get_turn_linear_speed(void)
{
	return turn_linear_speed;
}

void set_turn_linear_speed(float value)
{
	turn_linear_speed = value;
}

float get_turn_radius(void)
{
	return turn_radius;
}

void set_turn_radius(float value)
{
	turn_radius = value;
}

float get_turn_t0(void)
{
	return turn_t0;
}

void set_turn_t0(float value)
{
	turn_t0 = value;
}

float get_turn_t1(void)
{
	return turn_t1;
}

void set_turn_t1(float value)
{
	turn_t1 = value;
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
	max_angular_speed = max_angular_speed_defaults[mode];
	turn_linear_speed = turn_linear_speed_defaults[mode];
	turn_radius = turn_radius_defaults[mode];
	turn_t0 = turn_t0_defaults[mode];
	turn_t1 = turn_t1_defaults[mode];
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
 * @brief Get the straight distance that a turn adds to a straight movement.
 *
 * @param[in] turn Turn type.
 * @param[in] speed Configured speed level.
 *
 * @return The added distance.
 */
float get_move_turn_space(enum movement move, uint8_t speed)
{
	return turns[move][speed].space;
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
