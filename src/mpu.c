#include "mpu.h"

#define BYTE 8
#define MPU_CAL_SAMPLE_NUM 100
#define MPU_AVERAGE_FACTOR 2
#define MPU_COMPLEMENT_2_FACTOR 2
#define MPU_CAL_SAMPLE_US 1000

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
 * @brief Read a MPU register.
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
 * @brief Write a MPU register with a given value.
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
 * @brief MPU-6500 board setup.
 *
 * MPU is configured as follows:
 *
 * - Configure SPI at low speed to write (less than 1MHz)
 * - Reset MPU restoring default settings and wait 100 ms
 * - Reset signal path and wait 100 ms
 * - Set SPI mode, reset I2C Slave module
 * - Sample Rate Divider (dix) equal to 0 where: SampleRate = InternalSample /
 *   (1 + div)
 * - Set DLPF (Dual Low Pass Filter) configuration to 0 with 250 Hz of
 *   bandwidth and InternalSample = 8 kHz
 * - Configure gyroscope's Z-axis with DLPF, -2000 dps and 16.4 LSB
 * - Configure SPI at high speed (less than 20MHz)
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
 * @brief Get gyroscope's Z-axis angular speed in bits per second.
 */
int16_t get_gyro_z_raw(void)
{
	return ((mpu_read_register(MPU_GYRO_ZOUT_H) << BYTE) |
		mpu_read_register(MPU_GYRO_ZOUT_L));
}

/**
 * @brief Get gyroscope's Z-axis angular speed in degrees per second.
 */
float get_gyro_z_dps(void)
{
	return (get_gyro_z_raw() / MPU_GYRO_SENSITIVITY_2000_DPS);
}

/**
 * @brief Calibrate the gyroscope's Z axis.
 *
 * This function should be executed when the robot is stopped. The average of
 * gyroscope z output will be substracted from the gyro output from that
 * moment on. To write MPU registers, the SPI speed is changed to low speed.
 */
void gyro_z_calibration(void)
{
	int16_t zout_c2;
	float zout_av = 0;
	int8_t i;

	for (i = 0; i < MPU_CAL_SAMPLE_NUM; i++) {
		zout_av =
		    ((float)get_gyro_z_raw() + zout_av) / MPU_AVERAGE_FACTOR;
		sleep_us(MPU_CAL_SAMPLE_US);
	}
	zout_c2 = -(int16_t)(zout_av * MPU_COMPLEMENT_2_FACTOR);
	setup_spi_low_speed();
	mpu_write_register(MPU_Z_OFFS_USR_H,
			   ((uint8_t)((zout_c2 & MPU_MASK_H) >> BYTE)));
	mpu_write_register(MPU_Z_OFFS_USR_L, (uint8_t)(zout_c2 & MPU_MASK_L));
	setup_spi_high_speed();
	sleep_us(100000);
}
