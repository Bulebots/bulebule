#include "buttons.h"

/**
 * @brief Function to read button left.
 */
bool button_left_read(void)
{
	return (bool)(gpio_get(GPIOA, GPIO11));
}

/**
 * @brief Function to read button right.
 */
bool button_right_read(void)
{
	return (bool)(gpio_get(GPIOA, GPIO12));
}
