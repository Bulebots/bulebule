#include "serial.h"

#define BUFFER_SIZE 256

static struct data_buffer {
	char data[BUFFER_SIZE];
	uint32_t size;
} buffer;

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
 * @brief Process a command received.
 */
static void process_command(void)
{
	if (!strncmp(buffer.data, "battery", BUFFER_SIZE))
		LOG_INFO("Battery voltage: %.2f V", get_battery_voltage());
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
