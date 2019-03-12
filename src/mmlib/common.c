#include "common.h"

/**
 * @brief Return the sign of a number.
 *
 * @param number The number to get the sign from.
 *
 * @return The sign of the number.
 */
int sign(float number)
{
	return (int)(number > 0) - (int)(number < 0);
}
