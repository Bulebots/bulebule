#include "encoder.h"

/* Total number of counts */
static volatile int32_t left_total_count;
static volatile int32_t right_total_count;

/* Difference between the current count and the latest count */
static volatile int32_t left_diff_count;
static volatile int32_t right_diff_count;

/**
 * @brief Read left motor encoder counter.
 */
static uint16_t read_encoder_left(void)
{
	return (uint16_t)timer_get_counter(TIM2);
}

/**
 * @brief Read right motor encoder counter.
 */
static uint16_t read_encoder_right(void)
{
	return (uint16_t)timer_get_counter(TIM4);
}

/**
 * @brief Read left motor encoder counter difference.
 *
 * The difference is with respect to the last motor encoder count read.
 */
int32_t get_encoder_left_diff_count(void)
{
	return left_diff_count;
}

/**
 * @brief Read right motor encoder counter difference.
 *
 * The difference is with respect to the last motor encoder count read.
 */
int32_t get_encoder_right_diff_count(void)
{
	return right_diff_count;
}

/**
 * @brief Read left motor encoder total count.
 *
 * The total count is simply the sum of all encoder counter differences.
 */
uint16_t get_encoder_left_total_count(void)
{
	return left_total_count;
}

/**
 * @brief Read right motor encoder total count.
 *
 * The total count is simply the sum of all encoder counter differences.
 */
uint16_t get_encoder_right_total_count(void)
{
	return right_total_count;
}

/**
 * @brief Update encoder readings.
 *
 * - Read raw encoder counters.
 * - Update the count differences (with respect to latest reading).
 * - Add counts to the total count sum.
 */
void update_encoder_readings(void)
{
	static uint16_t left_latest_count;
	static uint16_t right_latest_count;

	uint16_t left_count;
	uint16_t right_count;

	left_count = read_encoder_left();
	right_count = read_encoder_right();

	left_diff_count = (int32_t)(left_count - left_latest_count);
	right_diff_count = (int32_t)(right_count - right_latest_count);

	left_total_count += left_diff_count;
	right_total_count += right_diff_count;

	left_latest_count = left_count;
	right_latest_count = right_count;
}
