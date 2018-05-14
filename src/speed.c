#include "speed.h"

/**
 * Maximum acceleration and deceleration.
 *
 * - Linear acceleration is defined in meters per second squared.
 * - Linear deceleration is defined in meters per second squared.
 * - Angular acceleration is defined in radians per second squared.
 */
static volatile float linear_acceleration = 5.;
static volatile float linear_deceleration = 5.;
static volatile float angular_acceleration = 32. * PI;

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
