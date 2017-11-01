#include "serial.h"

#define BUFFER_SIZE 256

static struct data_buffer {
	char data[BUFFER_SIZE];
	uint32_t size;
} buffer;

static bool run_linear_speed_profile_signal;
static bool run_angular_speed_profile_signal;

/**
 * @brief Push a single char to the serial received buffer.
 */
static void push_received(char data)
{
	if (buffer.size >= BUFFER_SIZE) {
		LOG_ERROR("Serial buffer overflow!");
		return;
	}
	buffer.data[buffer.size++] = data;
}

/**
 * @brief Clear the serial received buffer.
 */
static void clear_received(void)
{
	buffer.data[0] = '\0';
	buffer.size = 0;
}

/**
 * @brief Parse a float number in the received buffer.
 */
static float parse_float(const char *left_strip)
{
	char *pointer;

	pointer = buffer.data;
	pointer += strlen(left_strip);
	return strtof(pointer, NULL);
}

/**
 * @brief Process a command received.
 */
static void process_command(void)
{
	if (!strcmp(buffer.data, "battery")) {
		log_battery_voltage();
	} else if (!strcmp(buffer.data, "control_variables")) {
		log_control_variables();
	} else if (!strcmp(buffer.data, "run linear_speed_profile")) {
		run_linear_speed_profile_signal = true;
	} else if (!strcmp(buffer.data, "run angular_speed_profile")) {
		run_angular_speed_profile_signal = true;
	} else if (!strncmp(buffer.data, "set kp_angular ",
			    strlen("set kp_angular "))) {
		set_kp_angular(parse_float("set kp_angular "));
	} else if (!strncmp(buffer.data, "set kd_angular ",
			    strlen("set kd_angular "))) {
		set_kd_angular(parse_float("set kd_angular "));
	} else {
		LOG_WARNING("Unknown command: `%s`!", buffer.data);
	}
	clear_received();
}

/**
 * @brief USART interruption routine.
 *
 * On RX interruption it will process the data received and, in case it finds
 * a `'\0'` character, the `process_command` function will be executed.
 **/
void usart3_isr(void)
{
	static uint8_t data;

	/* Only execute on RX interrupt */
	if (((USART_CR1(USART3) & USART_CR1_RXNEIE) != 0) &&
	    ((USART_SR(USART3) & USART_SR_RXNE) != 0)) {
		data = usart_recv(USART3);
		push_received(data);
		if (data == '\0')
			process_command();
	}
}

/**
 * @brief Execute commands received.
 */
void execute_commands(void)
{
	if (run_linear_speed_profile_signal) {
		run_linear_speed_profile_signal = false;
		run_linear_speed_profile();
	} else if (run_angular_speed_profile_signal) {
		run_angular_speed_profile_signal = false;
		run_angular_speed_profile();
	}
}
