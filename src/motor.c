#include "motor.h"

/**
 * @brief Set left motor power.
 *
 * Power is set modulating the PWM signal sent to the motor driver.
 *
 * @param[in] power Power value from -DRIVER_PWM_PERIOD to DRIVER_PWM_PERIOD.
 */
void power_left(int32_t power)
{
	if (power > 0) {
		gpio_set(GPIOB, GPIO12);
		gpio_clear(GPIOB, GPIO13);
	} else {
		gpio_clear(GPIOB, GPIO12);
		gpio_set(GPIOB, GPIO13);
	}
	if (power < 0)
		power = -power;
	if (power > DRIVER_PWM_PERIOD)
		power = DRIVER_PWM_PERIOD;
	timer_set_oc_value(TIM3, TIM_OC3, power);
}

/**
 * @brief Set right motor power.
 *
 * Power is set modulating the PWM signal sent to the motor driver.
 *
 * @param[in] power Power value from -DRIVER_PWM_PERIOD to DRIVER_PWM_PERIOD.
 */
void power_right(int32_t power)
{
	if (power > 0) {
		gpio_set(GPIOB, GPIO14);
		gpio_clear(GPIOB, GPIO15);
	} else {
		gpio_clear(GPIOB, GPIO14);
		gpio_set(GPIOB, GPIO15);
	}
	if (power < 0)
		power = -power;
	if (power > DRIVER_PWM_PERIOD)
		power = DRIVER_PWM_PERIOD;
	timer_set_oc_value(TIM3, TIM_OC4, power);
}

/**
 * @brief Break both motors.
 *
 * Set driver controlling signals to high to short break the driver outputs.
 * The break will also set both motors power to zero.
 */
void drive_break(void)
{
	gpio_set(GPIOB, GPIO12 | GPIO13 | GPIO14 | GPIO15);
	power_left(0);
	power_right(0);
}
