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

/**
 * @brief Blink both LEDs a couple of times.
 */
void blink_burst(void)
{
	int i;

	for (i = 0; i < 10; i++) {
		led_left_on();
		led_right_on();
		sleep_ticks(100);
		led_left_off();
		led_right_off();
		sleep_ticks(100);
	}
}

/**
 * @brief Blink both LEDs alternately to report collision detection.
 */
void blink_collision(void)
{
	int i;

	for (i = 0; i < 10; i++) {
		led_left_on();
		led_right_off();
		sleep_ticks(200);
		led_left_off();
		led_right_on();
		sleep_ticks(200);
	}
	led_right_off();
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

/**
 * @brief Read left button, requiring consecutive positive reads.
 *
 * Readings are performed each system clock tick.
 *
 * @param[in] count Required number of positive reads.
 */
bool button_left_read_consecutive(uint32_t count)
{
	uint32_t initial_ticks = get_clock_ticks();

	while (get_clock_ticks() - initial_ticks < count) {
		if (!gpio_get(GPIOA, GPIO11))
			return false;
	}
	return true;
}

/**
 * @brief Read right button, requiring consecutive positive reads.
 *
 * Readings are performed each system clock tick.
 *
 * @param[in] count Required number of positive reads.
 */
bool button_right_read_consecutive(uint32_t count)
{
	uint32_t initial_ticks = get_clock_ticks();

	while (get_clock_ticks() - initial_ticks < count) {
		if (!gpio_get(GPIOA, GPIO12))
			return false;
	}
	return true;
}

/**
 * @brief Wait for a close front sensor signal.
 *
 * @param[in] close_distance Distance to be considered as close, in meters.
 */
void wait_front_sensor_close_signal(float close_distance)
{
	while (1) {
		if (get_front_right_distance() < close_distance ||
		    get_front_left_distance() < close_distance)
			break;
	}
}

/**
 * @brief Set initial search direction for the solver.
 */
void initialize_solver_direction(void)
{
	while (1) {
		if (button_left_read_consecutive(500)) {
			set_search_initial_direction(NORTH);
			led_left_on();
			break;
		}
		if (button_right_read_consecutive(500)) {
			set_search_initial_direction(EAST);
			led_right_on();
			break;
		}
	}
	sleep_ticks(2000);
}
