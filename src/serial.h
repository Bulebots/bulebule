#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdlib.h>
#include <string.h>

#include <libopencm3/stm32/usart.h>

#include "calibration.h"
#include "control.h"
#include "encoder.h"
#include "logging.h"
#include "move.h"
#include "speed.h"

bool serial_transfer_complete(void);
void serial_wait_send_available(uint32_t timeout);
void serial_send(char *data, int size);
void execute_command(void);

#endif /* __SERIAL_H */
