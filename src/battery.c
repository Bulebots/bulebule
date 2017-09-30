#include "battery.h"

/**
 * @brief ADC1 and ADC2 interruption routine.
 *
 * - Manage the ADC2 analog watchdog interruption flag.
 * - Send a message.
 * - Toggle a LED.
 * - Disable analog watchdog interruptions on injected channels.
 */
void adc1_2_isr(void)
{
	if (adc_get_flag(ADC2, ADC_SR_AWD)) {
		adc_clear_flag(ADC2, ADC_SR_AWD);
		printf("WARNING: Battery low!\n");
		gpio_toggle(GPIOC, GPIO13);
		adc_disable_analog_watchdog_injected(ADC2);
	}
}
