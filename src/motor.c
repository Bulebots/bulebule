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
	bool forward = true;

	if (power < 0) {
		power = -power;
		forward = false;
	}
	if (power > MAX_PWM_PERIOD) {
		power = MAX_PWM_PERIOD;
		saturated_left += 1;
	} else {
		saturated_left = 0;
	}
	if (forward) {
		timer_set_oc_value(TIM3, TIM_OC1, MAX_PWM_PERIOD);
		timer_set_oc_value(TIM3, TIM_OC2, MAX_PWM_PERIOD - power);
	} else {
		timer_set_oc_value(TIM3, TIM_OC1, MAX_PWM_PERIOD - power);
		timer_set_oc_value(TIM3, TIM_OC2, MAX_PWM_PERIOD);
	}
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
	bool forward = true;

	if (power < 0) {
		power = -power;
		forward = false;
	}
	if (power > MAX_PWM_PERIOD) {
		power = MAX_PWM_PERIOD;
		saturated_right += 1;
	} else {
		saturated_right = 0;
	}
	if (forward) {
		timer_set_oc_value(TIM3, TIM_OC3, MAX_PWM_PERIOD);
		timer_set_oc_value(TIM3, TIM_OC4, MAX_PWM_PERIOD - power);
	} else {
		timer_set_oc_value(TIM3, TIM_OC3, MAX_PWM_PERIOD - power);
		timer_set_oc_value(TIM3, TIM_OC4, MAX_PWM_PERIOD);
	}
}

/**
 * @brief Break both motors (short the motor winding).
 */
void drive_break(void)
{
	timer_set_oc_value(TIM3, TIM_OC1, MAX_PWM_PERIOD);
	timer_set_oc_value(TIM3, TIM_OC2, MAX_PWM_PERIOD);
	timer_set_oc_value(TIM3, TIM_OC3, MAX_PWM_PERIOD);
	timer_set_oc_value(TIM3, TIM_OC4, MAX_PWM_PERIOD);
}

/**
 * @brief Disable the motor driver (let both motors coast).
 */
void drive_off(void)
{
	timer_set_oc_value(TIM3, TIM_OC1, 0);
	timer_set_oc_value(TIM3, TIM_OC2, 0);
	timer_set_oc_value(TIM3, TIM_OC3, 0);
	timer_set_oc_value(TIM3, TIM_OC4, 0);
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
