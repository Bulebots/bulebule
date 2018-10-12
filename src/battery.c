#include "battery.h"

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
	uint8_t channels[16];

	channels[0] = ADC_CHANNEL0;
	adc_set_regular_sequence(ADC2, 1, channels);
	adc_start_conversion_direct(ADC2);
	while (!adc_eoc(ADC2))
		;
	battery_bits = adc_read_regular(ADC2);
	return battery_bits * ADC_LSB * VOLT_DIV_FACTOR;
}
