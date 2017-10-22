#include "hmi.h"

/**
 * @brief Function to toggle the left LED.
 */
void led_left_toggle(void)
{
	gpio_toggle(GPIOB, GPIO4);
}

/**
 * @brief Function to toggle the right LED.
 */
void led_right_toggle(void)
{
	gpio_toggle(GPIOB, GPIO5);
}

/**
 * @brief Function to power ON the left LED.
 */
void led_left_on(void)
{
	gpio_set(GPIOB, GPIO4);
}

/**
 * @brief Function to power ON the right LED.
 */
void led_right_on(void)
{
	gpio_set(GPIOB, GPIO5);
}

/**
 * @brief Function to power OFF the left LED.
 */
void led_left_off(void)
{
	gpio_clear(GPIOB, GPIO4);
}

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
