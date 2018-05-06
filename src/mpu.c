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
#define MPU_Z_OFFS_USR_H 23
#define MPU_Z_OFFS_USR_L 24

#define MPU_MASK_H 0xFF00
#define MPU_MASK_L 0x00FF

#define MPU_GYRO_SENSITIVITY_2000_DPS 16.4

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
 * - Configure spi with low speed to write (less than 1MHz)
 * - Reset mpu restoring default settings and wait 100 ms.
 * - Reset signal path and wait 100 ms.
 * - Set SPI mode, reset I2C Slave module
 * - Sample Rate Divider (dix) equal to 0 where: SampleRate = InternalSample /
 *   (1 + div)
 * - DLPF(Dual Low Pass Filter) configuration to 0 with 250 Hz of bandwidth
 *   and InternalSample = 8 KHz
 * - Gyroscope configuration to use DLPF and to select +-2000 dps and 16.4 LSB
 * - Configure spi with high speed (less than 20MHz)
 * - Wait 100 ms
 */
void mpu_setup(void)
{
	setup_spi_low_speed();
	mpu_write_register(MPU_PWR_MGMT_1, 0x80);
	sleep_us(100000);
	mpu_write_register(MPU_SIGNAL_PATH_RESET, 0x07);
	sleep_us(100000);
	mpu_write_register(MPU_USER_CTRL, 0x10);
	mpu_write_register(MPU_SMPLRT_DIV, 0x00);
	mpu_write_register(MPU_CONFIG, 0x00);
	mpu_write_register(MPU_GYRO_CONFIG, 0x18);
	setup_spi_high_speed();
	sleep_us(100000);
}

/**
 * @brief Get gyroscope z current value on bits per second
 */
int16_t get_gyro_z_raw(void)
{
	return ((mpu_read_register(MPU_GYRO_ZOUT_H) << BYTE) |
		mpu_read_register(MPU_GYRO_ZOUT_L));
}

/**
 * @brief Get gyroscope z current value on degrees per second
 */
float get_gyro_z_dps(void)
{
	return ((int16_t)((mpu_read_register(MPU_GYRO_ZOUT_H) << BYTE) |
			  mpu_read_register(MPU_GYRO_ZOUT_L)) /
		MPU_GYRO_SENSITIVITY_2000_DPS);
}

/**
 * @brief Gyroscope z calibration
 *
 * This function should be executed when the robot is stopped. The gyro z
 * output at that moment shall be substracted from the gyro output since then.
 * The offset values are saved on 2’s complement on 16 bits with spi low
 * speed. After write, the offset value we will wait 100 ms.
 */
void gyro_z_calibration(void)
{
	int16_t zout_c2;

	zout_c2 = -get_gyro_z_raw();
	setup_spi_low_speed();
	mpu_write_register(MPU_Z_OFFS_USR_H,
			   ((uint8_t)((zout_c2 & MPU_MASK_H) >> BYTE)));
	mpu_write_register(MPU_Z_OFFS_USR_L, (uint8_t)(zout_c2 & MPU_MASK_L));
	setup_spi_high_speed();
	sleep_us(100000);
}
