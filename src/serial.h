#ifndef __SERIAL_H
#define __SERIAL_H

#include <libopencm3/cm3/sync.h>
#include <libopencm3/stm32/usart.h>

#include "mmlib/logging.h"
#include "mylibopencm3.h"

#define RECEIVE_BUFFER_SIZE 256

bool serial_acquire_transfer_lock(void);
void serial_send(char *data, int size);
bool get_received_command_flag(void);
void set_received_command_flag(bool value);
char *get_received_serial_buffer(void);

#endif /* __SERIAL_H */
