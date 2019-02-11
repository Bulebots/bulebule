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
 * @brief Blink LEDs a defined number of times.
 *
 * @param[in] count number of blinks.
 * @param[in] time that LEDs are ON and OFF.
 */
void repeat_blink(uint8_t count, uint16_t time)
{
	int i;

	for (i = 0; i < count; i++) {
		led_left_on();
		led_right_on();
		sleep_ticks(time);
		led_left_off();
		led_right_off();
		sleep_ticks(time);
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
 * @brief Warn low battery using speaker sounds.
 */
void speaker_warn_low_battery(void)
{
	speaker_play('C', 4, 0, 0.05);
	sleep_ticks(50);
	speaker_play('C', 3, 0, 0.05);
	sleep_ticks(50);
}

/**
 * @brief Notify about an error with a low pitch sustained sound.
 */
void speaker_play_error(void)
{
	speaker_play('C', 3, 0, 2.);
}

/**
 * @brief Play three fast, high tones to note a successful operation.
 */
void speaker_play_success(void)
{
	for (int i = 0; i < 3; i++) {
		speaker_play('C', 8, 0, 0.05);
		sleep_ticks(50);
	}
}

/**
 * @brief Play two fast, high tones to note that a button is pushed.
 */
void speaker_play_button(void)
{
	for (int i = 0; i < 2; i++) {
		speaker_play('C', 8, 0, 0.05);
		sleep_ticks(50);
	}
}

/**
 * @brief Play an epic composition before competition.
 */
void speaker_play_competition(void)
{
	speaker_play('C', 7, 0, 0.15);
	sleep_seconds(0.15);
	speaker_play('C', 7, 0, 0.15);
	speaker_play('G', 7, 0, 0.60);
	sleep_seconds(0.15);
	speaker_play('G', 7, 0, 0.15);
	speaker_play('A', 7, 0, 0.15);
	speaker_play('G', 7, 0, 0.15);
	speaker_play('F', 7, 0, 0.15);
	speaker_play('G', 7, 0, 0.45);
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

/**
 * @brief Select a force level for exploration or run phases.
 *
 * It starts at a minimum defined force and increases that force by steps.
 *
 * @param[in] minimum_force Minimum force.
 * @param[in] force_step Force increase on each step.
 *
 * @return The selected force.
 */
float hmi_configure_force(float minimum_force, float force_step)
{
	uint8_t force = 0;

	while (1) {
		if (button_right_read_consecutive(500)) {
			if (force == 10)
				force = 0;
			else
				force += 1;
			repeat_blink(force, 300);
		}
		if (button_left_read_consecutive(1000)) {
			led_left_on();
			led_right_on();
			sleep_ticks(2000);
			return force * force_step + minimum_force;
		}
	}
}

/**
 * @brief Function to choose if reuse the saved EEPROM maze.
 *
 * @return true if the maze saved on EEPROM is restored to RAM.
 */
bool reuse_maze(void)
{
	if (maze_is_saved()) {
		led_bluepill_on();
		while (1) {
			if (button_left_read_consecutive(500)) {
				speaker_play_button();
				sleep_seconds(1);
				load_maze();
				return true;
			}
			if (button_right_read_consecutive(500)) {
				speaker_play_button();
				sleep_seconds(1);
				reset_maze();
				break;
			}
		}
	}
	led_bluepill_off();
	return false;
}
