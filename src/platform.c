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
