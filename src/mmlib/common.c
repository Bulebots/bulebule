#include "common.h"

/**
 * @brief Read a status flag.
 *
 * @param[in] ADC register address base.
 *
 * @param[in] status register flag.
 *
 * @returns flag set.
 *
 * @see adc_reg_base and ADC_SR values from libopencm3 library.
 */
bool adc_get_flag(uint32_t adc_peripheral, uint32_t flag)
{
	if ((ADC_SR(adc_peripheral) & flag) != 0)
		return true;

	return false;
}

/**
 * @brief Clear a status flag.
 *
 * @param[in] ADC register address base.
 *
 * @param[in] Status register flag.
 *
 * @see adc_reg_base and ADC_SR values from libopencm3 library.
 */
void adc_clear_flag(uint32_t adc_peripheral, uint32_t flag)
{
	ADC_SR(adc_peripheral) = ~flag;
}

/**
 * @brief Return the sign of a number.
 *
 * @param number The number to get the sign from.
 *
 * @return The sign of the number.
 */
int sign(float number)
{
	return (int)(number > 0) - (int)(number < 0);
}
