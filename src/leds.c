#include "leds.h"

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
 * @brief Function to toggle the Bluepill board LED.
 */
void led_bluepill_toggle(void)
{
	gpio_toggle(GPIOC, GPIO13);
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
 * @brief Function to power ON the Bluepill board LED.
 */
void led_bluepill_on(void)
{
	gpio_clear(GPIOC, GPIO13);
}

/**
 * @brief Function to power OFF the left LED.
 */
void led_left_off(void)
{
	gpio_clear(GPIOB, GPIO4);
}

/**
 * @brief Function to power OFF the right LED.
 */
void led_right_off(void)
{
	gpio_clear(GPIOB, GPIO5);
}

/**
 * @brief Function to power OFF the Bluepill board LED.
 */
void led_bluepill_off(void)
{
	gpio_set(GPIOC, GPIO13);
}
