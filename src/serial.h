#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdlib.h>
#include <string.h>

#include <libopencm3/stm32/usart.h>

#include "mmlib/control.h"
#include "mmlib/encoder.h"
#include "mmlib/speed.h"

#include "calibration.h"
#include "logging.h"
#include "move.h"

bool serial_acquire_transfer_lock(void);
void serial_send(char *data, int size);
void execute_command(void);

#endif /* __SERIAL_H */
