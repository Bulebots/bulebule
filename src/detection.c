#include "detection.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

/**
 * @brief TIM1 interruption routine.
 *
 * - Manage the update event interruption flag.
 * - LED toggling each second.
 */
void tim1_up_isr(void)
{
	if (timer_get_flag(TIM1, TIM_SR_UIF)) {
		timer_clear_flag(TIM1, TIM_SR_UIF);
		gpio_toggle(GPIOA, GPIO7);
	}
}
