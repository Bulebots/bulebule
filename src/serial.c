#include "serial.h"

static mutex_t _send_lock;
static volatile bool received;
static char receive_buffer[RECEIVE_BUFFER_SIZE];

/**
 * @brief Try to acquire the serial transfer lock.
 *
 * @return Whether the lock was acquired or not.
 */
bool serial_acquire_transfer_lock(void)
{
	return (bool)mutex_trylock(&_send_lock);
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
 * @brief Receive data from serial.
 *
 * DMA is configured to read from USART3 (Bluetooth) a number `size` of bytes.
 * It writes all those bytes to `data`.
 *
 * An interruption is generated when the transfer is complete.
 */
static void serial_receive(void)
{
	dma_channel_reset(DMA1, DMA_CHANNEL3);

	dma_set_peripheral_address(DMA1, DMA_CHANNEL3, (uint32_t)&USART3_DR);
	dma_set_memory_address(DMA1, DMA_CHANNEL3, (uint32_t)receive_buffer);
	dma_set_number_of_data(DMA1, DMA_CHANNEL3, RECEIVE_BUFFER_SIZE);
	dma_set_read_from_peripheral(DMA1, DMA_CHANNEL3);
	dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL3);
	dma_set_peripheral_size(DMA1, DMA_CHANNEL3, DMA_CCR_PSIZE_8BIT);
	dma_set_memory_size(DMA1, DMA_CHANNEL3, DMA_CCR_MSIZE_8BIT);
	dma_set_priority(DMA1, DMA_CHANNEL3, DMA_CCR_PL_HIGH);

	dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL3);

	dma_enable_channel(DMA1, DMA_CHANNEL3);

	usart_enable_rx_dma(USART3);
}

/**
 * @brief DMA 1 channel 2 interruption routine.
 *
 * Executed on serial transfer complete. Clears the interruption flag, and
 * disables serial transfer DMA until next call to `serial_send()`.
 *
 * It will also release the serial transfer lock.
 */
void dma1_channel2_isr(void)
{
	if (dma_get_interrupt_flag(DMA1, DMA_CHANNEL2, DMA_TCIF))
		dma_clear_interrupt_flags(DMA1, DMA_CHANNEL2, DMA_TCIF);

	dma_disable_transfer_complete_interrupt(DMA1, DMA_CHANNEL2);
	usart_disable_tx_dma(USART3);
	dma_disable_channel(DMA1, DMA_CHANNEL2);
	mutex_unlock(&_send_lock);
}

/**
 * @brief DMA 1 channel 3 interruption routine.
 *
 * Executed on serial receive complete. Clears the interruption flag, and
 * resets the receive DMA by calling `serial_receive()`.
 *
 * This function should never be reached. Commands should be processed before
 * on USART idle line interruption.
 **/
void dma1_channel3_isr(void)
{
	if (dma_get_interrupt_flag(DMA1, DMA_CHANNEL3, DMA_TCIF))
		dma_clear_interrupt_flags(DMA1, DMA_CHANNEL3, DMA_TCIF);

	dma_disable_transfer_complete_interrupt(DMA1, DMA_CHANNEL3);
	usart_disable_rx_dma(USART3);
	dma_disable_channel(DMA1, DMA_CHANNEL3);
	LOG_ERROR("Receive buffer is full! Resetting...");
	serial_receive();
}

/**
 * @brief USART interruption routine.
 *
 * On RX interruption it will process the data received and, in case it finds
 * a `'\0'` character, the `process_command` function will be executed.
 */
void usart3_isr(void)
{
	/* Only execute on idle interrupt */
	if (((USART_CR1(USART3) & USART_CR1_IDLEIE) != 0) &&
	    usart_idle_line_detected(USART3)) {
		received = true;
		usart_clear_idle_line_detected(USART3);
		serial_receive();
	}
}

bool get_received_command_flag(void)
{
	return received;
}

void set_received_command_flag(bool value)
{
	received = value;
}

char *get_received_serial_buffer(void)
{
	return receive_buffer;
}
