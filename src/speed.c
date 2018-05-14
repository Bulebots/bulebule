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

const float linear_acceleration_defaults[NUM_MODES] = {5., 0., 0.};
const float linear_deceleration_defaults[NUM_MODES] = {5., 0., 0.};
const float angular_acceleration_defaults[NUM_MODES] = {32. * PI, 0., 0.};

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

/**
 * @brief Set speed parameters with default values from a predefined mode.
 */
void set_speed_mode(uint8_t mode)
{
	linear_acceleration = linear_acceleration_defaults[mode];
	linear_deceleration = linear_deceleration_defaults[mode];
	angular_acceleration = angular_acceleration_defaults[mode];
}
