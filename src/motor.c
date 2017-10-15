#include "motor.h"

/**
 * @brief Set left motor power.
 *
 * Power is set modulating the PWM signal sent to the motor driver.
 *
 * @param[in] power Power value from 0 to DRIVER_PWM_PERIOD.
 */
void power_left(uint32_t power)
{
	timer_set_oc_value(TIM3, TIM_OC3, power);
}

/**
 * @brief Set right motor power.
 *
 * Power is set modulating the PWM signal sent to the motor driver.
 *
 * @param[in] power Power value from 0 to DRIVER_PWM_PERIOD.
 */
void power_right(uint32_t power)
{
	timer_set_oc_value(TIM3, TIM_OC4, power);
}

/**
 * @brief Set driving direction to forward in both motors.
 */
void drive_forward(void)
{
	gpio_clear(GPIOB, GPIO13 | GPIO15);
	gpio_set(GPIOB, GPIO12 | GPIO14);
}

/**
 * @brief Set driving direction to backward in both motors.
 */
void drive_backward(void)
{
	gpio_clear(GPIOB, GPIO12 | GPIO14);
	gpio_set(GPIOB, GPIO13 | GPIO15);
}

/**
 * @brief Break both motors.
 *
 * Set driver controlling signals to high to short break the driver outputs.
 * The break will be effective with any PWM input signal in the motor driver.
 */
void drive_break(void)
{
	gpio_set(GPIOB, GPIO12 | GPIO13 | GPIO14 | GPIO15);
}
