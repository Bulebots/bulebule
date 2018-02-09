#include "gyro.h"

#define GYRO_READ 0x80

#define GYRO_SIGNAL_PATH_RESET 104
#define GYRO_PWR_MGMT_1 107
#define GYRO_USER_CTRL 106
#define GYRO_WHOAMI 117

/**
 * @brief Read a gyroscope register.
 *
 * @param[in] address Register address.
 */
uint8_t gyro_read_register(uint8_t address)
{
	uint8_t reading;

	gpio_clear(GPIOB, GPIO12);
	spi_send(SPI2, (GYRO_READ | address));
	spi_read(SPI2);
	spi_send(SPI2, 0x00);
	reading = spi_read(SPI2);
	gpio_set(GPIOB, GPIO12);

	return reading;
}

/**
 * @brief Write a gyroscope register with a given value.
 *
 * @param[in] address Register address.
 * @param[in] address Register value.
 */
void gyro_write_register(uint8_t address, uint8_t value)
{
	gpio_clear(GPIOB, GPIO12);
	spi_send(SPI2, address);
	spi_read(SPI2);
	spi_send(SPI2, value);
	spi_read(SPI2);
	gpio_set(GPIOB, GPIO12);
}

/**
 * @brief Read the WHOAMI register value.
 *
 * This is a read-only register set to 0x70 after reset.
 */
uint8_t gyro_who_am_i(void)
{
	return gyro_read_register(GYRO_WHOAMI);
}
