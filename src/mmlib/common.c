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

/**
 * @brief Check if the received buffer starts with the given string.
 *
 * @param[in] string String buffer.
 * @param[in] start_string Prefix to look for at the start of the string buffer.
 */
bool starts_with(char *string, char *start_string)
{
	return (bool)!strncmp(string, start_string, strlen(start_string));
}
