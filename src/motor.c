#include "motor.h"

static volatile uint32_t saturated_left;
static volatile uint32_t saturated_right;

/**
 * @brief Set left motor power.
 *
 * Power is set modulating the PWM signal sent to the motor driver.
 *
 * This function checks for possible PWM saturation. If that is the case the
 * value will be limited to the maximum PWM allowed and the `saturated_left`
 * variable will be incremented by one. This variable is later used to check
 * if multiple consecutive saturated values occurred, which is interpreted as
 * a collision.
 *
 * @param[in] power Power value from -MAX_PWM_PERIOD to MAX_PWM_PERIOD.
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
	if (power > MAX_PWM_PERIOD) {
		power = MAX_PWM_PERIOD;
		saturated_left += 1;
	} else {
		saturated_left = 0;
	}
	timer_set_oc_value(TIM3, TIM_OC3, power);
}

/**
 * @brief Set right motor power.
 *
 * Power is set modulating the PWM signal sent to the motor driver.
 *
 * This function checks for possible PWM saturation. If that is the case the
 * value will be limited to the maximum PWM allowed and the `saturated_right`
 * variable will be incremented by one. This variable is later used to check
 * if multiple consecutive saturated values occurred, which is interpreted as
 * a collision.
 *
 * @param[in] power Power value from -MAX_PWM_PERIOD to MAX_PWM_PERIOD.
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
	if (power > MAX_PWM_PERIOD) {
		power = MAX_PWM_PERIOD;
		saturated_right += 1;
	} else {
		saturated_right = 0;
	}
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

/**
 * @brief Disable the motor driver.
 *
 * Will ignore the PWM signals and will not short-break the motors, meaning
 * that the wheels will be free to run.
 */
void drive_off(void)
{
	gpio_clear(GPIOB, GPIO12 | GPIO13 | GPIO14 | GPIO15);
	power_left(0);
	power_right(0);
}

/**
 * @brief Return the maximum consecutive saturated PWM values.
 */
uint32_t pwm_saturation(void)
{
	if (saturated_right > saturated_left)
		return saturated_right;
	return saturated_left;
}

/**
 * @brief Reset the PWM saturation counters.
 */
void reset_pwm_saturation(void)
{
	saturated_left = 0;
	saturated_right = 0;
}
