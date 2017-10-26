#include "logging.h"

/**
 * @brief Make `printf` send characters through the USART.
 *
 * Both standard output and standard error are redirected through the USART.
 */
int _write(int file, char *ptr, int len)
{
	int i;

	if (file == STDOUT_FILENO || file == STDERR_FILENO) {
		for (i = 0; i < len; i++)
			usart_send_blocking(USART3, ptr[i]);
		return i;
	}

	errno = EIO;
	return -1;
}

/**
 * @brief Log information about linear speed relevant variables.
 *
 * These include:
 *
 * - Target linear speed and ideal (expected) linear speed.
 * - Actual speed of both wheels (left and right).
 * - PWM output value for both motors.
 */
void log_linear_speed(void)
{
	float left_speed = get_encoder_left_speed();
	float right_speed = get_encoder_right_speed();
	float target_speed = get_target_linear_speed();
	float ideal_speed = get_ideal_linear_speed();
	int pwm_left = get_left_pwm();
	int pwm_right = get_right_pwm();

	LOG_INFO("%f,%f,%f,%f,%d,%d", target_speed, ideal_speed, left_speed,
		 right_speed, pwm_left, pwm_right);
}
