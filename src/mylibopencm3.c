#include "mylibopencm3.h"

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
 * @brief USART idle line interrupt enable.
 *
 * @param[in] usart USART block register address base.
 */
void usart_enable_idle_line_interrupt(uint32_t usart)
{
	USART_CR1(usart) |= USART_CR1_IDLEIE;
}

/**
 * @brief USART idle line interrupt disable.
 *
 * @param[in] usart USART block register address base.
 */
void usart_disable_idle_line_interrupt(uint32_t usart)
{
	USART_CR1(usart) &= ~USART_CR1_IDLEIE;
}

/**
 * @brief Check USART idle line detected bit in Status Register.
 *
 * @param[in] usart USART block register address base.
 */
bool usart_idle_line_detected(uint32_t usart)
{
	return ((USART_SR(usart) & USART_SR_IDLE) != 0);
}

/**
 * @brief Clear USART idle line detected bit in Status Register.
 *
 * Clear is performed by a software sequence: a read to the USART_SR register
 * followed by a read to the USART_DR register.
 *
 * @param[in] usart USART block register address base.
 *
 * @see Reference Manual (RM0008): Status register (USART_SR).
 */
void usart_clear_idle_line_detected(uint32_t usart)
{
	USART_SR(usart);
	USART_DR(usart);
}
