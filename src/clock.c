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
