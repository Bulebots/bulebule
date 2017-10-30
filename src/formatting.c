#include "formatting.h"

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
