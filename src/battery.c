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
		LOG_WARNING("Battery low!");
		gpio_toggle(GPIOC, GPIO13);
		adc_disable_analog_watchdog_injected(ADC2);
	}
}

/**
 * @brief Function to get battery level.
 *
 * This function reads the level of the battery from the register configured
 * on the ADC2. The conversion is triggered on sensors state machine function
 * sm_emitter_adc.
 *
 * The value is converted from bits to voltage taking into account that the
 * battery level is read through a voltage divider.
 */
float get_battery_level(void)
{
	uint16_t battery_bits;
	float battery_level_mv;

	battery_bits = adc_read_injected(ADC2, 3);
	battery_level_mv =
	    (battery_bits * V_REF_MV * VOLT_DIV_FACTOR) / ADC_12_BITS;
	return battery_level_mv;
}
