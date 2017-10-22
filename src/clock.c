#include "clock.h"

static volatile uint32_t clock_ticks;

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
