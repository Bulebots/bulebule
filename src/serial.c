#include "serial.h"

#define BUFFER_SIZE 256

static struct data_buffer {
	char data[BUFFER_SIZE];
	unsigned int size;
} buffer;

static bool run_angular_speed_profile_signal;
static bool run_linear_speed_profile_signal;
static bool run_static_turn_right_profile_signal;
static bool run_front_sensors_calibration_signal;
static char run_movement_sequence_signal[BUFFER_SIZE];

/**
 * @brief Check if the serial transfer is complete.
 *
 * @return Whether the transfer has been completed.
 */
bool serial_transfer_complete(void)
{
	return (bool)usart_get_flag(USART3, USART_SR_TC);
}

/**
 * @brief Wait until able to send through serial, or timeout.
 *
 * @param[in] timeout Timeout duration, in ticks.
 */
void serial_wait_send_available(uint32_t timeout)
{
	wait_until(serial_transfer_complete, timeout);
}

/**
 * @brief Send data through serial.
 *
 * DMA is configured to read from `data` a number `size` of bytes. It then
 * writes all those bytes to USART3 (Bluetooth).
 *
 * An interruption is generated when the transfer is complete.
 *
 * @param[in] data Data to send.
 * @param[in] size Size (number of bytes) to send.
 */
void serial_send(char *data, int size)
{
	dma_channel_reset(DMA1, DMA_CHANNEL2);

	dma_set_peripheral_address(DMA1, DMA_CHANNEL2, (uint32_t)&USART3_DR);
	dma_set_memory_address(DMA1, DMA_CHANNEL2, (uint32_t)data);
	dma_set_number_of_data(DMA1, DMA_CHANNEL2, size);
	dma_set_read_from_memory(DMA1, DMA_CHANNEL2);
	dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL2);
	dma_set_peripheral_size(DMA1, DMA_CHANNEL2, DMA_CCR_PSIZE_8BIT);
	dma_set_memory_size(DMA1, DMA_CHANNEL2, DMA_CCR_MSIZE_8BIT);
	dma_set_priority(DMA1, DMA_CHANNEL2, DMA_CCR_PL_VERY_HIGH);

	dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL2);

	dma_enable_channel(DMA1, DMA_CHANNEL2);

	usart_enable_tx_dma(USART3);
}

/**
 * @brief DMA 1 channel 2 interruption routine.
 *
 * Executed on serial transfer complete. Clears the interruption flag, and
 * disables serial transfer DMA until next call to `serial_send()`.
 **/
void dma1_channel2_isr(void)
{
	if (dma_get_interrupt_flag(DMA1, DMA_CHANNEL2, DMA_TCIF))
		dma_clear_interrupt_flags(DMA1, DMA_CHANNEL2, DMA_TCIF);

	dma_disable_transfer_complete_interrupt(DMA1, DMA_CHANNEL2);
	usart_disable_tx_dma(USART3);
	dma_disable_channel(DMA1, DMA_CHANNEL2);
}

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
 *
 * The parsing will start after stripping a defined string from the start of
 * the buffer.
 */
static float parse_float(const char *left_strip)
{
	char *pointer;

	pointer = buffer.data;
	pointer += strlen(left_strip);
	return strtof(pointer, NULL);
}

/**
 * @brief Parse a float number in the received buffer.
 *
 * The parsing will start after a defined number of spaces that are expected
 * before the float in the string.
 */
static float parse_spaced_float(int spaces_before)
{
	unsigned int i;
	char *pointer;

	pointer = buffer.data;
	for (i = 0; i < buffer.size; i++) {
		if (buffer.data[i] == ' ')
			spaces_before--;
		if (buffer.data[i] == '\0')
			return 0.;
		pointer++;
		if (spaces_before == 0)
			break;
	}
	return strtof(pointer, NULL);
}

/**
 * @brief Check if the received buffer starts with the given string.
 */
static bool starts_with(char *start_string)
{
	return (bool)!strncmp(buffer.data, start_string, strlen(start_string));
}

/**
 * @brief Process a command received.
 */
static void process_command(void)
{
	LOG_DEBUG("Processing \"%s\"", buffer.data);

	if (!strcmp(buffer.data, "battery"))
		log_battery_voltage();
	else if (!strcmp(buffer.data, "configuration_variables"))
		log_configuration_variables();
	else if (!strcmp(buffer.data, "run linear_speed_profile"))
		run_linear_speed_profile_signal = true;
	else if (!strcmp(buffer.data, "run angular_speed_profile"))
		run_angular_speed_profile_signal = true;
	else if (!strcmp(buffer.data, "run static_turn_right_profile"))
		run_static_turn_right_profile_signal = true;
	else if (!strcmp(buffer.data, "run front_sensors_calibration"))
		run_front_sensors_calibration_signal = true;
	else if (starts_with("move "))
		strcpy(run_movement_sequence_signal, buffer.data);
	else if (starts_with("set micrometers_per_count "))
		set_micrometers_per_count(parse_spaced_float(2));
	else if (starts_with("set wheels_separation "))
		set_wheels_separation(parse_spaced_float(2));
	else if (starts_with("set max_linear_speed "))
		set_max_linear_speed(parse_spaced_float(2));
	else if (starts_with("set linear_acceleration "))
		set_linear_acceleration(parse_spaced_float(2));
	else if (starts_with("set linear_deceleration "))
		set_linear_deceleration(parse_spaced_float(2));
	else if (starts_with("set angular_acceleration "))
		set_angular_acceleration(parse_spaced_float(2));
	else if (starts_with("set kp_linear "))
		set_kp_linear(parse_float("set kp_linear "));
	else if (starts_with("set kd_linear "))
		set_kd_linear(parse_float("set kd_linear "));
	else if (starts_with("set kp_angular "))
		set_kp_angular(parse_float("set kp_angular "));
	else if (starts_with("set kd_angular "))
		set_kd_angular(parse_float("set kd_angular "));
	else if (starts_with("set ki_angular_side "))
		set_ki_angular_side(parse_spaced_float(2));
	else if (starts_with("set ki_angular_front "))
		set_ki_angular_front(parse_spaced_float(2));
	else if (starts_with("set kp_angular_side "))
		set_kp_angular_side(parse_spaced_float(2));
	else if (starts_with("set kp_angular_front "))
		set_kp_angular_front(parse_spaced_float(2));
	else
		LOG_WARNING("Unknown command: `%s`!", buffer.data);

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
	} else if (run_static_turn_right_profile_signal) {
		run_static_turn_right_profile_signal = false;
		run_static_turn_right_profile();
	} else if (run_front_sensors_calibration_signal) {
		run_front_sensors_calibration_signal = false;
		run_front_sensors_calibration();
	} else if (strlen(run_movement_sequence_signal)) {
		run_movement_sequence(run_movement_sequence_signal);
		run_movement_sequence_signal[0] = '\0';
	}
}
