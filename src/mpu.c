#include "mpu.h"

#define BYTE 8
#define MPU_READ 0x80

#define MPU_SMPLRT_DIV 25
#define MPU_CONFIG 26
#define MPU_GYRO_CONFIG 27
#define MPU_SIGNAL_PATH_RESET 104
#define MPU_PWR_MGMT_1 107
#define MPU_USER_CTRL 106
#define MPU_WHOAMI 117

#define MPU_GYRO_ZOUT_H 71
#define MPU_GYRO_ZOUT_L 72

/**
 * @brief Read a mpu register.
 *
 * @param[in] address Register address.
 */
static uint8_t mpu_read_register(uint8_t address)
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
 * @brief Write a mpu register with a given value.
 *
 * @param[in] address Register address.
 * @param[in] address Register value.
 */
static void mpu_write_register(uint8_t address, uint8_t value)
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
uint8_t mpu_who_am_i(void)
{
	return mpu_read_register(MPU_WHOAMI);
}

/**
 * @brief MPU-6500 board setup to configure the gyroscope z
 *
 * - Reset mpu restoring default settings
 * - Reset signal path
 * - Set SPI mode, reset I2C Slave module
 * - Sample Rate Divider (dix) equal to 0 where: SampleRate = InternalSample /
 *   (1 + div)
 * - DLPF(Dual Low Pass Filter) configuration to 0 with 250 Hz of bandwidth
 *   and InternalSample = 8 KHz
 * - Gyroscope configuration to use DLPF and to select +-2000 dps and 16.4 LSB
 *   º/s.
 */
void mpu_setup(void)
{
	mpu_write_register(MPU_PWR_MGMT_1, 0x80);
	sleep_us(100000);
	mpu_write_register(MPU_SIGNAL_PATH_RESET, 0x07);
	sleep_us(100000);
	mpu_write_register(MPU_USER_CTRL, 0x10);
	mpu_write_register(MPU_SMPLRT_DIV, 0x00);
	mpu_write_register(MPU_CONFIG, 0x00);
	mpu_write_register(MPU_GYRO_CONFIG, 0x18);
}

/**
 * @brief Get gyroscope z current raw value
 */
int16_t get_gyro_z_raw(void)
{
	return ((mpu_read_register(MPU_GYRO_ZOUT_H) << BYTE) |
		mpu_read_register(MPU_GYRO_ZOUT_L));
}
