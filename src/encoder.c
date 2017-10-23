#include "encoder.h"

/* Difference between the current count and the latest count */
static volatile int32_t left_diff_count;
static volatile int32_t right_diff_count;

/* Total number of counts */
static volatile int32_t left_total_count;
static volatile int32_t right_total_count;

/* Total travelled distance, in micrometers */
static volatile int32_t left_micrometers;
static volatile int32_t right_micrometers;

/* Speed, in meters per second */
static volatile float left_speed;
static volatile float right_speed;

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
int32_t get_encoder_left_total_count(void)
{
	return left_total_count;
}

/**
 * @brief Read right motor encoder total count.
 *
 * The total count is simply the sum of all encoder counter differences.
 */
int32_t get_encoder_right_total_count(void)
{
	return right_total_count;
}

/**
 * @brief Read left motor encoder travelled distance in micrometers.
 */
int32_t get_encoder_left_micrometers(void)
{
	return left_micrometers;
}

/**
 * @brief Read right motor encoder travelled distance in micrometers.
 */
int32_t get_encoder_right_micrometers(void)
{
	return right_micrometers;
}

/**
 * @brief Read left motor speed in meters per second.
 */
float get_encoder_left_speed(void)
{
	return left_speed;
}

/**
 * @brief Read right motor speed in meters per second.
 */
float get_encoder_right_speed(void)
{
	return right_speed;
}

/**
 * @brief Update encoder readings.
 *
 * - Read raw encoder counters.
 * - Update the count differences (with respect to latest reading).
 * - Calculate distance travelled.
 * - Calculate speed.
 */
void update_encoder_readings(void)
{
	static uint16_t last_left_count;
	static uint16_t last_right_count;

	uint16_t left_count;
	uint16_t right_count;

	left_count = read_encoder_left();
	right_count = read_encoder_right();
	left_total_count += left_diff_count;
	right_total_count += right_diff_count;
	left_diff_count = (int32_t)(left_count - last_left_count);
	right_diff_count = (int32_t)(right_count - last_right_count);

	left_micrometers = (int32_t)(left_total_count * MICROMETERS_PER_COUNT);
	right_micrometers =
	    (int32_t)(right_total_count * MICROMETERS_PER_COUNT);

	left_speed = left_diff_count *
		     (MICROMETERS_PER_COUNT / MICROMETERS_PER_METER) *
		     SYSTICK_FREQUENCY_HZ;
	right_speed = right_diff_count *
		      (MICROMETERS_PER_COUNT / MICROMETERS_PER_METER) *
		      SYSTICK_FREQUENCY_HZ;

	last_left_count = left_count;
	last_right_count = right_count;
}
