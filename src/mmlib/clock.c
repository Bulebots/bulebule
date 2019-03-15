#include "clock.h"

static volatile uint32_t clock_ticks;
static volatile uint32_t stopwatch_counter;

/**
 * @brief Get the current clock ticks count.
 *
 * This is the total number of ticks that the clock has been running since the
 * last reset.
 *
 * @note Clock ticks are incemented with the `clock_tick` function.
 *
 * @return The current clock ticks count.
 */
uint32_t get_clock_ticks(void)
{
	return clock_ticks;
}

/**
 * @brief Update system clock incrementing the clock tick counter.
 */
void clock_tick(void)
{
	clock_ticks++;
}

/**
 * @brief Sleep (i.e.: do nothing) for a number of ticks.
 *
 * @param[in] ticks Sleep period, in ticks.
 *
 * @note Clock ticks are incemented with the `clock_tick` function.
 */
void sleep_ticks(uint32_t ticks)
{
	uint32_t awake = clock_ticks + ticks;

	while (awake > clock_ticks)
		;
}

/**
 * @brief Sleep (i.e.: do nothing) for a number of seconds.
 *
 * @param[in] seconds Sleep duration, in seconds.
 */
void sleep_seconds(float seconds)
{
	sleep_ticks((uint32_t)(seconds * SYSTICK_FREQUENCY_HZ));
}

/**
 * @brief Start the stopwatch to measure elapsed time.
 *
 * To be used with `stopwatch_stop()` function.
 */
void stopwatch_start(void)
{
	stopwatch_counter = read_cycle_counter();
}

/**
 * @brief Return the elapsed seconds since `stopwatch_start()` was called.
 */
float stopwatch_stop(void)
{
	return (float)(read_cycle_counter() - stopwatch_counter) /
	       (float)SYSCLK_FREQUENCY_HZ;
}

/**
 * @brief Sleep for a number of microseconds.
 *
 * @param[in] us Sleep period, in microseconds.
 */
void sleep_us(uint32_t us)
{
	uint32_t initial_cycles = read_cycle_counter();
	uint32_t sleep_cycles = (uint32_t)(
	    SYSCLK_FREQUENCY_HZ * ((float)us / (float)MICROSECONDS_PER_SECOND));

	while (read_cycle_counter() - initial_cycles <= sleep_cycles)
		;
}

/**
 * @brief Sleep for a number of microseconds since `cycle_counter`.
 *
 * @param[in] cycle_counter Cycle counter value used as starting point.
 * @param[in] us Sleep period, in microseconds.
 */
void sleep_us_after(uint32_t cycle_counter, uint32_t us)
{
	uint32_t sleep_cycles = (uint32_t)(
	    SYSCLK_FREQUENCY_HZ * ((float)us / (float)MICROSECONDS_PER_SECOND));

	while (read_cycle_counter() - cycle_counter <= sleep_cycles)
		;
}

/**
 * @brief Execute a function each period during a defined time span.
 *
 * @param[in] period Execution period, in ticks.
 * @param[in] function Function to execute.
 * @param[in] during Duration of the time span, in ticks.
 */
void each(uint32_t period, void (*function)(void), uint32_t during)
{
	uint32_t ticks_initial;
	uint32_t ticks_current;

	ticks_initial = get_clock_ticks();
	ticks_current = ticks_initial;
	while (ticks_current - ticks_initial < during) {
		if (ticks_current % period == 0)
			function();
		sleep_ticks(1);
		ticks_current = get_clock_ticks();
	}
}

/**
 * @brief Wait until the given function returns true, or timeout.
 *
 * @param[in] timeout Timeout duration, in ticks.
 * @param[in] function Function to execute.
 *
 * @return False if the timeout occurred before the function returned true.
 */
bool wait_until(bool (*function)(void), uint32_t timeout)
{
	uint32_t ticks_initial;

	ticks_initial = get_clock_ticks();
	while (get_clock_ticks() - ticks_initial < timeout) {
		if (function())
			return true;
	}
	return false;
}
