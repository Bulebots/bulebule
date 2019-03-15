#include "platform.h"

#define MPU_READ 0x80

/**
 * @brief Read the microcontroller clock cycle counter.
 *
 * This counter increases by one at `SYSCLK_FREQUENCY_HZ`.
 */
uint32_t read_cycle_counter(void)
{
	return dwt_read_cycle_counter();
}

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

/**
 * @brief Function to get battery voltage.
 *
 * This function reads the voltage of the battery from the register configured
 * on the ADC2.
 *
 * The value is converted from bits to voltage taking into account that the
 * battery voltage is read through a voltage divider.
 *
 *@return The battery voltage in volts.
 */
float get_battery_voltage(void)
{
	uint16_t battery_bits;

	adc_start_conversion_direct(ADC2);
	while (!adc_eoc(ADC2))
		;
	battery_bits = adc_read_regular(ADC2);
	return battery_bits * ADC_LSB * VOLT_DIV_FACTOR;
}

/**
 * @brief Read a MPU register.
 *
 * @param[in] address Register address.
 */
uint8_t mpu_read_register(uint8_t address)
{
	uint8_t reading;

	gpio_clear(GPIOB, GPIO12);
	spi_send(SPI2, (MPU_READ | address));
	spi_read(SPI2);
	spi_send(SPI2, 0x00);
	reading = spi_read(SPI2);
	gpio_set(GPIOB, GPIO12);

	return reading;
}

/**
 * @brief Write a MPU register with a given value.
 *
 * @param[in] address Register address.
 * @param[in] address Register value.
 */
void mpu_write_register(uint8_t address, uint8_t value)
{
	gpio_clear(GPIOB, GPIO12);
	spi_send(SPI2, address);
	spi_read(SPI2);
	spi_send(SPI2, value);
	spi_read(SPI2);
	gpio_set(GPIOB, GPIO12);
}

/**
 * @brief Turn on the speaker to play at the selected frequency.
 *
 * Frequency is set modulating the PWM signal sent to the speaker.
 *
 * @param[in] hz Frequency, in Hertz.
 *
 * @note The speaker and emitters both share TIM1. Therefore, this function
 * disables the emitters completely. They are enabled back on `speaker_off()`
 * call.
 */
void speaker_on(float hz)
{
	uint16_t period;

	setup_speaker();

	period = (uint16_t)(SPEAKER_BASE_FREQUENCY_HZ / hz);
	timer_set_period(TIM1, period);
	timer_set_oc_value(TIM1, TIM_OC3, period / 2);
	timer_enable_counter(TIM1);
	timer_enable_oc_output(TIM1, TIM_OC3);
}

/**
 * @brief Turn off the speaker.
 *
 * @note The speaker and emitters both share TIM1. This function makes sure
 * emitters are properly configured after turning off the speaker.
 */
void speaker_off(void)
{
	timer_disable_counter(TIM1);
	timer_disable_oc_output(TIM1, TIM_OC3);

	setup_emitters();
}
