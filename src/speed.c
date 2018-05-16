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
static volatile float max_angular_speed;
static volatile float turn_linear_speed;
static volatile float turn_radius;
static volatile float turn_t0;
static volatile float turn_t1;

const float linear_acceleration_defaults[NUM_MODES] = {5., 0., 0.};
const float linear_deceleration_defaults[NUM_MODES] = {5., 0., 0.};
const float angular_acceleration_defaults[NUM_MODES] = {32. * PI, 0., 0.};
const float max_linear_speed_defaults[NUM_MODES] = {.5, 0., 0.};
const float max_angular_speed_defaults[NUM_MODES] = {3 * PI, 0., 0.};
const float turn_linear_speed_defaults[NUM_MODES] = {0.448, 0., 0.};
const float turn_radius_defaults[NUM_MODES] = {0.07, 0., 0.};
const float turn_t0_defaults[NUM_MODES] = {166, 0., 0.};
const float turn_t1_defaults[NUM_MODES] = {260, 0., 0.};

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
 */
void set_speed_mode(uint8_t mode)
{
	linear_acceleration = linear_acceleration_defaults[mode];
	linear_deceleration = linear_deceleration_defaults[mode];
	angular_acceleration = angular_acceleration_defaults[mode];
	max_linear_speed = max_linear_speed_defaults[mode];
	max_angular_speed = max_angular_speed_defaults[mode];
	turn_linear_speed = turn_linear_speed_defaults[mode];
	turn_radius = turn_radius_defaults[mode];
	turn_t0 = turn_t0_defaults[mode];
	turn_t1 = turn_t1_defaults[mode];
}
