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
 * @brief Function to get battery voltage.
 *
 * This function reads the voltage of the battery from the register configured
 * on the ADC2. The conversion is triggered on sensors state machine function
 * sm_emitter_adc.
 *
 * The value is converted from bits to voltage taking into account that the
 * battery voltage is read through a voltage divider.
 *
 *@return The battery voltage in volts.
 */
float get_battery_voltage(void)
{
	uint16_t battery_bits;

	battery_bits = adc_read_injected(ADC2, 3);
	return battery_bits * ADC_LSB * VOLT_DIV_FACTOR;
}
