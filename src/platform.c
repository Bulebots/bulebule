#include "platform.h"

/**
 * @brief Read left motor encoder counter.
 */
uint16_t read_encoder_left(void)
{
	return (uint16_t)timer_get_counter(TIM2);
}

/**
 * @brief Read right motor encoder counter.
 */
uint16_t read_encoder_right(void)
{
	return (uint16_t)timer_get_counter(TIM4);
}
